
# VFEMU

## Introduction

VFEMU is an interesting machine emulator framework. 

Unlike QEMU etc. , VFEMU simulates at a lower level, 
which means that it can simulate a module (e.g. CPU, RAM, Disk, Monitor etc.) as well as its ports/pins, and how the ports/pins are connected to each other.

VFEMU might not be very fast, but we have more freedom.

Its API is not complicated. You can easily design your own module and connect existing modules in your favorite way.

Anyone can make your own computer and gadgets here!


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

Please see files in `example/`. 

We recommend that you start from `demo_tst001.cpp`, 
which is a small example about how to connect custom CPU 
and custom RAM to form a small computer.
