#ifndef RUNNER_WIN32_UTILS_H_
#define RUNNER_WIN32_UTILS_H_

#define PIPE_BUFFER_SIZE 1024

#include <vector>
#include "tlhelp32.h"

LPWSTR StringToLPCWSTR(std::string orig) {
    size_t origsize = orig.length() + 1;
    const size_t newsize = 100;
    size_t convertedChars = 0;
    wchar_t* wcstring = (wchar_t*)malloc(sizeof(wchar_t) * (orig.length() - 1));
    mbstowcs_s(&convertedChars, wcstring, origsize, orig.c_str(), _TRUNCATE);
    return wcstring;
}

std::string& ReplaceAll(std::string& str, const std::string& old_value, const std::string& new_value) {
    for (std::string::size_type pos(0); pos != std::string::npos; pos += new_value.length()) {
        if ((pos = str.find(old_value, pos)) != std::string::npos)
            str.replace(pos, old_value.length(), new_value);
        else break;
    }
    return str;
}

inline BYTE toHex(const BYTE& x) {
    return x > 9 ? x + 55 : x + 48;
}

std::string _encodeURL(std::string& sIn) {
    std::string sOut;
    for (long ix = 0; ix < (long)sIn.size(); ix++) {
        BYTE buf[4];
        memset(buf, 0, 4);
        if (isalnum((BYTE)sIn[ix])) {
            buf[0] = sIn[ix];
        }
        else if (isspace((BYTE)sIn[ix])) {
            buf[0] = '+';
        }
        else {
            buf[0] = '%';
            buf[1] = toHex((BYTE)sIn[ix] >> 4);
            buf[2] = toHex((BYTE)sIn[ix] % 16);
        }
        sOut += (char*)buf;
    }
    return sOut;
}


void SplitString(const std::string& str, const std::string& splits, std::vector<std::string>& res)
{
    if (str == "") return;
    std::string strs = str + splits;
    size_t pos = strs.find(splits);
    size_t step = splits.size();

    while (pos != strs.npos) {
        std::string temp = strs.substr(0, pos);
        res.push_back(temp);
        strs = strs.substr(pos + step, strs.size());
        pos = strs.find(splits);
    }
}

std::string Unicode2GBK(std::string& strValue) {
    std::vector<std::string> vcString;
    SplitString(strValue, "\\u", vcString);
    wchar_t* pwBuf = new wchar_t[strValue.length() + 1];
    memset(pwBuf, 0, (strValue.length() + 1) * sizeof(wchar_t));
    int j(0);
    for (std::vector<std::string>::iterator it = vcString.begin(); it != vcString.end(); ++it) {
        if (it->empty()) {
            continue;
        }
        unsigned short wcTmp = 0;
        unsigned char cTmp = 0;

        //因为有中文字符混合ASSCII码情况,所以条件为k < it->length()
        for (size_t k = 0; k < it->length(); ++k) {
            cTmp = (unsigned char)(*it)[k];

            if (cTmp <= '9')//0x30~0x39 即0~9
            {
                wcTmp |= (cTmp & 0x0f) << (it->length() - k - 1) * 4;
            }
            else if (cTmp >= 'a')//0x61~7a 即a~z
            {
                wcTmp |= (cTmp - 0x57) << (it->length() - k - 1) * 4;
            }
            else//0x41~5a 即A~Z
            {
                wcTmp |= (cTmp - 0x37) << (it->length() - k - 1) * 4;
            }
        }
        pwBuf[j++] = (wchar_t)wcTmp;
    }
    char* pDst = new char[strValue.length() + 1];
    memset(pDst, 0, (strValue.length() + 1) * sizeof(char));

    WideCharToMultiByte(CP_ACP, 0, pwBuf, -1, (char*)pDst, (int)strValue.length() - 1, NULL, NULL);
    std::string strRet(pDst);

    delete[]pwBuf;
    pwBuf = NULL;

    delete[]pDst;
    pDst = NULL;

    return strRet;
}

char* UnicodeToGB2312(wchar_t* szUnicodeString) {
    UINT nCodePage = 936; //GB2312
    int nLength = WideCharToMultiByte(nCodePage, 0, szUnicodeString, -1, NULL, 0, NULL, NULL);
    char* pBuffer = new char[nLength + sizeof(char)];
    WideCharToMultiByte(nCodePage, 0, szUnicodeString, -1, pBuffer, nLength, NULL, NULL);
    pBuffer[nLength] = 0;
    return pBuffer;
}

std::string W_To_A(const std::wstring& wstr, unsigned int codepage = CP_ACP)
{
    int nwstrlen = WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (nwstrlen > 0) {
        std::string str(nwstrlen - 1, '\0');
        WideCharToMultiByte(codepage, 0, wstr.c_str(), -1, (LPSTR)str.c_str(), nwstrlen, NULL, NULL);
        return std::move(str);
    }
    return ("");
}

std::wstring A_To_W(const std::string& str, unsigned int codepage = CP_ACP)
{
    int nstrlen = MultiByteToWideChar(codepage, 0, str.c_str(), -1, NULL, 0);
    if (nstrlen > 0) {
        std::wstring wstr(nstrlen, L'\0');
        MultiByteToWideChar(codepage, 0, str.c_str(), -1, (LPWSTR)wstr.c_str(), nstrlen);
        return std::move(wstr);
    }
    return (L"");
}

std::string GBK_2_UTF8(std::string gbkStr)
{
    std::string outUtf8 = "";
    int n = MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, NULL, 0);
    WCHAR* str1 = new WCHAR[n];
    MultiByteToWideChar(CP_ACP, 0, gbkStr.c_str(), -1, str1, n);
    n = WideCharToMultiByte(CP_UTF8, 0, str1, -1, NULL, 0, NULL, NULL);
    char* str2 = new char[n];
    WideCharToMultiByte(CP_UTF8, 0, str1, -1, str2, n, NULL, NULL);
    outUtf8 = str2;
    delete[] str1;
    str1 = NULL;
    delete[] str2;
    str2 = NULL;
    return outUtf8;
}

std::string To_BOOL(int bVal) {
    return bVal != 0 ? "true" : "false";
}

char* Str_To_LPSTR(std::string str) {
    size_t len = str.length();
    char* cs = (char*)malloc(len + 1);
    if (cs == NULL) return NULL;
    strcpy_s(cs, len + 1, str.c_str());
    return cs;
}

char* Vec_To_LPSTR(std::vector<unsigned char> vec) {
    char* cs = (char*)malloc((vec.size() + 1) * sizeof(char));
    if (cs == NULL) return NULL;
    for (int i = 0; i < vec.size(); i++) {
        cs[i] = vec[i];
    }
    cs[vec.size()] = '\0';
    return cs;
}

int Exec(char* cmd) {
    /*char* cmd_ch = Str_To_LPSTR(cmd);
    if (cmd_ch == NULL) return 1;*/
    int error = 0;
    STARTUPINFOA si = { 0 };
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;

    PROCESS_INFORMATION pi = { 0 };
    if (::CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NEW_CONSOLE | CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        ::Sleep(100);
        WaitForSingleObject(pi.hProcess, INFINITE);
        ::CloseHandle(pi.hThread);
        ::CloseHandle(pi.hProcess);
    }
    else {
        std::cout << "exec CreateProcessA error" << std::endl;
        ::CloseHandle(pi.hThread);
        error = 2;
    }
    //free(cmd_ch);
    return error;
}

int ExecForResult(std::string cmd, std::string &result) {
    char* cmd_ch = Str_To_LPSTR(cmd);
    if (cmd_ch == NULL) return 1;
    int error = 0;
    //初始化管道
    HANDLE hPipeRead;
    HANDLE hPipeWrite;
    SECURITY_ATTRIBUTES saOutPipe;
    ::ZeroMemory(&saOutPipe, sizeof(saOutPipe));
    saOutPipe.nLength = sizeof(SECURITY_ATTRIBUTES);
    saOutPipe.lpSecurityDescriptor = NULL;
    saOutPipe.bInheritHandle = TRUE;
    if (CreatePipe(&hPipeRead, &hPipeWrite, &saOutPipe, PIPE_BUFFER_SIZE)) {
        PROCESS_INFORMATION processInfo;
        ::ZeroMemory(&processInfo, sizeof(processInfo));
        STARTUPINFOA startupInfo;
        ::ZeroMemory(&startupInfo, sizeof(startupInfo));
        startupInfo.cb = sizeof(STARTUPINFO);
        startupInfo.dwFlags = STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW;
        startupInfo.hStdOutput = hPipeWrite;
        startupInfo.hStdError = hPipeWrite;
        startupInfo.wShowWindow = SW_HIDE;

        if (::CreateProcessA(NULL, cmd_ch, NULL, NULL, TRUE, CREATE_NEW_CONSOLE | CREATE_NO_WINDOW, NULL,
            NULL, &startupInfo, &processInfo)) {
            ::Sleep(20);

            WaitForSingleObject(processInfo.hProcess, 300);

            DWORD dwReadLen = 0;
            DWORD dwStdLen = 0;

            std::string s = "";

            if (PeekNamedPipe(hPipeRead, NULL, 0, NULL, &dwReadLen, NULL) && dwReadLen > 0) {
                std::cout << "szPipe dwReadLen:" << dwReadLen << std::endl;
                char* szPipeOut = (char*)calloc(sizeof(char), dwReadLen + 1);
                if (szPipeOut != NULL) {
                    ::ZeroMemory(szPipeOut, sizeof(dwReadLen + 1));
                    if (ReadFile(hPipeRead, szPipeOut, dwReadLen, &dwStdLen, NULL)) {
                        std::cout << "szPipe ReadFile" << std::endl;
                        s = std::string(szPipeOut);
                    }
                    free(szPipeOut);
                }
            }
            ::CloseHandle(processInfo.hThread);
            ::CloseHandle(processInfo.hProcess);

            result = s;
        }
        else {
            std::cout << "execForResult CreateProcessA error" << std::endl;
            ::CloseHandle(processInfo.hThread);
            error = 2;
        }
    }
    else {
        std::cout << "execForResult CreatePipe error" << std::endl;
        error = 3;
    }
    free(cmd_ch);
    return error;
}

std::wstring QueryReg(HKEY hkey, DWORD dwFlags, std::wstring SubKey, std::wstring Value) {
    if (dwFlags == RRF_RT_REG_DWORD) {
        DWORD ret;
        DWORD size;
        std::cout << "QueryReg " << dwFlags << std::endl;
        RegGetValueW(hkey,
            SubKey.c_str(),
            Value.c_str(),
            dwFlags,
            nullptr,
            &ret,
            &size);
        std::wcout << L"QueryReg result:" << ret << " " << size << std::endl;
        TCHAR vRamCharBuff[50];
        _stprintf(vRamCharBuff, _T("%lu"), ret);
        return std::wstring(vRamCharBuff);
    }
    std::wstring ret{};
    DWORD size{};

    RegGetValueW(hkey,
        SubKey.c_str(),
        Value.c_str(),
        dwFlags,
        nullptr,
        nullptr,
        &size);

    ret.resize(size);

    RegGetValueW(hkey,
        SubKey.c_str(),
        Value.c_str(),
        dwFlags,
        nullptr,
        &ret[0],
        &size);

    return ret.c_str();
}

bool IsProcessRun(const char* pName) {
    HANDLE hProcessSnap;
    HANDLE hProcess;
    PROCESSENTRY32 pe32;
    DWORD dwPriorityClass;
    bool bFind = false;
    // Take a snapshot of all processes in the system.
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        return false;
    }
    // Set the size of the structure before using it.
    pe32.dwSize = sizeof(PROCESSENTRY32);
    // Retrieve information about the first process,
    // and exit if unsuccessful
    if (!Process32First(hProcessSnap, &pe32)) {
        CloseHandle(hProcessSnap);          // clean the snapshot object
        return false;
    }

    // Now walk the snapshot of processes, and
    // display information about each process in turn
    do {
        // Retrieve the priority class.
        dwPriorityClass = 0;
        std::wstring name = pe32.szExeFile;
        //W_To_A(name).c_str();
        if (strstr(W_To_A(name).c_str(), pName) != NULL) {
            bFind = true;
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));
    CloseHandle(hProcessSnap);
    return bFind;
}


#endif  // RUNNER_WIN32_UTILS_H_