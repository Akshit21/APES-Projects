#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include "user_i2c.h"
#include "apds9301.h"

void test_apds9301(void** state)
{
  int8_t read_byte;
  int16_t read_word;
  int32_t apds_handle;
  /* Test connecting to device */
  assert_int_equal(i2c_connect_mutex(&apds_handle, APDS9301_ADDR), 0);

  /* Test power-on */
  assert_int_equal(i2c_write_byte_mutex(apds_handle, CMD | CONTROL_REG,
                                        (uint8_t)POWER_ON), 0);
  assert_int_equal(i2c_read_byte_mutex(apds_handle, CMD | CONTROL_REG,
                                       &read_byte), 0);
  assert_true(read_byte == (uint8_t)POWER_ON);

  /* Test setting up thresholds and interrupt */
  assert_int_equal(i2c_write_word_mutex(apds_handle, CMD|WORD|THRESHLOW_REG,
                                        (uint16_t)DEFAULT_THRESHLOW_VALUE), 0);
  assert_int_equal(i2c_read_word_mutex(apds_handle, CMD|WORD|THRESHLOW_REG,
                                       &read_word), 0);
  assert_true(read_word == (int16_t)DEFAULT_THRESHLOW_VALUE);
  assert_int_equal(i2c_write_word_mutex(apds_handle, CMD|WORD|THRESHHIGH_REG,
                                        (uint16_t)DEFAULT_THRESHHIGH_VALUE), 0);
  assert_int_equal(i2c_read_word_mutex(apds_handle, CMD|WORD|THRESHHIGH_REG,
                                       &read_word), 0);
  assert_true(read_word == (int16_t)DEFAULT_THRESHHIGH_VALUE);
  assert_int_equal(i2c_write_byte_mutex(apds_handle, CMD | INTERRUPT_REG,
                                        (uint8_t)DEFAULT_INTERRUPT_SETTING), 0);
  assert_int_equal(i2c_read_byte_mutex(apds_handle, CMD | INTERRUPT_REG,
                                       &read_byte), 0);
  assert_true(read_byte == (uint8_t)DEFAULT_INTERRUPT_SETTING);

  /* Power off the device */
  assert_int_equal(i2c_write_byte_mutex(apds_handle, CMD|CONTROL_REG,
                                        POWER_OFF), 0);
  assert_int_equal(i2c_read_reg_mutex(apds_handle, CMD|CONTROL_REG,
                                      &read_byte), 0);
  assert_true(read_byte == 0x00);
  i2c_disconnect_mutex(apds_handle);
}

int main(int argc, char ** argv)
{
  const struct CMUnitTest tests[] =
  {
    cmocka_unit_test(test_apds9301),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
