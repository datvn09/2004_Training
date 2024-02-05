# Anti Debug
## Code 

```
#include <iostream>
#include <cstring>
#include <Windows.h>
#define NT_GLOBAL_FLAG_DEBUGGED 0x70

DWORD pid;

bool Check1() {     //2. Object Handles -- CreateFile()
    char szFileName[MAX_PATH];
    if (0 == GetModuleFileNameA(NULL, szFileName, sizeof(szFileName)))
        return false;

    HANDLE hFile = CreateFileA(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        return true;
    }
    CloseHandle(hFile);
    return false;
}

bool Check2()     //8. Misc -- DbgPrint() 
{
    __try
    {
        RaiseException(DBG_PRINTEXCEPTION_C, 0, 0, 0);
    }
    __except (GetExceptionCode() == DBG_PRINTEXCEPTION_C)
    {
        return false;
    }

    return true;
}

BOOL isDebugged = TRUE;
LONG WINAPI CustomUnhandledExceptionFilter(PEXCEPTION_POINTERS pExceptionPointers)
{
    isDebugged = FALSE;
    return EXCEPTION_CONTINUE_EXECUTION;
}

int main()
{
    const char* flag = "datvn09KCSc";
    char input[50];

    std::cout << "Input password > ";
    std::cin >> input;

    //1. Debug Flags -- Using Win32 API -- CheckRemoteDebuggerPresent
    if (IsDebuggerPresent() == 1)
    {
        std::cout << "Debug detected 1.\n";
        exit(1);
    }

    //1. Debug Flags -- Using Win32 API -- NtGlobalFlag
    PDWORD pNtGlobalFlag = (PDWORD)(__readgsqword(0x60) + 0xBC);
    if ((*pNtGlobalFlag) & NT_GLOBAL_FLAG_DEBUGGED)
    {
        std::cout << "Debug detected 2.\n";
        exit(1);
    }

    //1. Debug Flags -- Using Win32 API -- CheckRemoteDebuggerPresent
    BOOL bDebuggerPresent = FALSE;
    BOOL success = CheckRemoteDebuggerPresent(GetCurrentProcess(), &bDebuggerPresent);
    if (success)
    {
        if (bDebuggerPresent)
        {
            std::cout << "Debug detected 3.\n";
            exit(1);
        }
    }

    //2. Object Handles -- CreateFile()
    if (Check1() == true)
    {
        std::cout << "Debug detected 4.\n";
        exit(1);
    }

    //3. Exceptions -- UnhandledExceptionFilter()
    PTOP_LEVEL_EXCEPTION_FILTER previousUnhandledExceptionFilter = SetUnhandledExceptionFilter(CustomUnhandledExceptionFilter);
    RaiseException(EXCEPTION_FLT_DIVIDE_BY_ZERO, 0, 0, NULL);
    SetUnhandledExceptionFilter(previousUnhandledExceptionFilter);
    if (isDebugged)
    {
        std::cout << "Debug detected 5.\n";
        exit(1);
    }

    //4. Timing -- GetTickCount()
    int t1 = static_cast<int>(GetTickCount64());
    for (int i = 0; i < 4; ++i)
    {
        Sleep(1u);
    }
    int t2 = static_cast<int>(GetTickCount64());
    if (((t2 - t1) / 1000) > 5)
    {
        std::cout << "Debug detected 6.\n";
        exit(1);
    }

    //5. Process Memory -- Hardware Breakpoints
    CONTEXT context = {};
    context.ContextFlags = CONTEXT_DEBUG_REGISTERS;
    GetThreadContext(GetCurrentThread(), &context);
    if (context.Dr0 || context.Dr1 || context.Dr2 || context.Dr3)
    {
        std::cout << "Debug detected 7.\n";
        exit(1);
    }

    //6. Assembly instructions -- DebugBreak
    BOOL isDebugged = TRUE;
    __try
    {
        DebugBreak();
    }
    __except (GetExceptionCode() == EXCEPTION_BREAKPOINT ? EXCEPTION_EXECUTE_HANDLER : EXCEPTION_CONTINUE_SEARCH)
    {
        isDebugged = FALSE;
    }
    if (isDebugged)
    {
        std::cout << "Debug detected 8.\n";
        exit(1);
    }

    //7. Direct debugger interaction -- Self-Debugging
    if (DebugActiveProcess(pid))
    {
        std::cout << "Debug detected 9.\n";
        exit(1);
    }
    else
    {
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        TerminateProcess(hProcess, 0);
    }

    //8. Misc -- DbgPrint()
    if (Check2() == true)
    {
        std::cout << "Debug detected 10.\n";
        exit(1);
    }
    

    if (strcmp(flag, input) == 0)
    {
        std::cout << "Your password is correct.";
    }
    else
    {
        std::cout << "Your password is incorrect.";
    }

    return 0;
}
```

- Chương trình c++ cơ bản (PE32+ executable (console) x86-64, for MS Windows) thực hiện kiểm tra chuỗi nhập vào so với chuỗi mặc định cho sẵn có sử dụng 10 kỹ thuật anti_debug cơ bản:

1. Debug Flags -- Using Win32 API -- IsDebuggerPresent()

![image](https://github.com/datvn09/2024_Training/assets/157048397/36b841eb-394c-4597-aee4-6ae4e44d3a5f)

Phát hiện và cản trở việc phân tích thủ công, hàm IsDebuggerPresent kiểm tra cờ BeingDebugged trong PEB để phát hiện trình gỡ lỗi.

2. Debug Flags -- Using Win32 API -- NtGlobalFlag

![image](https://github.com/datvn09/2024_Training/assets/157048397/8c9c2e4a-f1f9-45ff-951f-019a60ff4c19)

Có một số cờ cụ thể được đặt trong không gian địa chỉ quy trình khi nó được gỡ lỗi. NtGlobalFlag là tập hợp các cờ nằm ​​trong PEB có thể cho biết sự hiện diện của trình gỡ lỗi. 
    FLG_HEAP_ENABLE_TAIL_CHECK   0x10
    FLG_HEAP_ENABLE_FREE_CHECK   0x20
    FLG_HEAP_VALIDATE_PARAMETERS 0x40
    
Lưu ý: Điều này không phát hiện được trình gỡ lỗi Visual Studio (msvsmon).

![image](https://github.com/datvn09/2024_Training/assets/157048397/3db57a31-0f32-41ff-8df7-10ef77e34191)

![image](https://github.com/datvn09/2024_Training/assets/157048397/e0ba3fbb-c555-48bb-805b-1c9a5d55fff1)





