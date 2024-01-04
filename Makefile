CC = gcc
AS = nasm
LD = ld

CFLAGS += -O2 -Wall -Wextra -Wno-unused-variable -Wno-overlength-strings -std=gnu17 \
				 -ffreestanding -fno-stack-protector -fno-strict-aliasing \
				 -fno-stack-check -fno-lto  -fPIE \
				 -m64 -march=x86-64 -mno-80387 \
				 -mno-mmx \
				 -mno-red-zone

LDFLAGS += -m elf_x86_64 -nostdlib \
					-static -pie --no-dynamic-linker \
					-z text -z max-page-size=0x1000 \
					-T link.ld

ASFLAGS = -felf64

BIN = kernel.elf
ISO = bf64.iso
OBJ_FOLDER = obj
SRC_FOLDER = src

C_SRCS := $(shell find src/ -type f -name '*.c')
AS_SRCS := $(shell find src/ -type f -name '*.s')

C_OBJS	:= $(subst $(SRC_FOLDER), $(OBJ_FOLDER), $(C_SRCS:.c=.o))
AS_OBJS	:= $(subst $(SRC_FOLDER), $(OBJ_FOLDER), $(AS_SRCS:.s=.o))

all: $(BIN)

$(BIN): $(C_OBJS) $(AS_OBJS)
	@printf "  LD\t$(C_OBJS) $(AS_OBJS)\n"
	@$(LD) $(LDFLAGS) $(C_OBJS) $(AS_OBJS) -o $(BIN)

$(C_OBJS): $(C_SRCS) $(OBJ_FOLDER)
	@printf "  CC\t$(subst $(OBJ_FOLDER), $(SRC_FOLDER), $(@:.o=.c))\n"
	@$(CC) $(CFLAGS) -c $(subst $(OBJ_FOLDER), $(SRC_FOLDER), $(@:.o=.c)) -o $@

$(AS_OBJS): $(AS_SRCS) $(OBJ_FOLDER)
	@printf "  AS\t$(subst $(OBJ_FOLDER), $(SRC_FOLDER), $(@:.o=.s))\n"
	@$(AS) $(ASFLAGS) $(subst $(OBJ_FOLDER), $(SRC_FOLDER), $(@:.o=.s)) -o $@

iso: $(ISO)
$(ISO): $(BIN) limine
	make -C limine
	mkdir -p isodir 
	cp -v $(BIN) limine.cfg limine/limine-bios.sys \
		limine/limine-bios-cd.bin limine/limine-uefi-cd.bin isodir/
	mkdir -p isodir/EFI/BOOT 
	cp -v limine/BOOTX64.EFI isodir/EFI/BOOT/
	cp -v limine/BOOTIA32.EFI isodir/EFI/BOOT/
	xorriso -as mkisofs -b limine-bios-cd.bin \
		-no-emul-boot -boot-load-size 4 -boot-info-table \
		--efi-boot limine-uefi-cd.bin \
		-efi-boot-part --efi-boot-image --protective-msdos-label \
		isodir -o $(ISO)
	./limine/limine bios-install $(ISO)

raw: $(BIN) limine
	rm bf64.bin
	dd if=/dev/zero bs=1M count=0 seek=64 of=bf64.bin
	sgdisk bf64.bin -n 1:2048 -t 1:ef00
	./limine/limine bios-install bf64.bin
	mformat -i bf64.bin@@1M
	mmd -i bf64.bin@@1M ::/EFI ::/EFI/BOOT
	mcopy -i bf64.bin@@1M $(BIN) limine.cfg limine/limine-bios.sys ::/
	mcopy -i bf64.bin@@1M limine/BOOTX64.EFI ::/EFI/BOOT
	mcopy -i bf64.bin@@1M limine/BOOTIA32.EFI ::/EFI/BOOT

limine:
	git clone https://github.com/limine-bootloader/limine.git --branch=v5.x-branch-binary --depth=1 

$(OBJ_FOLDER):
	@printf "  MKDIR\t$(OBJ_FOLDER)/\n"
	@mkdir -p $(OBJ_FOLDER)

run: test
test: $(ISO)
	qemu-system-x86_64 -enable-kvm -m 4096 -smp 4 -cdrom $(ISO)

clean:
	@printf "  RM\t $(BIN)\n"
	@rm $(BIN) | true
	@printf "  RM\t $(OBJ_FOLDER)/\n"
	@rm -r $(OBJ_FOLDER) | true
	@printf "  RM\t $(ISO)\n"
	@rm $(ISO) | true
	@printf "  RM\t isodir/\n"
	@rm -r isodir | true
	@printf "  RM\t bf64.bin\n"
	@rm bf64.bin | true

