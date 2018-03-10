#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "user_i2c.h"
#include "apds9301.h"

void test_interact_to_av02(void**state)
{
  skip();
}

void test_interact_to_av02_mutex(void** state)
{
  uint8_t read_val;
  /* Connect to device */
  int32_t av02;
  av02 = i2c_connect_mutex("/dev/i2c-2", AV02_ADDR);
  assert_int_not_equal(av02, -1);

  /* Power on the device */
  assert_int_equal(i2c_write_reg_mutex(av02, CONTROL_REG, POWER_ON), 0);
  assert_int_equal(i2c_read_reg_mutex(av02, CONTROL_REG, &read_val, 1), 0);
  assert_true(read_val == 0x03);
  /* Power off the device */
  assert_int_equal(i2c_write_reg_mutex(av02, CONTROL_REG, POWER_OFF), 0);
  assert_int_equal(i2c_read_reg_mutex(av02, CONTROL_REG, &read_val, 1), 0);
  assert_true(read_val == 0x00);
  i2c_disconnect_mutex(av02);
}

int main(int argc, char ** argv)
{
  const struct CMUnitTest tests[] =
  {
    cmocka_unit_test(test_interact_to_av02),
    cmocka_unit_test(test_interact_to_av02_mutex),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
