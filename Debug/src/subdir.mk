################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/BlinkLed.c \
../src/Timer.c \
../src/_initialize_hardware.c \
../src/_write.c \
../src/main.c \
../src/stm32f401_discovery.c \
../src/stm32f401_discovery_accelerometer.c \
../src/stm32f401_discovery_audio.c \
../src/stm32f401_discovery_gyroscope.c \
../src/stm32f4xx_hal_msp.c \
../src/stm32f4xx_it.c 

OBJS += \
./src/BlinkLed.o \
./src/Timer.o \
./src/_initialize_hardware.o \
./src/_write.o \
./src/main.o \
./src/stm32f401_discovery.o \
./src/stm32f401_discovery_accelerometer.o \
./src/stm32f401_discovery_audio.o \
./src/stm32f401_discovery_gyroscope.o \
./src/stm32f4xx_hal_msp.o \
./src/stm32f4xx_it.o 

C_DEPS += \
./src/BlinkLed.d \
./src/Timer.d \
./src/_initialize_hardware.d \
./src/_write.d \
./src/main.d \
./src/stm32f401_discovery.d \
./src/stm32f401_discovery_accelerometer.d \
./src/stm32f401_discovery_audio.d \
./src/stm32f401_discovery_gyroscope.d \
./src/stm32f4xx_hal_msp.d \
./src/stm32f4xx_it.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F401xC -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/lsm303dlhc -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/l3gd20 -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Middlewares/ST/STM32_Audio/Addons/PDM -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/cs43l22 -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/stm32f4xx_hal_msp.o: ../src/stm32f4xx_hal_msp.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -fno-move-loop-invariants -Wall -Wextra  -g3 -DDEBUG -DUSE_FULL_ASSERT -DTRACE -DOS_USE_TRACE_SEMIHOSTING_STDOUT -DSTM32F401xC -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/lsm303dlhc -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/l3gd20 -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Middlewares/ST/STM32_Audio/Addons/PDM -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/cs43l22 -std=gnu11 -Wno-missing-prototypes -Wno-missing-declarations -MMD -MP -MF"$(@:%.o=%.d)" -MT"src/stm32f4xx_hal_msp.d" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


