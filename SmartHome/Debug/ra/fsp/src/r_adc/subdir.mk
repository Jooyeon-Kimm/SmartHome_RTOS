################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ra/fsp/src/r_adc/r_adc.c 

C_DEPS += \
./ra/fsp/src/r_adc/r_adc.d 

OBJS += \
./ra/fsp/src/r_adc/r_adc.o 

SREC += \
SmartHome.srec 

MAP += \
SmartHome.map 


# Each subdirectory must supply rules for building sources it contributes
ra/fsp/src/r_adc/%.o: ../ra/fsp/src/r_adc/%.c
	$(file > $@.in,-mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -Oz -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -fno-strict-aliasing -Wunused -Wuninitialized -Wall -Wextra -Wmissing-declarations -Wconversion -Wpointer-arith -Wshadow -Wlogical-op -Waggregate-return -Wfloat-equal -g -D_RENESAS_RA_ -D_RA_ORDINAL=1 -D_RA_CORE=CM33 -I"C:/Users/User/e2_studio/workspace/SmartHome/src" -I"." -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc/api" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/inc/instances" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/fsp/src/rm_freertos_port" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/aws/FreeRTOS/FreeRTOS/Source/include" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra/arm/CMSIS_6/CMSIS/Core/Include" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_gen" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/fsp_cfg/bsp" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/fsp_cfg" -I"C:/Users/User/e2_studio/workspace/SmartHome/ra_cfg/aws" -std=c99 -Wno-stringop-overflow -Wno-format-truncation --param=min-pagesize=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" -x c "$<")
	@echo Building file: $< && arm-none-eabi-gcc @"$@.in"

