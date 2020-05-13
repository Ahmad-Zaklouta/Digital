#include <stdio.h>
#include "system.h"
#include "i2c_avalon_wrapper.h"
#include "alt_types.h"
#include "io.h"
#include "unistd.h" //usleep()


int check_wrapper_busy(void)
{
  //Read control register of I2C-wrapper
  int res = IORD(I2C_WRAPPER_BASE,I2C_CTRL_REG);
  //check if i2c_wrapper state machine or i2c master is busy
   if (check_bit(res,6))
    {
      return 1;
    }
  else
    {
      return 0;
    }
}


void write_to_i2c_device(alt_u8 i2c_device_addr, alt_u8 i2c_reg_addr,alt_u8 no_bytes,alt_u32 data)
{
 //write to address register of i2c wrapper
  IOWR(I2C_WRAPPER_BASE,I2C_ADDR_REG, i2c_reg_addr << 8 | i2c_device_addr);
  //write to the data register of the i2c wrapper
  IOWR(I2C_WRAPPER_BASE,I2C_DATA_REG,data);
  //Set the corresponding bits of the ctrl register to enable the i2c communication
  IOWR(I2C_WRAPPER_BASE,I2C_CTRL_REG,no_bytes << 2 | I2C_CMD_ENA_WR);
  //Check for busy in the ctrl register (i2c busy or wrapper busy), and continue when no longer busy
  while(check_wrapper_busy()) continue;
}

alt_u32 read_from_i2c_device(alt_u8 i2c_device_addr,alt_u8 i2c_reg_addr,alt_u8 no_bytes)
{
  alt_u32 res = 0;
  //write to address register of i2c wrapper
  //6..0: 7-bits i2c device addres
  //15..8: 8 bits register address of i2c device
  IOWR(I2C_WRAPPER_BASE,I2C_ADDR_REG, i2c_reg_addr << 8 | i2c_device_addr);
  //Write to i2c wrapper ctrl register to enable i2c command
  IOWR(I2C_WRAPPER_BASE,I2C_CTRL_REG, I2C_CMD_ENA_WR);
  //Check for busy in the ctrl register (i2c busy or wrapper busy), and continue when no longer busy
  while(check_wrapper_busy()) continue;
  //dump_i2c_register();
  //  usleep(1000);
  //Read data to i2c device

  //two bytes to be read from the i2c device
  if (no_bytes == 2 )
    {
      IOWR(I2C_WRAPPER_BASE,I2C_CTRL_REG,I2C_TWO_BYTE << 2 | I2C_CMD_ENA_RD);
      while(check_wrapper_busy()) continue;
      res = IORD(I2C_WRAPPER_BASE,I2C_DATA_REG);
      //only the 16 MSB of the i2c wrapper data reg contains read data.
      return res >> 16;
    }
  //one byte to be read from the i2c device
  else if (no_bytes == 1)
    {
      IOWR(I2C_WRAPPER_BASE,I2C_CTRL_REG,I2C_ONE_BYTE << 2 | I2C_CMD_ENA_RD);
      while(check_wrapper_busy()) continue;
      res = IORD(I2C_WRAPPER_BASE,I2C_DATA_REG);
      //only the 16 MSB of the i2c wrapper data reg contains read data.
      //mask of the 8 MSB which might contain data from previous read command
      return (res >> 16) & 0xff;
    }
  else
    {
      //Only 1 or two bytes can be read from the data register of the
      //printf("Requested read of in correct number of bytes");
      return -1;
    }
}