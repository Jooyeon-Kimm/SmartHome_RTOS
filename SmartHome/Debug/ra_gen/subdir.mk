################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra_gen/common_data.c \
../ra_gen/hal_data.c \
../ra_gen/main.c \
../ra_gen/pin_data.c \
../ra_gen/thread_adc.c \
../ra_gen/thread_button.c \
../ra_gen/thread_gpt_.c \
../ra_gen/thread_timer.c \
../ra_gen/thread_uart.c \
../ra_gen/vector_data.c 

C_DEPS += \
./ra_gen/common_data.d \
./ra_gen/hal_data.d \
./ra_gen/main.d \
./ra_gen/pin_data.d \
./ra_gen/thread_adc.d \
./ra_gen/thread_button.d \
./ra_gen/thread_gpt_.d \
./ra_gen/thread_timer.d \
./ra_gen/thread_uart.d \
./ra_gen/vector_data.d 

OBJS += \
./ra_gen/common_data.o \
./ra_gen/hal_data.o \
./ra_gen/main.o \
./ra_gen/pin_data.o \
./ra_gen/thread_adc.o \
./ra_gen/thread_button.o \
./ra_gen/thread_gpt_.o \
./ra_gen/thread_timer.o \
./ra_gen/thread_uart.o \
./ra_gen/vector_data.o 

SREC += \
SmartHome.srec 

MAP += \
SmartHome.map 


# Each subdirectory must supply rules for building sources it contributes
ra_gen/%.o: ../ra_gen/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Oz -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_ORDINAL=1 -D_RA_CORE=CM33 -I"C:/Users/User/e2_studio/workspace/SmartHome/src" -I"." -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc/api" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc/instances" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/src/rm_freertos_port" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_gen" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/fsp_cfg/bsp" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/fsp_cfg" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/aws" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

