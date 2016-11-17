TARGET=main
EXECUTABLE=$(TARGET).elf

CC=arm-none-eabi-gcc
LD=arm-none-eabi-gcc
AR=arm-none-eabi-ar
AS=arm-none-eabi-as
CP=arm-none-eabi-objcopy
OD=arm-none-eabi-objdump

DEFS = -DUSE_STDPERIPH_DRIVER -DSTM32F10X_MD_VL -DHSE_VALUE=8000000
STARTUP = lib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/startup/gcc_ride7/startup_stm32f10x_md_vl.s

MCU = cortex-m3
MCFLAGS = -mcpu=$(MCU) -mthumb -mlittle-endian -mthumb-interwork

STM32_INCLUDES = -Ilib/CMSIS/CM3/DeviceSupport/ST/STM32F10x/ \
	-Ilib/CMSIS/CM3/CoreSupport/ \
	-Ilib/STM32F10x_StdPeriph_Driver/inc/

OPTIMIZE       = -Os

CFLAGS	= $(MCFLAGS)  $(OPTIMIZE)  $(DEFS) -I. -Iinc -I./ $(STM32_INCLUDES)
LDFLAGS =  -lm -lc -lnosys -Wl,-Tstm32_flash.ld
AFLAGS	= $(MCFLAGS)

OBJDIR = build

SRC_PROJ = src/main.c \
	src/delay.c \
  src/I2C.c \
  src/LiquidCrystal_I2C.c \
  src/USART.c \
	src/system_stm32f10x.c

OBJ_PROJ = $(OBJDIR)/main.o \
	$(OBJDIR)/delay.o \
  $(OBJDIR)/I2C.o \
  $(OBJDIR)/LiquidCrystal_I2C.o \
  $(OBJDIR)/USART.o \
	$(OBJDIR)/system_stm32f10x.o

SRC_LIB = lib/STM32F10x_StdPeriph_Driver/src/misc.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_adc.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_bkp.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_can.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_cec.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_crc.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_dac.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_dbgmcu.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_dma.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_exti.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_flash.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_fsmc.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_gpio.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_i2c.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_iwdg.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_pwr.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_rcc.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_rtc.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_sdio.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_spi.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_tim.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_usart.c \
	lib/STM32F10x_StdPeriph_Driver/src/stm32f10x_wwdg.c

OBJ_LIB = $(OBJDIR)/lib/misc.o \
	$(OBJDIR)/lib/stm32f10x_adc.o \
	$(OBJDIR)/lib/stm32f10x_bkp.o \
	$(OBJDIR)/lib/stm32f10x_can.o \
	$(OBJDIR)/lib/stm32f10x_cec.o \
	$(OBJDIR)/lib/stm32f10x_crc.o \
	$(OBJDIR)/lib/stm32f10x_dac.o \
	$(OBJDIR)/lib/stm32f10x_dbgmcu.o \
	$(OBJDIR)/lib/stm32f10x_dma.o \
	$(OBJDIR)/lib/stm32f10x_exti.o \
	$(OBJDIR)/lib/stm32f10x_flash.o \
	$(OBJDIR)/lib/stm32f10x_fsmc.o \
	$(OBJDIR)/lib/stm32f10x_gpio.o \
	$(OBJDIR)/lib/stm32f10x_i2c.o \
	$(OBJDIR)/lib/stm32f10x_iwdg.o \
	$(OBJDIR)/lib/stm32f10x_pwr.o \
	$(OBJDIR)/lib/stm32f10x_rcc.o \
	$(OBJDIR)/lib/stm32f10x_rtc.o \
	$(OBJDIR)/lib/stm32f10x_sdio.o \
	$(OBJDIR)/lib/stm32f10x_spi.o \
	$(OBJDIR)/lib/stm32f10x_tim.o \
	$(OBJDIR)/lib/stm32f10x_usart.o \
	$(OBJDIR)/lib/stm32f10x_wwdg.o


OBJ = $(SRC:%.c=$(OBJDIR)/%.o) 
OBJ += Startup.o

all: $(TARGET).bin

$(TARGET).bin: $(EXECUTABLE)
	$(CP) -O binary $^ $@

$(EXECUTABLE): $(OBJ_PROJ) $(OBJ_LIB) $(OBJDIR)/startup.o
	$(CC) $(LDFLAGS) $^ -o $@

$(OBJDIR)/lib/%.o: lib/STM32F10x_StdPeriph_Driver/src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/startup.o: $(STARTUP)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f Startup.lst  $(TARGET)  $(TARGET).lst $(OBJ) $(AUTOGEN)  $(TARGET).out  $(TARGET).bin  $(TARGET).map \
	 $(TARGET).dmp  $(EXECUTABLE)
