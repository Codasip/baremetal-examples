# Baremetal application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

REQUIRES += trng
REQUIRES += sdcard
CPPFLAGS += "-DDATA_LEN=1048576"

APP     = trng-demo
SOURCES = \
	$(DEMO_DIR)/src/trng-demo.c \
	$(DEMO_DIR)/src/fatfs/diskio.c \
	$(DEMO_DIR)/src/fatfs/ff.c \
	$(DEMO_DIR)/src/fatfs/ffunicode.c \
	$(DEMO_DIR)/src/fatfs/mmc.c

include $(DEMO_DIR)/../../share/app.mk
