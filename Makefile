YASM		:= yasm

MKIMAGE		:= tools/mkimage

BUILD_DIR	:= build

IMAGE		:= $(BUILD_DIR)/image.img
MBR			:= $(BUILD_DIR)/boot/mbr

.PHONY: all
all: image

.PHONY: clean
clean:
	rm -rf $(MBR)

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

$(IMAGE): $(MBR) $(MKIMAGE)
	@mkdir -p $(dir $@)
	$(MKIMAGE) $@ $(MBR)

.PHONY: mbr
mbr: $(MBR)

$(MBR): src/boot/mbr.S
	@mkdir -p $(dir $@)
	$(YASM) -f bin -o $@ $<
