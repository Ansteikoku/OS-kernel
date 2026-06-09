#include <stdint.h>

// kernel.c の関数を使えるように宣言
extern void print_string(int x, int y, const char* str);
void os_bridge_print(const char* message);

/**
 * bubblesOS (PHP) 用のエンジン開始処理
 */
void start_php_engine() {
    print_string(20, 17, "[bubblesOS] Running /init.php...");

    // C言語由来の起動ログを追加
    os_bridge_print("[INFO] GDT Base: 0x00000800, IDT Base: 0x00000000");
    os_bridge_print("[DEBUG] Kernel stack initialized at 0x00090000");

    // PHPエンジンの代わりに、init.php の実行結果をシミュレート
    os_bridge_print("bubblesOS boot sequence started.");
    os_bridge_print("[OK] Memory: 1024KB detected");
    os_bridge_print("[OK] Display: VGA Text Mode initialized");
    os_bridge_print("[OK] Drivers: Standard Keyboard loaded");
    os_bridge_print("Starting Bubble Shell...");
    os_bridge_print("Welcome to bubblesOS!");
}

/**
 * PysOS (Python) 用のエンジン開始処理
 */
void start_python_engine() {
    print_string(20, 17, "[PysOS] Running /init.py...");

    // C言語由来の起動ログを追加
    os_bridge_print("[INFO] CPU Feature Check: i686-Protected Mode");
    os_bridge_print("[DEBUG] CR0 register status: 0x80000011");

    // Pythonエンジンの代わりに、init.py の実行結果をシミュレート
    os_bridge_print("PysOS v0.1.0 initialization...");
    os_bridge_print("Starting vfs service... Done.");
    os_bridge_print("Starting proc service... Done.");
    os_bridge_print("Starting input service... Done.");
    os_bridge_print("PysOS Ready.");
}

/**
 * 将来、スクリプトから呼ばれるC言語の関数
 */
void os_bridge_print(const char* message) {
    static int line = 15;
    // シェルエリア（23行目以降）を絶対に侵食しないように制御
    if (line >= 23) line = 15;
    print_string(2, line++, message);
}