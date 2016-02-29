[Defines]
	PLATFORM_NAME		= vim
	PLATFORM_GUID		= 8b2d8f69-bbd8-44cf-85ed-913e0eb5cf73
	PLATFORM_VERSION	= 0.01
	DSC_SPECIFICATION	= 0x00010006
	OUTPUT_DIRECTORY	= Build/vim
	SUPPORTED_ARCHITECTURES	= IA32|X64|ARM|AARCH64
	BUILD_TARGETS		= DEBUG|RELEASE|NOOPT
	SKUID_IDENTIFIER	= DEFAULT

[LibraryClasses]
#
# Entry Point Libraries
#
	UefiApplicationEntryPoint|MdePkg/Library/UefiApplicationEntryPoint/UefiApplicationEntryPoint.inf
	ShellCEntryLib|ShellPkg/Library/UefiShellCEntryLib/UefiShellCEntryLib.inf
	UefiDriverEntryPoint|MdePkg/Library/UefiDriverEntryPoint/UefiDriverEntryPoint.inf
#
# Common Libraries
#
	BaseLib|MdePkg/Library/BaseLib/BaseLib.inf
	BaseMemoryLib|MdePkg/Library/BaseMemoryLib/BaseMemoryLib.inf
	UefiLib|MdePkg/Library/UefiLib/UefiLib.inf
	PrintLib|MdePkg/Library/BasePrintLib/BasePrintLib.inf
	PcdLib|MdePkg/Library/BasePcdLibNull/BasePcdLibNull.inf
	MemoryAllocationLib|MdePkg/Library/UefiMemoryAllocationLib/UefiMemoryAllocationLib.inf
	UefiBootServicesTableLib|MdePkg/Library/UefiBootServicesTableLib/UefiBootServicesTableLib.inf
	UefiRuntimeServicesTableLib|MdePkg/Library/UefiRuntimeServicesTableLib/UefiRuntimeServicesTableLib.inf
	!if $(DEBUG_ENABLE_OUTPUT)
		DebugLib|MdePkg/Library/UefiDebugLibConOut/UefiDebugLibConOut.inf
		DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
	!else   ## DEBUG_ENABLE_OUTPUT
		DebugLib|MdePkg/Library/BaseDebugLibNull/BaseDebugLibNull.inf
	!endif  ## DEBUG_ENABLE_OUTPUT

	DevicePathLib|MdePkg/Library/UefiDevicePathLib/UefiDevicePathLib.inf
	PeCoffGetEntryPointLib|MdePkg/Library/BasePeCoffGetEntryPointLib/BasePeCoffGetEntryPointLib.inf
	IoLib|MdePkg/Library/BaseIoLibIntrinsic/BaseIoLibIntrinsic.inf
	PciLib|MdePkg/Library/BasePciLibCf8/BasePciLibCf8.inf
	PciCf8Lib|MdePkg/Library/BasePciCf8Lib/BasePciCf8Lib.inf
	SynchronizationLib|MdePkg/Library/BaseSynchronizationLib/BaseSynchronizationLib.inf
	UefiRuntimeLib|MdePkg/Library/UefiRuntimeLib/UefiRuntimeLib.inf
	HiiLib|MdeModulePkg/Library/UefiHiiLib/UefiHiiLib.inf
	UefiHiiServicesLib|MdeModulePkg/Library/UefiHiiServicesLib/UefiHiiServicesLib.inf
	PerformanceLib|MdeModulePkg/Library/DxePerformanceLib/DxePerformanceLib.inf
	HobLib|MdePkg/Library/DxeHobLib/DxeHobLib.inf
	FileHandleLib|MdePkg/Library/UefiFileHandleLib/UefiFileHandleLib.inf
	SortLib|MdeModulePkg/Library/UefiSortLib/UefiSortLib.inf

	ShellLib|ShellPkg/Library/UefiShellLib/UefiShellLib.inf

	CacheMaintenanceLib|MdePkg/Library/BaseCacheMaintenanceLib/BaseCacheMaintenanceLib.inf

[Components]
	vim.inf

!include StdLib/StdLib.inc

