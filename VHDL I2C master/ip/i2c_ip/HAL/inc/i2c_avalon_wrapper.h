#include "alt_types.h"

//I2C wrapper
#define I2C_CTRL_REG  0x0
#define I2C_ADDR_REG  0x1
#define I2C_DATA_REG  0x2

#define I2C_CMD_ENA_WR  0x1
#define I2C_CMD_ENA_RD  0x3

#define I2C_ONE_BYTE  0x1
#define I2C_TWO_BYTE  0x2

//TMP175 temparature device
#define TMP175_ADDR 0x48

#define TMP175_TEMP_REG  0x0
#define TMP175_CONFIG_REG  0x1
#define TMP175_TLOW_REG  0x2
#define TMP175_THIGH_REG  0x3

#define check_bit(var,pos) ((var) & (1<<(pos)))

int check_wrapper_busy(void);

//Functions for reading and writing to I2C wrapper

alt_u32 read_from_i2c_device(alt_u8 i2c_device_addr,alt_u8 i2c_reg_addr,alt_u8 no_bytes);

void write_to_i2c_device(alt_u8 i2c_device_addr, alt_u8 i2c_reg_addr,alt_u8 no_bytes,
                         alt_u32 data);