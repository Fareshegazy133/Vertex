// Vertex

#include "Core/Types/String.h"

#include <algorithm>
#include <ostream>
#include <stdexcept>

namespace VCore
{
const VString VString::EmptyString = VString("");
const VString VString::NullString = VString(nullptr);

VString::VString()
{
	Allocate(0);
}

VString::VString(const char* OtherString)
{
	if (!OtherString)
	{ 
		Allocate(0);
		return; 
	}
	
	const uint32 StringLength = static_cast<uint32>(std::strlen(OtherString));
	Allocate(StringLength);
	
	if (StringLength > 0)
	{
		std::memcpy(Data, OtherString, StringLength);
	}
	
	Size = StringLength;
	Data[Size] = '\0';
}

VString::VString(const std::string& OtherString)
{
	const uint32 StringLength = static_cast<uint32>(OtherString.size());
	Allocate(StringLength);
	
	if (StringLength > 0)
	{
		std::memcpy(Data, OtherString.data(), StringLength);
	}
	
	Size = StringLength;
	Data[Size] = '\0';
}

VString::VString(const VString& OtherString)
{
	Allocate(OtherString.Size);
	
	if (OtherString.Size > 0)
	{
		std::memcpy( Data, OtherString.Data, OtherString.Size );
	}
	
	Size = OtherString.Size;
	Data[Size] = '\0';
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

VString::~VString()
{
	delete[] Data;
}

void VString::Reserve(const uint32 NewCapacity)
{
	if (NewCapacity <= Capacity) return;
	Reallocate(NewCapacity);
}

void VString::Resize(const uint32 NewSize, const char FillCharacter)
{
	if (NewSize == Size) return;
	EnsureCapacity(NewSize);
	
	if (NewSize > Size)
	{
		std::memset( Data + Size, FillCharacter, NewSize - Size );
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

void VString::Erase(const uint32 Position, const uint32 Count)
{
	if (Position > Size)
	{
		throw std::out_of_range("VString::Erase() position out of range");
	}
	
	if (Count == 0 || Position == Size) return;
	const uint32 ActualCount = std::min(Count, Size - Position);
	
	std::memmove(Data + Position, Data + Position + ActualCount, Size - Position - ActualCount + 1);
	Size -= ActualCount;
}

void VString::Insert(const uint32 Position, const char* String)
{
	if (Position > Size)
	{
		throw std::out_of_range("VString::Insert() position out of range");
	}
	
	if (!String) return;
	
	const uint32 StringLength = static_cast<uint32>(std::strlen(String));
	if (StringLength == 0) return;
	
	EnsureCapacity(Size + StringLength);
	std::memmove(Data + Position + StringLength, Data + Position, Size - Position + 1);
	std::memcpy(Data + Position, String, StringLength);
	Size += StringLength;
}

void VString::Insert(const uint32 Position, const std::string& String)
{
	Insert(Position, String.c_str());
}

void VString::Insert(const uint32 Position, const VString& String)
{
	if (Position > Size)
	{
		throw std::out_of_range("VString::Insert() position out of range");
	}
	
	if (String.Size == 0) return;
	
	if (this == &String)
	{
		const VString& Copy(String);
		Insert(Position, Copy.Data);
		return;
	}
	
	EnsureCapacity(Size + String.Size);
	std::memmove(Data + Position + String.Size, Data + Position, Size - Position + 1);
	std::memcpy(Data + Position, String.Data, String.Size);
	Size += String.Size;
}

void VString::Insert(const uint32 Position, const char Character)
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

void VString::Append(const char* String)
{
	if (!String) return;
	
	const uint32 StringLength = static_cast<uint32>(std::strlen(String));
	if (StringLength == 0) return;
	
	if (String >= Data && String <= Data + Size)
	{
		const VString Copy(String);
		Append(Copy);
		return;
	}
	
	EnsureCapacity(Size + StringLength);
	std::memcpy(Data + Size, String, StringLength);
	Size += StringLength;
	Data[Size] = '\0';
}

void VString::Append(const std::string& String)
{
	Append(String.c_str());
}

void VString::Append(const VString& String)
{
	if (String.Size == 0) return;
	
	if (this == &String)
	{
		const uint32 OldSize = Size;
		
		EnsureCapacity(Size * 2);
		std::memmove(Data + OldSize, Data, OldSize);
		
		Size = OldSize * 2;
		Data[Size] = '\0'; 
		return;
	}
	
	EnsureCapacity(Size + String.Size);
	std::memcpy(Data + Size, String.Data, String.Size);
	Size += String.Size;
	Data[Size] = '\0';
}

void VString::Append(const char Character)
{
	EnsureCapacity(Size + 1);
	Data[Size] = Character;
	Size++;
	Data[Size] = '\0';
}

uint32 VString::Find(const char* String, const uint32 StartPosition) const noexcept
{
	if (!String) return static_cast<uint32>(-1);
	if (StartPosition > Size) return static_cast<uint32>(-1);
	
	const char* Result = std::strstr(Data + StartPosition, String);
	if (!Result) return static_cast<uint32>(-1);
	
	return static_cast<uint32>(Result - Data);
}

uint32 VString::Find(const std::string& String, const uint32 StartPosition) const noexcept
{
	return Find(String.c_str(), StartPosition);
}

uint32 VString::Find(const VString& String, const uint32 StartPosition) const noexcept
{
	return Find(String.Data, StartPosition);
}

uint32 VString::Find(const char Character, const uint32 StartPosition) const noexcept
{
	if (StartPosition >= Size) return static_cast<uint32>(-1);
	const char* Result = std::strchr(Data + StartPosition, Character);
	
	if (!Result) return static_cast<uint32>(-1);
	return static_cast<uint32>(Result - Data);
}

VString VString::Substring(const uint32 StartPosition, const uint32 Count) const
{
	if (StartPosition > Size)
	{
		throw std::out_of_range("VString::Substring() position out of range");
	}
	
	const uint32 AvailableCharacters = Size - StartPosition;
	const uint32 ActualCount = std::min(Count, AvailableCharacters);
	
	VString Result;
	Result.EnsureCapacity(ActualCount);
	
	if (ActualCount > 0)
	{
		std::memcpy(Result.Data, Data + StartPosition, ActualCount);
	}
	
	Result.Size = ActualCount;
	Result.Data[Result.Size] = '\0';
	return Result;
}

uint32 VString::GetSize() const noexcept
{
	return Size;
}

uint32 VString::GetCapacity() const noexcept
{
	return Capacity;
}

const char* VString::GetData() const noexcept
{
	return Data;
}

char* VString::GetData() noexcept
{
	return Data;
}

const char* VString::CString() const noexcept
{
	return Data;
}

char* VString::CString() noexcept
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

char& VString::At(const uint32 Index)
{
	if (Index >= Size)
	{
		throw std::out_of_range("VString::At() index out of range");
	}
	
	return Data[Index];
}

const char& VString::At(const uint32 Index) const
{
	if (Index >= Size)
	{
		throw std::out_of_range("VString::At() index out of range");
	}
	
	return Data[Index];
}

bool VString::IsEmpty() const noexcept
{
	return Size == 0;
}

bool VString::Contains(const char* String) const noexcept
{
	return Find(String) != static_cast<uint32>(-1);
}

bool VString::Contains(const std::string& String) const noexcept
{
	return Find(String) != static_cast<uint32>(-1);
}

bool VString::Contains(const VString& String) const noexcept
{
	return Find(String) != static_cast<uint32>(-1);
}

bool VString::Contains(const char Character) const noexcept
{
	return Find(Character) != static_cast<uint32>(-1);
}

VString& VString::operator=(const char* OtherString)
{
	if (!OtherString)
	{
		Clear();
		return *this;
	}
	
	if (OtherString >= Data && OtherString <= Data + Size)
	{
		const VString Copy(OtherString);
		return *this = Copy;
	}
	
	const uint32 StringLength = static_cast<uint32>(std::strlen(OtherString));
	EnsureCapacity(StringLength);
	
	if (StringLength > 0)
	{
		std::memcpy( Data, OtherString, StringLength );
	}
	
	Size = StringLength;
	Data[Size] = '\0';
	return *this;
}

VString& VString::operator=(const std::string& OtherString)
{
	const uint32 StringLength = static_cast<uint32>(OtherString.size());
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
	EnsureCapacity(OtherString.Size);
	
	if (OtherString.Size > 0)
	{
		std::memcpy(Data, OtherString.Data, OtherString.Size);
	}
	
	Size = OtherString.Size;
	Data[Size] = '\0';
	return *this;
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

VString& VString::operator+=(const VString& OtherString)
{
	Append(OtherString);
	return *this;
}

VString& VString::operator+=(const char Character)
{
	Append(Character);
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
	if (!LeftString ) return RightString.IsEmpty();
	return std::strcmp(LeftString, RightString.Data) == 0;
}

bool operator==(const std::string& LeftString, const VString& RightString) noexcept
{
	const bool bSameSize = LeftString.size() == RightString.Size;
	return bSameSize && std::memcmp(LeftString.data(), RightString.Data, RightString.Size) == 0;
}

bool operator==(const VString& LeftString, const char* RightString) noexcept
{
	return RightString == LeftString;
}

bool operator==(const VString& LeftString, const std::string& RightString) noexcept
{
	return RightString == LeftString;
}

bool operator==(const VString& LeftString, const VString& RightString) noexcept
{
	if (LeftString.Size != RightString.Size) return false;
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

bool operator!=(const VString& LeftString, const char*& RightString) noexcept
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
	if (!LeftString) return true;
	return std::strcmp(LeftString, RightString.GetData()) < 0;
}

bool operator<(const std::string& LeftString, const VString& RightString) noexcept
{
	const uint32 CommonSize = static_cast<uint32>(std::min(LeftString.size(), static_cast<std::size_t>(RightString.Size)));
	const int32 Result = std::memcmp(LeftString.data(), RightString.Data, CommonSize);
	return Result != 0 ? Result < 0 : LeftString.size() < RightString.Size;
}

bool operator<(const VString& LeftString, const char* RightString) noexcept
{
	if (!RightString) return false;
	return std::strcmp(LeftString.Data, RightString) < 0;
}

bool operator<(const VString& LeftString, const std::string& RightString) noexcept
{
	return std::string_view(LeftString.Data, LeftString.Size) < std::string_view(RightString.data(), RightString.size());
}

bool operator<(const VString& LeftString, const VString& RightString) noexcept
{
	const uint32 CommonSize = std::min(LeftString.Size, RightString.Size);
	const int32 Result = std::memcmp(LeftString.Data, RightString.Data, CommonSize);
	return Result != 0 ? Result < 0 : LeftString.Size < RightString.Size;
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

char& VString::operator[](const uint32 Index)
{
	return Data[Index];
}

const char& VString::operator[](const uint32 Index) const
{
	return Data[Index];
}

VString::operator const char*() const noexcept
{
	return Data;
}

VString::operator std::string() const
{
	return std::string(Data, Size);
}

std::ostream& operator<<(std::ostream& Stream, const VString& String)
{
	Stream.write(String.Data, String.Size);
	return Stream;
}

void VString::Allocate(const uint32 NewCapacity)
{
	Data = new char[static_cast<std::size_t>(NewCapacity) + 1];
	Capacity = NewCapacity;
	Size = 0;
	Data[0] = '\0';
}

void VString::Reallocate(const uint32 NewCapacity)
{
	char* NewData = new char[static_cast<std::size_t>(NewCapacity) + 1];
	const uint32 CharactersToCopy = std::min(Size, NewCapacity);
	
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

void VString::EnsureCapacity(const uint32 RequiredCapacity)
{
	if (RequiredCapacity <= Capacity) return;
	Reallocate(std::max(Capacity == 0 ? 8 : Capacity * 2, RequiredCapacity));
}
}