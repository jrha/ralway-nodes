# RALWay CMRI Node Firmware


## Reference hardware implementation for a single node

* Arduino Nano (or pin-compatible clone)
* Arduino Nano breakout board [e.g.](https://robu.in/product/nano-328p-expansion-adapter-breakout-board-io-shield)
* Max485 based TTL to RS485 adapter [e.g.](https://www.banggood.com/10Pcs-5V-MAX485-TTL-To-RS485-Converter-Module-Board-For-Arduino-p-1152561.html?cur_warehouse=CN)


### Pinouts and Connections

                          +-----+
             +------------| USB |------------+
             |            +-----+            |
     ∙  B5   | [ ]D13/SCK        MISO/D12[ ] |   B4
     ∅       | [ ]3.3V           MOSI/D11[ ]~|   B3
     ∅       | [ ]V.ref     ___    SS/D10[ ]~|   B2
     ∙  C0   | [ ]A0       / N \       D9[ ]~|   B1
     ∙  C1   | [ ]A1      /  A  \      D8[ ] |   B0
     ∙  C2   | [ ]A2      \  N  /      D7[ ] |   D7
     ∙  C3   | [ ]A3       \_0_/       D6[ ]~|   D6
     ∙  C4   | [ ]A4/SDA               D5[ ]~|   D5
     ∙  C5   | [ ]A5/SCL               D4[ ] |   D4
     ∙       | [ ]A6              INT1/D3[ ]~|   D3
     ∙       | [ ]A7              INT0/D2[ ] |   D2  MAX485 DE & RE
     ∙       | [ ]5V                  GND[ ] |     
     ∅  C6   | [ ]RST                 RST[ ] |   C6  ∅
     ⏚       | [ ]GND   5V MOSI GND   TX1[ ] |   D0  MAX485 DI
     ∅       | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1  MAX485 R0
             |          [ ] [ ] [ ]          |
             |          MISO SCK RST         |
             | NANO-V3                       |
             +-------------------------------+
         
         
             +-------------------------------+
    D0 ←     | [ ] RO                VCC [ ] |
    D2 ←     | [ ] RE       MAX        B [ ] |    ∿  RS485 Bus
    D2 ←     | [ ] DE       485        A [ ] |    ∿  RS485 Bus
    D1 ←     | [ ] DI                GND [ ] |    ⏚  Common Ground
             +-------------------------------+
         
         
         
         ASCII Nano by BusyDucks under Creative Commons Attribution (BY) license
         http://busyducks.com/ascii-art-arduinos
