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





data_labels = ["x1_left", "x1_right",
               "x2_left", "x2_right",
               "y1_left", "y1_right",
               "y2_left", "y2_right",
               "z1_left", "z1_right",
               "z2_left", "z2_right"]

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

def read_data_loop(port, baudrate, N=-1,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT, save_filename=None, plot_refresh_rate=PLOT_UPDATE):
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

            """ Execute start sequence

            
            Set mode: single or continuous
            Set run: if single, set read flag to 1
            Set wakeup: wakeup 1
            
            """
            wakeup = 0
            data_line = set_wakeup(ser, wakeup,  message_timeout=message_timeout, )
            print(data_line)
            
            mode = 1
            data_line = set_mode(ser, mode,  message_timeout=message_timeout, )
            print(data_line)
            run = 1
            data_line = set_run(ser, run,  message_timeout=message_timeout, )
            print(data_line)
            wakeup = 1
            data_line = set_wakeup(ser, wakeup,  message_timeout=message_timeout, )
            print(data_line)


            tic = time.time()
            for i in range(N):
                # Checks if data available
                # TO DO
                # while 1:
                #   if check_data_pagination_available():
                #       break
                
                fifo_count = check_fifo_count(ser)
                print("FIFO count is: ",fifo_count)
                while(check_fifo_count(ser)==0):
                    continue


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
                    if toc-tic > plot_refresh_rate:
                        tic = time.time()
                        plot_data_segment(time_array[-plot_samples:], data_array[-plot_samples:,:])
                # time.sleep(0.1)
        
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
    # print("flushed")
    # plt.show()


        
def set_dac_value(port, baudrate, dac_signal, dac_channel, dac_value,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        dac_signal = dac_signal.upper()
        dac_channel = dac_channel.upper()
        dac_value = int(dac_value)
        command = "set_%s_%s"%(dac_signal, dac_channel)
        data = bytes("%s %d \n"%(command, dac_value), 'ascii')
        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line

def set_phase_delay(port, baudrate, phase_delay,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "set_C2V_CONFIG0"
        data = bytes("%s %d \n"%(command, phase_delay), 'ascii')
        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line

def set_en_oncyc(port, baudrate, en_oncyc,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "set_C2V_CONFIG1"
        data = bytes("%s %d \n"%(command, en_oncyc), 'ascii')
        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line


def set_en_onperiod(port, baudrate, en_onperiod,  message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "set_C2V_CONFIG2"
        data = bytes("%s %d \n"%(command, en_onperiod), 'ascii')
        ser.write(data)
        data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line

def set_mode(ser, mode,  message_timeout=MESSAGE_TIMEOUT):
    command = "set_mode"
    data = bytes("%s %d \n"%(command, mode), 'ascii')
    ser.write(data)
    data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line

def set_run(ser, run,  message_timeout=MESSAGE_TIMEOUT):
    command = "set_run"
    data = bytes("%s %d \n"%(command, run), 'ascii')
    ser.write(data)
    data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line

def set_wakeup(ser, wakeup,  message_timeout=MESSAGE_TIMEOUT):
    command = "set_wakeup"
    data = bytes("%s %d \n"%(command, wakeup), 'ascii')
    ser.write(data)
    data_line = read_serial_line(ser, timeout=message_timeout)
    return data_line


def check_fifo_count(ser):
    # reads the fifo count register and returns the count
    command = "fifo_count"
    ser.write(bytes("%s\n"%(command), 'ascii'))
    data_line = read_serial_line(ser, timeout=message_timeout)
    reg, count = data_line.strip().split(" ")
    count = int(count)
    return count

        
def set_debug_acquisition(port, baudrate, debug_acquisition, message_timeout=MESSAGE_TIMEOUT, serial_timeout=SERIAL_TIMEOUT):
    with serial.Serial(port, baudrate, timeout=serial_timeout) as ser:
        command = "debug_acquisition"
        data = bytes("%s %d \n"%(command, debug_acquisition), 'ascii')
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
    argParser.add_argument("-rd", "--read_data", help="Reads 1 or the given number of samples of all data", nargs='?', const=1, type=int)
    # argParser.add_argument("-rdN", "--read_data_length", help="Read data flag")
    argParser.add_argument("--debug_acquisition", metavar='debug_acquisition',help="set flag to debug acquisition by sending synthetic data")

    argParser.add_argument("-rm", "--read_memory", help="Read memory flag", action="store_true")
    argParser.add_argument("-rl", "--read_memory_length", metavar='read_memory_length', help="Read memory length")
    argParser.add_argument("-a", "--reg_addr", metavar='register_address',help="Register address")
    argParser.add_argument("-v", "--reg_value", metavar='write_value',help="Write_value")
    argParser.add_argument("-plot", "--plot", help="Plots the retreived read_data samples. Passed argument is number of samples to plot.", nargs='?', const=100, type=int)
    argParser.add_argument("--plot_refresh_rate", help="Set the plot refresh rate", type=float)
    argParser.add_argument("-s", "--save_file", nargs='?', )
    argParser.add_argument("--dac", nargs=3, metavar=("VACT|VEXC|VCOMP", "X1|X2|..|Z2", "VALUE"), help="Set respective DAC (16bit) register value. Example: -dac VACT X1 1024")
    argParser.add_argument("--phase_delay", type=int, metavar='PhaseDelay',help="C2V Configuration. PhaseDelay: defines the delay between modulation and demodualtion signals (delay = Nx7.2deg)")
    argParser.add_argument("--en_oncyc", type=int, metavar='enable_on_cycle_count',help="C2V Configuration. EN_onCycCnt: defines the number of delay cycles between demodualtion signal and the switch EN (delay = Nx20ns)")
    argParser.add_argument("--en_onperiod", type=int, metavar='enable_on_period',help="C2V Configuration. EN_onPeriod: defines the number of cycles the EN stays active (Period= Nx20ns)")
    # argParser.add_argument("--mode", type=int, metavar='operation_mode',help="Mode 0 = single acquisition, mode 1 = continuous")
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
    if args.plot_refresh_rate is not None:
        config['DEFAULT']["plotrefreshrate"] = str(args.plot_refresh_rate)



    with open(CONFIG_FILENAME, 'w') as configfile:
        config.write(configfile)


    port = config['DEFAULT']["Port"]
    baudrate = int(config['DEFAULT']["BaudRate"])
    serial_timeout = float(config['DEFAULT']["SerialTimeout"])
    message_timeout = float(config['DEFAULT']["MessageTimeout"])
    plot_refresh_rate = float(config['DEFAULT']["plotrefreshrate"])

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

    # Setting phase configuration values
    if args.phase_delay is not None:
        phase_delay = args.phase_delay
        data_line = set_phase_delay(port, baudrate, phase_delay,  message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)

    if args.en_oncyc is not None:
        en_oncyc = args.en_oncyc
        data_line = set_en_oncyc(port, baudrate, en_oncyc,  message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)
      
    if args.en_onperiod is not None:
        en_onperiod = args.en_onperiod
        print(en_onperiod)
        data_line = set_en_onperiod(port, baudrate, en_onperiod,  message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)
      


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
                              save_filename=args.save_file, 
                              plot_refresh_rate=plot_refresh_rate)
            # data = parse_data_single_line(data_line)


    elif args.read_memory:
        assert args.reg_addr is not None, "read_memory: must provide starting register address ."
        assert args.read_memory_length is not None, "read_memory: must provide lenght to read in bytes."

        reg_addr = int( args.reg_addr , 16)
        read_memory_length = int(args.read_memory_length, 16)
        data_line = read_memory(port, baudrate, reg_addr, read_memory_length, message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)

    elif args.dac is not None:
        dac_signal, dac_channel, dac_value = args.dac
        dac_signal = dac_signal.upper()
        dac_channel = dac_channel.upper()
        dac_value = int(dac_value)

        data_line = set_dac_value(port, baudrate, dac_signal, dac_channel, dac_value,  message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)
    
    elif args.debug_acquisition is not None:

        data_line = set_debug_acquisition(port, baudrate, args.debug_acquisition, message_timeout=message_timeout, serial_timeout=serial_timeout)
        print(data_line)
    
    
      




    plt.show(block=True)
    # elif args.command:
    # # if args.command is not None:
    #     command = args.command[0]
    #     reg_addr = args.command[1]
    #     reg_value = args.command[2]
    


    