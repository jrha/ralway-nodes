# RALWay CMRI Node Firmware


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
