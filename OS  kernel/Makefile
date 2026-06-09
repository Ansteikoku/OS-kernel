# コンパイラとツールの設定
# 注意: ターゲット環境に合わせて i686-elf-gcc などのクロスコンパイラを使用してください
CC = i686-elf-gcc
AS = nasm
LD = i686-elf-gcc
GRUB_RESCUE = i686-elf-grub-mkrescue

# コンパイルフラグ
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -m32
LDFLAGS = -ffreestanding -O2 -nostdlib -lgcc -T linker.ld

# ターゲット
all: polyglot.iso

# バイナリのビルド設定
bubblesos.bin: boot.o kernel_bubbles.o engine.o
	$(LD) $(LDFLAGS) -o $@ $^

pysos.bin: boot.o kernel_pys.o engine.o
	$(LD) $(LDFLAGS) -o $@ $^

polyglot.bin: boot.o kernel_polyglot.o engine.o
	$(LD) $(LDFLAGS) -o $@ $^

# 個別オブジェクトのコンパイル
kernel_bubbles.o: kernel.c
	$(CC) -c $< -o $@ $(CFLAGS) -DOS_BUBBLES

kernel_pys.o: kernel.c
	$(CC) -c $< -o $@ $(CFLAGS) -DOS_PYS

kernel_polyglot.o: kernel.c
	$(CC) -c $< -o $@ $(CFLAGS)

engine.o: engine.c
	$(CC) -c $< -o $@ $(CFLAGS)

boot.o: boot.asm
	$(AS) -felf32 $< -o $@

# ISOイメージの作成
polyglot.iso: bubblesos.bin pysos.bin polyglot.bin grub.cfg
	rm -rf isodir
	mkdir -p isodir/boot/grub
	cp bubblesos.bin isodir/boot/
	cp pysos.bin isodir/boot/
	cp polyglot.bin isodir/boot/
	cp grub.cfg isodir/boot/grub/grub.cfg
	$(GRUB_RESCUE) -o polyglot.iso isodir
	rm -rf isodir # ISO作成後に作業ディレクトリを完全に削除
	@echo "----------------------------------------------------------"
	@echo "SUCCESS: polyglot.iso has been created."
	@echo "UTM Note: Disable 'UEFI Boot', use 'i440FX' machine,"
	@echo "          and turn OFF 'USB Support' for keyboard input."
	@echo "If the keyboard doesn't work in UTM, please use: make run"
	@echo "----------------------------------------------------------"

clean:
	rm -f *.o *.bin *.iso grub.cfg.bak
	rm -rf isodir isodir_bubbles isodir_pys

# 実行コマンド (QEMUを使用)
run: polyglot.iso 
	qemu-system-i386 -cdrom polyglot.iso -vga std