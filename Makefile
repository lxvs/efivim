#TARGET=DEBUG
TARGET=RELEASE
ARCH=X64
TOOLCHAIN=GCC5
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
	qemu-system-x86_64 -bios $(OVMF) -enable-kvm -serial mon:stdio -net none -drive format=raw,file=$?

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
	bash -c 'pushd edk2; \
		export WORKSPACE="$(PWD)/edk2" PACKAGES_PATH="$(PWD):$(PWD)/edk2-libc"; \
		source ./edksetup.sh; \
		: hack to patch out -Werror for lua build; \
		sed -i -e "/GCC_ALL_CC_FLAGS/s/-Werror //g" Conf/tools_def.txt; \
		popd; \
		build -n 4 -a $(ARCH) -p $(PLATFORM) -m $(MODULE) -b $(TARGET) -t $(TOOLCHAIN); \
	'

.PHONY: vim
vim:	vim/src/auto/config.h

vim/src/auto/config.h:
	touch $@

edk2: .edk2
	touch $@

.edk2:
	make -C edk2/BaseTools
	touch $@

clean:
	rm -f $(EFIBIN) $(EFIIMG)
	rm -rf $(BUILDDIR)

nuke:	clean
	rm -f .edk2
	git -C edk2 clean -df
	rm -f vim/src/auto/config.h
	git -C vim clean -df

.PHONY:	qemu clean nuke

.DELETE_ON_ERROR:

