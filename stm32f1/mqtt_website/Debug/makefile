################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F1xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
mqtt_website.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
mqtt_website.list \

OBJCOPY_BIN += \
mqtt_website.bin \


# All Target
all: mqtt_website.elf secondary-outputs

# Tool invocations
mqtt_website.elf: $(OBJS) $(USER_OBJS) D:\git\microcontroller\stm32_git\stm32f1\mqtt_website\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "mqtt_website.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\git\microcontroller\stm32_git\stm32f1\mqtt_website\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="mqtt_website.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

mqtt_website.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "mqtt_website.list"
	@echo 'Finished building: $@'
	@echo ' '

mqtt_website.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "mqtt_website.bin"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_BIN)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
