global _main
extern _printf
extern _scanf

section .data
    s1 db "Please enter a first number: ", 0
    s2 db "Please enter a second number: ", 0
    s3 db "The product of two numbers is: ", 0
    format db "%d", 0

section .bss
    num1 resb 1
    num2 resb 1
    result resb 2

section .text

_main:
    ; Setup stack frame
    push ebp
    mov ebp, esp

    ; in chuỗi s1
    push s1
    call _printf
    add esp, 4

    ; Nhập num1
    mov eax, num1 
    push eax
    push format     ; format ("%d")
    call _scanf
    add esp, 8

    ; in chuỗi s2
    push s2
    call _printf
    add esp, 4

    ; nhập num2
    mov ebx, num2
    push ebx
    push format     ; %d
    call _scanf
    add esp, 8


    ; tính tích
    mov bl, [num2] 
    mov  al, [num1]
	mul bl
    mov [result], eax

    ; in chuỗi s3
    push s3
    call _printf
    add esp, 4

    ; in kết quả 
    mov eax, [result]
    push eax 
	push format     
    call _printf
    add esp, 8
    
    ; Cleanup and exit
    mov esp, ebp
    pop ebp
    xor eax, eax
    ret
