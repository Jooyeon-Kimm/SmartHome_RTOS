################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/common.c \
../src/hal_entry.c \
../src/thread_adc_entry.c \
../src/thread_button_entry.c \
../src/thread_gpt__entry.c \
../src/thread_timer_entry.c \
../src/thread_uart_entry.c 

C_DEPS += \
./src/common.d \
./src/hal_entry.d \
./src/thread_adc_entry.d \
./src/thread_button_entry.d \
./src/thread_gpt__entry.d \
./src/thread_timer_entry.d \
./src/thread_uart_entry.d 

OBJS += \
./src/common.o \
./src/hal_entry.o \
./src/thread_adc_entry.o \
./src/thread_button_entry.o \
./src/thread_gpt__entry.o \
./src/thread_timer_entry.o \
./src/thread_uart_entry.o 

SREC += \
SmartHome.srec 

MAP += \
SmartHome.map 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Oz -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_ORDINAL=1 -D_RA_CORE=CM33 -I"C:/Users/User/e2_studio/workspace/SmartHome/src" -I"." -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc/api" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc/instances" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/src/rm_freertos_port" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_gen" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/fsp_cfg/bsp" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/fsp_cfg" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/aws" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

