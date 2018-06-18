#TARGET=DEBUG
TARGET=RELEASE
ARCH=X64
TOOLCHAIN=GCC49
PLATFORM=vim.dsc
MODULE=vim.inf

SGDISK=/sbin/sgdisk
OVMF=/usr/share/ovmf/OVMF.fd

EFIBIN=vim.efi
EFIIMG=vim.img
BUILDDIR=edk2/Build/AppPkg/$(TARGET)_$(TOOLCHAIN)/$(ARCH)

APP=$(BUILDDIR)/$(EFIBIN)

$(EFIBIN):	$(APP)
	cp $? $@

qemu: $(EFIIMG)
	qemu-system-x86_64 -bios $(OVMF) -enable-kvm -serial mon:stdio -net none -display sdl -drive format=raw,file=$?

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
		mmd -i $@@@1M efi/tools \
	)
	mcopy -o -i $@@@1M $(APP) ::efi/tools/vim.efi
	touch $@

$(APP):	edk2 vim
	bash -c 'pushd edk2; export WORKSPACE="$(PWD)/edk2" PACKAGES_PATH="$(PWD)"; source ./edksetup.sh; popd; build -v -a $(ARCH) -p $(PLATFORM) -m $(MODULE) -b $(TARGET) -t $(TOOLCHAIN) '

vim:
	git clone https://github.com/vim/vim.git
	cd vim && git checkout 0366c0161e988e32420d2f37111a60129684905b
	# we didn't run autoconf, so make the source cope.
	touch vim/src/auto/config.h

edk2:
	git clone https://github.com/tianocore/edk2.git
	cd edk2 && git checkout 1f739a851ce8ea8c9c4d9c4c7a5862fd44ab6ab4
	make -C edk2/BaseTools

clean:
	rm -f $(EFIBIN) $(EFIIMG)
	rm -rf $(BUILDDIR)

nuke:	clean
	rm -rf vim edk2

.PHONY:	qemu clean nuke

.DELETE_ON_ERROR:

