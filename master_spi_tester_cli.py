import numpy as np

import matplotlib.pyplot as plt

import serial 
import argparse
import configparser

import time


MESSAGE_TIMEOUT = 1
SERIAL_TIMEOUT = 1

def read_serial_all(ser, timeout=MESSAGE_TIMEOUT):
    assert type(ser) == type(serial.Serial()), "must provide opened serial port instance"
    data_line = ""
    tic = time.time()
    while 1:
        while ser.in_waiting:  # Or: while ser.inWaiting():
            
            data_line = ser.readline()
            print(data_line)
            tic = time.time()
        toc = time.time()
        if toc-tic > timeout:
            # print("Wait timeout reached", timeout)
            data_line = "timeout"
            break
    return data_line


def read_serial_line(ser, timeout=MESSAGE_TIMEOUT):
    assert type(ser) == type(serial.Serial()), "must provide opened serial port instance"
    data_line = ""
    tic = time.time()
    # while not ser.in_waiting:  # Or: while ser.inWaiting():tic = time.time()
    #     toc = time.time()
    #     if toc-tic > timeout:
    #         # print("Wait timeout reached", timeout)
    #         data_line = "timeout"
    #         break
    #     continue
    data_line = ser.readline()
    # print(data_line)
    return data_line



def write_register(port, baudrate, register_address, value, message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
     # Connecting
    with serial.Serial(port, baudrate, timeout=SERIAL_TIMEOUT) as ser:
        command = "write_reg"
        data = bytes("%s %d %d\n"%(command, register_address, value), 'ascii')
        ser.write(data)
   
        # The write command is confirmed by reading the register and returning written value
        data_line = read_serial_line(ser, timeout=message_timeout)
        # data_line = read_serial_all(ser, timeout=message_timeout)
    return data_line





def read_register(port, baudrate, register_address, message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
     # Connecting
    with serial.Serial(port, baudrate, timeout=SERIAL_TIMEOUT) as ser:
        command = "read_reg"
        ser.write(bytes("%s %d\n"%(command, register_address), 'ascii'))

        data_line = read_serial_line(ser, timeout=message_timeout)

    return data_line

def read_data(port, baudrate, message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "read_data"
        data = bytes("%s\n"%(command), 'ascii')

        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line

        
def read_memory(port, baudrate, register_address, length,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "read_memory"
        data = bytes("%s %d %d\n"%(command, register_address, length), 'ascii')
        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line




CONFIG_FILENAME = 'master_spi_tester.ini'
if __name__ == "__main__":

    config = configparser.ConfigParser()
    config.read(CONFIG_FILENAME)
    config.sections()

    # port = config['DEFAULT']["Port"]
    # baudrate = config['DEFAULT']["BaudRate"]


    ### Parsing argumetns
    argParser = argparse.ArgumentParser(description="Master SPI Tester for FPGA")
    argParser.add_argument("-p", "--port", help="COM Port")
    argParser.add_argument("-b", "--baud", help="Baud Rate")
    argParser.add_argument("-st", "--serial_timeout", help="Serial port timeout")
    argParser.add_argument("-t", "--timeout", help="Message reply timeout")
    # argParser.add_argument("-c", "--command", nargs='+', help="Command TBD", )#choices=['rock', 'paper', 'scissors'])
    # argParser.add_argument("-wr", "--write_reg", nargs=2, metavar=('WRITE_REG', 'VALUE'),
    #                        help="Writes the [VALUE] to the [WRITE_REG] register address, and then reads from the same register to confirm.")
    # argParser.add_argument("-rd", "--read_reg", nargs=1, metavar='READ_REG',
    #                         help="Reads from [READ_REG] register address.")

    argParser.add_argument("-w", "--write", help="Write flag", action="store_true")
    argParser.add_argument("-r", "--read", help="Read flag", action="store_true")
    argParser.add_argument("-rd", "--read_data", help="Read data flag", action="store_true")
    argParser.add_argument("-rm", "--read_memory", help="Read memory flag", action="store_true")
    argParser.add_argument("-rl", "--read_memory_length", metavar='read_memory_length', help="Read memory length")
    argParser.add_argument("-a", "--reg_addr", metavar='register_address',
                            help="Register address")
    argParser.add_argument("-v", "--reg_value", metavar='write_value',
                            help="Write_value")
    
    # argParser.add_argument("--verbose", help="Verbose flag", action="store_true")


    args = argParser.parse_args()


    # Loading config parameters from command line
    if args.port is not None:
        config['DEFAULT']["Port"] = args.port
    if args.baud is not None:
        config['DEFAULT']["BaudRate"] = args.baud
    if args.serial_timeout is not None:
        config['DEFAULT']["SerialTimeout"] = args.serial_timeout
    if args.timeout is not None:
        config['DEFAULT']["MessageTimeout"] = args.timeout

    with open(CONFIG_FILENAME, 'w') as configfile:
        config.write(configfile)


    port = config['DEFAULT']["Port"]
    baudrate = int(config['DEFAULT']["BaudRate"])
    serial_timeout = float(config['DEFAULT']["SerialTimeout"])
    message_timeout = float(config['DEFAULT']["MessageTimeout"])

    print("Port is %s"%(port))
    print("Baud Rate is %s"%(baudrate))
    print("Serial timeout is %s"%(serial_timeout))



    if args.write:
        assert args.reg_addr is not None, "write_register: must provide register address."
        assert args.reg_value is not None, "write_register: must provide register value to write."
        reg_addr = int( args.reg_addr , 16)
        reg_value = int(args.reg_value, 16)
        data_line = write_register(port, baudrate, reg_addr, reg_value, message_timeout=message_timeout, serial_timeout=serial_timeout)

        print(data_line)
    elif args.read:
        assert args.reg_addr is not None, "read_register: must provide register address."
        reg_addr = int( args.reg_addr , 16)
        data_line = read_register(port, baudrate, reg_addr, message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)

    elif args.read_data:
        data_line = read_data(port, baudrate, message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)

    elif args.read_memory:
        assert args.reg_addr is not None, "read_memory: must provide starting register address ."
        assert args.read_memory_length is not None, "read_memory: must provide lenght to read in bytes."

        reg_addr = int( args.reg_addr , 16)
        read_memory_length = int(args.read_memory_length, 16)
        data_line = read_data(port, baudrate, message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)

    # elif args.command:
    # # if args.command is not None:
    #     command = args.command[0]
    #     reg_addr = args.command[1]
    #     reg_value = args.command[2]
    
    