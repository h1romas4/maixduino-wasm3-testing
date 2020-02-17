# maixduino-wasm3-testing

WebAssembly interpreter [Wasm3](https://github.com/wasm3/wasm3) on Maixduino.

![](https://github.com/h1romas4/maixduino-wasm3-testing/blob/master/docs/images/maixduino-wasm3-01.jpg)

[https://www.youtube.com/watch?v=FSZE7P-SMDc](https://www.youtube.com/watch?v=FSZE7P-SMDc)

## Require

* [Kendryte GNU Toolchain v8.2.0-20190213](https://github.com/kendryte/kendryte-gnu-toolchain/releases/tag/v8.2.0-20190213)
* [kflash](https://github.com/kendryte/kflash.py)

```
$ pwd
/opt/kendryte-toolchain/bin
$ ./riscv64-unknown-elf-gcc -v
gcc version 8.2.0 (GCC)
$ kflash -h
...
positional arguments:
  firmware              firmware bin path
...
```

## Build

```
git clone --recursive https://github.com/h1romas4/maixduino-wasm3-testing.git
cd maixduino-wasm3-testing
```

### maxiduino-wasm3-bitblt

```
cd maxiduino-wasm3-bitblt
mkdir build && cd build
# set -DTOOLCHAIN to directory Kendryte GNU Toolchain bin path
cmake .. -DPROJ=bitblt -DSDK_ROOT=./kendryte-standalone-sdk -DTOOLCHAIN=/opt/kendryte-toolchain/bin
make
kflash -p /dev/ttyUSB0 -b 1500000 -B goE -s -t bitblt.bin
```

## Dependencies

|name|version|
|-|-|
|[Wasm3](https://github.com/wasm3/wasm3)|0.4.6|
|[AssemblyScript](https://github.com/AssemblyScript/assemblyscript)|0.9.1|
|[kendryte-standalone-sdk](https://docs.espressif.com/projects/esp-idf/en/v3.2.3/get-started/index.html)|V0.5.6|
