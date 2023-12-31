#pragma once

#include <cstdio>
#include <vector>

namespace Xroads
{
    inline bool FileExists(const std::string& filename)
    {
        FILE* filu = fopen(filename.c_str(), "rb");
        if (filu == nullptr)
            return false;
        fclose(filu);
        return true;
    }
    inline std::vector<u8> ReadFile(const std::string& filename)
    {
        std::vector<u8> ret;

        FILE* filu = fopen(filename.c_str(), "rb");
        if (filu == nullptr)
            return ret;

        fseek(filu, 0, SEEK_END);
        int filesize = ftell(filu);
        fseek(filu, 0, SEEK_SET);

        ret.assign(filesize, 0);

        if (fread(ret.data(), filesize, 1, filu) != 1)
            ret.clear();

        fclose(filu);
        return ret;
    }

    struct Files
    {
        std::wstring save_path; //includes trailing backslash
        void Init();
        void Quit();

        bool FileExists(std::wstring filename);

        std::wstring GetFullPath(std::wstring filename);

        std::vector<std::byte> ReadTextFile(std::wstring filename);
        void WriteTextFile(std::wstring filename, std::vector<std::byte> contents);
        void DeleteFile(std::wstring filename);
    };
}
