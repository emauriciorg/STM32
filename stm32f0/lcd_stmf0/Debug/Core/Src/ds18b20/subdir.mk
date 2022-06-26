################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/ds18b20/ds18b20.c \
../Core/Src/ds18b20/onewire.c 

OBJS += \
./Core/Src/ds18b20/ds18b20.o \
./Core/Src/ds18b20/onewire.o 

C_DEPS += \
./Core/Src/ds18b20/ds18b20.d \
./Core/Src/ds18b20/onewire.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/ds18b20/%.o Core/Src/ds18b20/%.su: ../Core/Src/ds18b20/%.c Core/Src/ds18b20/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x8 -c -I../Core/Inc -I../Core/Inc/ds18b20 -I../Core/Inc/rtc -I../Core/lcd -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -Ofast -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-ds18b20

clean-Core-2f-Src-2f-ds18b20:
	-$(RM) ./Core/Src/ds18b20/ds18b20.d ./Core/Src/ds18b20/ds18b20.o ./Core/Src/ds18b20/ds18b20.su ./Core/Src/ds18b20/onewire.d ./Core/Src/ds18b20/onewire.o ./Core/Src/ds18b20/onewire.su

.PHONY: clean-Core-2f-Src-2f-ds18b20

