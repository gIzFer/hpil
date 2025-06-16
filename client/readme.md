# HP3468A client

Example on how to interact with the firmware. I made it to mainly use it with the HP3468A. No idea of how it will interact with any other HP-IL device.

----
##### Contents
- [How to use](#How-to-use)
- [Code "docs"](#Code-"docs)
 - [setDisplay](#setDisplay)
 - [setDigits](#setDigits)
 - [setTriggerMode](#setTriggerMode)
 - [setAutoZero](#setAutoZero)
 - [setFunction](#setFunction)
 - [readSingleValue](#readSingleValue)
 - [getStatus](#getStatus)
 - [getCal](#getCal)
 - [printStatus](#printStatus)
 - [triggerReading](#triggerReading)

 ----
## How to use

`multimeter = HP3468A(args.usb_id)`
in my case I get it at /dev/ttyUSBn, so i.e. I call `./main.py --usb_id 0` to run the example

uhh... Not very multiOS compatible... change accordingly to your OS... :P
`self.ser = serial.Serial("/dev/ttyUSB" + str(usb_id), 115200, timeout=1)`

## Code "docs"

### setDisplay

**desc**: changes display mode to either normal or to show text

**method**: `setDisplay(displayMode_, text="")`

**params**:

-   `displayMode_`: enum `displayModes.normal` or `displayModes.text`
-   `text`: empty by default. automatically truncated if length exceeds available space on display

_always write text in uppercase characters otherwise cursed things might show_

_(they "stored other characters in the lowercase characters space")_

### setDigits

**desc**: changes meter digits. Precision and sampling speed change accordingly

**method**: `setDigits(digits)`

**params**:

-   `digits`: valid values are 3, 4 and 5

### setTriggerMode

**desc**: trigger internally or single trigger

**method**: `setTriggerMode(triggerMode_)`

**params**:

-   `triggerMode_`:  enum `triggerModes.internal` or `triggerModes.single`

### setAutoZero

**desc**: Enable or disable autozero, enabled by default. (Nulls internal offset errors)

**method**: `setAutoZero(value)`

**params**:

-   `value`:  True or False

### setFunction

**desc**: Configure function and its range

**method**: `setFunction(function_, range=0)`

**params**:

-   `function_`: enum `functions`
-   `range`: numerical value.

Since range depends on option parameters values are combined here:

-   `functions.DC_voltage`: 0 (Autorange), 1 (.3V), 2 (3V), 3 (30V), 4 (300V)
-   `functions.AC_voltage`: 0 (Autorange), 1 (.3V), 2 (3V), 3 (30V), 4 (300V)
-   `functions.ohms_2_wire`: 0 (Autorange), 1 (300Ω), 2 (3KΩ), 3 (30KΩ), 4 (300KΩ), 5 (3MΩ), 6 (30MΩ)
-   `functions.ohms_4_wire`: 0 (Autorange), 1 (300Ω), 2 (3KΩ), 3 (30KΩ), 4 (300KΩ), 5 (3MΩ), 6 (30MΩ)
-   `functions.DC_current`: 0 (Autorange), 1 (3A)
-   `functions.AC_current`: 0 (Autorange), 1 (3A)
-   `functions.ohms_extended`: 0 (Autorange), 1 (10MΩ//Rx)

### readSingleValue

**desc**: read currently measured value

**method**: `readSingleValue()`

**params**: none, returns float

### getStatus

**desc**: probes the meter for the status bytes (unsure if it works correctly, need to do more research and testing on this)

**method**: `getStatus()`

**params**: none, returns nothing, use `printStatus()` to get reply

### getCal

**desc**: gets the meter's raw cal data

**method**: `getCal()`

**params**: none, simply prints the cal data

### printStatus

**desc**: prints response to `getStatus()`

**method**: `printStatus():`

**params**: none, simply prints data gotten by `getStatus():`

### triggerReading

**desc**: triggers a reading

**method**: `triggerReading():`

**params**: none, simply triggers a reading, use `readSingleValue()` to get readen value
