SerialFiller
============

Like a serial killer, but friendlier. A C++ serial publish/subscribe based communication protocol.

- Simple publish/subscribe system
- Ability to any type of data on a topic
- COBS encoding for reliable, low-overhead framing of packets
- CRC16 check for packet integrity (uses CRC16-CCITT, polynomial 0x1021, which does not suffer from the inability to detect '0x00' bytes at the start of the packet)
- Platform agnostic data I/O (you fill in the hardware abstraction layer by providing a callback for `SerialFiller::txDataReady_` and call `SerialFiller::HandleRxData()` when new RX data is available).
- Functionality backed by numerous unit tests
- CMake based build system

Examples
--------

**Setup:**

````c++
#include <SerialFiller/SerialFiller.hpp>

mn::SerialFiller serialFiller;
````

**Publish example:**

````c++
// Publish the data { 0x01, 0x02, 0x03 } on topic "mytopic" 
serialFiller.Publish("mytopic", { 0x01, 0x02, 0x03 });
````

**Subscribe example:**

````c++
// Provide a callback for "mytopic" messages using
// lambda notation.
serialFiller.Subscribe("mytopic", [](std::vector<uint8_t> rxData) -> void {
        std::cout << "Received packet on mytopic!" << std::endl;
        
        std::cout << " Data = ";
        for(auto dataByte : rxData) {
            std::cout << std::to_string(dataByte);
        }
        std::cout << std::endl;
    });
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