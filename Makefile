YASM		:= yasm
ARCH		:= x86_64-elf

CROSS_CC		:= $(ARCH)-gcc
CROSS_LD		:= $(ARCH)-ld
CROSS_OBJCOPY	:= $(ARCH)-objcopy
CROSS_AR		:= $(ARCH)-ar

BUILD_DIR		:= build

COMMON_CFLAGS		:= 	-Iinclude -L$(BUILD_DIR)/lib -pipe -MMD -ffreestanding -nostdlib \
						-msse -msse2 -z max-page-size=0x1000 -mno-red-zone               \
						-O2 -Wall -Wextra
BOOTLOADER_CFLAGS	:= $(COMMON_CFLAGS) -fno-builtin -fPIC -mcmodel=small
EFFEL_CFLAGS		:= $(COMMON_CFLAGS) -mcmodel=kernel -D__KERNEL=1

MAKE_VOLUME		:= tools/make_volume
VOLUME_FILE		:= Volume.txt

IMAGE			:= $(BUILD_DIR)/image.img

STAGE1			:= $(BUILD_DIR)/boot/stage1
STAGE2			:= $(BUILD_DIR)/boot/stage2
STAGE3			:= $(BUILD_DIR)/boot/stage3

STAGE3_MAIN		:= $(BUILD_DIR)/boot/stage3.o
STAGE3_LOADER	:= $(BUILD_DIR)/boot/stage3_loader.o

BOOTLOADER_VOLUME		:= $(BUILD_DIR)/boot/bootloader_volume
BOOTLOADER_PARTITION	:= $(BUILD_DIR)/boot/bootloader_partition
EFFEL					:= $(BUILD_DIR)/effel/effel
EFFEL_SRC				:= $(shell find src/effel -name '*.c')
EFFEL_OBJ				:= $(EFFEL_SRC:src/%.c=$(BUILD_DIR)/%.o)

LIBK					:= $(BUILD_DIR)/lib/libk.a
LIBK_SRC				:= $(shell find src/libc -name '*.c')
LIBK_OBJ				:= $(LIBK_SRC:src/libc/%.c=$(BUILD_DIR)/obj/libk/%.o)

DEPS					:= $(shell find $(BUILD_DIR) -name '*.d' 2>/dev/null)

.PHONY: all
all: image

-include $(DEPS)

.PHONY: clean
clean:
	rm -rf $(STAGE1)

.PHONY: mrproper
mrproper:
	rm -rf $(BUILD_DIR)

.PHONY: re
re: mrproper all

.PHONY: run
run: $(IMAGE)
	bochs -f bochs/bochsrc -q

.PHONY: image
image: $(IMAGE)

$(IMAGE): $(BOOTLOADER_VOLUME) $(BOOTLOADER_PARTITION) $(EFFEL) $(MAKE_VOLUME) $(VOLUME_FILE)
	@mkdir -p $(dir $@)
	$(MAKE_VOLUME) $(VOLUME_FILE) $@

$(BOOTLOADER_VOLUME): $(STAGE1)
	@mkdir -p $(dir $@)
	cp $< $@

$(BOOTLOADER_PARTITION): $(STAGE2) $(STAGE3)
	@mkdir -p $(dir $@)
	cat $(STAGE2) $(STAGE3) > $@

$(STAGE1): src/boot/stage1.S
	@mkdir -p $(dir $@)
	$(YASM) -f bin -o $@ $<

$(STAGE2): src/boot/stage2.S
	@mkdir -p $(dir $@)
	$(YASM) -f bin -o $@ $<

$(STAGE3_LOADER): src/boot/stage3_loader.S
	@mkdir -p $(dir $@)
	$(YASM) -f elf64 -o $@ $<

$(STAGE3_MAIN): src/boot/stage3.c
	@mkdir -p $(dir $@)
	$(CROSS_CC) $(BOOTLOADER_CFLAGS) -c $< -o $@

$(STAGE3): $(STAGE3_LOADER) $(STAGE3_MAIN) src/boot/stage3.ld
	@mkdir -p $(dir $@)
	$(CROSS_CC) $(BOOTLOADER_CFLAGS) -T src/boot/stage3.ld $(STAGE3_LOADER) $(STAGE3_MAIN) -lgcc -o $@.elf
	$(CROSS_OBJCOPY) -O binary $@.elf $@

$(BUILD_DIR)/effel/%.o: src/effel/%.c
	@mkdir -p $(dir $@)
	$(CROSS_CC) $(EFFEL_CFLAGS) -I src/effel -c $< -o $@

$(EFFEL): $(EFFEL_OBJ) $(LIBK)
	@mkdir -p $(dir $@)
	$(CROSS_CC) $(EFFEL_CFLAGS) -T src/effel/effel.ld $(EFFEL_OBJ) -lk -lgcc -o $@

$(LIBK): $(LIBK_OBJ)
	@mkdir -p $(dir $@)
	$(CROSS_AR) rcs $@ $(LIBK_OBJ)

$(BUILD_DIR)/obj/libk/%.o: src/libc/%.c
	@mkdir -p $(dir $@)
	$(CROSS_CC) $(EFFEL_CFLAGS) -c $< -o $@
