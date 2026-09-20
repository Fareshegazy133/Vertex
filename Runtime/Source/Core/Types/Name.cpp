// Vertex

#include <utility>

#include "Core/Types/Name.h"

namespace VCore
{
VName::VName(const char Character)
{
	const char Buffer[1] = { Character };
	*this = MakeName(std::string_view(Buffer, 1));
}

VName::VName(const char* String)
{
	if (!String)
	{
		Index = INDEX_NONE;
		HashValue = 0;
		return;
	}

	*this = MakeName(std::string_view(String));
}

VName::VName(const std::string& String)
{
	*this = MakeName(std::string_view(String));
}

VName::VName(const std::string_view String)
{
	*this = MakeName(String);
}

VName::VName(const VString& String)
{
	*this = MakeName(std::string_view(String));
}

const char* VName::ToCString() const
{
	return ToString().CString();
}

VString VName::ToString() const
{
	return GetNameRegistry().GetString(Index);
}

uint32 VName::GetIndex() const noexcept
{
	return Index;
}

uint32 VName::GetHash() const noexcept
{
	return HashValue;
}

bool VName::IsValid() const noexcept
{
	return Index != INDEX_NONE;
}

bool VName::IsNone() const noexcept
{
	return Index == INDEX_NONE;
}

bool VName::Equals(const char Character) const noexcept
{
	const char Buffer[1] = { Character };
	return Equals(std::string_view(Buffer, 1));
}

bool VName::Equals(const char* String) const noexcept
{
	if (!String) return IsNone();
	return Equals(std::string_view(String));
}

bool VName::Equals(const std::string& String) const noexcept
{
	return Equals(std::string_view(String));
}

bool VName::Equals(const std::string_view String) const noexcept
{
	if (String.empty()) return IsNone();
	return HashValue == VString(String).GetHash() && std::string_view(ToString()) == String;
}

bool VName::Equals(const VString& String) const noexcept
{
	return Equals(std::string_view(String));
}

bool VName::Equals(const VName& Name) const noexcept
{
	return Index == Name.Index;
}

VName& VName::operator=(const char Character)
{
	const char Buffer[1] = { Character };
	*this = MakeName(std::string_view(Buffer, 1));
	return *this;
}

VName& VName::operator=(const char* String)
{
	if (!String)
	{
		Index = INDEX_NONE;
		HashValue = 0;
		return *this;
	}

	*this = MakeName(std::string_view(String));
	return *this;
}

VName& VName::operator=(const std::string& String)
{
	*this = MakeName(std::string_view(String));
	return *this;
}

VName& VName::operator=(const std::string_view String)
{
	*this = MakeName(String);
	return *this;
}

VName& VName::operator=(const VString& String)
{
	*this = MakeName(std::string_view(String));
	return *this;
}

bool operator==(const char LeftCharacter, const VName& RightName) noexcept
{
	return RightName.Equals(LeftCharacter);
}

bool operator==(const char* LeftString, const VName& RightName) noexcept
{
	return RightName.Equals(LeftString);
}

bool operator==(const std::string& LeftString, const VName& RightName) noexcept
{
	return RightName.Equals(LeftString);
}

bool operator==(const std::string_view LeftString, const VName& RightName) noexcept
{
	return RightName.Equals(LeftString);
}

bool operator==(const VString& LeftString, const VName& RightName) noexcept
{
	return RightName.Equals(LeftString);
}

bool operator==(const VName& LeftName, const char RightCharacter) noexcept
{
	return LeftName.Equals(RightCharacter);
}

bool operator==(const VName& LeftName, const char* RightString) noexcept
{
	return LeftName.Equals(RightString);
}

bool operator==(const VName& LeftName, const std::string& RightString) noexcept
{
	return LeftName.Equals(RightString);
}

bool operator==(const VName& LeftName, const std::string_view RightString) noexcept
{
	return LeftName.Equals(RightString);
}

bool operator==(const VName& LeftName, const VString& RightString) noexcept
{
	return LeftName.Equals(RightString);
}

bool operator==(const VName& LeftName, const VName& RightName) noexcept
{
	return LeftName.Index == RightName.Index;
}

bool operator!=(const char LeftCharacter, const VName& RightName) noexcept
{
	return !(LeftCharacter == RightName);
}

bool operator!=(const char* LeftString, const VName& RightName) noexcept
{
	return !(LeftString == RightName);
}

bool operator!=(const std::string& LeftString, const VName& RightName) noexcept
{
	return !(LeftString == RightName);
}

bool operator!=(const std::string_view LeftString, const VName& Right) noexcept
{
	return !(LeftString == Right);
}

bool operator!=(const VString& LeftString, const VName& RightName) noexcept
{
	return !(LeftString == RightName);
}

bool operator!=(const VName& LeftName, const char RightCharacter) noexcept
{
	return !(LeftName == RightCharacter);
}

bool operator!=(const VName& LeftName, const char* RightString) noexcept
{
	return !(LeftName == RightString);
}

bool operator!=(const VName& LeftName, const std::string& RightString) noexcept
{
	return !(LeftName == RightString);
}

bool operator!=(const VName& LeftName, const std::string_view RightString) noexcept
{
	return !(LeftName == RightString);
}

bool operator!=(const VName& LeftName, const VString& RightString) noexcept
{
	return !(LeftName == RightString);
}

bool operator!=(const VName& LeftName, const VName& RightName) noexcept
{
	return !(LeftName == RightName);
}

bool operator<(const VName& LeftName, const VName& RightName) noexcept
{
	return LeftName.Index < RightName.Index;
}

bool operator>(const VName& LeftName, const VName& RightName) noexcept
{
	return RightName < LeftName;
}

bool operator<=(const VName& LeftName, const VName& RightName) noexcept
{
	return RightName >= LeftName;
}

bool operator>=(const VName& LeftName, const VName& RightName) noexcept
{
	return !(LeftName < RightName);
}

VName::VName(const uint32 NameIndex, const uint32 NameHash) noexcept
	: Index(NameIndex), HashValue(NameHash)
{}

VNameRegistry::VNameRegistry()
{
	Names.Emplace(VString());
	Hashes.Emplace(0);
}

uint32 VNameRegistry::FindOrAdd(const std::string_view Name, uint32& OutHash)
{
	if (Name.empty())
	{
		OutHash = 0;
		return INDEX_NONE;
	}

	const VString Key(Name);
	std::scoped_lock Lock(Mutex);

	if (const uint32* FoundIndex = NameToIndex.Find(Key))
	{
		const uint32 Index = *FoundIndex;
		OutHash = Hashes[Index];
		return Index;
	}

	const uint32 NewIndex = static_cast<uint32>(Names.Num());
	VString NewName(Name);
	const uint32 Hash = NewName.GetHash();

	Names.Emplace(std::move(NewName));
	Hashes.Emplace(Hash);
	NameToIndex.Emplace(Key, NewIndex);
	OutHash = Hash;
	return NewIndex;
}

const VString& VNameRegistry::GetString(const uint32 Index) const
{
	std::scoped_lock Lock(Mutex);
	if (std::cmp_greater_equal(Index, Names.Num())) return Names[0];
	return Names[Index];
}

VNameRegistry& GetNameRegistry()
{
	static VNameRegistry Registry;
	return Registry;
}

VName MakeName(const std::string_view Name)
{
	uint32 Hash = 0;
	const uint32 Index = GetNameRegistry().FindOrAdd(Name, Hash);
	return VName(Index, Hash);
}
}