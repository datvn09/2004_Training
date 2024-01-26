## Bài 3:
### Yêu Cầu
Chương trình asm: tìm index của substring (giống string.find(substring) python)
- Nhập 2 chuỗi: string A, substring B
- Tìm vị trí khớp đầu tiên của B trong A
- In kết quả
Code trên Windows và Linux (tùy chọn x86, x64)

#### Trên Linux: [Bai3_Linux.asm](https://github.com/datvn09/2004_Training/blob/main/Task2/Bai3/Bai3_Linux.asm)
##### Code
```
section .data
    input_message db "Nhap chuoi A: ", 0xA, 0
    len1 equ $ - input_message
    input_message_b db "Nhap chuoi B: ", 0xA, 0
    len2 equ $ - input_message_b
    output_message db "Vi tri dau tien cua B trong A la: ",  0xA, 0
    len3 equ $ - output_message
    not_found_message db "Khong tim thay B trong A.",  0xA, 0
    len4 equ $ - not_found_message

section .bss
    A resb 100
    B resb 100
    result resb 2

section .text
    global _start

_start:
    ; in chuỗi input_message
    mov eax, 4
    mov ebx, 1
    mov ecx, input_message
    mov edx, len1
    int 0x80

    ; nhập chuỗi A
    mov eax, 3
    mov ebx, 0
    mov ecx, A
    mov edx, 100
    int 0x80

    ; in chuỗi input_message_b
    mov eax, 4
    mov ebx, 1
    mov ecx, input_message_b
    mov edx, len2
    int 0x80

    ; nhập chuỗi B
    mov eax, 3
    mov ebx, 0
    mov ecx, B
    mov edx, 100
    int 0x80

    ; Gọi hàm để tìm vị trí của B trong A
    lea eax, A
    lea ebx, B
    call find_substring

    ; In vị trí đầu tiên hoặc thông báo không tìm thấy
    cmp ecx, -1
    je not_found
    mov eax, ecx
    mov esi, result
    call __to_string        ; chuyển kết quả sang kiểu str để in ra màn hình

    ; In chuôi output_message
    mov eax, 4
    mov ebx, 1
    mov ecx, output_message
    mov edx, len3
    int 0x80

    ; in kết quả
    mov eax, 4
    mov ebx, 1
    mov ecx, result 
    mov edx, 2
    int 0x80
    jmp end_program         ; gọi hàm end

not_found:
    mov eax, 4
    mov ebx, 1
    mov ecx, not_found_message
    mov edx, len4
    int 0x80

end_program:
    ; Kết thúc chương trình
    mov eax, 1
    xor ebx, ebx
    int 0x80

find_substring:
    ; Hàm tìm vị trí đầu tiên của chuỗi con trong chuỗi cha
    ; Input: eax - con trỏ đến chuỗi cha (A)
    ;        ebx - con trỏ đến chuỗi con (B)
    ; Output: ecx - vị trí đầu tiên của B trong A (nếu tìm thấy), -1 nếu không tìm thấy

    xor ecx, ecx    ; ecx sẽ lưu vị trí của chuỗi con trong chuỗi cha
    xor esi, esi
    xor edi, edi
    xor edx, edx
    jmp search_loop

search_loop:
    mov cl, byte [eax + edi]     ; Load byte from A   
    mov dl, byte [ebx + esi]     ; Load byte from B
    cmp cl, dl                  ; so sánh bytes
    jne not_equal               ; nhảy nếu không bằng

continue_search:
    inc edi                 ; Tăng vị trí của chuỗi cha
    inc esi                 ; Tăng vị trí của chuỗi con
    cmp byte [ebx + esi], 0xA ; kiểm tra chuỗi B để hết hay chưa
    je found                ; nhảy nếu B end
    jmp search_loop         ; Continue the loop

not_equal:
    inc edi                 ; Tăng vị trí của chuỗi cha
    test cl, cl             ; Kiểm tra chuỗi A hết hay chưa
    jnz search_loop         ; nhảy nếu chuỗi A chưa end
    xor ecx, ecx
    mov ecx, -1             ; Chuỗi con không tìm thấy trong chuỗi cha --> gán ecx =-1 rồi out hàm
    ret

found: 
    xor ecx, ecx
    sub edi, esi     ; edi-esi để tìm vị trí đầu tiên chuỗi B xuất hiện trong A
    mov ecx, edi     ; Nếu tìm thấy, lưu vị trí của chuỗi con trong chuỗi cha vào eax
    ret

__to_string:
    push eax

    mov ecx, 1
    mov edi, 1
    mov ebx, 10
    get_divisor:        ; vd eax=123/10-->eax=12, ecx=1*10-->...-> ecx=100
        xor edx, edx    ; 1234--> ecx=1000
        div ebx
        
        cmp eax, 0      ; khi nào eax = 0 thì nhảy
        je _after
        imul ecx, 10
        inc edi
        jmp get_divisor
    _after:
        pop eax
        push edi

    to_string:
        xor edx, edx
        div ecx 
        add al, 48
        mov byte[esi], al       ; result là 1 str được chuyển từ số sang theo thứ tự từ trái sang phải
        inc esi

        push edx                ; đưa số dư vào ngăn xếp
        xor edx, edx
        mov eax, ecx
        mov ebx, 10
        div ebx
        mov ecx, eax
        
        pop eax                ; lấy số dư ra

        cmp ecx, 0
        jg to_string
    pop edx    
    ret
```
##### Biên dịch chương trình:
`nasm -f elf32 -o Bai3_Linux.o Bai3_Linux.asm`

`ld -m elf_i386 -o Bai3_Linux Bai3_Linux.o`
##### Test case 
![image](https://github.com/datvn09/2004_Training/assets/157048397/fa07bce8-6824-462d-aa72-ad4790f3d9d0)

![image](https://github.com/datvn09/2004_Training/assets/157048397/10ec5432-e9be-45da-97c0-5adec223ae65)

![image](https://github.com/datvn09/2004_Training/assets/157048397/6f477f24-9419-4f0e-a194-a9337ab3a0e9)

#### Trên Windows 
##### Code [Bai3_Windows.asm](https://github.com/datvn09/2004_Training/blob/main/Task2/Bai3/Bai3_Windows.asm)
```
global _main
extern _printf
extern _scanf

section .data
    input_message db "Nhap chuoi A: ", 0xA, 0
    input_message_b db "Nhap chuoi B: ", 0xA, 0
    output_message db "Vi tri dau tien cua B trong A la: ",  0xA, 0
    not_found_message db "Khong tim thay B trong A.",  0xA, 0
    format db "%s", 0
    format_result db "%d", 0

section .bss
    A resb 100
    B resb 100
    result resb 2

section .text

_main:
    ; Setup stack frame
    push ebp
    mov ebp, esp

    ; in chuỗi input_message
    push input_message
    call _printf
    add esp, 4

    ; nhập chuỗi A
    mov eax, A
    push eax
    push format         ; %s
    call _scanf
    add esp, 8

    ; in chuỗi input_message_b
    push input_message_b
    call _printf
    add esp, 4

    ; nhập chuỗi B
    mov eax, B
    push eax
    push format         ; %s
    call _scanf
    add esp, 8

    ; Gọi hàm để tìm vị trí của B trong A
    lea eax, [A]
    lea ebx, [B]
    call find_substring

    ; In vị trí đầu tiên hoặc thông báo không tìm thấy
    cmp ecx, -1
    je not_found
    mov [result], ecx

    ; In chuỗi output_message
    mov eax, output_message
    push eax
    call _printf
    add esp, 4

    ; in kết quả nếu tìm thấy vị trí
    mov eax, [result] 
    push eax
    push format_result
    call _printf
    add esp, 8

    jmp end_program

not_found:
    mov eax, not_found_message
    push eax
    call _printf
    add esp, 4

end_program:
    ; Kết thúc chương trình
    mov esp, ebp
    pop ebp
    xor eax, eax
    ret

find_substring:
    xor ecx, ecx  
    xor esi, esi
    xor edi, edi
    xor edx, edx
    jmp search_loop

search_loop:
    mov cl, byte [eax + edi]     ; Load byte from A   
    mov dl, byte [ebx + esi]     ; Load byte from B
    cmp cl, dl             
    jne not_equal           

continue_search:
    inc edi
    inc esi                 
    cmp byte [ebx + esi], 0
    je found              
    jmp search_loop         

not_equal:
    inc edi                 
    test cl, cl             
    jnz search_loop         
    xor ecx, ecx
    mov ecx, -1            
    ret

found: 
    xor ecx, ecx
    sub edi, esi
    mov ecx, edi     ; Nếu tìm thấy, lưu vị trí của chuỗi con trong chuỗi cha vào ecx
    ret

```

##### Biên dịch chương trình
`nasm -f win32 -o Bai3_Windows.o Bai3_Windows.asm`

`gcc -m32 -o Bai3_Windows Bai3_Windows.o`
##### Test case

![image](https://github.com/datvn09/2004_Training/assets/157048397/1b3fa911-3cfc-4dc4-bba5-ac730a85839f)

![image](https://github.com/datvn09/2004_Training/assets/157048397/384c9039-bf85-4855-8011-e954ca9bb3ed)










