all: lib.o

CC=arm-none-eabi-gcc
AR=arm-none-eabi-ar

OPTIMIZE = -Os

CMIS_PATH = ../CMSIS/CM3
CONFIG_PATH = ../../inc

MCFLAGS = -mcpu=cortex-m3 -mthumb -mlittle-endian -mthumb-interwork

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -DHSE_VALUE=8000000

CFLAGS = $(MCFLAGS) $(OPTIMIZE) $(DEFS) -I./inc \
	-I$(CMIS_PATH)/DeviceSupport/ST/STM32F10x \
	-I$(CMIS_PATH)/CoreSupport \
	-I$(CONFIG_PATH) \
	-Wno-int-to-pointer-cast -Wno-pointer-to-int-cast

FILES = misc \
	stm32f10x_adc \
	stm32f10x_bkp \
	stm32f10x_can \
	stm32f10x_cec \
	stm32f10x_crc \
	stm32f10x_dac \
	stm32f10x_dbgmcu \
	stm32f10x_dma \
	stm32f10x_exti \
	stm32f10x_flash \
	stm32f10x_fsmc \
	stm32f10x_gpio \
	stm32f10x_i2c \
	stm32f10x_iwdg \
	stm32f10x_pwr \
	stm32f10x_rcc \
	stm32f10x_rtc \
	stm32f10x_sdio \
	stm32f10x_spi \
	stm32f10x_tim \
	stm32f10x_usart \
	stm32f10x_wwdg \

SRC = $(addprefix src/, $(addsuffix .c, $(FILES)))
OBJ = $(addprefix obj/, $(addsuffix .o, $(FILES)))

obj/%.o: src/%.c inc/%.h $(CONFIG_PATH)/stm32f10x_conf.h
	$(CC) $(CFLAGS) $< -c -o $@

lib.o: $(OBJ)
	$(AR) cr $@ $^

clean:
	rm -f lib.o $(OBJ)

.PHONY: all clean

