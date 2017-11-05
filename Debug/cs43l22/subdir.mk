################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/cs43l22/cs43l22.c 

OBJS += \
./cs43l22/cs43l22.o 

C_DEPS += \
./cs43l22/cs43l22.d 


# Each subdirectory must supply rules for building sources it contributes
cs43l22/cs43l22.o: /home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/cs43l22/cs43l22.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F401xC -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/lsm303dlhc -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/l3gd20 -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Middlewares/ST/STM32_Audio/Addons/PDM -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/cs43l22 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


