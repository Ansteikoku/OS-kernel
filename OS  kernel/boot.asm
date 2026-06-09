; =============================================================================
; bubblesOS & PysOS Bootloader (core/boot.asm)
; =============================================================================

; Multiboot ヘッダーの定義
MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
FLAGS    equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; スタック領域の確保 (16KB)
section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

; エントリポイント
section .text
global _start:function (_start.end - _start)
_start:
    ; スタックポインタの設定
    mov esp, stack_top

    ; C言語のカーネルメイン関数を呼び出し
    extern kernel_main
    call kernel_main

    ; kernel_main から戻った場合はCPUを停止
    cli
.hang:
    hlt
    jmp .hang
.end: