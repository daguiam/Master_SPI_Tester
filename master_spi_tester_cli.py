import datetime 

import numpy as np

import matplotlib.pyplot as plt

import serial 
import argparse
import configparser

import time


MESSAGE_TIMEOUT = 1
SERIAL_TIMEOUT = 1

PLOT_UPDATE = 2





data_labels = ["x0_left", "x0_right",
               "x1_left", "x1_right",
               "y0_left", "y0_right",
               "y1_left", "y1_right",
               "z0_left", "z0_right",
               "z1_left", "z1_right"]

def read_serial_all(ser, timeout=MESSAGE_TIMEOUT):
    assert type(ser) == type(serial.Serial()), "must provide opened serial port instance"
    data_line = ""
    tic = time.time()
    while 1:
        while ser.in_waiting:  # Or: while ser.inWaiting():
            
            data_line = ser.readline()
            data_line = data_line.decode('ascii').strip()

            print(data_line)
            tic = time.time()
        toc = time.time()
        if toc-tic > timeout:
            # print("Wait timeout reached", timeout)
            data_line = "timeout"
            break
    return data_line


def read_serial_line(ser, timeout=MESSAGE_TIMEOUT):
    # assert type(ser) == type(serial.Serial()), "must provide opened serial port instance"
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
    data_line = data_line.decode('ascii').strip()
    assert data_line != "", "read_serial_line: Read empty line. Error in communication?"

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
        data = parse_data_single_line(data_line)
    return data

        
def read_memory(port, baudrate, register_address, length,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "read_memory"
        data = bytes("%s %d %d\n"%(command, register_address, length), 'ascii')
        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line




def parse_data_single_line(data_line, delimiter=" "):
    """ Parses the dataline of space-delimited integer counts for each data point.
    
    Returns an array of integers"""
    
    data = data_line.strip()
    data_array = data.split(delimiter)
    data_array = [int(d) for d in data_array]
    return data_array

def read_data_loop(port, baudrate, N=-1,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT, save_filename=None):
    """ Continuously reads the data available in the memory and parses it
    """

    assert N>=1, "N number of samples to acquire must be higher than 1"
    global plot_flag
    global plot_samples

    try:
        time_array = []
        data_array = np.empty((0,12), int)

        fd = None
        save_flag = False
        if save_filename is not None:
            save_flag = True
        
        if save_flag:
            print("Saving data samples to ", save_filename)
            fd = open(save_filename, "a")
            fd.write("#time,")
            header = ",".join(data_labels)
            fd.write("%s\n"%(header))


        with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:

            tic = time.time()
            for i in range(N):
                # Checks if data available
                # TO DO
                # while 1:
                #   if check_data_pagination_available():
                #       break


                command = "read_data"
                data = bytes("%s\n"%(command), 'ascii')
                ser.write(data)

                data_line = read_serial_line(ser, timeout=message_timeout)
                data = parse_data_single_line(data_line)

                t = time.time()
                print(t, "\t", data)

                time_array.append(t)
                data_array = np.vstack([data_array, data])

                if save_flag:
                    fd.write("%f,"%(t))
                    line = ",".join([str(d) for d in data])
                    fd.write(line)
                    fd.write('\n')

                if plot_flag:
                    toc = time.time()
                    if toc-tic > PLOT_UPDATE:
                        tic = time.time()
                        plot_data_segment(time_array[-plot_samples:], data_array[-plot_samples:,:])
                time.sleep(0.1)
        
        if save_flag:
            fd.close()
    except KeyboardInterrupt:

        if save_flag:
            fd.close()
        print("Keyboard interrupt. Stopped acquisition")

    return (time_array, data_array)


                        

def plot_data_segment(time, data):
    global plot_figure
    global plot_ax
    global plot_lines
    if plot_figure is None:
        plt.ion()
        plot_figure, plot_ax = plt.subplots()
        for label in data_labels:
            hl, = plot_ax.plot(range(10), np.ones(10), label=label)
            plot_lines.append(hl)


        # plt.sca(plot_ax)
        # plt.plot(x,y)
        plt.legend(loc=3, fontsize=8, ncol=2)
        plt.ylabel("Counts")
        plt.xlabel("Time [s]")
        plot_figure.canvas.draw()
        plt.show(block=False)

    lines = plot_ax.get_lines()
    # lines = plot_lines

    # x = [datetime.date.fromtimestamp(t) for t in time]
    x = time
    # print(list(lines))
    # plot_ax.clear()
    for i in range(len(lines)):
        label = data_labels[i]
        y = data[:,i]

        # print(x,y, len(x), len(y))
        line = lines[i]
        # print(line)
        lines[i].set_xdata(x)
        # line.set_xdata(range(len(y)))
        lines[i].set_ydata(y)

        # print(line.get_ydata())
    
        # plt.sca(plot_ax)
        # plt.plot(x,y)
    
        # break

    plot_ax.set_xlim(min(x), max(x))
    plot_ax.set_ylim(data.min(), data.max())
    plot_figure.canvas.draw()
    plot_figure.canvas.flush_events()
    print("flushed")
    # plt.show()





        

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
    argParser.add_argument("-rd", "--read_data", help="Reads 1 or the given number of samples of all data", nargs='?', const=1, type=int)
    # argParser.add_argument("-rdN", "--read_data_length", help="Read data flag")
    argParser.add_argument("-rm", "--read_memory", help="Read memory flag", action="store_true")
    argParser.add_argument("-rl", "--read_memory_length", metavar='read_memory_length', help="Read memory length")
    argParser.add_argument("-a", "--reg_addr", metavar='register_address',
                            help="Register address")
    argParser.add_argument("-v", "--reg_value", metavar='write_value',
                            help="Write_value")
    argParser.add_argument("-plot", "--plot", help="Plots the retreived read_data samples. Passed argument is number of samples to plot.", nargs='?', const=100, type=int)
    argParser.add_argument("-s", "--save_file", nargs='?', )
    # argParser.add_argument("--save_file", help="Read memory flag", action="store_true")

    
    
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

    # print(args)

    
    plot_figure = None
    plot_ax = None
    plot_lines = []
    plot_flag = False
    plot_samples = 0

    save_file_flag = False
    save_filename = None


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

    elif args.read_data is not None:

        # plot_flag = False
        if args.plot is not None:
            plot_flag = True
            plot_samples = args.plot # number of samples to keep in plot

        # if args.save_file is not None:
        #     save_file_flag = True
        #     save_filename = args.save_file

        #     # create figure
        #     plot_figure, plot_ax = plt.subplots(figsize=(10, 8))
        N = args.read_data
        data = read_data_loop(port, baudrate, N, 
                              message_timeout=message_timeout, serial_timeout=serial_timeout,
                              save_filename=args.save_file)
            # data = parse_data_single_line(data_line)


    elif args.read_memory:
        assert args.reg_addr is not None, "read_memory: must provide starting register address ."
        assert args.read_memory_length is not None, "read_memory: must provide lenght to read in bytes."

        reg_addr = int( args.reg_addr , 16)
        read_memory_length = int(args.read_memory_length, 16)
        data_line = read_memory(port, baudrate, reg_addr, read_memory_length, message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)




    plt.show(block=True)
    # elif args.command:
    # # if args.command is not None:
    #     command = args.command[0]
    #     reg_addr = args.command[1]
    #     reg_value = args.command[2]
    


    