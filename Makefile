# Project Name
TARGET = daisy1

# Sources
CPP_SOURCES = main.cpp
# C_SOURCES = audio.c

# Library Locations
LIBDAISY_DIR = libDaisy
LDFLAGS += -u _printf_float

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile

format: *.cpp
	clang-format -i *.cpp
	
libDaisy:
	git clone --depth 1 https://github.com/electro-smith/libDaisy.git
	cd libDaisy && make -j32