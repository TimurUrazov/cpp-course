                section         .text

                global          _start
_start:

                sub             rsp, 4 * 128 * 8
                lea             rdi, [rsp + 3 * 128 * 8]
                mov             rcx, 128
                call            read_long
                lea             rdi, [rsp + 2 * 128 * 8]
                call            read_long
                lea             rsi, [rsp + 3 * 128 * 8]

                mov             r15, rsp
                sub             rsp, 129 * 8
                mov             r14, rsp

                call            mul_long_long
                
                mov             rdi, r15
                mov             rcx, 256
                call            write_long

                mov             al, 0x0a
                call            write_char

                jmp             exit

; multiplies two numbers
;    rdi -- address of multiplier #1 (long number)
;    rsi -- address of multiplier #2 (long number)
;    rcx -- length of long numbers in qwords
;    r14 -- address of intermediate computation buffer (multiplication of #1 and short number in #2)
; result:
;    multiplication is written to r15

mul_long_long:
                push            rsp
                push            rdi
                push            rsi
                push            rcx

                xor             r10, r10
                mov             r9, rcx

                push            rdi
                add             rcx, rcx
                mov             rdi, r15
                call            set_zero
                pop             rdi
                mov             rcx, r9
                mov             r8, rdi

.loop:          
                inc             rcx
                mov             rdi, r14
                call            set_zero
                mov             rdi, r8
                dec             rcx
                
                mov             rbx, [rsi]
                call            mul_long_short
                lea             rsi, [rsi + 8]
                call            add_long_long
                
                inc             r10
                dec             r9
                jnz             .loop

                pop             rcx
                pop             rsi
                pop             rdi
                pop             rsp
                ret

; adds two long numbers with shift
;    r14 -- address of summand #1 (long number)
;    r15 -- address of summand #2 (long number)
;    r10 -- size of shift in qwords
;    rcx -- length of long numbers in qwords
; result:
;    sum is written to r15
add_long_long:
                push            r14
                push            r15
                push            rax
                push            rcx
                inc             rcx
                clc

                lea             r15, [r15 + 8 * r10]
.loop:
                mov             rax, [r14]
                lea             r14, [r14 + 8]
                adc             [r15], rax
                lea             r15, [r15 + 8]
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             r15
                pop             r14
                ret

; adds 64-bit number to long number
;    rdi -- address of summand #1 (long number)
;    rax -- summand #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    sum is written to rdi
add_long_short:
                push            rdi
                push            rcx
                push            rdx

                xor             rdx,rdx
.loop:
                add             [rdi], rax
                adc             rdx, 0
                mov             rax, rdx
                xor             rdx, rdx
                add             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rdx
                pop             rcx
                pop             rdi
                ret

; multiplies long number by a short
;    rdi -- address of multiplier #1 (long number)
;    rbx -- multiplier #2 (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    product is written to r14
mul_long_short:
                push            rsi
                push            rax
                push            rdi
                push            r14
                push            rcx

                xor             rsi, rsi
.loop:
                mov             rax, [rdi]
                mul             rbx
                add             rax, rsi
                adc             rdx, 0
                mov             [r14], rax
                add             rdi, 8
                add             r14, 8
                mov             rsi, rdx
                dec             rcx
                jnz             .loop
                adc             [r14], rdx

                pop             rcx
                pop             r14
                pop             rdi
                pop             rax
                pop             rsi
                ret


; divides long number by a short
;    rdi -- address of dividend (long number)
;    rbx -- divisor (64-bit unsigned)
;    rcx -- length of long number in qwords
; result:
;    quotient is written to rdi
;    rdx -- remainder
div_long_short:
                push            rdi
                push            rax
                push            rcx

                lea             rdi, [rdi + 8 * rcx - 8]
                xor             rdx, rdx

.loop:
                mov             rax, [rdi]
                div             rbx
                mov             [rdi], rax
                sub             rdi, 8
                dec             rcx
                jnz             .loop

                pop             rcx
                pop             rax
                pop             rdi
                ret

; assigns a zero to long number
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
set_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep stosq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; checks if a long number is a zero
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
; result:
;    ZF=1 if zero
is_zero:
                push            rax
                push            rdi
                push            rcx

                xor             rax, rax
                rep scasq

                pop             rcx
                pop             rdi
                pop             rax
                ret

; read long number from stdin
;    rdi -- location for output (long number)
;    rcx -- length of long number in qwords
read_long:
                push            rcx
                push            rdi

                call            set_zero

                mov             r14, rdi
.loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              .done
                cmp             rax, '0'
                jb              .invalid_char
                cmp             rax, '9'
                ja              .invalid_char

                sub             rax, '0'
                mov             rbx, 10
                call            mul_long_short
                call            add_long_short
                jmp             .loop

.done:
                pop             rdi
                pop             rcx
                ret

.invalid_char:
                mov             rsi, invalid_char_msg
                mov             rdx, invalid_char_msg_size
                call            print_string
                call            write_char
                mov             al, 0x0a
                call            write_char

.skip_loop:
                call            read_char
                or              rax, rax
                js              exit
                cmp             rax, 0x0a
                je              exit
                jmp             .skip_loop

; write long number to stdout
;    rdi -- argument (long number)
;    rcx -- length of long number in qwords
write_long:
                push            rax
                push            rcx

                mov             rax, 20
                mul             rcx
                mov             rbp, rsp
                sub             rsp, rax

                mov             rsi, rbp

.loop:
                mov             rbx, 10
                call            div_long_short
                add             rdx, '0'
                dec             rsi
                mov             [rsi], dl
                call            is_zero
                jnz             .loop

                mov             rdx, rbp
                sub             rdx, rsi
                call            print_string

                mov             rsp, rbp
                pop             rcx
                pop             rax
                ret

; read one char from stdin
; result:
;    rax == -1 if error occurs
;    rax \in [0; 255] if OK
read_char:
                push            rcx
                push            rdi

                sub             rsp, 1
                xor             rax, rax
                xor             rdi, rdi
                mov             rsi, rsp
                mov             rdx, 1
                syscall

                cmp             rax, 1
                jne             .error
                xor             rax, rax
                mov             al, [rsp]
                add             rsp, 1

                pop             rdi
                pop             rcx
                ret
.error:
                mov             rax, -1
                add             rsp, 1
                pop             rdi
                pop             rcx
                ret

; write one char to stdout, errors are ignored
;    al -- char
write_char:
                sub             rsp, 1
                mov             [rsp], al

                mov             rax, 1
                mov             rdi, 1
                mov             rsi, rsp
                mov             rdx, 1
                syscall
                add             rsp, 1
                ret

exit:
                mov             rax, 60
                xor             rdi, rdi
                syscall

; print string to stdout
;    rsi -- string
;    rdx -- size
print_string:
                push            rax

                mov             rax, 1
                mov             rdi, 1
                syscall

                pop             rax
                ret


                section         .rodata
invalid_char_msg:
                db              "Invalid character: "
invalid_char_msg_size: equ             $ - invalid_char_msg
