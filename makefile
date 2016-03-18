#******************************************************************************
#
# File:		makefile
#
# Project:	rcc		
#
# Micro:	STM32F103RB 
#
#******************************************************************************


# Target
TARGET = rcc

# Compiler
CC = arm-none-eabi-gcc


# Compiler options
CFLAGS = -Wall -mcpu=cortex-m3 -mlittle-endian -mthumb -DSTM32F103xE
# -Wall				Warnings, all.
# -mcpu				Specify processor, cortex-m3
# -mlittle-endian	Byte ordering
# -mthumb			Generate Thumb code. Cortex M3 doesn't support ARM mode.
# -D				Specify target processor (used in device header file).

# Linker options
LFLAGS = -mcpu=cortex-m3 -mlittle-endian -mthumb -DSTM32F103xE -lc -Wl,-Map=out.map
# -mcpu				Specify processor, cortex-m3
# -mlittle-endian	Byte ordering
# -mthumb			Generate Thumb code. Cortex M3 doesn't support ARM mode.
# -D				Specify target processor (used in device header file).
# -lc				Link with libc.a library
# -Wl,-Map=out.map	Generate map file

# Linker script
LK_SCRIPT = stm32f103_rcc.ld

 

# Include file directories
STM32CUBE_ROOT = /home/vince/STM32Cube_FW_F1
#STM32CUBE_ROOT = /home/pmatthews/stm32Cubef1
INCLUDE = $(STM32CUBE_ROOT)/Drivers/CMSIS/Device/ST/STM32F1xx/Include
INCLUDE_CORE = $(STM32CUBE_ROOT)//Drivers/CMSIS/Include

# Object files
OBJFILES = \
startup_rcc.o \
system.o \
main.o \
gpio.o \
uart.o \
timer.o \
spi.o \
cc2500_regs.o \
cc_hal.o \
adc.o \
keyscan.o \
led.o \
textio.o \
command.o \
pwm.o 



# Header files
HDRFILES = \
uart.h \
spi.h \
gpio.h \
adc.h \
led.h \
cc2500.h \
pwm.h 


# All target
all: $(TARGET).hex makefile
	@echo "Build complete"


# Obj to Hex
$(TARGET).hex : $(TARGET).elf
	@echo "Creating hex file" 
	arm-none-eabi-objcopy -Oihex $< $@
	@echo
	
# -Oihex		Create Intel HEX file
# $<			First dependancy


# Linking
$(TARGET).elf : $(OBJFILES) makefile $(LK_SCRIPT)
	@echo "Linking $@"
	$(CC) $(LFLAGS) -T$(LK_SCRIPT) -Wl,-gc-sections $(OBJFILES) -o $@  
	@echo 
	

# Compiling
# $@	Target
# $< 	First pre-requisite
# -Os	Optimise for speed
# -c	Compile only, don't link.


# Compile assembly code file 
%.o : %.s
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -Os -c $<
	@echo

# Compile a C-file
%.o : %.c
	@echo "Compiling $<"
	$(CC) $(CFLAGS) -I$(INCLUDE) -I$(INCLUDE_CORE) -Os -c $<
	@echo


.PHONY: clean
clean:
	@echo "clean"
	rm *.o
	rm *.elf
	
