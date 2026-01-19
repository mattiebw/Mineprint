#pragma once

// Credit(mware): From https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Core/UUID.h

struct UUID
{
    UUID() : m_UUID(Random::ULong())
    {
    }

    UUID(u64 uuid);

    ~UUID()                                = default;
    UUID(const UUID& other)                = default;
    UUID(UUID&& other) noexcept            = default;
    UUID& operator=(const UUID& other)     = default;
    UUID& operator=(UUID&& other) noexcept = default;

    operator u64() const { return m_UUID; }

private:
    u64 m_UUID;
};

// Custom hashing for UUIDs.
namespace std
{
    template <typename T>
    struct hash;

    template <>
    struct hash<UUID>
    {
        std::size_t operator()(const UUID& uuid) const noexcept
        {
            return uuid;
        }
    };
}
