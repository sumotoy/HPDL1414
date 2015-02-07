# HPDL1414
A fast library for HPDL/DLG/DLR1414 displays and Teensy3/Arduino's
I have coded another library for those displays but never finished and I was not happy. I got several Intelligent Displays NOS of this type so I build up another library that deals with limitations of those displays (pretty old but great looking) and let me use as any other display without loose precious I/O from my microcontrollers.
Displays are connected to a GPIO expander via SPI, the MCP23s17 and let you drive 6 displays directly by using only 3 wires that can be fully shared with other 7 GPIO from MCP series thanks to microchip HAEN feature and 2 of this wires can be shared with other SPI devices so at the end in the worst situation you will loose just one wire!
The HPDL1414 (or other strain from other companies) have a limited number of characters so don't expect graphic capabilities.<br>
I have included some simple special effects on text, the one used in many sci-fi movies that give an extra bonus to this library.<br>
I have also find an hidden feature that let me blank the display so the library support blanking for other char.
Here's the features:<br><br>

- Support any display feature
- 6x4 char directly drived, more by adding a chip and slightly modify a function
- Scrolling
- Blanking of any char
- Does not use any lookup table so tiny memory footprints
- Only 3 wires (sharable) used
- Fast SPI operation, fully SPI transaction compatible
- Compatible with Teensy3, UNO, any 8 bit arduino, DUE and 1284P/640P
- Will be compatible with energia and his supported processor in some days
- Compatible with IDE 1.0.6, IDE 1.0.6 Teensy and IDE 1.5.8
For wiring, look inside the .h file of the library!<br>
Of course you will need my gpio_expander, a GPIO universal library since this one use it
https://github.com/sumotoy/gpio_expander
Pictures and videos that proof it's working will come in soon<br>
