:start
esptool.exe --chip esp32s3   --baud 921600 --before default_reset --after hard_reset write_flash -z --flash_mode dio --flash_freq 80m --flash_size 4MB 0x0 T-QT-demo.ino.bootloader.bin 0x8000 T-QT-demo.ino.partitions.bin 0xe000 boot_app0.bin 0x10000 firmware.bin

@echo Press any key,keey find device praogram
pause
goto start

