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
blue_pill_test_ID.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
blue_pill_test_ID.list \

OBJCOPY_BIN += \
blue_pill_test_ID.bin \


# All Target
all: blue_pill_test_ID.elf secondary-outputs

# Tool invocations
blue_pill_test_ID.elf: $(OBJS) $(USER_OBJS) D:\git\stm32_git\stm32f1\bluepill_ide_test\STM32F103C8TX_FLASH.ld
	arm-none-eabi-gcc -o "blue_pill_test_ID.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"D:\git\stm32_git\stm32f1\bluepill_ide_test\STM32F103C8TX_FLASH.ld" --specs=nosys.specs -Wl,-Map="blue_pill_test_ID.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

blue_pill_test_ID.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "blue_pill_test_ID.list"
	@echo 'Finished building: $@'
	@echo ' '

blue_pill_test_ID.bin: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O binary $(EXECUTABLES) "blue_pill_test_ID.bin"
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
