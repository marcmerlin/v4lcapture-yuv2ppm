CC=gcc
CPP=g++

BUILD_DIR ?= ./
SRC_DIRS ?= ./

CFLAGS ?= -Wall -O2

all: v4lcapture v4lcapture_single v4lcapture_long yuv2ppm
	

$(BUILD_DIR)/%: %.c %.cpp
	$(CC) $(CFLAGS) -c $< -o $@
	chmod 755 $@
