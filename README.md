# VGA-Interface
This project is for a course that I am taking for my Bachelor degree in CS; the course name is Embedded Systems for the Internet of Thing
## Goal
The end goal is to have a library that can be easily adapted to any platform or board, 
the communication with the display should be done using technology that any board implement like SPI, I2C or UART.
## Code covrage
The curent implementation uses the DiscoF407VG, timers to obtain a two clock that drive the HSYNC and VSYNC of the VGA protocol.
For the pixel a SPI with data obtained using the DMA is used, the clock of the SPI is at raflay 36MHZ.
## TODO
- [ ] Use a FPGA
  - [ ] to drive the pixel
  - [ ] and H/VSync
  - [ ] with SDRAM as framebuffer to have a more consistent image
- [ ] Make the library setup as simple as possible
  - [ ] Add a single header file that user of the library can simple import
  - [ ] Define a simple protocol to comunicate with the FPGA possibly a one or two wire protoocl
