# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += flash

APP     = spi-demo
SOURCES = $(DEMO_DIR)/src/spi-demo.c \
          $(DEMO_DIR)/src/s25fl128s.c


include $(DEMO_DIR)/../../share/app.mk
