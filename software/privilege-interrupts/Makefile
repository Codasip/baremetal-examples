# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += clint
REQUIRES += user_mode
REQUIRES += supervisor_mode

APP     = privilege-interrupts
SOURCES = $(DEMO_DIR)/src/privilege-interrupts.c

include $(DEMO_DIR)/../../share/app.mk
