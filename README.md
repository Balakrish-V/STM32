# STM32

CMSIS - STM32L496ZGT6 (Bare Metal Coding)

Setup:

Use Keil MicroVision.
Project -> New MicroVision Project -> Name (.uvproj) -> Select Target Device.
Enable CMSIS CORE and Device Startup in Run-Time Environment.
Right-click Source Group 1 -> Add new item. Include required header files. Press F7 to fetch CMSIS files.
Debug Configuration:

Options for Target -> Debug -> Use: ST-Link Debugger.
UART Configuration:

Determine UART to configure. Set GPIO clock, alternate function, pull-up/down, and baud rate.
Verify FTDI LED blinks with correct serial port and baud rate using a terminal.
UART4_IT.c TX Operation:

TX sends a string only on Enter ('\r'). Echo data must be enabled for visibility.
DMA Configuration:

Check DMA2 Request Mapping in the datasheet for the correct DMA, channel, and peripheral request line.
