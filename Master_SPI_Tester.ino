/*
Master SPI Tester for FPGA controller boards to readout accelerometer data

Purpose:
Evaluate how to read/write and validate the register bank on the FPGA


#### Pinouts:
## Tester board with teensy by Pedro 2022:

const int SS_pin = 10;
const int SCK_pin = 13;
const int MISO_pin = 12;
const int MOSI_pin = 11;

## Final FPGA board on for launch
const int SS_pin = ??;
const int SCK_pin = ??;
const int MISO_pin = ??;
const int MOSI_pin = ??;


*/


// the sensor communicates using SPI, so include the library:
#include <SPI.h>


#include "fpga.h"


const byte READ = 0b11111110;     // FPGA read command
const byte WRITE = 0b00000001;   // FPGA write command

#define SPI_CLK 4000000
#define SPI_MODE SPI_MODE0

#define SERIAL_BAUDRATE 115200

// Tester board with teensy by Pedro 2022:
// Uneeded since using the SPI library and on teensy these are already the correct pinouts (no need to add SS pin)
const int SS_pin = 10;
const int SCK_pin = 13;
const int MISO_pin = 12;
const int MOSI_pin = 11;

const int chipSelectPin = SS_pin;

// const int LED_pin = LED_BUILTIN;


// D1, D2, D3, D4, D5, D6, D7, D8
int LED_array_pins[] = {33, 34, 35, 36, 37, 38, 39, 40};
const int LED_array_pins_size = 8;



void setup() {
  // digitalWrite(SS, HIGH);  // Start with SS high

  // pinMode(SS_pin, OUTPUT);
  // pinMode(SCK_pin, OUTPUT);
  // pinMode(MISO_pin, INPUT);
  // pinMode(MOSI_pin, OUTPUT);

  Serial.begin(SERIAL_BAUDRATE);
  SPI.begin();
  pinMode(chipSelectPin, OUTPUT);
  

  for (int i=0; i<LED_array_pins_size; i++){
    
    int pin = LED_array_pins[i];
    pinMode(pin, OUTPUT);
  }



  Serial.println("Master SPI Tester ");
  Serial.println("Setup Complete");
  Serial.println("Waiting for commands...");

}


unsigned int ni = 0;


void loop() {
  int read_value = 0xfffa;
  // unsigned int read_value = 0xfa;
  String command_string = "";
  String command = "";
  String parameter1 = "";
  String parameter2  = "";
  byte reg_addr = 0;
  byte reg_value = 0;


  if(Serial.available() > 0){


    // String command_string = Serial.readString();  //read until timeout
    command_string = Serial.readStringUntil('\n');  //read until terminator
    // Serial.print("Command received: ");
    // Serial.println(command_string);
    command = splitStringDelimiter(command_string, 0, ' ');
    parameter1 = splitStringDelimiter(command_string, 1, ' ');
    parameter2 = splitStringDelimiter(command_string, 2, ' ');

    reg_addr = (byte) byte(parameter1.toInt());
    reg_value = (byte) byte(parameter2.toInt());

    if (0){
      Serial.print(command);
      Serial.print(reg_addr, HEX);
      Serial.println(reg_addr, HEX);
    }
    // Serial.println(command);
    // Serial.println(parameter1);
    // Serial.println(parameter2);

    if (command == "device_id") {
      read_value =  readRegister(DEVID, 1);
      highlight_led_byte(read_value);

    } else if (command == "write_reg") {
      writeRegister(reg_addr, reg_value);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print(reg_addr);
      Serial.print(" ");
      Serial.println(read_value);
      
    } else if (command == "read_reg") {
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print(reg_addr);
      Serial.print(" ");
      Serial.println(read_value);
      
    } else if (command == "read_data") {

      // Reads complete pagination of data (12 registers)
      // byte data_registers[] = {DATAX0_LEFT_N0_ADDR, DATAX0_RIGHT_N0_ADDR,
      //                   DATAX1_LEFT_N0_ADDR, DATAX1_RIGHT_N0_ADDR,
      //                   DATAY0_LEFT_N0_ADDR, DATAY0_RIGHT_N0_ADDR,
      //                   DATAY1_LEFT_N0_ADDR, DATAY1_RIGHT_N0_ADDR,()
      //                   DATAZ0_LEFT_N0_ADDR, DATAZ0_RIGHT_N0_ADDR,
      //                   DATAZ1_LEFT_N0_ADDR, DATAZ1_RIGHT_N0_ADDR
      //                   };

      byte data_registers[] = {DATAX1_LEFT, DATAX1_RIGHT,
                        DATAX2_LEFT, DATAX2_RIGHT,
                        DATAY1_LEFT, DATAY1_RIGHT,
                        DATAY2_LEFT, DATAY2_RIGHT,
                        DATAZ1_LEFT, DATAZ1_RIGHT,
                        DATAZ2_LEFT, DATAZ2_RIGHT
                        };
      int N_registers = sizeof(data_registers);
          

      for (int i=0; i< N_registers; i++){
        reg_addr = data_registers[i];
        read_value =  readRegister(reg_addr, 3);
        // highlight_led_byte(reg_addr );

        
        // ####### DEBUG PURPOSES ONLY ############
        if (i==0){
          int sine_value = (int) 1000*sin(2*PI*ni/20)+500;
          read_value = sine_value;
          ni++;
        }

        Serial.print(read_value);
        Serial.print(" ");
      }
      Serial.println("");
      
      
    } else if (command == "read_memory") {
      // Reads memory starting from reg_addr and for reg_value bytes
      highlight_led_byte(reg_addr);
      Serial.print(reg_addr);
      for (int i=0; i<reg_value; i++){
        read_value =  readRegister(reg_addr+i, 1);
        Serial.print(" ");
        Serial.print(read_value);
      }
      Serial.println("");
  

    } else if (command == "fifo_status") {
      // Reads FIFO count
      int fifo_count=0;
      int fifo_empty=0;
      int fifo_full=0;
      reg_addr = FIFO_STATUS;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      fifo_count = read_value & 0x70;
      fifo_count = fifo_count >> 4;
      fifo_empty = read_value & 0x01;
      fifo_full = read_value & 0x04;
      fifo_full = fifo_full>>2;
      Serial.print("fifo_count");
      Serial.print(" ");
      Serial.println(fifo_count);
      Serial.print("fifo_empty");
      Serial.print(" ");
      Serial.println(fifo_empty);
      Serial.print("fifo_full");
      Serial.print(" ");
      Serial.println(fifo_full);

    } else if (command == "fifo_count") {
      // Reads FIFO count
      int fifo_count=0;
      reg_addr = FIFO_STATUS;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      fifo_count = read_value & 0x70;
      fifo_count = fifo_count >> 4;
      Serial.print("fifo_count");
      Serial.print(" ");
      Serial.println(fifo_count);
      
    } else if (command == "fifo_empty") {
      // Reads FIFO empty
      int fifo_empty=0;

      reg_addr = FIFO_STATUS;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      fifo_empty = read_value & 0x01;
      Serial.print("fifo_empty");
      Serial.print(" ");
      Serial.println(fifo_empty);
      
    } else if (command == "fifo_full") {
      // Reads FIFO full
      int fifo_full=0;
      reg_addr = FIFO_STATUS;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      fifo_full = read_value & 0x04;
      fifo_full = fifo_full>>2;
      Serial.print("fifo_full");
      Serial.print(" ");
      Serial.println(fifo_full);
    
    } else if (command == "fifo_size") {
      // read fifo size
      int fifo_size=0;
      reg_addr = FIFO_SIZE;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      fifo_size = read_value & 0x7;
      
      Serial.print("fifo_size");
      Serial.print(" ");
      Serial.println(fifo_size);

    } else if (command == "set_fifo_size") {
      // set fifo size
      
      reg_value = (byte) byte(parameter1.toInt());
      reg_addr = FIFO_SIZE;
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x7);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("fifo_size");
      Serial.print(" ");
      Serial.println(read_value);


    } else if (command == "mode") {
      // read mode
      reg_addr = MODE;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      read_value = read_value & 0x7;
      
      Serial.print("mode");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "set_mode") {
      // set mode
      reg_value = (byte) byte(parameter1.toInt());
      reg_addr = MODE;
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x7);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("mode");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "wakeup") {
      // read wakeup
      reg_addr = WAKEUP;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      read_value = read_value & 0x1;
      
      Serial.print("wakeup");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "set_wakeup") {
      // set wakeup
      reg_value = (byte) byte(parameter1.toInt());
      reg_addr = WAKEUP;
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x1);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("wakeup");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "run") {
      // read run
      reg_addr = RUN;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      read_value = read_value & 0x1;
      
      Serial.print("run");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "set_run") {
      // set run
      reg_addr = RUN;
      reg_value = (byte) byte(parameter1.toInt());
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x1);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("run");
      Serial.print(" ");
      Serial.println(read_value);


    } else if (command == "C2V_CONFIG0") {
      // read
      reg_addr = C2V_CONFIG0;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      read_value = read_value & 0x1f;
      Serial.print("C2V_CONFIG0");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_C2V_CONFIG0") {
      // set
      reg_value = (byte) byte(parameter1.toInt()); 
      reg_addr = C2V_CONFIG0;
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x1f);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("C2V_CONFIG0");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "C2V_CONFIG1") {
      // read
      reg_addr = C2V_CONFIG1;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      read_value = read_value & 0x1f;
      Serial.print("C2V_CONFIG1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_C2V_CONFIG1") {
      // set
      reg_value = (byte) byte(parameter1.toInt()); 
      reg_addr = C2V_CONFIG1;
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x1f);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("C2V_CONFIG1");
      Serial.print(" ");
      Serial.println(read_value);

    } else if (command == "C2V_CONFIG2") {
      // read
      reg_addr = C2V_CONFIG2;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 1);
      read_value = read_value & 0x1f;
      Serial.print("C2V_CONFIG2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_C2V_CONFIG2") {
      // set
      reg_value = (byte) byte(parameter1.toInt()); 
      reg_addr = C2V_CONFIG2;
      highlight_led_byte(reg_addr);
      writeRegister(reg_addr, reg_value&0x1f);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(reg_addr);
      Serial.print("C2V_CONFIG2");
      Serial.print(" ");
      Serial.println(read_value);




    // For setting the DAC values, two write operations are needed for the MSB and LSB..
    // a re-conversion of the string parameter to integer  is also required
    } else if (command == "VCOMP_X1") {
      // Reads two consecutive bytes
      reg_addr = VCOMP_X1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VCOMP_X1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VCOMP_X1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VCOMP_X1_MSB, VCOMP_X1_LSB, dac_value);
      reg_addr = VCOMP_X1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VCOMP_X1");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VEXC_X1") {
      // Reads two consecutive bytes
      reg_addr = VEXC_X1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VEXC_X1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VEXC_X1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VEXC_X1_MSB, VEXC_X1_LSB, dac_value);
      reg_addr = VEXC_X1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VEXC_X1");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VACT_X1") {
      // Reads two consecutive bytes
      reg_addr = VACT_X1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VACT_X1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VACT_X1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VACT_X1_MSB, VACT_X1_LSB, dac_value);
      reg_addr = VACT_X1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VACT_X1");
      Serial.print(" ");  
      Serial.println(read_value);
    
  
    } else if (command == "VCOMP_X2") {
      // Reads two consecutive bytes
      reg_addr = VCOMP_X2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VCOMP_X2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VCOMP_X2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VCOMP_X2_MSB, VCOMP_X2_LSB, dac_value);
      reg_addr = VCOMP_X2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VCOMP_X2");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VEXC_X2") {
      // Reads two consecutive bytes
      reg_addr = VEXC_X2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VEXC_X2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VEXC_X2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VEXC_X2_MSB, VEXC_X2_LSB, dac_value);
      reg_addr = VEXC_X2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VEXC_X2");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VACT_X2") {
      // Reads two consecutive bytes
      reg_addr = VACT_X2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VACT_X2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VACT_X2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VACT_X2_MSB, VACT_X2_LSB, dac_value);
      reg_addr = VACT_X2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VACT_X2");
      Serial.print(" ");  
      Serial.println(read_value);
    
      } else if (command == "VCOMP_Y1") {
      // Reads two consecutive bytes
      reg_addr = VCOMP_Y1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VCOMP_Y1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VCOMP_Y1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VCOMP_Y1_MSB, VCOMP_Y1_LSB, dac_value);
      reg_addr = VCOMP_Y1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VCOMP_Y1");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VEXC_Y1") {
      // Reads two consecutive bytes
      reg_addr = VEXC_Y1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VEXC_Y1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VEXC_Y1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VEXC_Y1_MSB, VEXC_Y1_LSB, dac_value);
      reg_addr = VEXC_Y1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VEXC_Y1");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VACT_Y1") {
      // Reads two consecutive bytes
      reg_addr = VACT_Y1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VACT_Y1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VACT_Y1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VACT_Y1_MSB, VACT_Y1_LSB, dac_value);
      reg_addr = VACT_Y1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VACT_Y1");
      Serial.print(" ");  
      Serial.println(read_value);
    
  

    } else if (command == "VCOMP_Y2") {
      // Reads two consecutive bytes
      reg_addr = VCOMP_Y2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VCOMP_Y2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VCOMP_Y2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VCOMP_Y2_MSB, VCOMP_Y2_LSB, dac_value);
      reg_addr = VCOMP_Y2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VCOMP_Y2");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VEXC_Y2") {
      // Reads two consecutive bytes
      reg_addr = VEXC_Y2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VEXC_Y2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VEXC_Y2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VEXC_Y2_MSB, VEXC_Y2_LSB, dac_value);
      reg_addr = VEXC_Y2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VEXC_Y2");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VACT_Y2") {
      // Reads two consecutive bytes
      reg_addr = VACT_Y2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VACT_Y2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VACT_Y2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VACT_Y2_MSB, VACT_Y2_LSB, dac_value);
      reg_addr = VACT_Y2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VACT_Y2");
      Serial.print(" ");  
      Serial.println(read_value);
    


      } else if (command == "VCOMP_Z1") {
      // Reads two consecutive bytes
      reg_addr = VCOMP_Z1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VCOMP_Z1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VCOMP_Z1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VCOMP_Z1_MSB, VCOMP_Z1_LSB, dac_value);
      reg_addr = VCOMP_Z1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VCOMP_Z1");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VEXC_Z1") {
      // Reads two consecutive bytes
      reg_addr = VEXC_Z1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VEXC_Z1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VEXC_Z1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VEXC_Z1_MSB, VEXC_Z1_LSB, dac_value);
      reg_addr = VEXC_Z1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VEXC_Z1");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VACT_Z1") {
      // Reads two consecutive bytes
      reg_addr = VACT_Z1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VACT_Z1");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VACT_Z1") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VACT_Z1_MSB, VACT_Z1_LSB, dac_value);
      reg_addr = VACT_Z1_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VACT_Z1");
      Serial.print(" ");  
      Serial.println(read_value);
    
  

      } else if (command == "VCOMP_Z2") {
      // Reads two consecutive bytes
      reg_addr = VCOMP_Z2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VCOMP_Z2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VCOMP_Z2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VCOMP_Z2_MSB, VCOMP_Z2_LSB, dac_value);
      reg_addr = VCOMP_Z2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VCOMP_Z2");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VEXC_Z2") {
      // Reads two consecutive bytes
      reg_addr = VEXC_Z2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VEXC_Z2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VEXC_Z2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VEXC_Z2_MSB, VEXC_Z2_LSB, dac_value);
      reg_addr = VEXC_Z2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VEXC_Z2");
      Serial.print(" ");  
      Serial.println(read_value);
    
    } else if (command == "VACT_Z2") {
      // Reads two consecutive bytes
      reg_addr = VACT_Z2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      Serial.print("VACT_Z2");
      Serial.print(" ");
      Serial.println(read_value);
    } else if (command == "set_VACT_Z2") {
      // set
      int dac_value = 0;
      dac_value = parameter1.toInt();
      SetDAC_Value(VACT_Z2_MSB, VACT_Z2_LSB, dac_value);
      reg_addr = VACT_Z2_MSB;
      highlight_led_byte(reg_addr);
      read_value =  readRegister(reg_addr, 2);
      highlight_led_byte(reg_addr);
      Serial.print("VACT_Z2");
      Serial.print(" ");  
      Serial.println(read_value);
    
  



    } else {
      Serial.println("Something else");
    }
    


  }
}

int SetDAC_Value(byte ADDR_MSB, byte ADDR_LSB, int dac_value){
  // Sets the MSB and LSB byte registers for a short dac_value
  byte reg_addr;
  byte reg_value;
  int aux = 0;
  reg_addr = ADDR_MSB;
  aux = dac_value & 0xff00;
  aux = aux >> 8;
  reg_value = (byte) aux;
  highlight_led_byte(reg_addr);
  writeRegister(reg_addr, reg_value);
  reg_addr = ADDR_LSB;
  aux = dac_value & 0x00ff;
  reg_value = (byte) aux ;
  highlight_led_byte(reg_addr);
  writeRegister(reg_addr, reg_value);
  return 0;

}



String splitStringDelimiter(String str, int split_index, char delimiter){
  String aux = str.trim();
  String splitAux = aux;
  int idx_from = 0;
  int idx_to = 0;
  
  // Splits the string how many times in split index and returns the string between those delimiters
  for (int i=0; i<split_index+1; i++){
    aux = aux.trim();
    if (aux==""){
      splitAux = aux;
      break;
    }
    idx_from = 0;
    idx_to = aux.indexOf(delimiter, idx_from);
    splitAux = aux.substring(idx_from, idx_to);
    if (idx_to == -1){
      aux = String ("");
    } else {
      idx_from = idx_to+1;
      aux = aux.substring(idx_from);
    }
    // Serial.print("splitAux:");
    // Serial.println(splitAux);
  }
  // Serial.print("splitAux final:");
  // Serial.println(splitAux);
  return splitAux;
}

// String splitStr(String str, int strIndex)
// {
//   char delimiter = ' ' ;
//   if (strIndex == -1)
//     return "";
//   byte i = str.indexOf(delimiter,strIndex)+1;
//   strIndex = str.indexOf(delimiter,i);
//   if (strIndex == -1)
//     return "";
//   else
//     return str.substring(i,strIndex);
// }

void writeRegister(byte thisRegister, byte thisValue) {

  // FPGA expects the register address in the upper 7 bits
  // of the byte. So shift the bits left by one bits:
  thisRegister = thisRegister <<1;
  // now combine the register address and the command into one byte:
  byte dataToSend = thisRegister | WRITE;

  // gain control of the SPI port
  // and configure settings
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE));
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);

  SPI.transfer(dataToSend); //Send register location
  SPI.transfer(thisValue);  //Send value to record into register

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
  // release control of the SPI port
  SPI.endTransaction();
}




//Read byte from the FPGA:
unsigned int readRegister(byte thisRegister, int bytesToRead ) {
  byte inByte = 0;           // incoming byte from the SPI
  unsigned int result = 0;
  
  // Serial.print(thisRegister, BIN);
  // Serial.print("\t");
  // FPGA expects the register address in the upper 7 bits
  // of the byte. So shift the bits left by one bits:
  thisRegister = thisRegister << 1;
  // now combine the address and the command into one byte
  byte dataToSend = thisRegister & READ;
  // Serial.println(thisRegister, BIN);
  // gain control of the SPI port
  // and configure settings
  SPI.beginTransaction(SPISettings(SPI_CLK, MSBFIRST, SPI_MODE));
  // take the chip select low to select the device:
  digitalWrite(chipSelectPin, LOW);
  // send the device the register you want to read:
  SPI.transfer(dataToSend);
  // send a value of 0 to read the first byte returned:
  result = SPI.transfer(0x00);
  // decrement the number of bytes left to read:


  bytesToRead--;
  // if you still have another byte to read:
  
  while (bytesToRead > 0) {
    // shift the first byte left, then get the second byte:
    result = result << 8;
    inByte = SPI.transfer(0x00);
    // combine the byte you just got with the previous one:
    result = result | inByte;
    // decrement the number of bytes left to read:
   
    bytesToRead--;
  }
  

      // Serial.println("finished");

  // take the chip select high to de-select:
  digitalWrite(chipSelectPin, HIGH);
  // release control of the SPI port
  SPI.endTransaction();
  // return the result:
  return(result);
}


void highlight_led_byte(byte value){
  // Serial.println("Highlight");
  // Serial.println(value, HEX);
  for (int i=0; i<LED_array_pins_size; i++){
    int bitval = value>>i ;
    bitval = bitval & 0x1;
    int pin = LED_array_pins[i];
    if (bitval==1){
      digitalWrite(pin, HIGH);
    } else {
      digitalWrite(pin, LOW);
    }
  }
}





