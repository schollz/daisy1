# ouroboros

First thing with daisy

## prequisites

Install libusb

```
sudo apt install -y libusb-1.0-0-dev screen
```

Build openocd

```
git clone https://git.code.sf.net/p/openocd/code openocd-code
cd openocd-code/
./bootstrap
./configure --enable-stlink
make -j8
sudo make install
```

Add udev rules ([info](https://forum.electro-smith.com/t/st-link-and-cortex-debugger-on-ubuntu-24-04/5260))

```
cd dev/udev-rules
sudo cp *.* /etc/udev/rules.d/
sudo systemctl reload udev
sudo udevadm trigger
```

Install libncurses5-dev and lib32ncurses5-dev

```
wget http://archive.ubuntu.com/ubuntu/pool/universe/n/ncurses/libtinfo5_6.4-2_amd64.deb && sudo dpkg -i libtinfo5_6.4-2_amd64.deb && rm -f libtinfo5_6.4-2_amd64.deb
wget http://archive.ubuntu.com/ubuntu/pool/universe/n/ncurses/libncurses5_6.4-2_amd64.deb && sudo dpkg -i libncurses5_6.4-2_amd64.deb && rm -f libncurses5_6.4-2_amd64.deb
sudo apt install lib32ncurses5-dev libncurses5 libncurses5-dev -y
```

Install `arm-none-eabi-gcc` (correct version):

```
cd $HOME/bin
wget https://github.com/schollz/ouroboros/releases/download/prereqs/gcc-arm-none-eabi-10-2020-q4-major.zip
unzip gcc-arm-none-eabi-10-2020-q4-major.zip
rm gcc-arm-none-eabi-10-2020-q4-major.zip
```

Export the path to the `arm-none-eabi-gcc`:

```
export LD_LIBRARY_PATH=$HOME/bin/gcc-arm-none-eabi-10-2020-q4-major/arm-none-eabi/lib:$LD_LIBRARY_PATH
sudo ldconfig
export PATH=~/bin/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH
```

## cloning

```
git clone https://github.com/schollz/ouroboros
cd ouroboros
git submodule update --init --recursive
cd libDaisy && make all -j8
cd DaisySP && make all -j8
```

## building

```
make
```

## uploading

If `APP_TYPE=BOOT_SRAM` is enabled in the `Makefile` then:

Hold boot, press reset and then:

```
> make program-boot
```

Now, press reset and then quickly press boot and then upload:

```
> make program-dfu
```

If `APP_TYPE=BOOT_FLASH` is disabled in the `Makefile` then:

```
make program
```

## logging

Use

```
make midicom
```

for debugging.

after `make program`, run

```

while true; do; screen /dev/ttyACM0; sleep 0.5; done

```

## debugging

(make sure you have `openocd` installed).

```
./gdb
```

within gdb you can do `continue` to keep running, and `bt full` to get a full stack trace and `info local` to get information about variables.

## TODO:

```

sndWet = DelayN.ar(sndWet, 0.03, 0.03);
sndWet = sndWet + PitchShift.ar(sndWet, 0.13, 2,0,1,1*shimmer/2);
sndWet = sndWet + PitchShift.ar(sndWet, 0.1, 4,0,1,0.5*shimmer/2);
sndWet = Fverb2.ar(sndWet[0],sndWet[1],200,
decay:LFNoise2.kr(1/5).range(60,90),
tail_density:LFNoise2.kr(1/5).range(70,90),
);

```

```

```
