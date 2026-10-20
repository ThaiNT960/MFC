#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdio>
#include <cwchar>
#include <string>

#pragma comment(lib, "Advapi32.lib")

const wchar_t* RUN_KEY =
L"Software\\Microsoft\\Windows\\CurrentVersion\\Run";

const wchar_t* VALUE_NAME = L"AppChat1";

const wchar_t* CHAT_EXE =
L"C:\\Users\\ADMIN\\Desktop\\mfc\\Tuần 4\\x64\\Debug\\AppChat1.exe";

int RegisterAutorun()
{
    DWORD attributes = GetFileAttributesW(CHAT_EXE);
    if (attributes == INVALID_FILE_ATTRIBUTES ||
        (attributes & FILE_ATTRIBUTE_DIRECTORY))
    {
        wprintf(L"Khong tim thay AppChat1.exe. Kiem tra lai CHAT_EXE.\n");
        return 1;
    }

    HKEY runKey = nullptr;
    LSTATUS result = RegCreateKeyExW(
        HKEY_CURRENT_USER,
        RUN_KEY,
        0,
        nullptr,
        0,
        KEY_SET_VALUE,
        nullptr,
        &runKey,
        nullptr);

    if (result != ERROR_SUCCESS)
    {
        wprintf(L"Khong mo duoc Registry Run. Loi: %ld\n", result);
        return 1;
    }

    // Đường dẫn có khoảng trắng nên cần đặt trong dấu nháy kép.
    std::wstring command = L"\"";
    command += CHAT_EXE;
    command += L"\"";

    result = RegSetValueExW(
        runKey,
        VALUE_NAME,
        0,
        REG_SZ,
        reinterpret_cast<const BYTE*>(command.c_str()),
        static_cast<DWORD>((command.size() + 1) * sizeof(wchar_t)));

    RegCloseKey(runKey);

    if (result != ERROR_SUCCESS)
    {
        wprintf(L"Dang ky autorun that bai. Loi: %ld\n", result);
        return 1;
    }

    wprintf(L"Da dang ky autorun cho AppChat1.\n");
    return 0;
}

int RemoveAutorun()
{
    HKEY runKey = nullptr;
    LSTATUS result = RegOpenKeyExW(
        HKEY_CURRENT_USER,
        RUN_KEY,
        0,
        KEY_SET_VALUE,
        &runKey);

    if (result != ERROR_SUCCESS)
    {
        wprintf(L"Khong mo duoc Registry Run. Loi: %ld\n", result);
        return 1;
    }

    result = RegDeleteValueW(runKey, VALUE_NAME);
    RegCloseKey(runKey);

    if (result != ERROR_SUCCESS)
    {
        wprintf(L"Khong xoa duoc autorun. Loi: %ld\n", result);
        return 1;
    }

    wprintf(L"Da xoa autorun cua AppChat1.\n");
    return 0;
}

int wmain(int argc, wchar_t* argv[])
{
    if (argc != 2)
    {
        wprintf(L"Cach dung: ChatAutorun.exe add | remove\n");
        return 1;
    }

    if (wcscmp(argv[1], L"add") == 0)
        return RegisterAutorun();

    if (wcscmp(argv[1], L"remove") == 0)
        return RemoveAutorun();

    wprintf(L"Lenh khong hop le. Dung add hoac remove.\n");
    return 1;
}
