# Remove the following line
$(error Configuration file was not set properly! Replace contents of the config.mk file in the top-level directory of Bare-metal Examples repository to match the target, or modify it to include one of the example configuration files)

# Uncomment one of the following likes to select execution environment
#CONFIG_ENVIRONMENT ?= FPGA_SEMIHOSTING
#CONFIG_ENVIRONMENT ?= FPGA_UART
#CONFIG_ENVIRONMENT ?= SIMULATOR

THIS_DIR := $(subst config.mk,.,$(lastword $(MAKEFILE_LIST)))

# Uncomment one of the following lines to inlude the desired config file
#include $(THIS_DIR)/lib/targets/configs/config-l31-carbon.mk
#include $(THIS_DIR)/lib/targets/configs/config-l31-helium.mk
#include $(THIS_DIR)/lib/targets/configs/config-l31-fluorine-iiot.mk
#include $(THIS_DIR)/lib/targets/configs/config-l31-copper.mk
#include $(THIS_DIR)/lib/targets/configs/config-l31-nickel.mk
#include $(THIS_DIR)/lib/targets/configs/config-l31-zinc.mk
#include $(THIS_DIR)/lib/targets/configs/config-a730-default.mk
#include $(THIS_DIR)/lib/targets/configs/config-l110-eagle.mk
#include $(THIS_DIR)/lib/targets/configs/config-l110-andromeda.mk

# If building from top-level, uncomment one of the following lines to select demo to build
#DEMO_APP=aead-demo
#DEMO_APP=cache-counter-demo
#DEMO_APP=cache-info-demo
#DEMO_APP=cache-write-through
#DEMO_APP=clic-interrupts
#DEMO_APP=clic-interrupts-vectored
#DEMO_APP=clic-priority
#DEMO_APP=clint-ipi
#DEMO_APP=clint-timer
#DEMO_APP=clint-timer-interrupt
#DEMO_APP=counter-demo
#DEMO_APP=csr-demo
#DEMO_APP=ecall-demo
#DEMO_APP=exception-demo
#DEMO_APP=FreeRTOS-demo
#DEMO_APP=fsbl
#DEMO_APP=gpio-demo
#DEMO_APP=gpio-interrupt-demo
#DEMO_APP=hello-world
#DEMO_APP=hpmcounter-demo
#DEMO_APP=i2c-demo
#DEMO_APP=interrupts-simple
#DEMO_APP=interrupts-vectored
#DEMO_APP=memory-test
#DEMO_APP=memory-perf
#DEMO_APP=mp-demo
#DEMO_APP=mutex-demo
#DEMO_APP=oob-demo
#DEMO_APP=pic-interrupts
#DEMO_APP=plic-interrupts
#DEMO_APP=plic-priority
#DEMO_APP=pmp-demo
#DEMO_APP=privilege-drop
#DEMO_APP=privilege-interrupts
#DEMO_APP=privilege-interrupts-delegated
#DEMO_APP=rdtime
#DEMO_APP=spi-demo
#DEMO_APP=tcm-demo
#DEMO_APP=timing-demo
#DEMO_APP=trng-demo
#DEMO_APP=uart-demo
#DEMO_APP=wfi-demo
