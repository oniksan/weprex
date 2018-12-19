# Weprex (Webstella protocols exchange)

## Dowload binary distributive
### Windows 7, 8, 10
#### 32-bit installer:
https://github.com/oniksan/weprex/releases/download/0.1.1/weprex-0.1.1-i686.32bit.-installer.exe
#### 64-bit installer:
https://github.com/oniksan/weprex/releases/download/0.1.1/weprex-0.1.1-x86-64-installer.exe
### Other platforms
#### Compile the executable file yourself:
- Dowload Qt 5.12 or later
- Dowload Qt Creator 4.8
- Open Project "weprex.pro"
- Compile (specialized third-party libraries are not required)
- Put "weprex_V.X.X_manual.pdf" to working directory
- Use it

## About
Weprex is cross-platform GUI software allows polling devices (as "master")
using various data transfer protocols by request-responce type.<br/>
The software provides data polling in real time
(relative to predetermined intervals and hardware support capabilities)
and their presentation in graphical and tabular form.

## Screenshots
![Main screen](/screenshots/main.png)<br/>
![Chart](/screenshots/chart.png)<br/>
![Table](/screenshots/table.png)<br/>
![Log](/screenshots/log.png)<br/>
![Modbus registers](/screenshots/regs.png)<br/>
![Interface adding](/screenshots/iface.png)<br/>
      

## Software assignment
- Industrial automation
- Building automation
- Communication with microcontrollers

## Features (in current version)
### Supported
- Modbus TCP protocol (master)
- Modbus RTU protocol (master)
- TCP/IP interface for any of supported protocols
- Serial interface (COM, UART, RS-232, RS-485 etc.) for any of supported protocols
- Output polled data in different views: floating point, integer (signed/unsigned) decimal, octal, hexidecimal, binary, ASCII 
- Two-scale data representation on the chart of integer, floating point and binary data
- Tabular view of data
- Logging raw request-response packages data in hex view
- Saving preset and default sessions
- Simultaneous polling of multiple interfaces

### Not supported yet
- Protocols other than those listed above
- Slave regime

### Remark
Support for new protocols and features will be introduced:
- when used by the author for personal purposes
- with sufficient demand for this software from users
- upon request of users when providing the necessary documentation and/or devices

## Contacts
Any suggestions and requests:
- Сreate pull requests
- Or send an e-mail: weprexsoft@gmail.com

## Manual
Will be presented in the near future.
