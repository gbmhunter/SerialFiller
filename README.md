SerialFiller
============

Like a serial killer, but friendlier. A C++ serial publish/subscribe based communication protocol.

Examples
--------

````c++
#include <SerialFiller/SerialFiller.hpp>

mn::SerialFiller serialFiller;

// Publish the data { 0x01, 0x02, 0x03 } on topic "mytopic" 
serialFiller.Publish("mytopic", { 0x01, 0x02, 0x03 });
````

Building
--------

Clone this repository. Then `cd` into the repo's root directory and run:

````bash
~/SerialFiller$ mkdir build
~/SerialFiller$ cd build
~/SerialFiller/build$ cmake ..
~/SerialFiller/build$ make
````

Installing
----------

Once SerialFiller has been built, run:

````bash
~/SerialFiller/build$ sudo make install
````

On a typical Linux system, this will install the static library `libSerialFiller.a` into `/usr/local/bin` and the header files into `/usr/local/include/SerialFiller` (a directory is created inside `/usr/local/include` as not to pollute the system folder space).