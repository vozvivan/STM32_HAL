################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../system/src/stm32f4-hal/stm32f4xx_hal.c \
../system/src/stm32f4-hal/stm32f4xx_hal_cortex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_crc.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dfsdm.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dma.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dma2d.c \
../system/src/stm32f4-hal/stm32f4xx_hal_dma_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_flash.c \
../system/src/stm32f4-hal/stm32f4xx_hal_gpio.c \
../system/src/stm32f4-hal/stm32f4xx_hal_i2c.c \
../system/src/stm32f4-hal/stm32f4xx_hal_i2c_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_i2s.c \
../system/src/stm32f4-hal/stm32f4xx_hal_i2s_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_iwdg.c \
../system/src/stm32f4-hal/stm32f4xx_hal_pwr.c \
../system/src/stm32f4-hal/stm32f4xx_hal_rcc.c \
../system/src/stm32f4-hal/stm32f4xx_hal_rcc_ex.c \
../system/src/stm32f4-hal/stm32f4xx_hal_spi.c \
../system/src/stm32f4-hal/stm32f4xx_hal_uart.c 

OBJS += \
./system/src/stm32f4-hal/stm32f4xx_hal.o \
./system/src/stm32f4-hal/stm32f4xx_hal_cortex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_crc.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dfsdm.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dma.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dma2d.o \
./system/src/stm32f4-hal/stm32f4xx_hal_dma_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_flash.o \
./system/src/stm32f4-hal/stm32f4xx_hal_gpio.o \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c.o \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_i2s.o \
./system/src/stm32f4-hal/stm32f4xx_hal_i2s_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_iwdg.o \
./system/src/stm32f4-hal/stm32f4xx_hal_pwr.o \
./system/src/stm32f4-hal/stm32f4xx_hal_rcc.o \
./system/src/stm32f4-hal/stm32f4xx_hal_rcc_ex.o \
./system/src/stm32f4-hal/stm32f4xx_hal_spi.o \
./system/src/stm32f4-hal/stm32f4xx_hal_uart.o 

C_DEPS += \
./system/src/stm32f4-hal/stm32f4xx_hal.d \
./system/src/stm32f4-hal/stm32f4xx_hal_cortex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_crc.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dfsdm.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dma.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dma2d.d \
./system/src/stm32f4-hal/stm32f4xx_hal_dma_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_flash.d \
./system/src/stm32f4-hal/stm32f4xx_hal_gpio.d \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c.d \
./system/src/stm32f4-hal/stm32f4xx_hal_i2c_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_i2s.d \
./system/src/stm32f4-hal/stm32f4xx_hal_i2s_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_iwdg.d \
./system/src/stm32f4-hal/stm32f4xx_hal_pwr.d \
./system/src/stm32f4-hal/stm32f4xx_hal_rcc.d \
./system/src/stm32f4-hal/stm32f4xx_hal_rcc_ex.d \
./system/src/stm32f4-hal/stm32f4xx_hal_spi.d \
./system/src/stm32f4-hal/stm32f4xx_hal_uart.d 


# Each subdirectory must supply rules for building sources it contributes
system/src/stm32f4-hal/%.o: ../system/src/stm32f4-hal/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM GNU C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=soft -Os -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -ffreestanding -flto -Wall -Wextra  -g -DNDEBUG -DSTM32F401xC -DUSE_HAL_DRIVER -DHSE_VALUE=8000000 -I"../include" -I"../system/include" -I"../system/include/cmsis" -I"../system/include/stm32f4-hal" -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/lsm303dlhc -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/l3gd20 -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Middlewares/ST/STM32_Audio/Addons/PDM -I/home/ivan/stm/STM32Cube_FW_F4_V1.16.0/Drivers/BSP/Components/cs43l22 -std=gnu11 -Wno-bad-function-cast -Wno-conversion -Wno-sign-conversion -Wno-unused-parameter -Wno-sign-compare -Wno-missing-prototypes -Wno-missing-declarations -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


