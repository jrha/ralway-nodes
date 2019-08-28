# RALWay CMRI Node Firmware


## Reference hardware implementation for a single node

* Arduino Nano (or pin-compatible clone)
* Arduino Nano breakout board [e.g.](https://robu.in/product/nano-328p-expansion-adapter-breakout-board-io-shield)
* Max485 based TTL to RS485 adapter [e.g.](https://www.banggood.com/10Pcs-5V-MAX485-TTL-To-RS485-Converter-Module-Board-For-Arduino-p-1152561.html?cur_warehouse=CN)


### Pinouts

Digital pins 0, 1 and 2 are used for serial communication, pin 2 for DE & RE on the Max485

                      +-----+
         +------------| USB |------------+
         |            +-----+            |
    B5   | [ ]D13/SCK        MISO/D12[ ] |   B4
         | [ ]3.3V           MOSI/D11[ ]~|   B3
         | [ ]V.ref     ___    SS/D10[ ]~|   B2
    C0   | [ ]A0       / N \       D9[ ]~|   B1
    C1   | [ ]A1      /  A  \      D8[ ] |   B0
    C2   | [ ]A2      \  N  /      D7[ ] |   D7
    C3   | [ ]A3       \_0_/       D6[ ]~|   D6
    C4   | [ ]A4/SDA               D5[ ]~|   D5
    C5   | [ ]A5/SCL               D4[ ] |   D4
         | [ ]A6              INT1/D3[ ]~|   D3
         | [ ]A7              INT0/D2[ ] |   D2
         | [ ]5V                  GND[ ] |     
    C6   | [ ]RST                 RST[ ] |   C6
         | [ ]GND   5V MOSI GND   TX1[ ] |   D0
         | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
         |          [ ] [ ] [ ]          |
         |          MISO SCK RST         |
         | NANO-V3                       |
         +-------------------------------+
         
         http://busyducks.com/ascii-art-arduinos
