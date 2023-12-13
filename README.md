
# VFEMU

## Introduction

VFEMU is an interesting machine emulator framework. 

Unlike QEMU etc. , VFEMU simulates at a lower level, 
which means that it can simulate a module (e.g. CPU, RAM, Disk, Monitor etc.) as well as its ports/pins, and how the ports/pins are connected to each other.

VFEMU might not be very fast, but we have more freedom.

The API is not complicated. You can easily design your own module and connect existing modules in your favorite way.

Anyone can make your own computer and gadgets here!


## Getting Started

### Build the Project 

Go to the root directory of this project, and then do
```bash
mkdir build
cd build/
cmake ..
```


## Examples

See files in `example/`. The code should not be difficult to understood.

We recommend that you start from `demo_toyram8x8.cpp`.
