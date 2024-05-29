# Baremetal SDK application Makefile
DEMO_DIR := $(subst Makefile,.,$(lastword $(MAKEFILE_LIST)))

APP     = memory-perf
SOURCES = $ \
	$(DEMO_DIR)/src/memory.c \
	$(DEMO_DIR)/src/memory-perf.c

CFLAGS  = -O1

include $(DEMO_DIR)/../../share/app.mk
