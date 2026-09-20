// Vertex

#include "Core/Types/String.h"

#include <ostream>
#include <stdexcept>
#include "Core/Containers/Hash.h"

namespace VCore
{
const VString VString::EmptyString = VString("");
const VString VString::NullString = VString(nullptr);

VString::VString()
	: Data(nullptr)
{
	Allocate(0);
}

VString::VString(VString&& OtherString) noexcept
{
	Data = OtherString.Data;
	Size = OtherString.Size;
	Capacity = OtherString.Capacity;

	OtherString.Data = nullptr;
	OtherString.Size = 0;
	OtherString.Capacity = 0;
}

VString::VString(const char* String)
	: Data(nullptr)
{
	if (!String)
	{
		Allocate(0);
		return;
	}

	const TSize StringLength = std::strlen(String);
	Allocate(StringLength);

	if (StringLength > 0)
	{
		std::memcpy(Data, String, StringLength);
	}

	Size = StringLength;
	Data[Size] = '\0';
}

VString::VString(const char* String, const TSize Length)
	: Data(nullptr)
{
	if (!String || Length == 0)
	{
		Allocate(0);
		return;
	}

	Allocate(Length);
	std::memcpy(Data, String, Length);
	Size = Length;
	Data[Size] = '\0';
}

VString::VString(const std::string& String)
	: VString(std::string_view(String))
{}
	
VString::VString(const std::string_view String)
: Data(nullptr)
{
	const TSize StringLength = String.size();
	Allocate(StringLength);

	if (StringLength > 0)
	{
		std::memcpy(Data, String.data(), StringLength);
	}

	Size = StringLength;
	Data[Size] = '\0';
}

VString::VString(const VString& String)
	: Data(nullptr)
{
	Allocate(String.Size);

	if (String.Size > 0)
	{
		std::memcpy(Data, String.Data, String.Size);
	}

	Size = String.Size;
	Data[Size] = '\0';
}

VString::~VString()
{
	delete[] Data;
	Data = nullptr;
}

void VString::Reserve(const TSize NewCapacity)
{
	if (NewCapacity <= Capacity) return;
	Reallocate(NewCapacity);
}

void VString::Resize(const TSize NewSize, const char FillCharacter)
{
	if (NewSize == Size) return;
	EnsureCapacity(NewSize);

	if (NewSize > Size)
	{
		std::memset(Data + Size, static_cast<unsigned char>(FillCharacter),	NewSize - Size);
	}

	Size = NewSize;
	Data[Size] = '\0';
}

void VString::Clear() noexcept
{
	Size = 0;

	if (Data)
	{
		Data[0] = '\0';
	}
}

void VString::Empty() noexcept
{
	Clear();
}

void VString::Erase(const TSize Position, const TSize Count)
{
	if (Position > Size)
	{
		throw std::out_of_range("VString::Erase() position out of range");
	}

	if (Count == 0 || Position == Size) return;
	const TSize ActualCount = Count == INDEX_NONE ? Size - Position : std::min(Count, Size - Position);
	
	std::memmove(Data + Position, Data + Position + ActualCount, Size - Position - ActualCount + 1);
	Size -= ActualCount;
}

void VString::Insert(const TSize Position, const char Character)
{
	if (Position > Size)
	{
		throw std::out_of_range("VString::Insert() position out of range");
	}

	EnsureCapacity(Size + 1);
	std::memmove(Data + Position + 1, Data + Position, Size - Position + 1);
	Data[Position] = Character;
	Size++;
}

void VString::Insert(const TSize Position, const char* String)
{
	if (!String) return;
	Insert(Position, std::string_view(String));
}

void VString::Insert(const TSize Position, const std::string& String)
{
	Insert(Position, std::string_view(String));
}

void VString::Insert(const TSize Position, const std::string_view String)
{
	if (Position > Size)
	{
		throw std::out_of_range("VString::Insert() position out of range");
	}

	if (String.empty()) return;
	
	const char* StringBegin = Data;
	const char* StringEnd = Data + Size;

	if (String.data() >= StringBegin && String.data() <= StringEnd)
	{
		const VString Temporary(String);
		Insert(Position, Temporary);
		return;
	}

	const TSize StringLength = String.size();
	EnsureCapacity(Size + StringLength);
	std::memmove(Data + Position + StringLength, Data + Position, Size - Position + 1);
	std::memcpy(Data + Position, String.data(), StringLength);
	Size += StringLength;
}

void VString::Insert(const TSize Position, const VString& String)
{
	if (this == &String)
	{
		const VString& Temporary(String);
		Insert(Position, Temporary);
		return;
	}

	Insert(Position, std::string_view(String.Data, String.Size));
}

void VString::Append(const char Character)
{
	EnsureCapacity(Size + 1);
	Data[Size] = Character;
	Size++;
	Data[Size] = '\0';
}

void VString::Append(const char* String)
{
	if (!String) return;
	Append(std::string_view(String));
}

void VString::Append(const std::string& String)
{
	Append(std::string_view(String));
}

void VString::Append(const std::string_view String)
{
	if (String.empty()) return;

	const char* StringBegin = Data;
	const char* StringEnd = Data + Size;

	if (String.data() >= StringBegin &&String.data() <= StringEnd)
	{
		const VString Temporary(String);
		Append(Temporary);
		return;
	}

	const TSize StringLength = String.size();
	EnsureCapacity(Size + StringLength);
	std::memcpy(Data + Size, String.data(), StringLength);
	Size += StringLength;
	Data[Size] = '\0';
}

void VString::Append(const VString& String)
{
	if (this == &String)
	{
		const TSize OldSize = Size;
		EnsureCapacity(Size + OldSize);
		std::memmove(Data + OldSize, Data, OldSize);

		Size += OldSize;
		Data[Size] = '\0';
		return;
	}

	Append(std::string_view(String.Data, String.Size));
}

TSize VString::Find(const char Character, const TSize StartPosition) const noexcept
{
	if (StartPosition >= Size) return INDEX_NONE;
	const void* Result = std::memchr(Data + StartPosition, static_cast<unsigned char>(Character), Size - StartPosition);

	if (!Result) return INDEX_NONE;
	return static_cast<TSize>(static_cast<const char*>(Result) - Data);
}

TSize VString::Find(const char* String, const TSize StartPosition) const noexcept
{
	if (!String) return INDEX_NONE;
	return Find(std::string_view(String), StartPosition);
}

TSize VString::Find(const std::string& String, const TSize StartPosition) const noexcept
{
	return Find(std::string_view(String), StartPosition);
}

TSize VString::Find(const std::string_view String, const TSize StartPosition) const noexcept
{
	if (StartPosition > Size) return INDEX_NONE;
	if (String.empty()) return StartPosition;

	const std::string_view ThisString(Data, Size);
	const TSize Position = ThisString.find(String, StartPosition);

	if (Position == std::string_view::npos) return INDEX_NONE;
	return Position;
}

TSize VString::Find(const VString& String, const TSize StartPosition) const noexcept
{
	return Find(std::string_view(String.Data, String.Size), StartPosition);
}

TSize VString::FindLast(const char Character) const noexcept
{
	if (Size == 0) return INDEX_NONE;

	for (TSize Index = Size; Index > 0; Index--)
	{
		if (Data[Index - 1] == Character) return Index - 1;
	}

	return INDEX_NONE;
}

TSize VString::FindLast(const std::string_view String) const noexcept
{
	if (String.empty()) return Size;

	const std::string_view ThisString(Data, Size);
	const TSize Position = ThisString.rfind(String);

	if (Position == std::string_view::npos) return INDEX_NONE;
	return Position;
}

VString VString::Substring(const TSize StartPosition, const TSize Count) const
{
	if (StartPosition > Size)
	{
		throw std::out_of_range("VString::Substring() position out of range");
	}

	const TSize AvailableCharacters = Size - StartPosition;
	const TSize ActualCount = Count == INDEX_NONE ? AvailableCharacters : std::min(Count, AvailableCharacters);
	return VString(Data + StartPosition, ActualCount);
}

VString VString::Left(const TSize Count) const
{
	return Substring(0, std::min(Count, Size));
}

VString VString::Right(const TSize Count) const
{
	const TSize ActualCount = std::min(Count, Size);
	return Substring(Size - ActualCount, ActualCount);
}

uint32 VString::GetHash() const noexcept
{
	return HashBytes(Data, Size);
}
	
TSize VString::GetSize() const noexcept
{
	return Size;
}

TSize VString::GetCapacity() const noexcept
{
	return Capacity;
}

char* VString::GetData() noexcept
{
	return Data;
}

const char* VString::GetData() const noexcept
{
	return Data;
}

char* VString::CString() noexcept
{
	return Data;
}

const char* VString::CString() const noexcept
{
	return Data;
}

char& VString::Front()
{
	if (IsEmpty())
	{
		throw std::out_of_range("VString::Front() called on empty string");
	}

	return Data[0];
}

const char& VString::Front() const
{
	if (IsEmpty())
	{
		throw std::out_of_range("VString::Front() called on empty string");
	}

	return Data[0];
}

char& VString::Back()
{
	if (IsEmpty())
	{
		throw std::out_of_range("VString::Back() called on empty string");
	}

	return Data[Size - 1];
}

const char& VString::Back() const
{
	if (IsEmpty())
	{
		throw std::out_of_range("VString::Back() called on empty string");
	}

	return Data[Size - 1];
}

char& VString::At(const TSize Index)
{
	if (!IsValidIndex(Index))
	{
		throw std::out_of_range("VString::At() index out of range");
	}

	return Data[Index];
}

const char& VString::At(const TSize Index) const
{
	if (!IsValidIndex(Index))
	{
		throw std::out_of_range("VString::At() index out of range");
	}

	return Data[Index];
}

bool VString::IsEmpty() const noexcept
{
	return Size == 0;
}

bool VString::IsValidIndex(const TSize Index) const noexcept
{
	return Index < Size;
}

bool VString::StartsWith(const char Prefix) const noexcept
{
	return !IsEmpty() && Data[0] == Prefix;
}

bool VString::StartsWith(const char* Prefix) const noexcept
{
	if (!Prefix) return false;
	return StartsWith(std::string_view(Prefix));
}

bool VString::StartsWith(const std::string& Prefix) const noexcept
{
	return StartsWith(std::string_view(Prefix));
}

bool VString::StartsWith(const std::string_view Prefix) const noexcept
{
	if (Prefix.size() > Size) return false;
	return std::memcmp(Data, Prefix.data(), Prefix.size()) == 0;
}

bool VString::StartsWith(const VString& Prefix) const noexcept
{
	return StartsWith(std::string_view(Prefix.Data, Prefix.Size));
}

bool VString::EndsWith(const char Suffix) const noexcept
{
	return !IsEmpty() && Data[Size - 1] == Suffix;
}

bool VString::EndsWith(const char* Suffix) const noexcept
{
	if (!Suffix) return false;
	return EndsWith(std::string_view(Suffix));
}

bool VString::EndsWith(const std::string& Suffix) const noexcept
{
	return EndsWith(std::string_view(Suffix));
}

bool VString::EndsWith(const std::string_view Suffix) const noexcept
{
	if (Suffix.size() > Size) return false;
	return std::memcmp(Data + Size - Suffix.size(), Suffix.data(), Suffix.size()) == 0;
}

bool VString::EndsWith(const VString& Suffix) const noexcept
{
	return EndsWith(std::string_view(Suffix.Data, Suffix.Size));
}

bool VString::Contains(const char Character) const noexcept
{
	return Find(Character) != INDEX_NONE;
}

bool VString::Contains(const char* String) const noexcept
{
	return Find(String) != INDEX_NONE;
}

bool VString::Contains(const std::string& String) const noexcept
{
	return Find(String) != INDEX_NONE;
}

bool VString::Contains(const std::string_view String) const noexcept
{
	return Find(String) != INDEX_NONE;
}

bool VString::Contains(const VString& String) const noexcept
{
	return Find(String) != INDEX_NONE;
}

VString& VString::operator=(const char* OtherString)
{
	if (!OtherString)
	{
		Clear();
		return *this;
	}

	return *this = std::string_view(OtherString);
}

VString& VString::operator=(const std::string& OtherString)
{
	return *this = std::string_view(OtherString);
}

VString& VString::operator=(const std::string_view OtherString)
{
	const char* OtherBegin = Data;
	const char* OtherEnd = Data + Size;

	if (OtherString.data() >= OtherBegin && OtherString.data() <= OtherEnd)
	{
		const VString Temporary(OtherString);
		return *this = Temporary;
	}

	const TSize StringLength = OtherString.size();
	EnsureCapacity(StringLength);

	if (StringLength > 0)
	{
		std::memcpy(Data, OtherString.data(), StringLength);
	}

	Size = StringLength;
	Data[Size] = '\0';
	return *this;
}

VString& VString::operator=(const VString& OtherString)
{
	if (this == &OtherString) return *this;
	return *this = std::string_view(OtherString.Data, OtherString.Size);
}

VString& VString::operator=(VString&& OtherString) noexcept
{
	if (this == &OtherString) return *this;

	delete[] Data;
	Data = OtherString.Data;
	Size = OtherString.Size;
	Capacity = OtherString.Capacity;

	OtherString.Data = nullptr;
	OtherString.Size = 0;
	OtherString.Capacity = 0;
	return *this;
}

VString& VString::operator+=(const char Character)
{
	Append(Character);
	return *this;
}

VString& VString::operator+=(const char* OtherString)
{
	Append(OtherString);
	return *this;
}

VString& VString::operator+=(const std::string& OtherString)
{
	Append(OtherString);
	return *this;
}

VString& VString::operator+=(const std::string_view OtherString)
{
	Append(OtherString);
	return *this;
}

VString& VString::operator+=(const VString& OtherString)
{
	Append(OtherString);
	return *this;
}

VString operator+(const char* LeftString, const VString& RightString)
{
	VString Result(LeftString);
	Result += RightString;
	return Result;
}

VString operator+(const std::string& LeftString, const VString& RightString)
{
	VString Result(LeftString);
	Result += RightString;
	return Result;
}

VString operator+(const VString& LeftString, const char* RightString)
{
	VString Result(LeftString);
	Result += RightString;
	return Result;
}

VString operator+(const VString& LeftString, const std::string& RightString)
{
	VString Result(LeftString);
	Result += RightString;
	return Result;
}

VString operator+(const VString& LeftString, const VString& RightString)
{
	VString Result(LeftString);
	Result += RightString;
	return Result;
}

bool operator==(const char* LeftString, const VString& RightString) noexcept
{
	if (!LeftString) return RightString.IsEmpty();
	return std::string_view(LeftString) == std::string_view(RightString);
}

bool operator==(const std::string& LeftString, const VString& RightString) noexcept
{
	return std::string_view(LeftString) == std::string_view(RightString);
}

bool operator==(const VString& LeftString, const char* RightString) noexcept
{
	return RightString == LeftString;
}

bool operator==(const VString& LeftString, const std::string& RightString) noexcept
{
	return std::string_view(LeftString) == std::string_view(RightString);
}

bool operator==(const VString& LeftString, const VString& RightString) noexcept
{
	if (LeftString.Size != RightString.Size) return false;
	if (LeftString.Size == 0) return true;
	return std::memcmp(LeftString.Data, RightString.Data, LeftString.Size) == 0;
}

bool operator!=(const char* LeftString, const VString& RightString) noexcept
{
	return !(LeftString == RightString);
}

bool operator!=(const std::string& LeftString, const VString& RightString) noexcept
{
	return !(LeftString == RightString);
}

bool operator!=(const VString& LeftString, const char* RightString) noexcept
{
	return !(LeftString == RightString);
}

bool operator!=(const VString& LeftString, const std::string& RightString) noexcept
{
	return !(LeftString == RightString);
}

bool operator!=(const VString& LeftString, const VString& RightString) noexcept
{
	return !(LeftString == RightString);
}

bool operator<(const char* LeftString, const VString& RightString) noexcept
{
	if (!LeftString) return std::string_view() < std::string_view(RightString);
	return std::string_view(LeftString) < std::string_view(RightString);
}

bool operator<(const std::string& LeftString, const VString& RightString) noexcept
{
	return LeftString.compare(std::string_view(RightString)) < 0;
}

bool operator<(const VString& LeftString, const char* RightString) noexcept
{
	if (!RightString) return std::string_view(LeftString) < std::string_view();
	return std::string_view(LeftString) < std::string_view(RightString);
}

bool operator<(const VString& LeftString, const std::string& RightString) noexcept
{
	return std::string_view(LeftString).compare(RightString) < 0;
}

bool operator<(const VString& LeftString, const VString& RightString) noexcept
{
	return std::string_view(LeftString) < std::string_view(RightString);
}

bool operator>(const char* LeftString, const VString& RightString) noexcept
{
	return RightString < LeftString;
}

bool operator>(const std::string& LeftString, const VString& RightString) noexcept
{
	return RightString < LeftString;
}

bool operator>(const VString& LeftString, const char* RightString) noexcept
{
	return RightString < LeftString;
}

bool operator>(const VString& LeftString, const std::string& RightString) noexcept
{
	return RightString < LeftString;
}

bool operator>(const VString& LeftString, const VString& RightString) noexcept
{
	return RightString < LeftString;
}

bool operator<=(const char* LeftString, const VString& RightString) noexcept
{
	return RightString >= LeftString;
}

bool operator<=(const std::string& LeftString, const VString& RightString) noexcept
{
	return RightString >= LeftString;
}

bool operator<=(const VString& LeftString, const char* RightString) noexcept
{
	return RightString >= LeftString;
}

bool operator<=(const VString& LeftString, const std::string& RightString) noexcept
{
	return RightString >= LeftString;
}

bool operator<=(const VString& LeftString, const VString& RightString) noexcept
{
	return RightString >= LeftString;
}

bool operator>=(const char* LeftString, const VString& RightString) noexcept
{
	return !(LeftString < RightString);
}

bool operator>=(const std::string& LeftString, const VString& RightString) noexcept
{
	return !(LeftString < RightString);
}

bool operator>=(const VString& LeftString, const char* RightString) noexcept
{
	return !(LeftString < RightString);
}

bool operator>=(const VString& LeftString, const std::string& RightString) noexcept
{
	return !(LeftString < RightString);
}

bool operator>=(const VString& LeftString, const VString& RightString) noexcept
{
	return !(LeftString < RightString);
}

char& VString::operator[](const TSize Index)
{
	return Data[Index];
}

const char& VString::operator[](const TSize Index) const
{
	return Data[Index];
}

VString::operator const char*() const noexcept
{
	return Data;
}

VString::operator std::string_view() const noexcept
{
	return std::string_view(Data, Size);
}

VString::operator std::string() const
{
	return std::string(Data, Size);
}

std::ostream& operator<<(std::ostream& Stream, const VString& String)
{
	Stream.write(String.Data, static_cast<std::streamsize>(String.Size));
	return Stream;
}

void VString::Allocate(const TSize NewCapacity)
{
	Data = new char[static_cast<std::size_t>(NewCapacity) + 1];
	Capacity = NewCapacity;
	Size = 0;
	Data[0] = '\0';
}

void VString::Reallocate(const TSize NewCapacity)
{
	char* NewData = new char[static_cast<std::size_t>(NewCapacity) + 1];
	const TSize CharactersToCopy = std::min(Size, NewCapacity);

	if (CharactersToCopy > 0)
	{
		std::memcpy(NewData, Data, CharactersToCopy);
	}

	NewData[CharactersToCopy] = '\0';
	delete[] Data;
	Data = NewData;
	Capacity = NewCapacity;
	Size = CharactersToCopy;
}

void VString::EnsureCapacity(const TSize RequiredCapacity)
{
	if (RequiredCapacity <= Capacity) return;
	constexpr TSize InitialCapacity = 8;

	const TSize DoubledCapacity = Capacity == 0	? InitialCapacity : Capacity * 2;
	const TSize NewCapacity = std::max(DoubledCapacity, RequiredCapacity);
	Reallocate(NewCapacity);
}
}
