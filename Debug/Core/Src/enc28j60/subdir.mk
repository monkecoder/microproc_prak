################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/enc28j60/arp.c \
../Core/Src/enc28j60/enc28j60.c \
../Core/Src/enc28j60/ethernet.c \
../Core/Src/enc28j60/icmp.c \
../Core/Src/enc28j60/ip.c 

OBJS += \
./Core/Src/enc28j60/arp.o \
./Core/Src/enc28j60/enc28j60.o \
./Core/Src/enc28j60/ethernet.o \
./Core/Src/enc28j60/icmp.o \
./Core/Src/enc28j60/ip.o 

C_DEPS += \
./Core/Src/enc28j60/arp.d \
./Core/Src/enc28j60/enc28j60.d \
./Core/Src/enc28j60/ethernet.d \
./Core/Src/enc28j60/icmp.d \
./Core/Src/enc28j60/ip.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/enc28j60/%.o Core/Src/enc28j60/%.su: ../Core/Src/enc28j60/%.c Core/Src/enc28j60/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F410Rx -c -I../Core/Inc -I"C:/Users/Ilya/Documents/stm32 workspace/microproc_prak/Core/Inc/ssd1306" -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -I"C:/Users/Ilya/Documents/stm32 workspace/microproc_prak/Core/Inc/enc28j60" -I"C:/Users/Ilya/Documents/stm32 workspace/microproc_prak/Core/Src/enc28j60" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-enc28j60

clean-Core-2f-Src-2f-enc28j60:
	-$(RM) ./Core/Src/enc28j60/arp.d ./Core/Src/enc28j60/arp.o ./Core/Src/enc28j60/arp.su ./Core/Src/enc28j60/enc28j60.d ./Core/Src/enc28j60/enc28j60.o ./Core/Src/enc28j60/enc28j60.su ./Core/Src/enc28j60/ethernet.d ./Core/Src/enc28j60/ethernet.o ./Core/Src/enc28j60/ethernet.su ./Core/Src/enc28j60/icmp.d ./Core/Src/enc28j60/icmp.o ./Core/Src/enc28j60/icmp.su ./Core/Src/enc28j60/ip.d ./Core/Src/enc28j60/ip.o ./Core/Src/enc28j60/ip.su

.PHONY: clean-Core-2f-Src-2f-enc28j60

