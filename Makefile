
# Project Name
TARGET = daisy1

USE_DAISYSP_LGPL = 1

APP_TYPE=BOOT_SRAM
# without BOOT_SRAM: make program
# with BOOT_SRAM:
# hold boot, press reset and then
# > make program-boot
# press reset and then quickly press boot
# > make program-dfu

# Sources
CPP_SOURCES = main.cpp \
	lib/resampler.cpp \
	lib/tape.cpp \
	lib/tapehead.cpp \
	lib/balance2.cpp \
	lib/lfo.cpp \
	lib/lpf_biquad.cpp \
	lib/compressor.cpp \
	lib/daisy_midi.cpp \
	lib/chords.cpp
# lib/fverb2.cpp \
# lib/lpf_oberheim.cpp \
# lib/lpf_sallenkey_onepole.cpp \
# lib/lpf_korg.cpp \
# lib/lpf_diodeladder.cpp \
	
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
