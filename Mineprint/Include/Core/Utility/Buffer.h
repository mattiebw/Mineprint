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

// An owning scoped buffer.
struct ScopedBuffer : public Buffer
{
    ~ScopedBuffer()
    {
        Release();
    }
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

class BufferReadWriter
{
public:
    BufferReadWriter(u8* buffer, u32 bufferSize)
        : m_BufferBegin(buffer), m_BufferSize(bufferSize), m_ReadOffset(0), m_WriteOffset(0)
    {
    }

    explicit BufferReadWriter(const Buffer& buffer)
        : m_BufferBegin(buffer.Data), m_BufferSize(static_cast<u32>(buffer.Size)), m_ReadOffset(0), m_WriteOffset(0)
    {
    }

    FORCEINLINE u32 GetReadOffset() const { return m_ReadOffset; }
    FORCEINLINE u32 GetWriteOffset() const { return m_WriteOffset; }
    FORCEINLINE u8* GetBufferAtReadOffset() const { return m_BufferBegin + m_ReadOffset; }
    FORCEINLINE u8* GetBufferAtWriteOffset() const { return m_BufferBegin + m_WriteOffset; }
    FORCEINLINE u8* GetBuffer() const { return m_BufferBegin; }
    FORCEINLINE u32 GetBufferSize() const { return m_BufferSize; }
    FORCEINLINE u32 GetRemainingReadSize() const { return m_BufferSize - m_ReadOffset; }
    FORCEINLINE u32 GetRemainingWriteSize() const { return m_BufferSize - m_WriteOffset; }

    void Reset()
    {
        m_ReadOffset  = 0;
        m_WriteOffset = 0;
    }

    void SeekForReading(u32 offset)
    {
        m_ReadOffset = offset;
    }

    void SeekForWriting(u32 offset)
    {
        m_WriteOffset = offset;
    }

    void SkipForReading(u32 offset)
    {
        m_ReadOffset += offset;
    }

    void SkipForWriting(u32 offset)
    {
        m_WriteOffset += offset;
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

    bool Write(const void* data, u32 size)
    {
        if (m_WriteOffset + size > m_BufferSize)
            return false;

        memcpy(m_BufferBegin + m_WriteOffset, data, size);
        m_WriteOffset += size;
        return true;
    }
    
    template<typename T>
    bool Write(const T& value)
    {
        return Write(&value, sizeof(T));
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
    u32 m_WriteOffset;
};
