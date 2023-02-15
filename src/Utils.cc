#include "Utils.h"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <vector>

DWORD Memory::ReadDWORD(HANDLE hProcess, DWORD addr)
{
    DWORD_PTR ptr = NULL;
    SIZE_T bytesRead = 0;

    ReadProcessMemory(hProcess, (DWORD *)addr, &ptr, 4, &bytesRead);

    return ptr;
}

void Memory::Read(HANDLE hProcess, DWORD addr, void *structure, int size)
{
    SIZE_T bytesRead = 0;

    ReadProcessMemory(hProcess, (DWORD *)addr, structure, size, &bytesRead);
}

DWORD Memory::ReadDWORDFromBuffer(void *buff, int position)
{
    DWORD result;
    memcpy(&result, (char *)buff + position, 4);
    return result;
}

BOOL Process::IsProcessRunning(DWORD pid)
{
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL)
    {
        return FALSE;
    }
    DWORD exitCode;
    GetExitCodeProcess(hProcess, &exitCode);
    CloseHandle(hProcess);
    return exitCode == STILL_ACTIVE;
}

bool Character::ContainsOnlyASCII(const char *buff, int maxSize)
{
    for (int i = 0; i < maxSize; ++i)
    {
        if (buff[i] == 0)
            return true;
        if ((unsigned char)buff[i] > 127)
            return false;
    }
    return true;
}

bool Character::ArrayStartsWith(const char *array, const char *start)
{
    int len = strlen(start);
    for (int i = 0; i < len; ++i)
    {
        if (array[i] != start[i])
            return false;
    }
    return true;
}


bool Character::VectorEndsWith(const std::vector<char> &array, const char *end)
{
    if(array.size() < strlen(end))
        return false;
    int len = strlen(end);
    for (int i = 0; i < len; ++i)
    {
        if (array[array.size() - i - 1] != end[len - i - 1])
            return false;
    }
    return true;
}

bool Character::VectorStartsWith(const std::vector<char> &array, const char *start)
{
    if(array.size() < strlen(start))
        return false;
    int len = strlen(start);
    for (int i = 0; i < len; ++i)
    {
        if (array[i] != start[i])
            return false;
    }
    return true;
}

std::string Character::ToLower(std::string str)
{
    std::string strLower;
    strLower.resize(str.size());

    std::transform(str.begin(),
                   str.end(),
                   strLower.begin(),
                   ::tolower);

    return strLower;
}

std::string Character::RandomString(const int len)
{
    std::string str = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
    std::string newstr;
    int pos;
    while (newstr.size() != len)
    {
        pos = ((rand() % (str.size() - 1)));
        newstr += str.substr(pos, 1);
    }
    return newstr;
}

std::string Character::Format(const char *c, const char *args...)
{
    va_list argptr;
    va_start(argptr, args);
    char buffer[1024];
    vsprintf_s(buffer, c, argptr);
    va_end(argptr);
    return std::string(buffer);
}