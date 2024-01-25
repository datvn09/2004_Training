## Bài 1:
### Yêu Cầu
Chương trình asm: nhân 2 số (không tràn thanh ghi, thừa số trong đoạn [0, 2^32 - 1] với 32 bit, [0, 2^64 - 1] với 64 bit):
- Cho phép nhập 2 số
- Tính nhân
- In kết quả
Code trên Windows và Linux (tùy chọn x86, x64)

#### Trên Linux: [Bai1_Linux](https://github.com/datvn09/2004_Training/blob/main/Task2/Bai1/Bai1_Linux.asm)
##### Code
```
section .data
s1 db 'Please enter a first number: ', 0xA, 0
lens1 equ $ - s1
s2 db 'Please enter a second number: ', 0xA, 0
lens2 equ $ - s2
s3 db 'The product of two numbers is: ', 0xA, 0
lens3 equ $ - s3

section .bss
num1 resb 11
num2 resb 11
result resb 50

section .text
    global _start   
	
_start:            
    ; in chuỗi s1
    mov eax, 4
    mov ebx, 1
    mov ecx, s1
    mov edx, lens1
    int 0x80

    ; nhập số thứ nhất
    mov eax, 3
    mov ebx, 0  
    mov ecx, num1
    mov edx, 11  
    int 0x80

    ; in chuỗi s2
    mov eax, 4
    mov ebx, 1
    mov ecx, s2
    mov edx, lens2
    int 0x80

   ; nhập số thứ 2
    mov eax, 3
    mov ebx, 0  
    mov ecx, num2
    mov edx, 11 
    int 0x80

    ; in chuỗi s3
    mov eax, 4
    mov ebx, 1
    mov ecx, s3
    mov edx, lens3
    int 0x80

    ; chuyển chuỗi num1 nhập từ bàn phím thành số nguyên
    mov eax, num1
    push eax
    call str2int32      ; hàm chuyển str to int
    mov ebx, eax 

    ; chuyển chuỗi num2 nhập từ bàn phím thành số nguyên
    mov eax, num2
    push eax
    call str2int32

    ; tính tích num1 và num2
    mul ebx                 ; kết quả được lưu edx:eax
    mov esi, result         ; chuẩn giá trị tính được thành chuỗi để chuẩn bị xuất ra màn hình
    call __to_string        ; gọi hàm chuyển int to string

    ; in kết quả ra màn hình
    mov eax, 4
    mov ebx, 1
    mov ecx, result
    int 0x80

    ; Exit
    mov eax, 1   ; system call number (sys_exit)
    int 0x80     ; call kernel

str2int32:
    push ebp
    mov ebp, esp
    xor eax, eax
    xor edx, edx 
    mov esi, [ebp + 8]      ; gán tham số truyền vào cho esi
    jmp loop

loop:
    movzx ecx, byte [esi]   ; lấy từng byte của chuỗi
    cmp ecx, 10             ; kiểm tra kết thúc chuỗi hay chưa
    je  end                 ; nếu chuỗi kết thúc gọi hàm end
    cmp ecx, 0              ; kiểm tra kết thúc chuỗi hay chưa
    je  end
    sub ecx, '0'            ; đổi từng byte từ string to int bằng cách trừ '0'
    imul eax, 10            ; nhân 10. ví dụ num1='123'-->ecx=1,eax=0-->eax=1,ecx=2-->eax=1*10+2=12...
    add eax, ecx            
    inc esi
    cmp ecx, 9
    jbe loop
    jmp end

end:                        ; eax=((0*10+1)*10+2)*10+3  vd với num1='123'
    pop ebp
    ret

div_q64:
    push ecx
    push eax
    mov ecx, dword [esp + 12] ; the first parameter
    mov eax, edx
    xor edx, edx
    div ecx
    push eax
    mov eax, dword [esp + 4]
    div ecx
    pop edx
    add esp, 4
    pop ecx
    ret

remainder_q64:
    push ecx
    push eax
    mov ecx, dword [esp + 12] ; the first parameter
    mov eax, edx
    xor edx, edx
    div ecx
    push eax
    mov eax, dword [esp + 4]
    div ecx
    mov eax, edx
    add esp, 8
    pop ecx
    ret
    
__to_string:
    push eax        ; bit thấp
    push edx        ; bit cao

    mov edi, 1
    push 10

    calc_len:
        call div_q64
        
        cmp eax, 0
        jne calc_len_cont
        cmp edx, 0
        je _after

    calc_len_cont:
        inc edi
        jmp calc_len

    _after:
        add esp, 4
        pop edx
        pop eax
        push edi

        mov ebx, 10

    to_string:
        dec edi

        push edx
        push eax
        push 0xa
        call remainder_q64
        add esp, 4
        add al, 48
        mov byte [esi + edi], al

        pop eax
        pop edx
        push ebx
        call div_q64
        add esp, 4
        
        cmp edi, 0
        jg to_string
    
    pop edx
    ret
```
##### Biên dịch chương trình:
`nasm -f elf32 -o Bai1_Linux.o Bai1_Linux.asm`
`ld -m elf_i386 -o Bai1_Linux Bai1_Linux.o`
##### Test case 
![image](https://github.com/datvn09/2004_Training/assets/157048397/10c45bb4-140c-429e-a3e4-2d25ffc94c41)
![image](https://github.com/datvn09/2004_Training/assets/157048397/48418d78-f636-41e3-adb6-73c529586348)
![image](https://github.com/datvn09/2004_Training/assets/157048397/b3c9c3b3-43dc-4239-8420-b9a6d557948f)







