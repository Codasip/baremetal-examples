# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += user_mode

APP     = privilege-drop
SOURCES = $(DEMO_DIR)/src/privilege-drop.c

include $(DEMO_DIR)/../../share/app.mk
