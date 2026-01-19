#pragma once

// Non-owning raw buffer class
// Adapted from https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/Buffer.h

struct Buffer
{
    u8*    Data = nullptr;
    size_t Size = 0;

    Buffer() = default;

    Buffer(u8* data, size_t size)
    {
        this->Data = data;
        this->Size = size;
    }

    Buffer(size_t size)
    {
        Allocate(size);
    }

    Buffer(const Buffer& other) = default;

    Buffer(Buffer&& other) noexcept
    {
        Data       = other.Data;
        Size       = other.Size;
        other.Data = nullptr;
        other.Size = 0;
    }

    Buffer& operator=(const Buffer& other) = default;

    Buffer& operator=(Buffer&& other) noexcept
    {
        Data       = other.Data;
        Size       = other.Size;
        other.Data = nullptr;
        other.Size = 0;
        return *this;
    };

    ~Buffer() = default; // The buffer is non-owning, so we don't want to free on deconstruct.

    FORCEINLINE_DEBUGGABLE void Allocate(size_t size)
    {
        Release();
        Data = new u8[size];
        Size = size;
    }

    FORCEINLINE_DEBUGGABLE void Release()
    {
        delete[] Data; // It's okay if data is nullptr.
        Data = nullptr;
        Size = 0;
    }

    NODISCARD FORCEINLINE_DEBUGGABLE Buffer Copy() const
    {
        Buffer result(Size);
        memcpy(result.Data, Data, Size);
        return result;
    }

    NODISCARD FORCEINLINE_DEBUGGABLE static Buffer Copy(Buffer buffer)
    {
        return buffer.Copy();
    }

    template <typename T>
    T* As()
    {
        return static_cast<T*>(Data);
    }

    operator bool() const
    {
        return Data != nullptr;
    }
};

// And also an owning scoped buffer:

struct ScopedBuffer
{
    ScopedBuffer(const Buffer& buffer)
        : m_Buffer(buffer)
    {
    }

    ScopedBuffer(Buffer&& buffer)
        : m_Buffer(std::move(buffer))
    {
    }

    ScopedBuffer(size_t size)
        : m_Buffer(size)
    {
    }

    ~ScopedBuffer()
    {
        m_Buffer.Release();
    }

    NODISCARD FORCEINLINE u8*    Data() const { return m_Buffer.Data; }
    NODISCARD FORCEINLINE size_t Size() const { return m_Buffer.Size; }

    template <typename T>
    NODISCARD FORCEINLINE T* As()
    {
        return m_Buffer.As<T>();
    }

    operator bool() const { return m_Buffer; }

private:
    Buffer m_Buffer;
};

class BufferReader
{
public:
    BufferReader(u8* buffer, u32 bufferSize)
        : m_BufferBegin(buffer), m_BufferSize(bufferSize), m_ReadOffset(0)
    {
    }

    explicit BufferReader(const Buffer& buffer)
        : m_BufferBegin(buffer.Data), m_BufferSize(static_cast<u32>(buffer.Size)), m_ReadOffset(0)
    {
    }

    FORCEINLINE u32 GetReadOffset() const { return m_ReadOffset; }
    FORCEINLINE u8* GetBufferAtOffset() const { return m_BufferBegin + m_ReadOffset; }
    FORCEINLINE u8* GetBuffer() const { return m_BufferBegin; }
    FORCEINLINE u32 GetBufferSize() const { return m_BufferSize; }
    FORCEINLINE u32 GetRemainingSize() const { return m_BufferSize - m_ReadOffset; }

    void Reset()
    {
        m_ReadOffset = 0;
    }

    void Seek(u32 offset)
    {
        m_ReadOffset = offset;
    }

    void Skip(u32 offset)
    {
        m_ReadOffset += offset;
    }

    template <typename T>
    T Read()
    {
        if (m_ReadOffset + sizeof(T) > m_BufferSize)
            return T();

        T value      = *reinterpret_cast<T*>(m_BufferBegin + m_ReadOffset);
        m_ReadOffset += sizeof(T);
        return value;
    }

    void CopyTo(u8* dest, u32 size)
    {
        if (m_ReadOffset + size > m_BufferSize)
            return;

        memcpy(dest, m_BufferBegin + m_ReadOffset, size);
        m_ReadOffset += size;
    }

private:
    u8* m_BufferBegin;
    u32 m_BufferSize;
    u32 m_ReadOffset;
};
