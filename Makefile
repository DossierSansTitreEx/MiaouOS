YASM		:= yasm

MKIMAGE		:= tools/mkimage

BUILD_DIR	:= build

IMAGE		:= $(BUILD_DIR)/image.img
STAGE1		:= $(BUILD_DIR)/boot/stage1

.PHONY: all
all: image

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

$(IMAGE): $(STAGE1) $(MKIMAGE)
	@mkdir -p $(dir $@)
	$(MKIMAGE) $@ $(STAGE1)

.PHONY: mbr
mbr: $(STAGE1)

$(STAGE1): src/boot/stage1.S
	@mkdir -p $(dir $@)
	$(YASM) -f bin -o $@ $<
