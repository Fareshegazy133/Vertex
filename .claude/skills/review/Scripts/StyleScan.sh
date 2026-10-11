#!/usr/bin/env bash
# StyleScan: mechanical style checks for Vertex, used by the /review skill.
#
# Checks what a model reading a diff can't see reliably (tabs vs spaces, BOMs,
# trailing whitespace, brace placement) plus the regex-checkable rules from
# CLAUDE.md "Code style", "The raylib rule", and "Build system", and a heuristic
# pass over class layout and function order ("Declaration order").
#
# Every hit is a CANDIDATE. The reviewer confirms each one against the source
# before reporting it, and drops false positives.
#
# Usage (from anywhere inside the repo):
#   StyleScan.sh                  files changed vs HEAD, plus untracked files (default)
#   StyleScan.sh --branch [base]  files changed since the merge base with <base> (default: master)
#   StyleScan.sh --all            every tracked and untracked file
#   StyleScan.sh <path>...        the given files or folders
#
# Always exits 0, so a finding never aborts the skill that runs it.

set -u
export LC_ALL=C

Root=$(git rev-parse --show-toplevel 2>/dev/null) || { echo "StyleScan: not inside a git repository."; exit 0; }
cd "$Root" || exit 0

Mode="changed"
Base="master"
Paths=()

case "${1:-}" in
	""|--changed)
		;;
	--branch)
		Mode="branch"
		Base="${2:-master}"
		;;
	--all)
		Mode="all"
		;;
	*)
		Mode="paths"
		Paths=("$@")
		;;
esac

ListFiles()
{
	case "$Mode" in
		changed)
			git -c core.safecrlf=false diff --name-only --diff-filter=d HEAD
			git ls-files --others --exclude-standard
			;;
		branch)
			local MergeBase
			MergeBase=$(git merge-base "$Base" HEAD 2>/dev/null) || { echo "StyleScan: no merge base with '$Base'." >&2; return; }
			git -c core.safecrlf=false diff --name-only --diff-filter=d "$MergeBase"
			git ls-files --others --exclude-standard
			;;
		all)
			git ls-files --cached --others --exclude-standard
			;;
		paths)
			git ls-files --cached --others --exclude-standard -- "${Paths[@]}"
			;;
	esac
}

# Only Vertex's own C++ and Lua. Fetched third-party sources are not ours to style.
mapfile -t Files < <(ListFiles | sort -u | grep -E '\.(h|hpp|inl|cpp|lua)$' | grep -v -E '^ThirdParty/[^/]+/Source/')

if [ "${#Files[@]}" -eq 0 ]; then
	echo "StyleScan ($Mode): no C++ or Lua files in scope."
	exit 0
fi

echo "StyleScan ($Mode): ${#Files[@]} file(s). Every hit is a candidate: confirm it against the source."

# Walks up from a file to the folder holding its <Module>.Module.lua.
ModuleRootOf()
{
	local Dir
	Dir=$(dirname "$1")
	while [ "$Dir" != "." ] && [ "$Dir" != "/" ]; do
		if compgen -G "$Dir/*.Module.lua" > /dev/null; then
			echo "$Dir"
			return
		fi
		Dir=$(dirname "$Dir")
	done
}

Total=0

for File in "${Files[@]}"; do
	[ -f "$File" ] || continue

	Kind="cpp"
	case "$File" in
		*.lua) Kind="lua" ;;
	esac

	# A .cpp includes its own header first. Only check it when that header exists in the module.
	OwnHeader=""
	case "$File" in
		*.cpp)
			ModuleRoot=$(ModuleRootOf "$File")
			Stem=$(basename "$File" .cpp)
			if [ -n "$ModuleRoot" ] && [ -n "$(find "$ModuleRoot" -name "$Stem.h" -print -quit)" ]; then
				OwnHeader="$Stem.h"
			fi
			;;
	esac

	MissingFinalNewline=0
	if [ -s "$File" ] && [ -n "$(tail -c1 "$File")" ]; then
		MissingFinalNewline=1
	fi

	Output=$(awk -v File="$File" -v Kind="$Kind" -v OwnHeader="$OwnHeader" -v MissingFinalNewline="$MissingFinalNewline" '
		# This program sits in a single-quoted bash string: an apostrophe anywhere in it,
		# comments included, ends the string and breaks the script.
		function Hit(Rule, Line, Text)
		{
			Count[Rule]++
			if (Count[Rule] <= 5)
			{
				gsub(/\t/, "  ", Text)
				if (length(Text) > 100)
				{
					Text = substr(Text, 1, 97) "..."
				}
				Lines[Rule] = Lines[Rule] sprintf("    L%d: %s\n", Line, Text)
			}
			if (!(Rule in Seen))
			{
				Seen[Rule] = 1
				Order[++RuleCount] = Rule
			}
		}

		# A .cpp or .inl may hold no comments at all, so there a comment is the finding,
		# whatever its shape.
		function CommentHit(Rule, Line, Text)
		{
			if (!IsImpl)
			{
				Hit(Rule, Line, Text)
			}
		}

		# Class layout and function order (CLAUDE.md "Declaration order"). A heuristic: it
		# follows braces to find class bodies, splits each body into declarations, and sorts
		# each function into one of the 13 rows. A const function that returns neither bool
		# nor void, and has no Get, Find, or Set prefix, may be a normal function or an other
		# getter, so it is left out of the row check. Within the operator row, only the seven
		# operator steps are checked, not the order inside a step.
		function TopKind()
		{
			return (Depth == 0) ? "file" : ScopeKind[Depth]
		}

		function AtStatementLevel(    Kind)
		{
			Kind = TopKind()
			return (Kind == "file" || Kind == "ns" || Kind == "class")
		}

		# Kind is "block", "enum", "ns", or "class:<class|struct>:<Name>".
		function PushScope(Kind,    Parts)
		{
			Depth++
			ScopeKind[Depth] = Kind
			if (Kind ~ /^class:/)
			{
				split(Kind, Parts, ":")
				ScopeKind[Depth] = "class"
				ClassName[Depth] = Parts[3]
				ClassIsClass[Depth] = (Parts[2] == "class")
				ClassAccess[Depth] = ClassIsClass[Depth] ? 3 : 1
				ClassSawSpec[Depth] = 0
				ClassAnySpec[Depth] = 0
				LastGroup[Depth] = 0
				LastAccess[Depth] = 0
				LastRow[Depth] = 0
				LastSub[Depth] = 0
				LastOpCat[Depth] = 0
			}
		}

		function PopScope()
		{
			if (Depth > 0)
			{
				Depth--
			}
		}

		# Drops "template <...>" from the front of a line and returns the rest.
		function StripTemplateHead(Text,    I, Ch, Level)
		{
			I = index(Text, "<")
			if (I == 0)
			{
				return ""
			}
			Level = 0
			for (; I <= length(Text); I++)
			{
				Ch = substr(Text, I, 1)
				if (Ch == "<")
				{
					Level++
				}
				else if (Ch == ">")
				{
					Level--
					if (Level == 0)
					{
						break
					}
				}
			}
			Text = substr(Text, I + 1)
			sub(/^[ \t]+/, "", Text)
			return Text
		}

		# Splits a declaration into SigBefore, SigName, SigParams, and SigAfter. For an
		# operator, SigOpCat is its step in the operator order: 1 assignment, 2 compound
		# assignment, 3 binary, 4 comparison, 5 subscript and call, 6 other, 7 conversion.
		# Returns 0 when the declaration is not a function.
		function ParseSignature(Text,    Start, Rest, Token, I, Level, Ch)
		{
			SigBefore = ""
			SigName = ""
			SigParams = ""
			SigAfter = ""
			SigOpCat = 0
			Token = ""
			if (match(Text, /(^|[^A-Za-z0-9_])operator([^A-Za-z0-9_]|$)/))
			{
				Start = RSTART + index(substr(Text, RSTART), "operator") - 1
				SigBefore = substr(Text, 1, Start - 1)
				Rest = substr(Text, Start + 8)
				sub(/^[ \t]+/, "", Rest)
				if (Rest ~ /^(new|delete)([^A-Za-z0-9_]|$)/)
				{
					Token = (Rest ~ /^new/) ? "new" : "delete"
					SigOpCat = 6
				}
				else if (match(Rest, /^[A-Za-z_][^(]*\(/))
				{
					Token = substr(Rest, 1, RLENGTH - 1)
					SigOpCat = 7
				}
				else
				{
					for (I = 1; I <= OpTokenCount; I++)
					{
						if (substr(Rest, 1, length(OpTokens[I])) == OpTokens[I])
						{
							Token = OpTokens[I]
							break
						}
					}
					if (Token == "")
					{
						return 0
					}
				}
				Rest = substr(Rest, length(Token) + 1)
				gsub(/[ \t]/, "", Token)
				SigName = "operator" Token
			}
			else
			{
				while (gsub(/<[^<>]*>/, "", Text) > 0)
				{
				}
				Start = index(Text, "(")
				if (Start == 0)
				{
					return 0
				}
				SigBefore = substr(Text, 1, Start - 1)
				if (SigBefore ~ /=/ || !match(SigBefore, /~?[A-Za-z_][A-Za-z0-9_]*[ \t]*$/))
				{
					return 0
				}
				SigName = substr(SigBefore, RSTART, RLENGTH)
				sub(/[ \t]+$/, "", SigName)
				SigBefore = substr(SigBefore, 1, RSTART - 1)
				if (SigName ~ /^(void|bool|char|auto|const|decltype|alignas|noexcept|requires|return|sizeof)$/)
				{
					return 0
				}
				Rest = substr(Text, Start)
			}
			Start = index(Rest, "(")
			if (Start == 0)
			{
				return 0
			}
			Level = 0
			for (I = Start; I <= length(Rest); I++)
			{
				Ch = substr(Rest, I, 1)
				if (Ch == "(")
				{
					Level++
				}
				else if (Ch == ")")
				{
					Level--
					if (Level == 0)
					{
						break
					}
				}
			}
			SigParams = substr(Rest, Start + 1, I - Start - 1)
			SigAfter = substr(Rest, I + 1)
			if (Token != "" && SigOpCat == 0)
			{
				if (Token == "=")
				{
					SigOpCat = 1
				}
				else if (Token ~ /^(\+=|-=|\*=|\/=|%=|&=|\|=|\^=|<<=|>>=)$/)
				{
					SigOpCat = 2
				}
				else if (Token ~ /^(\+|-|\*|\/|%|&|\||\^|<<|>>)$/)
				{
					SigOpCat = (SigParams ~ /^[ \t]*(void)?[ \t]*$/) ? 6 : 3
				}
				else if (Token ~ /^(==|<=>|!=|<|>|<=|>=)$/)
				{
					SigOpCat = 4
				}
				else if (Token == "[]" || Token == "()")
				{
					SigOpCat = 5
				}
				else
				{
					SigOpCat = 6
				}
			}
			return 1
		}

		# Identifies a function across its declaration and an out-of-line definition: the
		# name, the parameters without spaces or default arguments, and a trailing const.
		function MakeKey(    Params, Qualifier)
		{
			Params = SigParams
			while (gsub(/<[^<>]*>/, "", Params) > 0)
			{
			}
			gsub(/=[^,]*/, "", Params)
			gsub(/[ \t]/, "", Params)
			Qualifier = (SigAfter ~ /^[ \t]*const([^A-Za-z0-9_]|$)/) ? "const" : ""
			return SigName "(" Params ")" Qualifier
		}

		function CheckMember(Text,    Group, Access, Row, Sub, IsConst, Params, Owner, Count)
		{
			if (Text ~ /^(friend|static_assert)([^A-Za-z0-9_]|$)/)
			{
				return
			}
			gsub(/\[\[[^]]*\]\]/, "", Text)
			sub(/^[ \t]+/, "", Text)
			Group = 3
			if (Text ~ /^(using|typedef|enum|class|struct|union)([^A-Za-z0-9_]|$)/ || Text ~ /^[A-Z][A-Z0-9_]*\(/)
			{
				Group = 1
			}
			else if (ParseSignature(Text))
			{
				Group = 2
			}
			Access = ClassAccess[Depth]
			if (Group < LastGroup[Depth])
			{
				Hit("Class layout: types, then functions, then variables", StmtLine, StmtRaw)
			}
			else if (Group > LastGroup[Depth])
			{
				if (LastGroup[Depth] > 0 && !ClassSawSpec[Depth] && (ClassIsClass[Depth] || ClassAnySpec[Depth]))
				{
					Hit("Class layout: each group restates its access specifier", StmtLine, StmtRaw)
				}
				LastGroup[Depth] = Group
				LastAccess[Depth] = Access
				LastRow[Depth] = 0
				LastSub[Depth] = 0
				LastOpCat[Depth] = 0
			}
			else if (Access < LastAccess[Depth])
			{
				Hit("Class layout: each group runs public, then protected, then private", StmtLine, StmtRaw)
			}
			else if (Access > LastAccess[Depth])
			{
				LastAccess[Depth] = Access
				LastRow[Depth] = 0
				LastSub[Depth] = 0
				LastOpCat[Depth] = 0
			}
			ClassSawSpec[Depth] = 0
			if (Group != 2)
			{
				return
			}

			Owner = ClassName[Depth]
			Count = ++DeclKeyCount[Owner]
			DeclKey[Owner, Count] = MakeKey()
			DeclName[Owner, Count] = SigName

			IsConst = (SigAfter ~ /^[ \t]*const([^A-Za-z0-9_]|$)/)
			Sub = 0
			if (SigName == Owner)
			{
				Row = (SigParams ~ /^[ \t]*(void)?[ \t]*$/) ? 1 : 2
				Params = SigParams
				while (gsub(/<[^<>]*>/, "", Params) > 0)
				{
				}
				if (Params ~ ("^[ \t]*const[ \t]+" Owner "[ \t]*&([^&]|$)"))
				{
					Sub = 1
				}
				else if (Params ~ ("^[ \t]*" Owner "[ \t]*&&"))
				{
					Sub = 2
				}
			}
			else if (SigName == ("~" Owner))
			{
				Row = 3
			}
			else if (SigName ~ /^operator/)
			{
				Row = 13
			}
			else if (SigBefore ~ /(^|[^A-Za-z0-9_])virtual([^A-Za-z0-9_]|$)/ || SigAfter ~ /(^|[^A-Za-z0-9_])(override|final)([^A-Za-z0-9_]|$)/)
			{
				Row = 4
			}
			else if (SigBefore ~ /(^|[^A-Za-z0-9_])static([^A-Za-z0-9_]|$)/)
			{
				Row = 5
			}
			else if (StmtTemplate)
			{
				Row = 6
			}
			else if (SigName ~ /^Set[A-Z]/)
			{
				Row = 8
			}
			else if (SigName ~ /^Find([A-Z]|$)/)
			{
				Row = 10
			}
			else if (SigName ~ /^Get([A-Z]|$)/)
			{
				Row = 11
			}
			else if (IsConst && SigBefore ~ /(^|[^A-Za-z0-9_])bool[ \t]*$/)
			{
				Row = 12
			}
			else if (!IsConst || SigBefore ~ /(^|[^A-Za-z0-9_])void[ \t]*$/)
			{
				Row = 7
			}
			else
			{
				return
			}

			if (Row < LastRow[Depth])
			{
				Hit("Function order: a function sits above a row it belongs below", StmtLine, SigName " (row " Row ") is below " LastRowName[Depth] " (row " LastRow[Depth] ")")
				return
			}
			if (Row > LastRow[Depth])
			{
				LastRow[Depth] = Row
				LastSub[Depth] = 0
				LastOpCat[Depth] = 0
			}
			LastRowName[Depth] = SigName
			if (Row == 2)
			{
				if (Sub < LastSub[Depth])
				{
					Hit("Function order: constructors take your own parameters, then copy, then move", StmtLine, StmtRaw)
				}
				else
				{
					LastSub[Depth] = Sub
				}
			}
			if (Row == 13)
			{
				if (SigOpCat < LastOpCat[Depth])
				{
					Hit("Function order: operators follow the operator order", StmtLine, SigName " comes after " LastOpName[Depth])
				}
				else
				{
					LastOpCat[Depth] = SigOpCat
					LastOpName[Depth] = SigName
				}
			}
		}

		# An out-of-line member definition below its class, such as a class template defines,
		# follows the order the class declares its functions in.
		function CheckDefinition(Text,    Probe, Owner, Rest, Key, I, Found)
		{
			gsub(/\[\[[^]]*\]\]/, "", Text)
			while (gsub(/<[^<>]*>/, "", Text) > 0)
			{
			}
			Probe = Text
			Owner = ""
			while (match(Probe, /[A-Za-z_][A-Za-z0-9_]*::/))
			{
				Rest = substr(Probe, RSTART + RLENGTH)
				if ((substr(Probe, RSTART, RLENGTH - 2) in DeclKeyCount) && Rest ~ /^(~?[A-Za-z_][A-Za-z0-9_]*[ \t]*\(|operator([^A-Za-z0-9_]|$))/)
				{
					Owner = substr(Probe, RSTART, RLENGTH - 2)
					break
				}
				Probe = Rest
			}
			if (Owner == "" || !ParseSignature(Rest))
			{
				return
			}
			Key = MakeKey()
			Found = 0
			for (I = 1; I <= DeclKeyCount[Owner]; I++)
			{
				if (!((Owner, I) in DeclUsed) && DeclKey[Owner, I] == Key)
				{
					Found = I
					break
				}
			}
			for (I = 1; !Found && I <= DeclKeyCount[Owner]; I++)
			{
				if (!((Owner, I) in DeclUsed) && DeclName[Owner, I] == SigName)
				{
					Found = I
				}
			}
			if (!Found)
			{
				return
			}
			DeclUsed[Owner, Found] = 1
			if (Found < LastDef[Owner])
			{
				Hit("Definition order: definitions below the class follow its declaration order", StmtLine, StmtRaw)
			}
			else
			{
				LastDef[Owner] = Found
			}
		}

		function FinishStatement(    Text)
		{
			Text = Stmt
			gsub(/[ \t]+/, " ", Text)
			sub(/^ /, "", Text)
			sub(/ $/, "", Text)
			if (Text != "")
			{
				if (TopKind() == "class")
				{
					CheckMember(Text)
				}
				else
				{
					CheckDefinition(Text)
				}
			}
			Stmt = ""
			StmtTemplate = 0
		}

		# Feeds one line of comment-free code to the layout checks.
		function ScanLayout(Text, Trim, LineNo,    Scan, Head, Name, I, Ch)
		{
			Scan = Text
			gsub(/"([^"\\]|\\.)*"/, "\"\"", Scan)
			gsub("\047[^\047]*\047", "0", Scan)
			if (AtStatementLevel())
			{
				if (TopKind() == "class" && Trim ~ /^(public|protected|private)[ \t]*:$/)
				{
					ClassAccess[Depth] = (Trim ~ /^public/) ? 1 : ((Trim ~ /^protected/) ? 2 : 3)
					ClassSawSpec[Depth] = 1
					ClassAnySpec[Depth] = 1
					return
				}
				if (TopKind() == "class" && Stmt == "" && Trim ~ /^[A-Z][A-Z0-9_]*\(.*\)$/)
				{
					StmtLine = LineNo
					StmtRaw = Raw
					Stmt = Trim
					FinishStatement()
					return
				}
				Head = Trim
				if (Head ~ /^template[ \t]*</)
				{
					StmtTemplate = 1
					Head = StripTemplateHead(Head)
					if (Head == "" || Head ~ /^requires([^A-Za-z0-9_]|$)/)
					{
						return
					}
					Scan = Head
				}
				else if (Stmt == "" && Head ~ /^requires([^A-Za-z0-9_]|$)/)
				{
					return
				}
				if (Head ~ /^(class|struct|union)[ \t]/ && (Head !~ /;[ \t]*$/ || Head ~ /\{/))
				{
					Name = Head
					sub(/^(class|struct|union)[ \t]+([A-Z]+_API[ \t]+)?/, "", Name)
					sub(/[^A-Za-z0-9_].*$/, "", Name)
					PendingScope = "class:" ((Head ~ /^class/) ? "class" : "struct") ":" Name
				}
				else if (Head ~ /^namespace([^A-Za-z0-9_]|$)/ || Head ~ /^extern[ \t]+"/)
				{
					PendingScope = "ns"
				}
				else if (Head ~ /^enum([^A-Za-z0-9_]|$)/)
				{
					PendingScope = "enum"
				}
			}
			for (I = 1; I <= length(Scan); I++)
			{
				Ch = substr(Scan, I, 1)
				if (Ch == "{")
				{
					if (AtStatementLevel())
					{
						FinishStatement()
					}
					PushScope((PendingScope == "") ? "block" : PendingScope)
					PendingScope = ""
				}
				else if (Ch == "}")
				{
					PopScope()
				}
				else if (AtStatementLevel())
				{
					if (Ch == ";")
					{
						FinishStatement()
						PendingScope = ""
					}
					else
					{
						if (Stmt == "")
						{
							if (Ch ~ /[ \t]/)
							{
								continue
							}
							StmtLine = LineNo
							StmtRaw = Raw
						}
						Stmt = Stmt Ch
					}
				}
			}
			if (AtStatementLevel() && Stmt != "")
			{
				Stmt = Stmt " "
			}
		}

		# awk needs a pattern and its opening brace on one line.
		BEGIN {
			IsHeader = (File ~ /\.(h|hpp)$/)
			IsCpp = (File ~ /\.cpp$/)
			IsImpl = (File ~ /\.(cpp|inl)$/)
			IsPublic = (File ~ /\/Public\//)
			IsRaylibBackend = (File ~ /^Source\/Runtime\/Private\/Platform\/Raylib\//)
			IsLogBackend = (File ~ /\/Private\/Logging\//)
			IsDescriptor = (File ~ /(\.Module\.lua|^Vertex\.lua)$/)
			IsNumericTypes = (File ~ /(\/Core\/Types\/NumericTypes\.h|\/Types\/NumericTypesTests\.cpp)$/)
			BlockComment = 0
			FirstCode = ""
			FirstInclude = ""
			PrevLine = ""
			Depth = 0
			Stmt = ""
			StmtTemplate = 0
			PendingScope = ""
			InPreproc = 0
			# Longest first, so "<=>" wins over "<=" and "<".
			OpTokenCount = split("<=> <<= >>= ->* -> () [] ++ -- == != <= >= && || << >> += -= *= /= %= &= |= ^= + - * / % & | ^ ! ~ < > = ,", OpTokens, " ")
		}

		{
			Raw = $0
			sub(/\r$/, "", Raw)
			N = NR

			if (N == 1 && substr(Raw, 1, 3) == "\357\273\277")
			{
				Hit("Encoding: UTF-8 BOM (files are UTF-8 without BOM)", N, "")
				Raw = substr(Raw, 4)
			}

			# Whitespace rules apply to every line, comments included.
			if (Raw ~ /[ \t]+$/)
			{
				Hit("Whitespace: trailing whitespace", N, Raw)
			}
			if (match(Raw, /^[ \t]+/))
			{
				Lead = substr(Raw, 1, RLENGTH)
				Rest = substr(Raw, RLENGTH + 1)
				# " * text" continues a block comment; that single space is conventional.
				if (Lead ~ / / && Rest !~ /^\*/)
				{
					Hit("Indent: spaces in indentation (indent with tabs)", N, Raw)
				}
			}

			# Strip comments before the code checks, checking their format on the way
			# (CLAUDE.md "Comments"). Naive about comment markers inside strings.
			Code = Raw
			if (Kind == "cpp")
			{
				Stripped = Raw
				sub(/^[ \t]+/, "", Stripped)
				sub(/[ \t]+$/, "", Stripped)
				StartsComment = (!BlockComment && Stripped ~ /^\/\*/)
				LineHasComment = BlockComment
				if (BlockComment)
				{
					if (Code ~ /\*\//)
					{
						if (Stripped != "*/")
						{
							CommentHit("Comments: a block comment closes with */ alone on its line", N, Raw)
						}
						sub(/^.*\*\//, "", Code)
						BlockComment = 0
					}
					else
					{
						if (Stripped !~ /^\*( |$)/)
						{
							CommentHit("Comments: each line inside a block comment starts with \" * \"", N, Raw)
						}
						Code = ""
					}
				}
				# Whichever marker comes first decides: "// a /* b" is a line comment.
				HadComment = 0
				while (1)
				{
					LinePos = index(Code, "//")
					BlockPos = index(Code, "/*")
					if (LinePos > 0 && (BlockPos == 0 || LinePos < BlockPos))
					{
						if (N != 1)
						{
							CommentHit("Comments: // comment (use /** */; only the copyright line uses //)", N, Raw)
							LineHasComment = 1
						}
						Code = substr(Code, 1, LinePos - 1)
						break
					}
					if (BlockPos == 0)
					{
						break
					}
					LineHasComment = 1
					Rest = substr(Code, BlockPos)
					# The classic C-comment pattern: the shortest /* ... */, which awk has no lazy quantifier for.
					if (match(Rest, /^\/\*([^*]|\*+[^*\/])*\*+\//))
					{
						Comment = substr(Rest, 1, RLENGTH)
						if (Comment !~ /^\/\*\* [^ ](.*[^ ])? \*\/$/)
						{
							CommentHit("Comments: a one-line comment is /** Description */", N, Raw)
						}
						Code = substr(Code, 1, BlockPos - 1) substr(Rest, RLENGTH + 1)
						HadComment = 1
						continue
					}
					if (Rest !~ /^\/\*\*[ \t]*$/ || substr(Code, 1, BlockPos - 1) ~ /[^ \t]/)
					{
						CommentHit("Comments: a block comment opens with /** alone on its line", N, Raw)
					}
					Code = substr(Code, 1, BlockPos - 1)
					BlockComment = 1
					break
				}
				if (HadComment && Code ~ /[^ \t]/)
				{
					CommentHit("Comments: a comment sits on its own line, above what it describes", N, Raw)
				}
				if (Raw ~ /@info([^A-Za-z0-9_]|$)/)
				{
					CommentHit("Comments: @info tag (use @note)", N, Raw)
				}
				if (IsImpl && LineHasComment)
				{
					Hit("Comments: a .cpp or .inl has no comments besides the copyright line", N, Raw)
				}
			}
			else
			{
				sub(/--.*$/, "", Code)
			}

			Trimmed = Code
			sub(/[ \t]+$/, "", Trimmed)
			sub(/^[ \t]+/, "", Trimmed)

			# Allman: an opening brace goes on its own line, in C++ and in Lua tables.
			if (Trimmed ~ /.\{$/ && Trimmed !~ /^#/)
			{
				Hit("Braces: opening brace not on its own line (Allman)", N, Raw)
			}
			if (Trimmed ~ /^\}[ \t]*(else|catch)([^A-Za-z0-9_]|$)/)
			{
				Hit("Braces: else/catch on the closing-brace line (Allman)", N, Raw)
			}

			if (Kind == "cpp")
			{
				if (N == 1 && Raw != "// Copyright HNDRED GAMES. All Rights Reserved.")
				{
					Hit("File layout: first line must be the copyright line", N, Raw)
				}
				# One blank line after every enum value but the last (CLAUDE.md "Enums").
				# A value is a line ending in a comma; the last value has none.
				if (InEnum)
				{
					if (Trimmed ~ /^\}/)
					{
						InEnum = 0
					}
					else if (AfterEnumValue && Stripped != "")
					{
						Hit("Enums: one blank line after every value but the last", N, Raw)
					}
					AfterEnumValue = (Trimmed ~ /,$/)
				}
				else if (EnumPending && Trimmed == "{")
				{
					InEnum = 1
					EnumPending = 0
					AfterEnumValue = 0
				}
				# A forward declaration ("enum class E : int;") has no body to check.
				if (Code ~ /(^|[^A-Za-z0-9_])enum[ \t]+(class|struct)[^A-Za-z0-9_]/ && Trimmed !~ /;$/)
				{
					EnumPending = (Trimmed !~ /\{$/)
					InEnum = (Trimmed ~ /\{$/)
					AfterEnumValue = 0
				}
				# One blank line before every commented declaration, except right after an
				# opening brace, an access specifier, or an #if (CLAUDE.md "Comments", Format).
				# Enum values are left to the enum check above, so no line is reported twice.
				IsAccessSpecifier = (PrevStripped ~ /^(public|protected|private)[ \t]*:$/)
				if (StartsComment && !InEnum && PrevStripped != "" && PrevStripped != "{" && !IsAccessSpecifier && PrevStripped !~ /^#[ \t]*(if|ifdef|ifndef|elif|else)([^A-Za-z]|$)/)
				{
					Hit("Spacing: one blank line before every commented declaration", N, Raw)
				}
				if (IsAccessSpecifier && Stripped == "")
				{
					Hit("Spacing: no blank line after an access specifier", N - 1, PrevRaw)
				}
				PrevStripped = Stripped
				PrevRaw = Raw
				if (FirstCode == "" && Trimmed != "")
				{
					FirstCode = Trimmed
					FirstCodeLine = N
				}
				if (Trimmed ~ /^#[ \t]*include[ \t]*[<"]/)
				{
					if (FirstInclude == "")
					{
						FirstInclude = Trimmed
						FirstIncludeLine = N
					}
					if (Trimmed ~ /[<"\/](raylib|rlgl|raymath)\.h[>"]/)
					{
						HasRaylib = 1
						if (!IsRaylibBackend)
						{
							Hit("raylib rule 1: raylib header included outside Runtime/Private/Platform/Raylib/", N, Raw)
						}
					}
					if (Trimmed ~ /[<"\/][Ww]indows\.h[>"]/)
					{
						HasWindows = 1
					}
					if (Trimmed ~ /^#[ \t]*include[ \t]*"/ && Trimmed ~ /"(\.\.?\/|Public\/|Private\/|Source\/)/)
					{
						Hit("Includes: write includes from the module include root (no ../, Public/, Private/, Source/)", N, Raw)
					}
					# Include order: a .cpp starts with its own header (checked above, and exempt here), then quoted includes, then <standard> ones,
					# each run sorted alphabetically, ignoring case.
					IncludePath = Trimmed
					sub(/^#[ \t]*include[ \t]*[<"]/, "", IncludePath)
					sub(/[>"].*$/, "", IncludePath)
					IncludeGroup = (Trimmed ~ /^#[ \t]*include[ \t]*"/) ? 1 : 2
					IsOwnInclude = (IsCpp && IncludeCount == 0)
					IncludeCount++
					if (!IsOwnInclude)
					{
						if (PrevIncludeGroup != "" && IncludeGroup < PrevIncludeGroup)
						{
							Hit("Includes: quoted includes come before <standard> ones", N, Raw)
						}
						else if (PrevIncludeGroup != "" && IncludeGroup == PrevIncludeGroup && tolower(IncludePath) < tolower(PrevIncludePath))
						{
							Hit("Includes: sort each run of includes alphabetically", N, Raw)
						}
						PrevIncludeGroup = IncludeGroup
						PrevIncludePath = IncludePath
					}
				}
				if (IsHeader && Trimmed ~ /^#[ \t]*ifndef[ \t]+[A-Z0-9_]+_H(PP)?_?$/)
				{
					Hit("File layout: include guard (headers use #pragma once)", N, Raw)
				}
				if (Trimmed ~ /^#[ \t]*define[ \t]+[A-Za-z_]/)
				{
					Name = Trimmed
					sub(/^#[ \t]*define[ \t]+/, "", Name)
					sub(/[^A-Za-z0-9_].*$/, "", Name)
					if (Name !~ /^VX_[A-Z0-9_]+$/ || Name ~ /_API$/)
					{
						Hit("Naming: macro must be VX_-prefixed UPPER_SNAKE; <MODULE>_API comes from the build", N, Raw)
					}
				}
				if (Trimmed ~ /^#[ \t]*ifn?def[ \t]+VERTEX_ENABLE_ASSERTS/ || Code ~ /defined[ \t]*\(?[ \t]*VERTEX_ENABLE_ASSERTS/)
				{
					Hit("Defines: VERTEX_ENABLE_ASSERTS is always defined (0 or 1); test it with #if, never #ifdef/defined()", N, Raw)
				}
				if (Code ~ /(^|[^A-Za-z0-9_])m_[A-Za-z]/)
				{
					Hit("Naming: m_ member prefix (members are plain PascalCase)", N, Raw)
				}
				if (Code ~ /(^|[^A-Za-z0-9_])enum[ \t]+[A-Za-z_]/ && Code !~ /enum[ \t]+(class|struct)[^A-Za-z0-9_]/)
				{
					Hit("Naming: plain enum (use enum class)", N, Raw)
				}
				if (match(Code, /enum[ \t]+(class|struct)[ \t]+[A-Za-z_][A-Za-z0-9_]*/))
				{
					Name = substr(Code, RSTART, RLENGTH)
					sub(/^enum[ \t]+(class|struct)[ \t]+/, "", Name)
					if (Name !~ /^E[A-Z]/)
					{
						Hit("Naming: enum must be E + PascalCase", N, Raw)
					}
				}
				# Looks past an export macro: "class CORE_API VLogger".
				if (Code !~ /template/ && Code !~ /enum[ \t]+(class|struct)/ && match(Code, /(^|[^A-Za-z0-9_])(class|struct)[ \t]+([A-Z]+_API[ \t]+)?[A-Za-z_][A-Za-z0-9_]*/))
				{
					Name = substr(Code, RSTART, RLENGTH)
					sub(/^.*(class|struct)[ \t]+([A-Z]+_API[ \t]+)?/, "", Name)
					if (Name !~ /^[VT][A-Z]/)
					{
						Hit("Naming: class/struct must be V + PascalCase (class templates T + PascalCase)", N, Raw)
					}
				}
				if (match(Code, /template[ \t]*<.*>/))
				{
					Params = substr(Code, RSTART, RLENGTH)
					while (match(Params, /(typename|class)(\.\.\.)?[ \t]+[A-Za-z_][A-Za-z0-9_]*/))
					{
						Name = substr(Params, RSTART, RLENGTH)
						Params = substr(Params, RSTART + RLENGTH)
						sub(/^(typename|class)(\.\.\.)?[ \t]+/, "", Name)
						if (Name !~ /^T[A-Z]/)
						{
							Hit("Naming: template type parameter must be T + PascalCase", N, Raw)
						}
					}
				}
				# Heuristic: "bool X;" / "bool X =" / "bool X{" is a member or local; "bool X," / "bool X)" is a parameter.
				Probe = Code
				while (match(Probe, /(^|[^A-Za-z0-9_])bool[ \t]+[A-Za-z_][A-Za-z0-9_]*[ \t]*[;={,)]/))
				{
					Decl = substr(Probe, RSTART, RLENGTH)
					Probe = substr(Probe, RSTART + RLENGTH)
					Name = Decl
					sub(/^.*bool[ \t]+/, "", Name)
					Terminator = substr(Name, length(Name), 1)
					sub(/[ \t]*[;={,)]$/, "", Name)
					# "bool operator==(...)" returns a bool; it declares no variable.
					if (Name == "operator")
					{
						continue
					}
					if ((Terminator == "," || Terminator == ")") && Name ~ /^b[A-Z]/)
					{
						Hit("Naming: bool parameter takes no b prefix", N, Raw)
					}
					else if (Terminator != "," && Terminator != ")" && Name !~ /^b[A-Z]/)
					{
						Hit("Naming: bool member/local needs the b prefix", N, Raw)
					}
				}
				if (IsHeader && Code ~ /(^|[^A-Za-z0-9_])using[ \t]+namespace[^A-Za-z0-9_]/)
				{
					Hit("Headers: using namespace in a header leaks into every includer", N, Raw)
				}
				if (!IsLogBackend && Code ~ /(std::cout|std::cerr|(^|[^A-Za-z0-9_:])(printf|puts|fprintf)[ \t]*\(|std::(printf|puts|fprintf)[ \t]*\(|OutputDebugString)/)
				{
					Hit("Logging: direct console output (use Vertex::Log)", N, Raw)
				}
				if (Code ~ /(^|[^A-Za-z0-9_])default[ \t]*:/)
				{
					Hit("Switch: default: label (enum switches must not use one, so C4062 catches new values)", N, Raw)
				}
				# CLAUDE.md "Numeric types". The header that defines the aliases, and the test that
				# compares them with the std types, are exempt. String literals are blanked first,
				# so a log message cannot trip it. The main line is skipped; an exit code or a C API
				# that needs exactly int (the printf precision in Log.cpp) is an expected hit.
				if (!IsNumericTypes)
				{
					Probe = Code
					gsub(/"([^"\\]|\\.)*"/, "\"\"", Probe)
					if (Probe ~ /(^|[^A-Za-z0-9_])(short|long|unsigned|signed|float|double)([^A-Za-z0-9_]|$)/ ||
						Probe ~ /(^|[^A-Za-z0-9_])(u?int(_least|_fast)?[0-9]+_t|u?intmax_t|size_t|u?intptr_t)([^A-Za-z0-9_]|$)/ ||
						(Probe ~ /(^|[^A-Za-z0-9_])int([^A-Za-z0-9_]|$)/ && Probe !~ /(^|[^A-Za-z0-9_])main[ \t]*\(/))
					{
						Hit("Numeric types: use an alias from Core/Types/NumericTypes.h (int32, usize, float32, ...)", N, Raw)
					}
				}
				if (IsPublic)
				{
					if (Code ~ /(^|[^A-Za-z0-9_])(Vector2|Vector3|Vector4|Rectangle|Texture2D|RenderTexture2D|Camera2D|Camera3D|Matrix|KeyboardKey|MouseButton|GamepadButton|ConfigFlags|TraceLogLevel)([^A-Za-z0-9_]|$)/ ||
						Code ~ /(^|[^A-Za-z0-9_])(Color|Image|Font|Texture|Sound|Music|Shader|Mesh|Model|Camera)[ \t]*[&*]?[ \t]+[A-Za-z_]/ ||
						Code ~ /(^|[^A-Za-z0-9_])(KEY|MOUSE_BUTTON|GAMEPAD_BUTTON|FLAG|LOG)_[A-Z]/)
					{
						Hit("raylib rule 2: raylib-looking type or macro in a Public/ header", N, Raw)
					}
				}
				if (Code ~ /(^|[^A-Za-z0-9_])template</)
				{
					Hit("Templates: write template <, with a space", N, Raw)
				}
				if (Code ~ /std::(unique_ptr|make_unique)([^A-Za-z0-9_]|$)/)
				{
					Hit("Ownership: TUniquePtr and Vertex::MakeUnique, never std::unique_ptr or std::make_unique", N, Raw)
				}
				# A type declared with its body on the same line: "struct VFoo {};".
				if (Trimmed ~ /^(class|struct|union|enum)[ \t][^;{]*\{./)
				{
					Hit("Braces: opening brace not on its own line (Allman)", N, Raw)
				}
				# Preprocessor lines, and their continuations, stay out of the brace tracking.
				if (InPreproc || Trimmed ~ /^#/)
				{
					InPreproc = (Raw ~ /\\[ \t]*$/)
				}
				else
				{
					ScanLayout(Code, Trimmed, N)
				}
			}
			else if (IsDescriptor)
			{
				if (Trimmed ~ /^(project|workspace|filter|defines|links|includedirs|externalincludedirs|files|kind|language|location|targetdir|objdir|configurations|platforms|buildoptions|linkoptions|require|dofile|include|newaction|newoption)[ \t]*[({"'\'']/ || Code ~ /(^|[^A-Za-z0-9_.])(os|io|premake|path)\.[a-z]/)
				{
					Hit("Build: descriptor calls an API (Vertex.lua and *.Module.lua are pure data)", N, Raw)
				}
				if (FirstCode == "" && Trimmed != "")
				{
					FirstCode = Trimmed
					FirstCodeLine = N
				}
			}
		}

		END {
			if (MissingFinalNewline == 1)
			{
				Hit("Whitespace: no newline at end of file", NR, "")
			}
			if (Kind == "cpp")
			{
				if (IsHeader && FirstCode != "#pragma once")
				{
					Hit("File layout: #pragma once must follow the copyright line", FirstCodeLine, FirstCode)
				}
				if (IsCpp && OwnHeader != "" && FirstInclude !~ ("[\"/]" OwnHeader "\"$"))
				{
					Hit("File layout: a .cpp includes its own header (" OwnHeader ") first", FirstIncludeLine, FirstInclude)
				}
				if (HasRaylib && HasWindows)
				{
					Hit("raylib rule 3: raylib.h and windows.h in one translation unit", 1, "")
				}
			}
			if (IsDescriptor && FirstCode !~ /^return([^A-Za-z0-9_]|$)/)
			{
				Hit("Build: a descriptor must just return a table", FirstCodeLine, FirstCode)
			}

			for (I = 1; I <= RuleCount; I++)
			{
				Rule = Order[I]
				printf "  [%d] %s\n%s", Count[Rule], Rule, Lines[Rule]
				Findings += Count[Rule]
			}
			if (Findings > 0)
			{
				printf "@@TOTAL %d\n", Findings
			}
		}
	' "$File")

	if [ -n "$Output" ]; then
		FileTotal=$(printf '%s\n' "$Output" | sed -n 's/^@@TOTAL //p')
		Total=$((Total + ${FileTotal:-0}))
		echo
		echo "$File"
		printf '%s\n' "$Output" | grep -v '^@@TOTAL '
	fi
done

echo
if [ "$Total" -eq 0 ]; then
	echo "StyleScan: no mechanical findings."
else
	echo "StyleScan: $Total candidate finding(s)."
fi
exit 0
