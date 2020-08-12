################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../RF433MHz/virtualwire.cpp 

OBJS += \
./RF433MHz/virtualwire.o 

CPP_DEPS += \
./RF433MHz/virtualwire.d 


# Each subdirectory must supply rules for building sources it contributes
RF433MHz/virtualwire.o: ../RF433MHz/virtualwire.cpp
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DMCU_STM32F103RE -DSTM32F103xE -DUSE_HAL_DRIVER -DDEBUG -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"/home/quoclaptk/STM32CubeIDE/workspace_1.4.0/STM32F103RCT6_RF433MHz/RF433MHz" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-threadsafe-statics -fno-use-cxa-atexit -Wall -fstack-usage -MMD -MP -MF"RF433MHz/virtualwire.d" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

