TARGET=DEBUG
ARCH=X64
TOOLCHAIN=GCC49
PLATFORM=vim.dsc
MODULE=vim.inf

SGDISK=/usr/sbin/sgdisk

EFIBIN=vim.efi
EFIIMG=vim.img
BUILDDIR=edk2/Build/vim/$(TARGET)_$(TOOLCHAIN)/$(ARCH)

APP=$(BUILDDIR)/vim/$(TARGET)/$(EFIBIN)

$(EFIBIN):	$(APP)
	cp $? $@

qemu: $(EFIIMG)
	qemu-system-x86_64 -bios efi-bios.bin -enable-kvm -serial mon:stdio -net none -display sdl -drive format=raw,file=$?

$(EFIIMG):	$(APP)
	# 48M
	test -e $@ || (dd if=/dev/zero of=$@ bs=512 count=93750 2>/dev/null && \
		$(SGDISK) -Z $@ >/dev/null && \
		$(SGDISK) -N 1 $@ >/dev/null && \
		$(SGDISK) -t 1:ef00 $@ >/dev/null && \
		$(SGDISK) -c 1:"EFI" $@ >/dev/null && \
		$(SGDISK) -v $@ >/dev/null && \
		$(SGDISK) -p $@ && \
		mformat -i $@@@1M -v EFI -F -h 32 -t 44 -n 64 -c 1 && \
		mmd -i $@@@1M efi && \
		mmd -i $@@@1M efi/boot \
	)
	mcopy -o -i $@@@1M $(APP) ::efi/boot/bootx64.efi
	touch $@

$(APP):	edk2 vim
	pushd edk2; WORKSPACE="$(PWD)/edk2" PACKAGES_PATH="$(PWD)" source ./edksetup.sh; popd; build -v -a $(ARCH) -p $(PLATFORM) -m $(MODULE) -b $(TARGET) -t $(TOOLCHAIN)

vim:
	git clone https://github.com/vim/vim.git
	# we didn't run autoconf, so make the source cope.
	touch vim/src/auto/config.h

edk2:
	git clone https://github.com/tianocore/edk2.git
	cd edk2
	make -C edk2/BaseTools

clean:
	rm -f $(EFIBIN)
	rm -rf $(BUILDDIR)

nuke:	clean
	rm -rf vim edk2

.PHONY:	qemu clean nuke

.DELETE_ON_ERROR:

