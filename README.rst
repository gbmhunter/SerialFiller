.. role:: bash(code)
    :language: bash

.. role:: cpp(code)
    :language: cpp

============
SerialFiller
============

--------------------------------------------------------------------------------------------------
Like a serial killer, but friendlier. A C++ serial publish/subscribe based communication protocol.
--------------------------------------------------------------------------------------------------

.. image:: https://travis-ci.org/mbedded-ninja/SerialFiller.png?branch=master
	:target: https://travis-ci.org/mbedded-ninja/SerialFiller

- Simple publish/subscribe system for sending messages across serial links
- Ability to send any type of data on a "topic"
- Optional packet acknowledge functionality with built in thread blocking (using C++11/14 features) on packet send until packet acknowledge is received.
- Built-in thread safety (for performance reasons, thread safety can be disabled via :cpp:`SerialFiller::SetThreadSafetyEnabled(false)`)
- COBS encoding for reliable, low-overhead framing of packets
- CRC16 check for packet integrity (uses CRC16-CCITT, polynomial 0x1021, which does not suffer from the inability to detect :cpp:`0x00` bytes at the start of the packet)
- Platform agnostic data I/O (you fill in the hardware abstraction layer by providing a callback for :cpp:`SerialFiller::txDataReady_` and call :cpp:`SerialFiller::GiveRxData()` when new RX data is available).
- Functionality backed by numerous unit tests
- CMake based build system
- CLion project files provided (use of CLion is optional)

Examples
========

**Setup:**

.. code:: cpp

    #include <SerialFiller/SerialFiller.hpp>


    mn::SerialFiller serialFiller;


**Publish example:**

.. code:: cpp

    // Publish the data { 0x01, 0x02, 0x03 } on topic "mytopic"
    serialFiller.Publish("mytopic", { 0x01, 0x02, 0x03 });

**Subscribe example:**

.. code:: cpp

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

See the source code in the :bash:`examples` directory for more real-world examples!

Building/Installing
===================

Clone this repository. Then :bash:`cd` into the repo's root directory and do one of the following:

Use The Script
--------------

To build only:

.. code:: bash

    ~/SerialFiller$ ./tools/build.sh

To build AND install:

.. code:: bash

    ~/SerialFiller$ ./tools/build.sh -i

Manual
------

.. code:: bash

    ~/SerialFiller$ mkdir build
    ~/SerialFiller$ cd build
    ~/SerialFiller/build$ cmake ..
    ~/SerialFiller/build$ make


Once SerialFiller has been built, run:

.. code:: bash

    ~/SerialFiller/build$ sudo make install

Installation Info
-----------------

On a typical Linux system, the above install commands will install the static library :bash:`libSerialFiller.a` into :bash:`/usr/local/bin` and the header files into :bash:`/usr/local/include/SerialFiller` (a directory is created inside :bash:`/usr/local/include` as not to pollute the system folder space).

This command does NOT install the unit tests or examples.