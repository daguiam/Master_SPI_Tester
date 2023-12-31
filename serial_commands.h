
// Define the orders that can be sent and received
enum Order {
  device_id =  'a',
  mode =  'b',
  run =  'c',
  fifo_size =  'd',
  fifo_status =  'e',
  int_source =  'f',
  config_x0 =  'g',
  config_x1 =  'h',
  config_y0 =  'i',
  config_y1 =  'j',
  config_z0 =  'k',
  config_z1 =  'l',
  c2vconfig_x0 =  'm',
  c2vconfig_x1 =  'n',
  c2vconfig_y0 =  'o',
  c2vconfig_y1 =  'p',
  c2vconfig_z0 =  'q',
  c2vconfig_z1 =  'r',
  read_data_x0_left =  'A',
  read_data_x0_right =  'B',
  read_data_x1_left =  'C',
  read_data_x1_right =  'D',
  read_data_y0_left =  'E',
  read_data_y0_right =  'F',
  read_data_y1_left =  'G',
  read_data_y1_right =  'H',
  read_data_z0_left =  'I',
  read_data_z0_right =  'J',
  read_data_z1_left =  'K',
  read_data_z1_right =  'L',
  dac_x_high =  'M',
  dac_x_low =  'N',
  dac_x_addr =  'O',
  dac_y_high =  'P',
  dac_y_low =  'Q',
  dac_y_addr =  'R',
  dac_z_high =  'S',
  dac_z_low =  'T',
  dac_z_addr =  'U',
  dac_load_addr =  'V',

  HELP = '?',

  EOL = '\n',
 
};

typedef enum Order Order;