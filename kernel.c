#include "kernel.h"
#include "common.h"

typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];



struct sbiret sbi_call(long arg0, long arg1, long arg2, long arg3,
                      long arg4, long arg5, long fid, long eid){
    register long a0 __asm__("a0") = arg0;
    register long a1 __asm__("a1") = arg1;
    register long a2 __asm__("a2") = arg2;
    register long a3 __asm__("a3") = arg3;
    register long a4 __asm__("a4") = arg4;
    register long a5 __asm__("a5") = arg5;
    register long a6 __asm__("a6") = fid;
    register long a7 __asm__("a7") = eid;

    __asm__ __volatile__("ecall"
                         : "=r"(a0), "=r"(a1)
                         : "r"(a0), "r"(a1), "r"(a2), "r"(a3), 
                           "r"(a4), "r"(a5), "r"(a6), "r"(a7)
                         : "memory");
    
    return (struct sbiret){.error = a0, .value = a1};
};

void putchar(char ch){
    sbi_call(ch, 0, 0, 0, 0, 0, 0, 1); //Legacy SBI Console Putchar
}

void kernel_main(void){
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);
    PANIC("booted!");
    printf("unreachable here\n");
}

//リンカで指定されたプログラムの開始地点になる関数
__attribute__((section(".text.boot")))  //この関数を.text.boot(隣家で指定された開始地点)というセクションに置くということ
__attribute__((naked))                  //関数呼び出し時のsp(スタックポインタ)の初期化を避ける。これは、この関数内で初めてspを初期化するため
void boot(void) {
    __asm__ __volatile__(
        "mv sp, %[stack_top]\n"
        "j kernel_main\n"
        :
        : [stack_top] "r" (__stack_top)
    );
}