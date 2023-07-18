
// Register addresses

#define DEVICE_ID_ADDR      0x00  //R
#define MODE_ADDR             0x05  //RW
#define RUN_ADDR                0x06  //RW
#define FIFO_SIZE_ADDR        0x07  //RW
#define FIFO_STATUS_ADDR    0x08  //R
#define INT_SOURCE_ADDR        0x10  //R
#define CONFIGX0_ADDR        0x20  //Rw
#define CONFIGX1_ADDR        0x21  //Rw
#define CONFIGY0_ADDR        0x22  //Rw
#define CONFIGY1_ADDR        0x23  //Rw
#define CONFIGZ0_ADDR        0x24  //Rw
#define CONFIGZ1_ADDR        0x25  //Rw
#define C2VCONFX0_ADDR        0x27  //Rw 2bytes 
#define C2VCONFX1_ADDR        0x29  //Rw
#define C2VCONFY0_ADDR        0x2B  //Rw
#define C2VCONFY1_ADDR        0x2D  //Rw
#define C2VCONFZ0_ADDR        0x2F  //Rw
#define C2VCONFZ1_ADDR        0x31  //Rw

//----------N = 0
#define DATAX0_LEFT_N0_ADDR        0x36  //R 3bytes
#define DATAX0_RIGHT_N0_ADDR        0x39  //R
#define DATAX1_LEFT_N0_ADDR        0x3C  //R
#define DATAX1_RIGHT_N0_ADDR        0x3F  //R
#define DATAY0_LEFT_N0_ADDR        0x42  //R
#define DATAY0_RIGHT_N0_ADDR        0x45  //R
#define DATAY1_LEFT_N0_ADDR        0x48  //R
#define DATAY1_RIGHT_N0_ADDR        0x4B  //R
#define DATAZ0_LEFT_N0_ADDR        0x4E  //R
#define DATAZ0_RIGHT_N0_ADDR        0x51  //R
#define DATAZ1_LEFT_N0_ADDR        0x54  //R
#define DATAZ1_RIGHT_N0_ADDR        0x57  //R
#define DACXHIGH_ADDR        0x60  //Rw
#define DACXLOW_ADDR            0x61  //Rw
#define DACXADDR_ADDR        0x62  //Rw
#define DACYHIGH_ADDR        0x63  //Rw
#define DACYLOW_ADDR            0x64  //Rw
#define DACYADDR_ADDR        0x65  //Rw
#define DACZHIGH_ADDR        0x66  //Rw
#define DACZLOW_ADDR            0x67  //Rw
#define DACZADDR_ADDR        0x68  //Rw
#define DACLOAD_ADDR            0x69  //Rw