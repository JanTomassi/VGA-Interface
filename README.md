# VGA-Interface
This project is for a course that I am taking for my Bachelor's degree in CS; the course name is Embedded Systems for the Internet of Thing
## Goal
The goal is to have a library that can be easily adapted to any platform or board. The communication with the display should be done using technology that any board implements SPI, I2C, or UART.
## Code coverage
The current implementation uses the DiscoF407VG timers to obtain a two-clock that drives the VGA protocol's HSYNC and VSYNC.
For the pixel, an SPI with data obtained using the DMA is used; the clock of the SPI is at 36MHZ.
## TODO
- [ ] Use an FPGA
  - [ ] to drive the pixel
  - [ ] and H/VSync
  - [ ] with SDRAM as a framebuffer to have a more consistent image
- [ ] Make the library setup as simple as possible
  - [ ] Add a single header file that the user of the library import
  - [ ] Define a simple protocol to communicate with the FPGA, possibly a one or two-wire protocol
