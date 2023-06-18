    .data
    .text
    .globl getint
getint:
    .globl getch
getch:
    .globl getarray
getarray:
    .globl putint
putint:
    .globl putch
putch:
    .globl putarray
putarray:
    .globl starttime
starttime:
    .globl stoptime
stoptime:
    .globl main
main:
    li a0, 0
    li a1, 1
    add a0, a0, a1
    li a0, 0
    li a1, 1
    add a0, a0, a1
    ret
