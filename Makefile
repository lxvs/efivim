TARGET=DEBUG
ARCH=X64
TOOLCHAIN=GCC49
PLATFORM=vim.dsc
MODULE=vim.inf

EFIBIN=vim.efi
BUILDDIR=edk2/Build/vim/$(TARGET)_$(TOOLCHAIN)/$(ARCH)

APP=$(BUILDDIR)/vim/$(TARGET)/$(EFIBIN)

$(EFIBIN):	$(APP)
	cp $? $@

qemu: $(APP)
	qemu-system-x86_64 -bios efi-bios.bin -enable-kvm -serial mon:stdio -net none -display sdl -drive file=fat:$(BUILDDIR)/vim/$(TARGET)

$(APP):	edk2 vim
	pushd edk2; WORKSPACE="$(PWD)/edk2" PACKAGES_PATH="$(PWD)" source ./edksetup.sh; popd; build -v -a $(ARCH) -p $(PLATFORM) -m $(MODULE) -b $(TARGET) -t $(TOOLCHAIN)

vim:
	git clone https://github.com/vim/vim.git
	patch -d vim -p1 < efivim.patch
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

