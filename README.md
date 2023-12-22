
# VFEMU

## Introduction

VFEMU is an interesting machine emulator framework. 

Unlike QEMU etc., VFEMU simulates at a lower level, 
which means that it can simulate a module (e.g. CPU, RAM, Disk, Monitor, etc.) as well as its ports/pins, and how the ports/pins are connected.

It's somewhere between pure VM level and circuit level.

VFEMU might not be very fast, but we have more freedom.

Its API is not complicated. You can easily design your module and connect existing modules in your favorite way.

Anyone can make their virtual computer and gadgets here!


## Getting Started

### Build the Project 

Go to the root directory of this project, and then do
```bash
mkdir build
cd build/
cmake ..
```

And then you can run examples in this way
```bash
./examples/demo_tst001
```


## Examples

Please see the files in `example/`. 

We recommend that you start from `demo_tst001.cpp`, 
which is a small example of how to connect a custom CPU 
and custom RAM to form a small computer.

If you think it's simple enough, then you can try my custom NES Game Emulator `demo_tnes01.cpp`.

### NES Emulator
This is a good demonstration of the VFEMU framework.

Currently, we can boot many Mapper-0 games to their title screens and even demo screens.

It now has a basic implementation of the 6502 instruction set, an SDL2-based graphics output.

Related files:
```
examples/demo_tnes01.cpp
include/nes/*
modules/nes/*
```

To be continued...
