# Master SPI Tester

The project provides a way to test and evaluat the SPI communication of with the FPGA by using a Teensy 4.1 with an Arduino sketch to operate as an SPI Master.

The user can use directly the serial port of the Arduino, or the `master_spi_tester_cli.py` command line interface to send commands to the FPGA

# Getting Started

1. Connect the OBC cable from the Teensy connector to the FPGA board
2. Power on the board with the external power supply (confirm the shunt that powers the teensy from the USB 5V bus is cut).
3. Compile and Upload the `Master_SPI_Tester.ino` to the Teensy 4.1
4. Operate through serial port or the command line interface


# Operation

The Arduino code receives ascii commands and replies in ascii as well. This was decided for simplicity of implementation and considering a low throughput of data transmission.

The accepted commands are in ascii, and followed by the relevant parameters (integer bytes 8 bits) delimited by spaces such as the example below:

```
> # command param1 param2
> write_reg 5 255
b'5 7\r\n'
```

## Serial Port commands


### Read from register
 `read_reg REGISTER_ADDRESS` 

 Reads 1 byte from the `register_address` position and sends back through serial with the syntax `[register_address] [value]`, where the `value` is the byte stored in memory.

Example:
```
> read_reg 5 
5 7 
```


### Write to register 

`write_reg REGISTER_ADDRESS REGISTER_VALUE` 

Writes the `register_value` byte in the `register_address` memory position. It then reads the same register position and sends back the read value as in the `read_reg` command.

Example:
```
> write_reg 5 255
5 7 
```

### Read data

 `read_data`
 
 Reads the 12 memory positions corresponding to the data positions [`X0_left`, `X0_right`, ... , `Z1_left`, `Z1_right`]. Each of these positions is an integer of up to 24 bits.

Example:
```
> read_data
2352342 4324 42342 12312 3213 5345 23523 1231 31232 2434 2342 4636 
```

### Read memory sequence

 `read_memory REGISTER_ADDRESS LENGTH`
 
 Reads `length` number of bytes starting from `register_address` and replies each byte delimited by spaces

Example:
```
> read_memory 0 18
0 85 0 0 0 0 7 0 1 165 0 0 0 0 0 0 0 165 0
```




## Command Line Interface 

Using the `master_spi_tester_cli.py` tool.

Check the help command:

```
> python master_spi_tester_cli.py -h
usage: master_spi_tester_cli.py [-h] [-p PORT] [-b BAUD] [-st SERIAL_TIMEOUT] [-t TIMEOUT] [-w] [-r] [-rd [READ_DATA]] [-rm]
                                [-rl read_memory_length] [-a register_address] [-v write_value]

Master SPI Tester for FPGA

options:
  -h, --help            show this help message and exit
  -p PORT, --port PORT  COM Port
  -b BAUD, --baud BAUD  Baud Rate
  -st SERIAL_TIMEOUT, --serial_timeout SERIAL_TIMEOUT
                        Serial port timeout
  -t TIMEOUT, --timeout TIMEOUT
                        Message reply timeout
  -w, --write           Write flag
  -r, --read            Read flag
  -rd [READ_DATA], --read_data [READ_DATA]
                        Reads 1 or the given number of samples of all data
  -rm, --read_memory    Read memory flag
  -rl read_memory_length, --read_memory_length read_memory_length
                        Read memory length
  -a register_address, --reg_addr register_address
                        Register address
  -v write_value, --reg_value write_value
                        Write_value
```

### Serial configuration

The tool uses a the serial configuration stored in `master_spi_tester.ini`, which is updated everytime the tool is executed with explicit `--port`, `--baud`, or timeout arguments. The text configuration file may also be altered in notepad.

To set the port, baud rate and timeouts from the command line:

```
> python master_spi_tester_cli.py -p COM6 -b 9600 -st 1
Port is COM6
Baud Rate is 9600
Serial timeout is 1.0
```


### Read from register

Accepts HEX values for the register address and values 
Example:
```
> python master_spi_tester_cli.py -r -a 0x0
Port is COM6
Baud Rate is 9600
Serial timeout is 1.0
0 85
```


### Write to register 

Example:
```
> python master_spi_tester_cli.py -w -a 0x05 -v 0xff
Port is COM6
Baud Rate is 9600
Serial timeout is 1.0
5 7
```

### Read data

Example:
```
> python master_spi_tester_cli.py -rd
Port is COM6
Baud Rate is 9600
Serial timeout is 1.0
0 0 0 0 0 0 0 0 0 0 0 0 
```

### Read memory sequence

Example:
```
> python master_spi_tester_cli.py -rm -a 0x0 -rl 18
Port is COM6
Baud Rate is 9600
Serial timeout is 1.0
0 85 0 0 0 0 7 0 1 165 0 0 0 0 0 0 0 165 0 0 0 0 0 0 0
```



