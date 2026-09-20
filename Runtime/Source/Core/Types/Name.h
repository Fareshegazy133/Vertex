// Vertex

#pragma once

#include <mutex>
#include "Core/Containers/Map.h"
#include "Core/Types/String.h"

namespace VCore
{
class VName
{
public:
    VName() = default;
	VName(const VName&) = default;
	VName(VName&&) noexcept = default;
	VName(const char Character);
    VName(const char* String);
    VName(const std::string& String);
    VName(const std::string_view String);
    VName(const VString& String);
    ~VName() = default;
	
	const char* ToCString() const;
	VString ToString() const;
	
    uint32 GetIndex() const noexcept;
    uint32 GetHash() const noexcept;
	
	bool IsValid() const noexcept;
	bool IsNone() const noexcept;
	
	bool Equals(const char Character) const noexcept;
    bool Equals(const char* String) const noexcept;
    bool Equals(const std::string& String) const noexcept;
    bool Equals(const std::string_view String) const noexcept;
    bool Equals(const VString& String) const noexcept;
    bool Equals(const VName& Name) const noexcept;
	
	VName& operator=(const VName&) = default;
	VName& operator=(VName&&) noexcept = default;
	VName& operator=(const char Character);
	VName& operator=(const char* String);
	VName& operator=(const std::string& String);
	VName& operator=(const std::string_view String);
	VName& operator=(const VString& String);

	friend bool operator==(const char LeftCharacter, const VName& RightName) noexcept;
	friend bool operator==(const char* LeftString, const VName& RightName) noexcept;
	friend bool operator==(const std::string& LeftString, const VName& RightName) noexcept;
	friend bool operator==(const std::string_view LeftString, const VName& RightName) noexcept;
	friend bool operator==(const VString& LeftString, const VName& RightName) noexcept;
	friend bool operator==(const VName& LeftName, const char RightCharacter) noexcept;
	friend bool operator==(const VName& LeftName, const char* RightString) noexcept;
	friend bool operator==(const VName& LeftName, const std::string& RightString) noexcept;
	friend bool operator==(const VName& LeftName, const std::string_view RightString) noexcept;
	friend bool operator==(const VName& LeftName, const VString& RightString) noexcept;
	friend bool operator==(const VName& LeftName, const VName& RightName) noexcept;
    
	friend bool operator!=(const char LeftCharacter, const VName& RightName) noexcept;
    friend bool operator!=(const char* LeftString, const VName& RightName) noexcept;
    friend bool operator!=(const std::string& LeftString, const VName& RightName) noexcept;
    friend bool operator!=(const std::string_view LeftString, const VName& Right) noexcept;
    friend bool operator!=(const VString& LeftString, const VName& RightName) noexcept;
	friend bool operator!=(const VName& LeftName, const char RightCharacter) noexcept;
	friend bool operator!=(const VName& LeftName, const char* RightString) noexcept;
	friend bool operator!=(const VName& LeftName, const std::string& RightString) noexcept;
	friend bool operator!=(const VName& LeftName, const std::string_view RightString) noexcept;
	friend bool operator!=(const VName& LeftName, const VString& RightString) noexcept;
	friend bool operator!=(const VName& LeftName, const VName& RightName) noexcept;

    friend bool operator<(const VName& LeftName, const VName& RightName) noexcept;
    friend bool operator>(const VName& LeftName, const VName& RightName) noexcept;
    friend bool operator<=(const VName& LeftName, const VName& RightName) noexcept;
    friend bool operator>=(const VName& LeftName, const VName& RightName) noexcept;

private:
	friend VName MakeName(std::string_view Name);
    explicit VName(const uint32 NameIndex, const uint32 NameHash) noexcept;

private:
    uint32 Index = INDEX_NONE;
    uint32 HashValue = 0;
};

class VNameRegistry
{
public:
	VNameRegistry();
	uint32 FindOrAdd(const std::string_view Name, uint32& OutHash);
	const VString& GetString(const uint32 Index) const;

private:
	VMap<VString, uint32> NameToIndex;

	VArray<VString> Names;
	VArray<uint32> Hashes;
	
	mutable std::mutex Mutex;
};
	
	VNameRegistry& GetNameRegistry();
	VName MakeName(const std::string_view Name);
}