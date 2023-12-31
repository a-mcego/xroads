#include "xroads/Xroads.h"

#include "combaseapi.h"
#include "shlobj.h"
#include "shlwapi.h"
#include "KnownFolders.h"

#include "Steam.h"

namespace Xroads
{
    void Files::Init()
    {
        wchar_t* spt_c = nullptr;
        SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &spt_c);
        save_path = spt_c; //doesnt include backslash.
        CoTaskMemFree(spt_c);
        save_path += std::wstring(L"/") + GetEngine().appstate.wgamename;
        CreateDirectoryW(save_path.c_str(), nullptr);
        save_path += std::wstring(L"/") + ToWString(Steam::GetAccountID());
        CreateDirectoryW(save_path.c_str(), nullptr);
        save_path += L"/";
        std::wcout << "Save path defined as " << save_path << std::endl;
    }

    std::wstring Files::GetFullPath(std::wstring filename)
    {
        return save_path + filename;
    }

    bool Files::FileExists(std::wstring filename)
    {
        return PathFileExistsW(GetFullPath(filename).c_str());
    }

    std::vector<std::byte> Files::ReadTextFile(std::wstring filename)
    {
        if (!FileExists(filename))
            return std::vector<std::byte>();
        HANDLE hFile;
        hFile = CreateFileW(GetFullPath(filename).c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return std::vector<std::byte>();
        size_t size = GetFileSize(hFile, nullptr);
        std::vector<std::byte> ret(size, std::byte(0));
        DWORD bytes_read=0;
        ::ReadFile(hFile, ret.data(), size, &bytes_read, nullptr);
        CloseHandle(hFile);
        return ret;
    }

    void Files::WriteTextFile(std::wstring filename, std::vector<std::byte> contents)
    {
        HANDLE hFile;
        hFile = CreateFileW(GetFullPath(filename).c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile == INVALID_HANDLE_VALUE)
            return;
        DWORD bytes_written=0;
        WriteFile(hFile, contents.data(), contents.size(), &bytes_written, nullptr);
        CloseHandle(hFile);
    }

    void Files::Quit()
    {

    }
}
