#pragma once

class FileUtil
{
public:
    static Buffer ReadFileToBuffer(const std::filesystem::path &path, bool binary = true)
    {
        if (!exists(path))
        {
            MP_ERROR("Tried to read file that doesn't exist: {}", path.string());
            return Buffer(); // Return empty buffer
        }

        std::ifstream file(path, binary ? std::ios::binary : std::ios::in);
        if (!file.is_open())
        {
            MP_ERROR("Failed to open file: {}", path.string());
            return Buffer(); // Return empty buffer
        }

        // Seek to end. This works better than ios::ate, as it properly reports the size in bytes in all circumstances.
        file.ignore(std::numeric_limits<std::streamsize>::max());
        const std::streamsize pos = file.gcount();
        Buffer                buf(pos);
        file.seekg(0, std::ios::beg);
        file.read(reinterpret_cast<char*>(buf.Data), pos);
        file.close();

        return buf;
    }
    
    static Buffer ReadBinaryFileToBuffer(const std::filesystem::path &path)
    {
        return ReadFileToBuffer(path, true);
    }
    
    static Buffer ReadTextFileToBuffer(const std::filesystem::path &path)
    {
        return ReadFileToBuffer(path, false);
    }
    
    static std::string ReadFileToAsciiString(const std::filesystem::path &path, bool binary = false)
    {
        if (!exists(path))
        {
            MP_ERROR("Tried to read file that doesn't exist: {}", path.string());
            return ""; // Return empty string. TODO(mware): Is there a better way to do this?
        }
        
        std::ifstream file(path, binary ? std::ios::binary : std::ios::in);
        if (!file.is_open())
        {
            MP_ERROR("Failed to open file: {}", path.string());
            return "";
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        file.close();
        return buffer.str();
    }
    
    static std::string ReadBinaryFileToAsciiString(const std::filesystem::path &path)
    {
        return ReadFileToAsciiString(path, true);
    }
    
    static std::string ReadTextFileToAsciiString(const std::filesystem::path &path)
    {
        return ReadFileToAsciiString(path, false);
    }
};
