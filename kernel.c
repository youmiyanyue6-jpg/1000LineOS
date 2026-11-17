typedef unsigned char uint8_t;
typedef unsigned int uint32_t;
typedef uint32_t size_t;

extern char __bss[], __bss_end[], __stack_top[];
uint32_t c;

//アドレスbufからnバイトcをメモリに書き込む
void *memset(void *buf, char c, size_t n) {
    uint8_t *p = (uint8_t *) buf;
    while (n--)
        *p++ = c;
    return buf;
}

//アドレス__bssから__bss_endまで0を書き込む。
//bss領域は未初期化あるいは0初期化のグローバルあるいはstatic変数なのでこのようにする
void kernel_main(void) {
    memset(__bss, 0, (size_t) __bss_end - (size_t) __bss);

    for (;;);
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