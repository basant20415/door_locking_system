################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../buzzer.c \
../external_eeprom.c \
../gpio.c \
../mc2.c \
../motor.c \
../pwm.c \
../timer1_driver.c \
../twi.c \
../uart.c 

OBJS += \
./buzzer.o \
./external_eeprom.o \
./gpio.o \
./mc2.o \
./motor.o \
./pwm.o \
./timer1_driver.o \
./twi.o \
./uart.o 

C_DEPS += \
./buzzer.d \
./external_eeprom.d \
./gpio.d \
./mc2.d \
./motor.d \
./pwm.d \
./timer1_driver.d \
./twi.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -O0 -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=8000000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


