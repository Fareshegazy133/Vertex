#!/usr/bin/env bash
# StyleScan: mechanical style checks for Vertex, used by the /review skill.
#
# Checks what a model reading a diff can't see reliably (tabs vs spaces, BOMs,
# trailing whitespace, brace placement) plus the regex-checkable rules from
# CLAUDE.md "Code style", "The raylib rule", and "Build system".
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

		# awk needs a pattern and its opening brace on one line.
		BEGIN {
			IsHeader = (File ~ /\.(h|hpp)$/)
			IsCpp = (File ~ /\.cpp$/)
			IsImpl = (File ~ /\.(cpp|inl)$/)
			IsPublic = (File ~ /\/Public\//)
			IsRaylibBackend = (File ~ /^Source\/Runtime\/Private\/Platform\/Raylib\//)
			IsLogBackend = (File ~ /\/Private\/Logging\//)
			IsDescriptor = (File ~ /(\.Module\.lua|^Vertex\.lua)$/)
			BlockComment = 0
			FirstCode = ""
			FirstInclude = ""
			PrevLine = ""
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
					if (Name !~ /^V[A-Z0-9_]*$/ || Name ~ /_API$/)
					{
						Hit("Naming: macro must be V-prefixed UPPER_SNAKE; <MODULE>_API comes from the build", N, Raw)
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
				if (IsPublic)
				{
					if (Code ~ /(^|[^A-Za-z0-9_])(Vector2|Vector3|Vector4|Rectangle|Texture2D|RenderTexture2D|Camera2D|Camera3D|Matrix|KeyboardKey|MouseButton|GamepadButton|ConfigFlags|TraceLogLevel)([^A-Za-z0-9_]|$)/ ||
						Code ~ /(^|[^A-Za-z0-9_])(Color|Image|Font|Texture|Sound|Music|Shader|Mesh|Model|Camera)[ \t]*[&*]?[ \t]+[A-Za-z_]/ ||
						Code ~ /(^|[^A-Za-z0-9_])(KEY|MOUSE_BUTTON|GAMEPAD_BUTTON|FLAG|LOG)_[A-Z]/)
					{
						Hit("raylib rule 2: raylib-looking type or macro in a Public/ header", N, Raw)
					}
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
