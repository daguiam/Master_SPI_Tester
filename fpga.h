#define DEVID	0x00
#define WAKEUP	0x04
#define MODE	0x05
#define RUN	0x06
#define FIFO_SIZE	0x07
#define FIFO_STATUS	0x08
#define CONFIGX1	0x0A
#define CONFIGX2	0x0B
#define CONFIGY1	0x0C
#define CONFIGY2	0x0D
#define CONFIGZ1	0x0E
#define CONFIGZ2	0x0F
#define C2V_CONFIG0	0x10
#define C2V_CONFIG1	0x11
#define C2V_CONFIG2	0x12
#define DATAX1_LEFT	0x13
#define DATAX1_RIGHT	0x16
#define DATAX2_LEFT	0x19
#define DATAX2_RIGHT	0x1C
#define DATAY1_LEFT	0x1F
#define DATAY1_RIGHT	0x22
#define DATAY2_LEFT	0x25
#define DATAY2_RIGHT	0x28
#define DATAZ1_LEFT	0x2B
#define DATAZ1_RIGHT	0x2E
#define DATAZ2_LEFT	0x31
#define DATAZ2_RIGHT	0x34
#define VCOMP_X1_MSB	0x37
#define VCOMP_X1_LSB	0x38
#define VEXC_X1_MSB	0x39
#define VEXC_X1_LSB	0x3A
#define VACT_X1_MSB	0x3B
#define VACT_X1_LSB	0x3C
#define VCOMP_X2_MSB	0x3D
#define VCOMP_X2_LSB	0x3E
#define VEXC_X2_MSB	0x3F
#define VEXC_X2_LSB	0x40
#define VACT_X2_MSB	0x41
#define VACT_X2_LSB	0x42
#define VCOMP_Y1_MSB	0x43
#define VCOMP_Y1_LSB	0x44
#define VEXC_Y1_MSB	0x45
#define VEXC_Y1_LSB	0x46
#define VACT_Y1_MSB	0x47
#define VACT_Y1_LSB	0x48
#define VCOMP_Y2_MSB	0x49
#define VCOMP_Y2_LSB	0x4A
#define VEXC_Y2_MSB	0x4B
#define VEXC_Y2_LSB	0x4C
#define VACT_Y2_MSB	0x4E
#define VACT_Y2_LSB	0x4F
#define VCOMP_Z1_MSB	0x50
#define VCOMP_Z1_LSB	0x51
#define VEXC_Z1_MSB	0x52
#define VEXC_Z1_LSB	0x53
#define VACT_Z1_MSB	0x54
#define VACT_Z1_LSB	0x55
#define VCOMP_Z2_MSB	0x56
#define VCOMP_Z2_LSB	0x57
#define VEXC_Z2_MSB	0x58
#define VEXC_Z2_LSB	0x59
#define VACT_Z2_MSB	0x5A
#define VACT_Z2_LSB	0x5B




/*
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
#define DACLOAD_ADDR            0x69  //Rw*/