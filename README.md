# daisy1
First thing with daisy

```
git clone https://github.com/schollz/daisy1
cd daisy1
git submodule update --init --recursive 
cd libDaisy && make all
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

```
export LD_LIBRARY_PATH=/home/zns/bin/gcc-arm-none-eabi-10-2020-q4-major/arm-none-eabi/lib:$LD_LIBRARY_PATH
sudo ldconfig
export PATH=~/bin/gcc-arm-none-eabi-10-2020-q4-major/bin:$PATH
make openocd
```

```
make debug
```