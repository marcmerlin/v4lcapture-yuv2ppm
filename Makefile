CC=gcc
CPP=g++

BUILD_DIR ?= ./
SRC_DIRS ?= ./

CFLAGS ?= -Wall -O2

all: v4lcapture v4lcapture_single v4lcapture_long yuv2ppm

clean:
	@rm -f v4lcapture v4lcapture_single v4lcapture_long yuv2ppm
	@rm -f v4lcapture.exe v4lcapture_single.exe v4lcapture_long.exe yuv2ppm.exe

$(BUILD_DIR)/%: %.c %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	chmod 755 $@
