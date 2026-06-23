# GPS Tracker
### Use this bracelet to get live location and watch on any device 
> do not use to stalk people by giving them "unsuspicious" bracelet and looking where they are!!

## Zine
<img width="1543" height="2344" alt="Untitled-1" src="https://github.com/user-attachments/assets/c3b1a9b0-98d8-414d-a20a-ff8be50afb67" />

## Images
<img width="1920" height="1080" alt="3" src="https://github.com/user-attachments/assets/4353e65a-ce85-40f8-8ee0-78b1768431ca" />
<img width="1920" height="1080" alt="1" src="https://github.com/user-attachments/assets/0a0fd6fe-80b8-42f9-b727-eb2644edefc4" />
<img width="1920" height="1080" alt="4" src="https://github.com/user-attachments/assets/770807e1-7ce4-4dbf-8222-aec4ba5c7d6d" />
<img width="3307" height="2339" alt="tracker-1" src="https://github.com/user-attachments/assets/caf078c1-52d2-4adc-9e68-8ebe2179e5ea" />
<img width="2339" height="1654" alt="pcb-1" src="https://github.com/user-attachments/assets/626324ca-ad38-4234-9aab-dca2bbce93e4" />

## How to use?
1. Order components from Bill of Materials and 3D Print case
2. Solder electronics and assembly into case
3. Insert SIM-Card with cellular data over 2G 
4. place your parameters in firmware "modem.h" line 5 and 8
5. Open Firmware via nRF connect in VS Code using Add existing application
6. Add build configuration: board: nRF52dk_nRF52832
7. Wait for build -> connect to board via SWD: github.com/AndruPol/nrf52832-recover

 
## Features
* Button to Sleep/Wake
* LED indicator of work status
* BLE advertising for future app
* Send location data to personal Google Sheet
* Charge with Type-C
* Compatible with lots of watch straps

> In current version, software may have significant bugs, improper code, not enough features, but I will edit/fix as I get hardware to debug

## 4-layer PCB
**Built in KiCad 10.99 night build**
> You can't export 3D model in default KiCad 10.0.3 because of error   
Ignore User.Comments

1 Layer - F.Cu  
2 Layer - 3.3V  
3 Layer - GND  
4 Layer - B.Cu  

## Why did I build it?
My bro asked me if I can build one. I don't have any interest for GPS locators, or needness for one. I just wanted to try myself in designing Wearable and also help my bro, so I did it and I felt real hardship of building in limited space. Currently my design is really bad and MUST be fixed in next updates by tightening free space. 

## Bill of Materials
| Name | Quantity |
| --- | --- |
| nRF52832 | x1 |
| CH340C | x1 |
| USB-C 14 pin | x1 |
| TP4056-42-ESOP8 | x1 |
| FS8205A | x1 |
| DW01A | x1 |
| XC6206P332MR-G | x1 |
| TXS0102DCUR | x1 |
| XUNPU SMN-303 | x1
| A7680C-LANV | x1 |
| ZIISOR TX4G-FPC-4015 | x1 |
| 16-213SURC/S530-A4/TR8 | x1 |
| 2450AT18A100E | x1 |
| NX2016SA-32MHZ | x1 |
| SC-20S,32.768kHz | x1 |
| BLM21PG300SN1D | x1 |
| ESDBW5V0A1 | x1 |
| LXES15AAA1-153 | x1 |
| L80-R | x1 |
| 500 mAh 503040 Battery | x1 |
| Straps - Galaxy watch 6 18mm | x1 |
| ST-LINK V2 | x1 |
| B3U-3000P LED Diode | x1 |
| LXES15AAA1-153 | x1 |
| 100nF C | x14 |
| 10uF C | x2 |
| 1uF C | x4 |
| 12pF C | x6 |
| 100uF C | x2 |
| 4.7uF C | x1 |
| 0.8pF C | x1 |
| 10pF C | x1 |
| 33pF C | x1 |
| 100pF C | x1 |
| 10pF C | x1 |
| 1.2K R | x1 |
| 1K R | x3 |
| 100 R | x5 |
| 5.1K R | x2 |
| 10K R | x1 |
| 0 R | x1 |
| 10uH L | x1 |
| 3.9nH L | x1 |
| 15nH L | x1 |
| M2 Screws | x8 |
