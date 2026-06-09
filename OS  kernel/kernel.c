#include <stdint.h>

// VGAテキストモードのメモリ番地
#define VGA_ADDRESS 0xB8000
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// ビルドフラグによってOS名を切り替える（Makefileで設定することを想定）
#if defined(OS_BUBBLES)
    const char* OS_NAME = "bubblesOS v0.1.0-alpha";
    const uint8_t OS_COLOR = 0x1F; // 青背景 / 白文字
#elif defined(OS_PYS) // PysOSは黒背景/灰文字
    const char* OS_NAME = "PysOS v0.1.0-alpha";
    const uint8_t OS_COLOR = 0x07; // 黒背景 / 灰文字
#else
    const char* OS_NAME = "PolyglotOS v0.1.0-alpha"; // 統合版は青背景/黄文字
    const uint8_t OS_COLOR = 0x1E; 
#endif

// I/Oポートから1バイト読み込む関数
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

// I/Oポートに1バイト書き込む関数
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

// ハードウェアカーソルを移動する関数
void move_cursor(int x, int y) {
    uint16_t pos = y * SCREEN_WIDTH + x;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// 画面をクリアする関数
void clear_screen() {
    uint16_t* vga = (uint16_t*)VGA_ADDRESS;
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        vga[i] = (uint16_t)OS_COLOR << 8 | ' ';
    }
    move_cursor(0, 0);
}

// 指定した場所に文字を表示する関数
void print_string(int x, int y, const char* str) {
    uint16_t* vga = (uint16_t*)VGA_ADDRESS;
    int offset = y * SCREEN_WIDTH + x;

    for (int i = 0; str[i] != '\0'; i++) {
        vga[offset + i] = (uint16_t)OS_COLOR << 8 | str[i];
    }
}

// 1文字を表示して座標を進める関数
void print_char(int *x, int *y, char c) {
    uint16_t* vga = (uint16_t*)VGA_ADDRESS;
    if (c == '\n') {
        *x = 0;
        (*y)++;
    } else if (c == '\b') { // Backspace
        if (*x > 0) (*x)--;
        vga[(*y) * SCREEN_WIDTH + (*x)] = (uint16_t)OS_COLOR << 8 | ' ';
    } else {
        vga[(*y) * SCREEN_WIDTH + (*x)] = (uint16_t)OS_COLOR << 8 | c;
        (*x)++;
        if (*x >= SCREEN_WIDTH) {
            *x = 0;
            (*y)++;
        }
    }
}

// スキャンコードをASCII文字に変換する簡易テーブル
char scancode_to_ascii(uint8_t scancode) {
    static char map[] = {
        0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
        '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
        0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,
        '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' '
    };
    if (scancode < sizeof(map)) {
        return map[scancode];
    }
    return 0;
}

// 文字列比較関数 (C標準ライブラリがないため自作)
int strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// ロゴとOS情報を再表示する関数
void show_boot_info() {
    print_string(20, 3, "  ____       _             _       _    ___  ____  ");
    print_string(20, 4, " |  _ \\ ___ | |_   _  __ _| | ___ | |_ / _ \\/ ___| ");
    print_string(20, 5, " | |_) / _ \\| | | | |/ _` | |/ _ \\| __| | | \\___ \\ ");
    print_string(20, 6, " |  __/ (_) | | |_| | (_| | | (_) | |_| |_| |___) |");
    print_string(20, 7, " |_|   \\___/|_|\\__, |\\__, |_|\\___/ \\__|\\___/|____/ ");
    print_string(20, 8, "               |___/ |___/                          ");

    print_string(20, 10, "========================================");
    print_string(20, 11, OS_NAME);
    print_string(20, 12, "========================================");
    print_string(18, 13, "Type 'php' or 'python' to start engine.");
}

/**
 * 言語エンジンの実行（両方利用可能にする）
 */
extern void start_php_engine();
extern void start_python_engine();

/**
 * カーネルのエントリポイント
 * boot.asm から呼び出されます
 */
void kernel_main(void) {
    clear_screen();

    // GRUBの残骸が絶対に残らないよう、数回改行を挟むのと同様の効果を持たせる
    move_cursor(0, 0);
    show_boot_info();

#if defined(OS_BUBBLES)
    start_php_engine();
#elif defined(OS_PYS)
    start_python_engine();
#endif

    print_string(20, 14, "System Initialized.");

    // コマンドライン・シェル環境の初期化
    int cursor_x = 0;
    int cursor_y = 23; // 画面の下の方から入力を開始
    print_string(cursor_x, cursor_y, "> ");
    cursor_x = 2;

    char cmd_buf[64];
    int cmd_len = 0;

    while (1) {
        // キーボードコントローラのステータスを確認
        if (inb(0x64) & 0x01) {
            uint8_t scancode = inb(0x60);
            
            // キーが押された時（離された時は scancode > 0x80）
            if (!(scancode & 0x80)) {
                char c = scancode_to_ascii(scancode);
                if (c > 0) {
                    if (c == '\n') {
                        cmd_buf[cmd_len] = '\0';
                        cursor_x = 0;
                        cursor_y++;
                        if (cursor_y >= SCREEN_HEIGHT) cursor_y = 23;

                        // ==================================================
                        // ここに新しいコマンドを追加できます！
                        // ==================================================
                        if (strcmp(cmd_buf, "help") == 0) {
                            print_string(0, cursor_y, "Commands: help, cls, halt, reboot, info, php, python");
                            cursor_y++;
                        } else if (strcmp(cmd_buf, "cls") == 0) {
                            clear_screen();
                            show_boot_info();
                            cursor_y = 22; // 次の改行で23行目へ
                        } else if (strcmp(cmd_buf, "halt") == 0) {
                            print_string(0, cursor_y, "System halted safely.");
                            __asm__ volatile("cli; hlt");
                        } else if (strcmp(cmd_buf, "reboot") == 0) {
                            print_string(0, cursor_y, "Rebooting...");
                            // キーボードコントローラを介してリセット信号を送る (絶対に再起動させる)
                            uint8_t temp;
                            do {
                                temp = inb(0x64);
                            } while (temp & 0x02);
                            outb(0x64, 0xFE);
                        } else if (strcmp(cmd_buf, "php") == 0) {
                            start_php_engine();
                        } else if (strcmp(cmd_buf, "python") == 0) {
                            start_python_engine();
                        } else if (strcmp(cmd_buf, "info") == 0) {
                            print_string(0, cursor_y, "Kernel: Polyglot C-Core v1.0, Mode: i386-Protected");
                        } else if (cmd_len > 0) {
                            print_string(0, cursor_y, "Unknown C-Command.");
                        }

                        cursor_y++;
                        if (cursor_y >= SCREEN_HEIGHT) cursor_y = 23;
                        print_string(0, cursor_y, "> ");
                        cursor_x = 2;
                        cmd_len = 0;
                    } else if (c == '\b') {
                        if (cmd_len > 0) {
                            cmd_len--;
                            print_char(&cursor_x, &cursor_y, '\b');
                        }
                    } else if (cmd_len < 63) {
                        cmd_buf[cmd_len++] = c;
                        print_char(&cursor_x, &cursor_y, c);
                    }
                }
            }
        }
        
        // CPU負荷を下げるために少し待機
        __asm__ volatile("pause");
    }
}