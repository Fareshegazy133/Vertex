// Vertex

#pragma once

#include <string>
#include "Numeric.h"

namespace VCore
{
class VString
{
public:
	VString();
	VString(const char* OtherString);
	VString(const std::string& OtherString);
	VString(const VString& OtherString);
	VString(VString&& OtherString) noexcept;
	
	~VString();
	
	void Reserve(const uint32 NewCapacity);
	void Resize(const uint32 NewSize, const char FillCharacter = '\0');
	
	void Clear() noexcept;
	void Erase(const uint32 Position, const uint32 Count);
	
	void Insert(const uint32 Position, const char* String);
	void Insert(const uint32 Position, const std::string& String);
	void Insert(const uint32 Position, const VString& String);
	void Insert(const uint32 Position, const char Character);
	
	void Append(const char* String);
	void Append(const std::string& String);
	void Append(const VString& String);
	void Append(const char Character);
	
	uint32 Find(const char* String, const uint32 StartPosition = 0) const noexcept;
	uint32 Find(const std::string& String, const uint32 StartPosition = 0) const noexcept;
	uint32 Find(const VString& String, const uint32 StartPosition = 0) const noexcept;
	uint32 Find(const char Character, const uint32 StartPosition = 0) const noexcept;
	
	VString Substring(const uint32 StartPosition, const uint32 Count = static_cast<uint32>(-1)) const;

	uint32 GetSize() const noexcept;
	uint32 GetCapacity() const noexcept;
	
	const char* GetData() const noexcept;
	char* GetData() noexcept;
	
	const char* CString() const noexcept;
	char* CString() noexcept;
	
	char& Front();
	const char& Front() const;
	
	char& Back();
	const char& Back() const;
	
	char& At(const uint32 Index);
	const char& At(const uint32 Index) const;
	
	bool IsEmpty() const noexcept;
	bool Contains(const char* String) const noexcept;
	bool Contains(const std::string& String) const noexcept;
	bool Contains(const VString& String) const noexcept;
	bool Contains(const char Character) const noexcept;
	
	VString& operator=(const char* OtherString);
	VString& operator=(const std::string& OtherString);
	VString& operator=(const VString& OtherString);
	VString& operator=(VString&& OtherString) noexcept;
	
	VString& operator+=(const char* OtherString);
	VString& operator+=(const std::string& OtherString);
	VString& operator+=(const VString& OtherString);
	VString& operator+=(const char Character);
	
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
	friend bool operator!=(const VString& LeftString, const char*& RightString) noexcept;
	friend bool operator!=(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator!=(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator<(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator<(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator<(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator<(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator<(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator<=(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator<=(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator<=(const VString& LeftString, const char*& RightString) noexcept;
	friend bool operator<=(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator<=(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator>(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator>(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator>(const VString& LeftString, const char*& RightString) noexcept;
	friend bool operator>(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator>(const VString& LeftString, const VString& RightString) noexcept;
	
	friend bool operator>=(const char* LeftString, const VString& RightString) noexcept;
	friend bool operator>=(const std::string& LeftString, const VString& RightString) noexcept;
	friend bool operator>=(const VString& LeftString, const char* RightString) noexcept;
	friend bool operator>=(const VString& LeftString, const std::string& RightString) noexcept;
	friend bool operator>=(const VString& LeftString, const VString& RightString) noexcept;
	
	char& operator[](const uint32 Index);
	const char& operator[](const uint32 Index) const;
	
	operator const char*() const noexcept;
	operator std::string() const;
	
	friend std::ostream& operator<<(std::ostream& Stream, const VString& String);

public:
	static const VString EmptyString;
	static const VString NullString;
	
private:
	void Allocate(const uint32 NewCapacity);
	void Reallocate(const uint32 NewCapacity);
	void EnsureCapacity(const uint32 RequiredCapacity);

private:
	char* Data = nullptr;
	
	uint32 Size = 0;
	uint32 Capacity = 0;
};
}