

[![GH1 - Guitar Hero Embedded Project](GH1-readme-banner.png)](GH1%20Documentation.pdf)


[![Read Documentation](https://img.shields.io/badge/GH1-Read%20Documentation-cc3333?style=for-the-badge)](GH1%20Documentation.pdf)

GH1 is a portable g Guitar Hero style arcade rhythm game embedded system. It runs on
an STM32F4 Micro Controller (GH1-a) and is displayed on an ILI9341 Display. Every major 
aspect of the system from the mechanical enclosure and electrical schematics to the 
embedded firmware and gameplay software was personally designed and developed by me. GH1 
represents a complete hardware-software integration project, combining custom electronics, 
mechanical design, real-time graphics, DMA, audio processing, and embedded systems programming 
into a standalone guitar hero style platform. 

### ⚡ Engineering Highlights

- **End-to-End System Design** — Independently designed and developed the mechanical enclosure, electrical schematics, custom PCB, hardware integration, and embedded software.

- **STM32F407 ARM Cortex-M4 Microcontroller** — Custom C firmware running at **144 MHz**, integrating GPIO, SPI, SDIO, DAC, hardware timers, and DMA peripherals.

- **Custom 2-Layer PCB (GH1-b)** — Designed in **KiCad** and professionally fabricated, featuring button signal routing, JST-XH connectors, and onboard **5V-to-3.3V voltage regulation**.

- **Custom 3D-Printed Enclosure** — Designed an entire five-piece guitar-shaped housing in **FreeCAD**, complete with mounting structures, component recesses, and detailed engineering drawings.

- **DMA-Driven Audio Streaming** — Streams **44.1 kHz, 16-bit PCM WAV audio** directly from a microSD card using **SDIO, FatFS, and DMA double buffering**, enabling continuous playback without storing the entire song in RAM.

- **Hardware-Timed Audio Playback** — Timer-triggered DAC output drives a **PAM8403 audio amplifier** and dual **3W speakers**, integrating digital audio processing with analog sound reproduction.

- **High-Speed SPI Graphics** — Drives a **2.8-inch, 240 × 320 ILI9341 TFT display** over a **36 MHz SPI interface**, rendering five scrolling note lanes and real-time gameplay visuals.

- **Custom Arcade Input System** — Five illuminated **30mm arcade buttons** connected through the custom PCB, with GPIO input polling and timing-sensitive gameplay logic.

- **Portable Battery-Powered Architecture** — Powered by a **14.8V LiPo battery**, with DC-DC voltage regulation, an XT60 power switch, and an integrated power distribution system.

- **Comprehensive Engineering Documentation** — Includes electrical schematics, PCB layouts, CAD models, dimensioned mechanical drawings, MCU pin configurations, assembly diagrams, and firmware architecture.


<img width="495" height="878" alt="gh1 framed" src="https://github.com/user-attachments/assets/68930d53-085f-43f1-b9cb-c1b5a1c0e160" />
<img width="870" height="799" alt="gh1 pic 2" src="https://github.com/user-attachments/assets/ba97fab1-b7bd-49fe-a95a-e0ec4115ed58" />

