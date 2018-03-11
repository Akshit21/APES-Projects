#include <stdio.h>
#include <stdint.h>

#include "user_i2c.h"
#include "apds9301.h"
#include "tmp1021.h"


int main(int argc, char const *argv[]) {
  uint8_t input;
  int32_t av02_handle, tmp_handle;
  int8_t read_buf[4];
  int16_t *av02_data0=(int16_t*)&read_buf[0];
  int16_t *av02_data1=(int16_t*)&read_buf[2];
  /*Connect the light sensor onto i2c2 */
  av02_handle = i2c_connect_mutex("/dev/i2c-2", APDS9301_ADDR);
  /* Power on the light sensor */
  i2c_write_reg_mutex(av02_handle, CONTROL_REG, POWER_ON);

  /* Connect the temperature sensor */
  tmp_handle = i2c_connect("/dev/i2c-1", TMP1021_ADDR);

  /* Configure the termperature sensor */
  /* By default nothing to be configured */

  scanf("%c", &input);
  while(input!='q')
  {
    if(i2c_read_reg_mutex(av02_handle, DATA0_REG, read_buf, 4)!=0)
    {
      perror("Read failed");
      break;
    }
    printf("data0: %x\n", *av02_data0);
    printf("data1: %x\n", *av02_data1);
    if(i2c_read_reg_mutex(tmp_handle, TEMP_REG, read_buf, 2)!=0)
    {
      perror("Read failed");
      break;
    }
    printf("raw_temp: %x\n", *av02_data0);
    printf("temperatur: %f\n", (*av02_data0 & 0xFFF0)*0.0625);

    scanf("%c", &input);
  }
  printf("Disconnecting sensors.\n");
  i2c_disconnect_mutex(av02_handle);
  i2c_disconnect(tmp_handle);
  return 0;
}
