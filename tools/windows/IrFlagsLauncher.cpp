#define UNICODE
#define _UNICODE
#include <windows.h>
#include <string>
#include <vector>

static std::wstring quote(const std::wstring& value)
{
    if (value.find_first_of(L" \t\"") == std::wstring::npos)
        return value;
    std::wstring out = L"\"";
    unsigned slashes = 0;
    for (wchar_t c : value)
    {
        if (c == L'\\')
        {
            ++slashes;
            continue;
        }
        if (c == L'\"')
        {
            out.append(slashes * 2 + 1, L'\\');
            out += c;
            slashes = 0;
            continue;
        }
        out.append(slashes, L'\\');
        slashes = 0;
        out += c;
    }
    out.append(slashes * 2, L'\\');
    out += L'\"';
    return out;
}

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    wchar_t module[MAX_PATH] = {};
    DWORD length = GetModuleFileNameW(nullptr, module, MAX_PATH);
    if (!length || length >= MAX_PATH)
        return 2;
    std::wstring dir(module, length);
    const size_t slash = dir.find_last_of(L"\\/");
    if (slash != std::wstring::npos)
        dir.resize(slash);
    SetCurrentDirectoryW(dir.c_str());

    std::wstring core = dir + L"\\IrFlagsCore.exe";
    std::wstring command = quote(core) + L" -locale fa";
    const wchar_t* raw = GetCommandLineW();
    const wchar_t* tail = raw;
    if (*tail == L'\"')
    {
        ++tail;
        while (*tail && *tail != L'\"') ++tail;
        if (*tail) ++tail;
    }
    else
    {
        while (*tail && *tail != L' ' && *tail != L'\t') ++tail;
    }
    while (*tail == L' ' || *tail == L'\t') ++tail;
    if (*tail)
    {
        command += L" ";
        command += tail;
    }

    std::vector<wchar_t> mutableCommand(command.begin(), command.end());
    mutableCommand.push_back(0);
    STARTUPINFOW si = {};
    si.cb = sizeof(si);
    PROCESS_INFORMATION pi = {};
    if (!CreateProcessW(core.c_str(), mutableCommand.data(), nullptr, nullptr, FALSE, 0,
                        nullptr, dir.c_str(), &si, &pi))
    {
        MessageBoxW(nullptr, L"IrFlagsCore.exe اجرا نشد.", L"IrFlags", MB_ICONERROR | MB_OK);
        return 3;
    }
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);
    return 0;
}
