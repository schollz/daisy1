# daisy1
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
````

Install libncurses5-dev and lib32ncurses5-dev

```
wget http://archive.ubuntu.com/ubuntu/pool/universe/n/ncurses/libtinfo5_6.4-2_amd64.deb && sudo dpkg -i libtinfo5_6.4-2_amd64.deb && rm -f libtinfo5_6.4-2_amd64.deb
wget http://archive.ubuntu.com/ubuntu/pool/universe/n/ncurses/libncurses5_6.4-2_amd64.deb && sudo dpkg -i libncurses5_6.4-2_amd64.deb && rm -f libncurses5_6.4-2_amd64.deb
sudo apt install lib32ncurses5-dev libncurses5 libncurses5-dev -y 
```

Install `arm-none-eabi-gcc` (correct version):

```
cd $HOME/bin
wget https://github.com/schollz/daisy1/releases/download/prereqs/gcc-arm-none-eabi-10-2020-q4-major.zip
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
git clone https://github.com/schollz/daisy1
cd daisy1
git submodule update --init --recursive 
cd libDaisy && make all -j8
cd DaisySP && make all -j8
```

## building


```
make 
```

## uploading

```
make program
```

## logging

after `make program`, run

```
while true; do; screen /dev/ttyACM0; sleep 0.5; done
```

## debugging

(make sure you have `openocd` installed).

```
make debug
```