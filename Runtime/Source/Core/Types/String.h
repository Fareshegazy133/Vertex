// Vertex

#pragma once

#include <string>
#include "Core/Types/Numeric.h"

namespace VCore
{
class VString
{
public:
	VString();
	VString(VString&& OtherString) noexcept;
	VString(const char* String);
	VString(const char* String, const TSize Length);
	VString(const std::string& String);
	VString(const std::string_view String);
	VString(const VString& String);
	~VString();
	
	static VString FromInt8(const int8 Value);
	static VString FromUInt8(const uint8 Value);
	
	static VString FromInt16(const int16 Value);
	static VString FromUInt16(const uint16 Value);
	
	static VString FromInt32(const int32 Value);
	static VString FromUInt32(const uint32 Value);
	
	static VString FromInt64(const int64 Value);
	static VString FromUInt64(const uint64 Value);
	
	static VString FromFloat32(const float32 Value);
	static VString FromFloat64(const float64 Value);
	
	static VString FromChar(const char Character);
	static VString FromBool(const bool Value);
	
	void Reserve(const TSize NewCapacity);
	void Resize(const TSize NewSize, const char FillCharacter = '\0');
	
	void Clear() noexcept;
	void Empty() noexcept;
	void Erase(const TSize Position, const TSize Count = INDEX_NONE);
	
	void Insert(const TSize Position, const char Character);
	void Insert(const TSize Position, const char* String);
	void Insert(const TSize Position, const std::string& String);
	void Insert(const TSize Position, const std::string_view String);
	void Insert(const TSize Position, const VString& String);
	
	void Append(const char Character);
	void Append(const char* String);
	void Append(const std::string& String);
	void Append(const std::string_view String);
	void Append(const VString& String);
	
	TSize Find(const char Character, const TSize StartPosition = 0) const noexcept;
	TSize Find(const char* String, const TSize StartPosition = 0) const noexcept;
	TSize Find(const std::string& String, const TSize StartPosition = 0) const noexcept;
	TSize Find(const std::string_view String, const TSize StartPosition = 0) const noexcept;
	TSize Find(const VString& String, const TSize StartPosition = 0) const noexcept;
	
	TSize FindLast(const char Character) const noexcept;
	TSize FindLast(const std::string_view String) const noexcept;
	
	VString Substring(const TSize StartPosition, const TSize Count = INDEX_NONE) const;
	
	VString Left(const TSize Count) const;
	VString Right(const TSize Count) const;

	uint32 GetHash() const noexcept;
	TSize GetSize() const noexcept;
	TSize GetCapacity() const noexcept;
	
	char* GetData() noexcept;
	const char* GetData() const noexcept;
	
	char* CString() noexcept;
	const char* CString() const noexcept;
	
	char& Front();
	const char& Front() const;
	
	char& Back();
	const char& Back() const;
	
	char& At(const TSize Index);
	const char& At(const TSize Index) const;
	
	bool IsEmpty() const noexcept;
	bool IsValidIndex(const TSize Index) const noexcept;
	
	bool StartsWith(const char Prefix) const noexcept;
	bool StartsWith(const char* Prefix) const noexcept;
	bool StartsWith(const std::string& Prefix) const noexcept;
	bool StartsWith(const std::string_view Prefix) const noexcept;
	bool StartsWith(const VString& Prefix) const noexcept;
	
	bool EndsWith(const char Suffix) const noexcept;
	bool EndsWith(const char* Suffix) const noexcept;
	bool EndsWith(const std::string& Suffix) const noexcept;
	bool EndsWith(const std::string_view Suffix) const noexcept;
	bool EndsWith(const VString& Suffix) const noexcept;
	
	bool Contains(const char Character) const noexcept;
	bool Contains(const char* String) const noexcept;
	bool Contains(const std::string& String) const noexcept;
	bool Contains(const std::string_view String) const noexcept;
	bool Contains(const VString& String) const noexcept;
	
	VString& operator=(const char* OtherString);
	VString& operator=(const std::string& OtherString);
	VString& operator=(const std::string_view OtherString);
	VString& operator=(const VString& OtherString);
	VString& operator=(VString&& OtherString) noexcept;
	
	VString& operator+=(const char Character);
	VString& operator+=(const char* OtherString);
	VString& operator+=(const std::string& OtherString);
	VString& operator+=(const std::string_view OtherString);
	VString& operator+=(const VString& OtherString);
	
	friend VString operator+(const char* LeftString, const VString& RightString);
	friend VString operator+(const std::string& LeftString, const VString& RightString);
	friend VString operator+(const VString& LeftString, const char* RightString);
	friend VString operator+(const VString& LeftString, const std::string& RightString);
	friend VString operator+(const VString& LeftString, const VString& RightString);
	
	friend bool operator==(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator==(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator==(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator==(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator==(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator!=(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator!=(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator!=(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator!=(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator!=(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator<(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator<(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator<(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator<(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator<(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator>(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator>(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator>(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator>(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator>(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator<=(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator<=(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator<=(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator<=(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator<=(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator>=(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator>=(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator>=(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator>=(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator>=(const VString& LeftString, const VString& RightString) noexcept;
	
	char& operator[](const TSize Index);
	const char& operator[](const TSize Index) const;
	
	operator const char*() const noexcept;
	operator std::string_view() const noexcept;
	operator std::string() const;
	
	friend std::ostream& operator<<(std::ostream& Stream, const VString& String);

public:
	static const VString EmptyString;
	static const VString NullString;
	
private:
	void Allocate(const TSize NewCapacity);
	void Reallocate(const TSize NewCapacity);
	void EnsureCapacity(const TSize RequiredCapacity);

private:
	char* Data;
	TSize Size = 0;
	TSize Capacity = 0;
};

class VStringView
{
public:
	constexpr VStringView() noexcept = default;
	VStringView(const char* String) noexcept;
	constexpr VStringView(const char* String, const TSize InSize) noexcept;
	VStringView(const std::string& String) noexcept;
	VStringView(const std::string_view String) noexcept;
	VStringView(const VString& String) noexcept;

	const char& At(const uint32 Index) const;
	const char& Front() const;
	const char& Back() const;
	
	VStringView Substring(const TSize StartPosition, TSize Count = INDEX_NONE) const noexcept;

    VStringView Left(TSize Count) const noexcept;
    VStringView Right(TSize Count) const noexcept;
	
	int32 Compare(const VStringView& OtherStringView) const noexcept;
	uint32 Hash() const noexcept;

    TSize Find(const char Character, const TSize StartPosition = 0) const noexcept;
    TSize Find(const char* String, const TSize StartPosition = 0) const noexcept;
    TSize Find(const std::string_view String, const TSize StartPosition = 0) const noexcept;
    TSize Find(const VStringView& String, const TSize StartPosition = 0) const noexcept;
	
    TSize FindLast(const char Character) const noexcept;
    TSize FindLast(const char* String) const noexcept;
    TSize FindLast(const std::string_view String) const noexcept;
    TSize FindLast(const VStringView& String) const noexcept;
	
	constexpr const char* GetData() const noexcept;
	constexpr TSize GetSize() const noexcept;
	
	bool Contains(const char Character) const noexcept;
	bool Contains(const char* String) const noexcept;
	bool Contains(const std::string_view String) const noexcept;
	bool Contains(const VStringView& String) const noexcept;

	bool StartsWith(const char Character) const noexcept;
	bool StartsWith(const char* Prefix) const noexcept;
	bool StartsWith(const std::string_view Prefix) const noexcept;
	bool StartsWith(const VStringView& Prefix) const noexcept;

	bool EndsWith(const char Character) const noexcept;
	bool EndsWith(const char* Suffix) const noexcept;
	bool EndsWith(const std::string_view Suffix) const noexcept;
	bool EndsWith(const VStringView& Suffix) const noexcept;

	constexpr bool IsEmpty() const noexcept;
	constexpr bool IsValidIndex(const uint32 Index) const noexcept;

	friend bool operator==(const char* LeftString, const VStringView& RightStringView) noexcept;
	friend bool operator==(const std::string_view LeftStringView, const VStringView& RightStringView) noexcept;
	friend bool operator==(const VStringView& LeftStringView, const char* RightString) noexcept;
	friend bool operator==(const VStringView& LeftStringView, const std::string_view RightStringView) noexcept;
    friend bool operator==(const VStringView& LeftStringView, const VStringView& RightStringView) noexcept;
	
	friend bool operator!=(const char* LeftString, const VStringView& RightStringView) noexcept;
	friend bool operator!=(const std::string_view LeftStringView, const VStringView& RightStringView) noexcept;
	friend bool operator!=(const VStringView& LeftStringView, const char* RightString) noexcept;
	friend bool operator!=(const VStringView& LeftStringView, const std::string_view RightStringView) noexcept;
	friend bool operator!=(const VStringView& LeftStringView, const VStringView& RightStringView) noexcept;
	
	const char& operator[](const uint32 Index) const noexcept;
	operator std::string_view() const noexcept;
	
private:
	const char* Data;
	TSize Size = 0;
};

VStringView MakeView(VString String);
VStringView MakeView(const VString& String);
}