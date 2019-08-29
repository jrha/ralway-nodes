# RALWay CMRI Node Firmware

Sensors and actuators on RALWay module boards are monitored and controlled by  CMRI nodes emulated by Arduinos, the reference design below uses an Arduino Nano.

## Bus design

The nodes communicate with a master computer via RS485 over twisted pair with a common ground, the master computer (in our case a Raspberry Pi 3 running JMRI) sits roughly in the middle of the bus/layout. Each adjacent board passes the bus to the next and terminators are fitted at both ends of the bus/layout.

For example:

    [term]~[board]~[board]~[board]~[RPi]~[board]~[board]~[board]~[term]

Termination is standard for RS485, a single 130 ohm resistor across the data lines.

Bias resistors are built into the RS485 interface at the master computer and are 720 ohm pull-up and pull-down resistors on the data lines.

## Node addressing and bit space layout

Node addresses are fixed and are multiples of ten, starting from 10.

Nodes emulate a SUSIC node with configurable numbers of input and output cards, each providing 24 or 32 bits of IO. Therefore input and output bits are assigned in blocks of 24 or 32 bits. For each block of output bits we reserve a matching block of input bits to provide virtual feedback of output state as updates are not always processed immediately, especially if servos are being used (see below). This requires that the number of input cards must always be at least twice the number of output cards. For simplicity we do not allow cards sizes to be mixed and recommend that nodes always use 32-bit cards unless compatibility with existing systems is required.

For example a 32-bit node with three input cards and two output cards (quite a large node for an Arduino) would provide:

Card | Type   | Bit range | Provides
-----|--------|-----------|----
   0 | Output |   0 -  31 | Physical outputs.
   1 | Output |  32 -  63 | Physical outputs.
   2 | Input  |   0 -  31 | Internal reflection of output bits 0-31.
   3 | Input  |  32 -  63 | Internal reflection of output bits 32-63.
   4 | Input  |  64 -  71 | Physical inputs.


**Be aware that JMRI addresses bits starting at 1 rather than 0!**

## Sensors and actuators

* We are emulating CMRI nodes, so only support binary inputs and outputs.
* We are targeting JMRI, so will use the terms *closed* and *thrown* to refer to actuator states.

As an aside, if you are looking to control complex lighting effects DMX512 is a much better fit and is also supported by JMRI. We are focussing on rapid and reliable control of track infrastructure, although you could use it for anything.

### Occupancy detection

We use two forms of detection, infra-red reflection sensors for absolute positioning and current transformers for block detection.

Reflection sensors are simple if used with a board which provides an adjustable threshold and a digital output.

Current transformer based block detection requires more set up and configuration, we are using active boards with adjustable amplification, these must be connected to analog inputs (the Nano provides eight).

Every `ANALOG_SAMPLE_INTERVAL` milliseconds, a timer interrupt samples the analog inputs and fills an array of `ANALOG_READING_COUNT` readings, the envelope size of these is compared against a threshold value to determine block occupancy. Increasing `ANALOG_READING_COUNT` provides greater immunity to noise, but slows response to blocks becoming occupied.

### LEDs and lights

Simply drive directly from an output pin, or via a transistor for higher output devices.

### Servos

Servos can be connected to any PWM capable pin (the Nano has six) and we use these to drive points (turnouts) and semaphore signal heads. An angle for both the *closed* and *thrown* positions must be provided, finding these values is fairly straightforward with the **servo_tweaker** firmware in this repository.

Updates to servo position are made gradually to better simulate real-world infrastructure, the delay between updates is set by `SERVO_STEP_INTERVAL`. Only when the move has finally completed is the corresponding input bit updated, making use of this feedback in JMRI is highly recommended and can be supplemented with physical detection switches on points (turnouts) if desired.

When a servo is not being moved it is disconnected, this saves power and keeps the layout quiet.

**CAUTION!** Servos are updated in the main process, so changing `SERVO_STEP_INTERVAL` will alter the responsiveness of the entire node.

## Reference hardware implementation for a single node

* Arduino Nano (or pin-compatible clone)
* Arduino Nano breakout board [e.g.](https://robu.in/product/nano-328p-expansion-adapter-breakout-board-io-shield)
* Max485 based TTL to RS485 adapter [e.g.](https://www.banggood.com/10Pcs-5V-MAX485-TTL-To-RS485-Converter-Module-Board-For-Arduino-p-1152561.html?cur_warehouse=CN)


### Pinouts and Connections

It is recommended that the small number of connections between boards are wire-wrapped, although if you were feeling like spending more money you could of course build a PCB.

The reference design assumes that:
* The Nano breakout board provides a 5 volt regulator to supply the Nano.
* The Max485 board has been designed to be supplied with 5 volts.


                             +-----+
                +------------| USB |------------+
                |            +-----+            |
              ∙ | [ ]D13/SCK        MISO/D12[ ] | ∙
              ∅ | [ ]3.3V           MOSI/D11[ ]~| ∙
              ∅ | [ ]V.ref     ___    SS/D10[ ]~| ∙
              ∙ | [ ]A0       / N \       D9[ ]~| ∙
              ∙ | [ ]A1      /  A  \      D8[ ] | ∙
              ∙ | [ ]A2      \  N  /      D7[ ] | ∙
              ∙ | [ ]A3       \_0_/       D6[ ]~| ∙
              ∙ | [ ]A4/SDA               D5[ ]~| ∙
              ∙ | [ ]A5/SCL               D4[ ] | ∙
              ∙ | [ ]A6              INT1/D3[ ]~| ∙
              ∙ | [ ]A7              INT0/D2[ ] | MAX485 DE & RE
              ∙ | [ ]5V                  GND[ ] | ⏚
              ∅ | [ ]RST                 RST[ ] | ∅
              ⏚ | [ ]GND   5V MOSI GND   TX1[ ] | MAX485 DI
              ∅ | [ ]Vin   [ ] [ ] [ ]   RX1[ ] | MAX485 R0
                |          [ ] [ ] [ ]          |
                |          MISO SCK RST         |
                | NANO-V3                       |
                +-------------------------------+

                +-------------------------------+
        Nano D0 | [ ] RO                VCC [ ] | +5V
        Nano D2 | [ ] RE       MAX        B [ ] | ∿  RS485 Bus
        Nano D2 | [ ] DE       485        A [ ] | ∿  RS485 Bus
        Nano D1 | [ ] DI                GND [ ] | ⏚  Common Ground
                +-------------------------------+


ASCII Nano by BusyDucks under Creative Commons Attribution (BY) license, see http://busyducks.com/ascii-art-arduinos
