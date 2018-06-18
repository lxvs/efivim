[Components]
	vim.inf

[PcdsFixedAtBuild]
	!if $(TARGET) == "DEBUG"
		DEFINE DEBUG_PRINT_ERROR_LEVEL	= 0x80000040
		DEFINE DEBUG_PROPERTY_MASK		= 0xFF
	!endif

!include edk2/AppPkg/AppPkg.dsc

[LibraryClasses]
	!if $(TARGET) == "DEBUG"
		DEFINE DEBUG_ENABLE_OUTPUT = TRUE
		SerialPortLib|PcAtChipsetPkg/Library/SerialIoLib/SerialIoLib.inf
		DebugLib|MdePkg/Library/BaseDebugLibSerialPort/BaseDebugLibSerialPort.inf
		DebugPrintErrorLevelLib|MdePkg/Library/BaseDebugPrintErrorLevelLib/BaseDebugPrintErrorLevelLib.inf
	!endif

