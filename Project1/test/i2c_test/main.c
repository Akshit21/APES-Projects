#include <stdio.h>
#include <stdint.h>

#include "user_i2c.h"
#include "apds9301.h"


int main(int argc, char const *argv[]) {
  uint8_t input;
  int32_t av02_handle;
  uint8_t read_buf[4];
  uint16_t *av02_data0=(uint16_t*)&read_buf[0];
  uint16_t *av02_data1=(uint16_t*)&read_buf[2];
  av02_handle = i2c_connect_mutex("/dev/i2c-2", APDS9301_ADDR);
  /* Power on the av02 sensor */
  i2c_write_reg_mutex(av02_handle, CONTROL_REG, POWER_ON);

  scanf("%c", &input);
  while(input!='q')
  {
    if(i2c_read_reg_mutex(av02_handle, DATA0_REG, read_buf, 4)!=0)
    {
      perror("Read failed");
      break;
    }
    printf("raw: %x %x %x %x\n", read_buf[0], read_buf[1], read_buf[2], read_buf[3]);
    printf("data0: %x\n", *av02_data0);
    printf("data1: %x\n", *av02_data1);
    scanf("%c", &input);
  }
  printf("Disconnecting AV02.\n");
  i2c_disconnect_mutex(av02_handle);
  return 0;
}
