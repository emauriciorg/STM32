################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/rtc/rtc_1307.c \
../Core/Src/rtc/rtc_api.c 

OBJS += \
./Core/Src/rtc/rtc_1307.o \
./Core/Src/rtc/rtc_api.o 

C_DEPS += \
./Core/Src/rtc/rtc_1307.d \
./Core/Src/rtc/rtc_api.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/rtc/%.o Core/Src/rtc/%.su: ../Core/Src/rtc/%.c Core/Src/rtc/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m0 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F030x8 -c -I../Core/Inc -I../Core/Inc/rtc -I../Core/lcd -I../Drivers/STM32F0xx_HAL_Driver/Inc -I../Drivers/STM32F0xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F0xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-rtc

clean-Core-2f-Src-2f-rtc:
	-$(RM) ./Core/Src/rtc/rtc_1307.d ./Core/Src/rtc/rtc_1307.o ./Core/Src/rtc/rtc_1307.su ./Core/Src/rtc/rtc_api.d ./Core/Src/rtc/rtc_api.o ./Core/Src/rtc/rtc_api.su

.PHONY: clean-Core-2f-Src-2f-rtc

