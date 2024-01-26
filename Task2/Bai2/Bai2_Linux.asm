section .data
   s1 db 'Please enter a first number: ', 0xA, 0
   len1 equ $ - s1
   s2 db 'Please enter a second number: ', 0xA, 0
   len2 equ $ - s2
   s3 db 'The sum of two numbers is: ', 0xA, 0
   len3 equ $ - s3

section .bss
   num1 resb 100
   num2 resb 100
   len_num1 resb 1
   len_num2 resb 2
   sum  resb 100
   result resb 100

section .text
   global _start
_start:

    ; in chuoi s1
    mov eax, 4
    mov ebx, 1
    mov ecx, s1
    mov edx, len1
    int 0x80

    ; nhap so thu nhat
    mov eax, 3
    mov ebx, 0  
    mov ecx, num1
    mov edx, 100  
    int 0x80

    ; in chuoi s2
    mov eax, 4
    mov ebx, 1
    mov ecx, s2
    mov edx, len2
    int 0x80

    ; nhap so thu 2
    mov eax, 3
    mov ebx, 0  
    mov ecx, num2
    mov edx, 100 
    int 0x80

    ; in chuoi s3
    mov eax, 4
    mov ebx, 1
    mov ecx, s3
    mov edx, len3
    int 0x80

    ; calculate length of num1 and num2
    lea eax, num1
    call len_num
    xor edi, edi
    mov edi, ecx

    lea eax, num2
    call len_num
    xor esi, esi 
    mov esi, ecx
    call _sum           ; tính tổng

    xor eax, eax
    xor ecx, ecx
    mov eax, sum
    mov ecx, result     ; lấy kết quả để in
    call _result

    mov eax, 4
    mov ebx, 1
    mov ecx, result 
    mov edx, 100
    int 0x80

    mov eax, 1
    xor ebx, ebx 
    int 0x80

len_num:
   mov bl, byte [eax + esi]
   cmp bl, 0xA
   jne  not_equal
   mov ecx, esi 
   ret
not_equal:
   inc esi
   jmp len_num

_sum:
    xor ecx, ecx    
    xor edx, edx
    xor ebx, ebx
    sub edi, 1
    sub esi, 1
    mov al, 0
    mov ebx, 99
    mov byte [sum + ebx], 0xA
    dec ebx
    jmp _loop

_loop:
    mov cl, byte [num1 + edi]    
    mov dl, byte [num2+ esi]    
    sub cl, 48
    sub dl, 48
    add cl, dl
    add cl, al
    mov al, 0           
    cmp cl, 10
    jge _loop1
_loop2:
    add cl, 48
    mov byte [sum + ebx], cl
    xor ecx, ecx
    dec ebx
    dec edi
    dec esi
    cmp byte [num1 + edi], 0
    je _after2
    cmp byte [num2 + esi], 0
    je _after3
    jmp _loop
    
_loop1:
    sub cl, 10
    mov al, 1
    jmp _loop2

_after2:
    cmp byte [num2+ esi], 0
    je end
    mov dl, byte [num2+ esi]
    sub dl, 48
    add dl, al
    mov al, 0
    add dl, 48
    mov byte [sum + ebx], dl
    dec ebx
    dec esi
    jmp _after2

_after3:
    mov cl, byte [num2+ edi]
    sub cl, 48
    add cl, al
    mov al, 0
    add cl, 48
    mov byte [sum + ebx], cl
    dec ebx
    dec edi
    cmp byte [num2+ edi], 0
    je end
    jmp _after3

end:
    cmp al, 1
    je _after4
    ret
_after4:
    add al, 48
    mov byte [sum + ebx], al
    ret

_result:
    xor esi, esi
    xor edx, edx
    xor ebx, ebx
    mov ebx, 100
    xor edi, edi
_loop3:
    mov dl, byte [eax + edi]
    cmp dl, 0
    je _after
    cmp byte [eax + edi], 0xA
    je the_end
    mov byte [ecx + esi], dl
    inc ecx
    inc edi
    jmp _loop3

_after:
    inc edi 
    cmp edi, ebx
    je the_end
    jmp _loop3
the_end:
    ret

    

