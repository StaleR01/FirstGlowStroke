#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <comdef.h>

struct PModule
{
    DWORD dwBase;
    DWORD dwSize;
};

class memory
{
public:
    bool Attach(const char* ProcessName, DWORD rights)
    {
        HANDLE handle = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
        PROCESSENTRY32 entry;
        entry.dwSize = sizeof(entry);
        do
            if (!strcmp(_bstr_t(entry.szExeFile), ProcessName)) {
                pID = entry.th32ProcessID;
                CloseHandle(handle);
                _process = OpenProcess(rights, false, pID);
                return true;
            }
        while (Process32Next(handle, &entry));
        return false;
    }
    DWORD GetModule(const char* moduleName) {
        HANDLE module = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pID);
        MODULEENTRY32 mEntry;
        mEntry.dwSize = sizeof(mEntry);
        do {
            if (!strcmp(_bstr_t(mEntry.szModule), moduleName)) {
                CloseHandle(module);
                PModule mod = { reinterpret_cast<DWORD>(mEntry.hModule), mEntry.modBaseSize };
                return mod.dwBase;
            }
        } while (Module32Next(module, &mEntry));
        PModule mod = { static_cast<DWORD>(false), static_cast<DWORD>(false) };
        return mod.dwBase;
    }
    template <class T>
    T Read(const DWORD addr) {
        T _read;
        ReadProcessMemory(_process, reinterpret_cast<LPVOID>(addr), &_read, sizeof(T), NULL);
        return _read;
    }
    template <class T>
    void Write(const DWORD addr, T val) {
        WriteProcessMemory(_process, reinterpret_cast<LPVOID>(addr), &val, sizeof(T), NULL);
    }
private:
    HANDLE _process;
    DWORD pID;
};