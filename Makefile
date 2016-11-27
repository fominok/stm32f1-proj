TARGET=main
EXECUTABLE=$(TARGET).elf

export CC=arm-none-eabi-gcc
export LD=arm-none-eabi-gcc
export AR=arm-none-eabi-ar
export AS=arm-none-eabi-as
export CP=arm-none-eabi-objcopy
export OD=arm-none-eabi-objdump

export MCFLAGS = -mcpu=cortex-m3 -mthumb -mlittle-endian -mthumb-interwork
export OPTIMIZE = -Os
export DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -DHSE_VALUE=8000000

all: $(TARGET).bin

# Select the appropriate option for your device, the available options are listed below
# with a description copied from stm32f10x.h
# Make sure to set the startup code file to the right device family, too!
#
# STM32F10X_LD 		STM32F10X_LD: STM32 Low density devices
# STM32F10X_LD_VL	STM32F10X_LD_VL: STM32 Low density Value Line devices
# STM32F10X_MD		STM32F10X_MD: STM32 Medium density devices
# STM32F10X_MD_VL	STM32F10X_MD_VL: STM32 Medium density Value Line devices 
# STM32F10X_HD		STM32F10X_HD: STM32 High density devices
# STM32F10X_HD_VL	STM32F10X_HD_VL: STM32 High density value line devices
# STM32F10X_XL		STM32F10X_XL: STM32 XL-density devices
# STM32F10X_CL		STM32F10X_CL: STM32 Connectivity line devices 
#
# - Low-density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers
#   where the Flash memory density ranges between 16 and 32 Kbytes.
# 
# - Low-density value line devices are STM32F100xx microcontrollers where the Flash
#   memory density ranges between 16 and 32 Kbytes.
# 
# - Medium-density devices are STM32F101xx, STM32F102xx and STM32F103xx microcontrollers
#   where the Flash memory density ranges between 64 and 128 Kbytes.
# 
# - Medium-density value line devices are STM32F100xx microcontrollers where the 
#   Flash memory density ranges between 64 and 128 Kbytes.   
# 
# - High-density devices are STM32F101xx and STM32F103xx microcontrollers where
#   the Flash memory density ranges between 256 and 512 Kbytes.
# 
# - High-density value line devices are STM32F100xx microcontrollers where the 
#   Flash memory density ranges between 256 and 512 Kbytes.   
# 
# - XL-density devices are STM32F101xx and STM32F103xx microcontrollers where
#   the Flash memory density ranges between 512 and 1024 Kbytes.
# 
# - Connectivity line devices are STM32F105xx and STM32F107xx microcontrollers.
#
# HSE_VALUE sets the value of the HSE clock, 8MHz in this case 

STARTUP = lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md_vl.s

STM32_INCLUDES = -Ilib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	-Ilib/CMSIS/CM3/CoreSupport/ \
	-Ilib/STM32F10x_StdPeriph_Driver/inc/

CFLAGS = $(MCFLAGS) $(OPTIMIZE) $(DEFS) -Iinc $(STM32_INCLUDES) \
	 -Wall -Wextra -Wpedantic

LD_FLAGS = -lm -lc -lnosys -Wl,-T,stm32_flash.ld

FILES = main \
	delay \
	I2C \
	LiquidCrystal_I2C \
	USART \
	kbd \
	rtc \
	eeprom \
	system_stm32f10x

SRC = $(addprefix src/, $(addsuffix .c, $(FILES)))
OBJ = $(addprefix obj/, $(addsuffix .o, $(FILES)))

obj/%.o: src/%.c
	$(CC) $(CFLAGS) $< -c -o $@

STMLIB = lib/STM32F10x_StdPeriph_Driver

$(STMLIB)/lib.o :
	$(MAKE) -C $(STMLIB) lib.o

$(TARGET).bin: $(EXECUTABLE)
	$(CP) -O binary $^ $@

$(EXECUTABLE): $(OBJ) $(STMLIB)/lib.o $(STARTUP)
	$(CC) $(MCFLAGS) $^ $(LD_FLAGS) -s -o $@

clean:
	rm -f $(TARGET) $(OBJ) $(TARGET).bin $(EXECUTABLE)

mrproper:
	$(MAKE) clean
	$(MAKE) -C lib/STM32F10x_StdPeriph_Driver clean

.PHONY: all clean mrproper

