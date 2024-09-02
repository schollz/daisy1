export PICO_EXTRAS_PATH ?= $(CURDIR)/pico-extras
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk
NPROCS := $(shell grep -c 'processor' /proc/cpuinfo)

GOVERSION = go1.21.11
GOBIN = $(HOME)/go/bin
GOINSTALLPATH = $(GOBIN)/$(GOVERSION)

dobuild: pico-extras build
	make -C build -j$(NPROCS)

build: 
	rm -rf build
	mkdir build
	cd build && cmake ..
	make -C build -j$(NPROCS)
	echo "build success"

upload: dobuild
	./lib/upload.sh 

clean:
	rm -rf build

pico-extras:
	git clone https://github.com/raspberrypi/pico-extras.git pico-extras
	cd pico-extras && git checkout sdk-1.5.1 && git submodule update -i 