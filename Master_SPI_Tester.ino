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
  int read_value = 0xfa;
  // unsigned int read_value = 0xfa;


  
  if(Serial.available() > 0){


    // String command_string = Serial.readString();  //read until timeout
    String command_string = Serial.readStringUntil('\n');  //read until terminator
    // Serial.print("Command received: ");
    // Serial.println(command_string);
    String command = splitStringDelimiter(command_string, 0, ' ');
    String parameter1 = splitStringDelimiter(command_string, 1, ' ');
    String parameter2 = splitStringDelimiter(command_string, 2, ' ');

    byte reg_addr = (byte) byte(parameter1.toInt());
    byte reg_value = (byte) byte(parameter2.toInt());

    if (0){
      Serial.print(command);
      Serial.print(reg_addr, HEX);
      Serial.println(reg_addr, HEX);
    }
    // Serial.println(command);
    // Serial.println(parameter1);
    // Serial.println(parameter2);

    if (command == "device_id") {
      read_value =  readRegister(DEVICE_ID_ADDR, 1);
      highlight_led_byte(read_value);

    } else if (command == "write_reg") {
      writeRegister(reg_addr, reg_value);
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(read_value);
      Serial.print(reg_addr);
      Serial.print(" ");
      Serial.println(read_value);
      
    } else if (command == "read_reg") {
      read_value =  readRegister(reg_addr, 1);
      highlight_led_byte(read_value);
      Serial.print(reg_addr);
      Serial.print(" ");
      Serial.println(read_value);
      
    } else if (command == "read_data") {

      // Reads complete pagination of data (12 registers)
      byte data_registers[] = {DATAX0_LEFT_N0_ADDR, DATAX0_RIGHT_N0_ADDR,
                        DATAX1_LEFT_N0_ADDR, DATAX1_RIGHT_N0_ADDR,
                        DATAY0_LEFT_N0_ADDR, DATAY0_RIGHT_N0_ADDR,
                        DATAY1_LEFT_N0_ADDR, DATAY1_RIGHT_N0_ADDR,()
                        DATAZ0_LEFT_N0_ADDR, DATAZ0_RIGHT_N0_ADDR,
                        DATAZ1_LEFT_N0_ADDR, DATAZ1_RIGHT_N0_ADDR
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
      highlight_led_byte(read_value);
      Serial.print(reg_addr);
      for (int i=0; i<reg_value; i++){
        read_value =  readRegister(reg_addr+i, 1);
        Serial.print(" ");
        Serial.print(read_value);
      }
      Serial.println("");
  

    } else {
      Serial.println("Something else");
    }
    
    // // The first byte received is the instruction
    // Order order_received = read_order();
    //  //      Serial.print("Order: ");Serial.println(order_received);e

    // switch(order_received){
    //   case device_id:
    //     read_value =  readRegister(DEVICE_ID_ADDR, 1);
    //     Serial.print("value: \t");
    //     Serial.println(read_value, HEX);
    //     highlight_led_byte(read_value);
    //     break;
    //   case mode:
    //     read_value =  readRegister(DEVICE_ID_ADDR, 1);
    //     Serial.print("value: \t");
    //     Serial.println(read_value, HEX);
    //     highlight_led_byte(read_value);
    //     break;





    //   case EOL:
    //     //ignore
    //     break;
    //   default:
    //     Serial.println("Unrecognized command");
          
    //     break;
  

    // }



  }
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
  byte result = 0;
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
  if (bytesToRead > 0) {
    // shift the first byte left, then get the second byte:
    result = result << 8;
    inByte = SPI.transfer(0x00);
    // combine the byte you just got with the previous one:
    result = result | inByte;
    // decrement the number of bytes left to read:
    bytesToRead--;
  }
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





