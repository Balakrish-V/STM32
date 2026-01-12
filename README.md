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


Demo_Kalman_Filter code contains a basic kalman filter implementation ported from an arduino IDE to stm32 platform. 
So we are using a potentiometer which is very noisy for the analog input. The inbuilt ADC samples it. 
This filter gives us smoother variations of the sampled analog value into the digital value. 
It also includes the SWD output graph inside the Startup folder -> result.jpeg.
An additional logic is implemented along with it. 
Based on the ADC Values, we need to increase/decrease the delay between the toggling of the on-board RED LED. 
The higher the ADC value, the faster is the LED blinking and lesser is the delay between toggling of LED. 
Additionally using HAL_Delay in this application is not preferred as it consumes the CPU core. 
So ADC values will be leading and the Delay will be out of sync or the vice versa might be true as they all are in while(1) successively. 
So to say, we have used the NON-BLOCKING HAL_GetTick function for delays.
