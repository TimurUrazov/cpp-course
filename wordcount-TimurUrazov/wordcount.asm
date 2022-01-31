sys_exit:       equ             60
                section         .text
                global          _start

buf_size:       equ             8192
_start:
                xor             rbx, rbx
                xor             r8, r8
                sub             rsp, buf_size
                mov             rsi, rsp

read_again:
                xor             rax, rax
                xor             rdi, rdi
                mov             rdx, buf_size
                syscall

                test            rax, rax
                jz              quit
                js              read_error

                xor             rcx, rcx

check_char:
                cmp             rcx, rax
                je              read_again
                mov             r9, [rsi + rcx]

check_space:
                cmp             r9b, 0x20
                je              check_flag
                cmp             r9b, 0x09
                jb              flag_incr
                cmp             r9b, 0x0d
                ja              flag_incr

check_flag:
                test            r8, r8
                jz              skip

ascend:
                inc             rbx
                xor             r8, r8

skip:
                inc             rcx
                jmp             check_char

flag_incr:
                mov             r8, 1
                jmp             skip

quit:
                add             rbx, r8
                mov             rax, rbx
                call            print_int

                mov             rax, sys_exit
                xor             rdi, rdi
                syscall

print_int:
                mov             rsi, rsp
                mov             rbx, 10

                dec             rsi
                mov             byte [rsi], 0x0a

next_char:
                xor             rdx, rdx
                div             rbx
                add             dl, '0'
                dec             rsi
                mov             [rsi], dl
                test            rax, rax
                jnz             next_char

                mov             rax, 1
                mov             rdi, 1
                mov             rdx, rsp
                sub             rdx, rsi
                syscall

                ret

read_error:
                mov             rax, 1
                mov             rdi, 2
                mov             rsi, read_error_msg
                mov             rdx, read_error_len
                syscall

                mov             rax, sys_exit
                mov             rdi, 1
                syscall

                section         .rodata

read_error_msg: db              "read failure", 0x0a
read_error_len: equ             $ - read_error_msg
