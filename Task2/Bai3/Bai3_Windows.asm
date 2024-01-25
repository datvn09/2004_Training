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
    add esp, 4

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

