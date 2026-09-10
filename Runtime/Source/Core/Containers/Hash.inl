// Vertex

namespace VCore
{
namespace VHashFunctions
{
    inline VHashType HashBytes(const void* Data, const uint64 Size)
    {
        constexpr VHashType OffsetBasis = 14695981039346656037ull;
        constexpr VHashType Prime = 1099511628211ull;

        const unsigned char* Bytes = static_cast<const unsigned char*>(Data);
        VHashType Hash = OffsetBasis;

        for (uint64 Index = 0; Index < Size; Index++)
        {
            Hash ^= Bytes[Index];
            Hash *= Prime;
        }

        return Hash;
    }

    inline VHashType Combine(const VHashType A, const VHashType B)
    {
        VHashType Hash = A;
        Hash ^= B + 0x9e3779b97f4a7c15ull + (Hash << 6) + (Hash >> 2);
        return Hash;
    }
}

template <typename KeyType>
VHashType VHash<KeyType>::operator()(const KeyType& Key) const
{
    static_assert(std::is_trivially_copyable_v<KeyType>, "VHash<KeyType> requires a specialization for non-trivially-copyable types.");
    return VHashFunctions::HashBytes(&Key, sizeof(KeyType));
}

inline VHashType VHash<int32>::operator()(const int32 Key) const
{
    return VHashFunctions::HashBytes(&Key, sizeof(Key));
}

inline VHashType VHash<uint32>::operator()(const uint32 Key) const
{
    return VHashFunctions::HashBytes(&Key, sizeof(Key));
}

inline VHashType VHash<int64>::operator()(const int64 Key) const
{
    return VHashFunctions::HashBytes(&Key, sizeof(Key));
}

inline VHashType VHash<uint64>::operator()(const uint64 Key) const
{
    return VHashFunctions::HashBytes(&Key, sizeof(Key));
}

inline VHashType VHash<float32>::operator()(const float32 Key) const
{
    return VHashFunctions::HashBytes(&Key, sizeof(Key));
}

inline VHashType VHash<float64>::operator()(const float64 Key) const
{
    return VHashFunctions::HashBytes(&Key, sizeof(Key));
}

inline VHashType VHash<bool>::operator()(const bool Key) const
{
    return Key ? 1ull : 0ull;
}

inline VHashType VHash<char>::operator()(const char Key) const
{
    return static_cast<unsigned char>(Key);
}

inline VHashType VHash<VString>::operator()(const VString& Key) const
{
    return VHashFunctions::HashBytes(Key.GetData(), Key.GetSize());
}
}