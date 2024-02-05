#include <iostream>
#include <cstring>
#include <Windows.h>
#define NT_GLOBAL_FLAG_DEBUGGED 0x70

DWORD pid;

bool Check1() {     //2. Object Handles -- CreateFile()
    CHAR szFileName[MAX_PATH];
    if (0 == GetModuleFileNameA(NULL, szFileName, sizeof(szFileName)))
        return false;

    return INVALID_HANDLE_VALUE == CreateFileA(szFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
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

    //1. Debug Flags -- Using Win32 API -- IsDebuggerPresent()
    if (IsDebuggerPresent() == 1) // Hàm IsDebuggerPresent() trả về 1 nếu có một trình gỡ lỗi đang chạy và trả về 0 nếu không có.
    {
        std::cout << "Debug detected 1.\n";
        exit(1);   //kết thúc chương trình với mã lỗi 1 
    }

    //1. Debug Flags -- Using Win32 API -- NtGlobalFlag
    PDWORD pNtGlobalFlag = (PDWORD)(__readgsqword(0x60) + 0xBC);//Đọc giá trị từ offset 0x60 trong cấu trúc TEB của thread hiện tại.
    if ((*pNtGlobalFlag) & NT_GLOBAL_FLAG_DEBUGGED)  //Kiểm tra xem cờ NT_GLOBAL_FLAG_DEBUGGED có được set hay không
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
    RaiseException(EXCEPTION_FLT_DIVIDE_BY_ZERO, 0, 0, NULL);   //Gây ra một ngoại lệ
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
    GetThreadContext(GetCurrentThread(), &context);  // Lấy thông tin về trạng thái hiện tại của thanh ghi và thanh ghi gỡ lỗi
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
    if (!DebugActiveProcess(pid))  //kết nối đến quá trình với ID pid sử dụng hàm DebugActiveProcess
    {
        std::cout << "Debug detected 9.\n";
        HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
        TerminateProcess(hProcess, 0);
        exit(0);
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