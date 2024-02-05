# Anti Debug
## Code 

```
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

3. Debug Flags -- Using Win32 API -- CheckRemoteDebuggerPresent

Gọi hàm `NtQueryInformationProcess` `GetCurrentProcess()` để lấy thông tin kiểm tra xem trình gỡ lỗi (trong một quy trình khác trên cùng một máy) có được gắn vào quy trình hiện tại hay không.

![image](https://github.com/datvn09/2024_Training/assets/157048397/3c9233dc-4484-4372-a6fb-5e806e1164b0)

4. Object Handles -- CreateFile()

Thủ thuật này sử dụng kernel32!CreateFileW() (hoặc kernel32!CreateFileA()) để mở riêng tệp của quy trình hiện tại. Nếu cuộc gọi không thành công, có thể coi rằng quy trình hiện tại đang được chạy với sự có mặt của trình gỡ lỗi.

![image](https://github.com/datvn09/2024_Training/assets/157048397/c32da67c-f6e8-4976-bb69-8832184827e0)

5. Exceptions -- UnhandledExceptionFilter()

![image](https://github.com/datvn09/2024_Training/assets/157048397/532b0ad2-cf80-48b7-b86c-a61221bfa288)

6. Timing -- GetTickCount()

Có thể kiểm tra thời gian hệ thống trước và sau một khối lệnh nhất định và giả sử rằng thời gian trôi qua được đo phải nhỏ hơn một giá trị nào đó. Nếu ứng dụng đang được phân tích, có khả năng các điểm dừng được đặt trong khối lệnh đó. Nếu vậy thì thời gian thực hiện sẽ vượt quá thời gian giả định.

![image](https://github.com/datvn09/2024_Training/assets/157048397/f0517b57-9cf9-4cec-8a72-bf1decfa6427)

7. Process Memory -- Hardware Breakpoints

Hardware Breakpoints có thể được phát hiện bằng cách kiểm tra các thanh ghi gỡ lỗi DR0 đến DR3:

![image](https://github.com/datvn09/2024_Training/assets/157048397/96ddef6c-e9a0-45ac-a1ac-7a8904270f06)

8. Assembly instructions -- DebugBreak

Có thể tạo breakpoint interrupt trong mã của mình, mã này sẽ được trình gỡ lỗi hiểu là điểm dừng phần mềm (giống như do người dùng đặt)

![image](https://github.com/datvn09/2024_Training/assets/157048397/c4775504-4e1f-4483-9fb7-36d1b9d30f81)

9. Direct debugger interaction -- Self-Debugging

Nếu một tiến trình đang được gỡ lỗi thì không thể đính kèm một trình gỡ lỗi khác vào nó. Để kiểm tra xem ứng dụng có được gỡ lỗi hay không bằng cách tận dụng thực tế này, cần bắt đầu một quy trình khác để cố gắng đính kèm vào ứng dụng.

![image](https://github.com/datvn09/2024_Training/assets/157048397/90078d42-a073-4328-bf97-52a121fa9dd3)

10. Misc -- DbgPrint()

Các hàm gỡ lỗi như ntdll!DbgPrint() và kernel32!OutputDebugStringW() gây ra ngoại lệ DBG_PRINTEXCEPTION_C (0x40010006). Nếu một chương trình được thực thi với trình gỡ lỗi đính kèm thì trình gỡ lỗi sẽ xử lý ngoại lệ này. Nhưng nếu không có trình gỡ lỗi và trình xử lý ngoại lệ được đăng ký, thì ngoại lệ này sẽ bị trình xử lý ngoại lệ bắt.

![image](https://github.com/datvn09/2024_Training/assets/157048397/cf9a383f-21d9-4efe-9271-e324039ca6cb)











