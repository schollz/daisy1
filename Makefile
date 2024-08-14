
# Project Name
TARGET = daisy1

# Sources
CPP_SOURCES = main.cpp lib/tape.cpp lib/tapehead.cpp lib/crossfade.cpp
# C_SOURCES = audio.c

# Library Locations
LIBDAISY_DIR = libDaisy
DAISYSP_DIR = DaisySP
USE_FATFS = 1
LDFLAGS += -u _printf_float
XFADE_SAMPLES = 480

# Core location, and generic Makefile.
SYSTEM_FILES_DIR = $(LIBDAISY_DIR)/core
include $(SYSTEM_FILES_DIR)/Makefile



.venv:
	uv venv 
	uv pip install -r requirements.txt

lib/crossfade.h: .venv
	cd lib && ../.venv/bin/python crossfade.py $(XFADE_SAMPLES) > crossfade.h
	clang-format -i --style=google lib/crossfade.h 
	clang-format -i --style=google lib/crossfade.cpp