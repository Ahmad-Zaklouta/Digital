
#include <stdio.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "HAL/inc/alt_types.h"
#include "HAL/inc/io.h"
#include "HAL/inc/sys/alt_irq.h"
#include "i2c_avalon_wrapper.h"
#include "unistd.h"

#define RESOLU 0.0625
#define SCALE 10000
#define RESOLU_SCALED 625

volatile int edge_capture;
static void handle_button_interrupts(void* context);
static void init_button_pio();
static void initTMP175();
alt_u32 read_ctrl = 0;
volatile int data, tmp,tmp_int,tmp_dec, old_data;

int main(){

	printf("Hello from Nios II!\n");
	initTMP175();
	init_button_pio();

	while(1){

		if (read_ctrl != 0){
			// Read data from MSB of data register
			data = (read_from_i2c_device(TMP175_ADDR,TMP175_TEMP_REG,I2C_TWO_BYTE)) >> 4;
			// Write data to LEDs
			IOWR(LED_PIO_BASE, 0,data);
			// Floating point consideration
			tmp = data * RESOLU_SCALED;
			tmp_int = tmp / SCALE;			// integer value
			tmp_dec = tmp - tmp_int*SCALE;	// decimal value
			if(data != old_data){
				old_data = data;
				printf("Temperature: %3lu\.%04lu C\n",tmp_int,tmp_dec);
			}
		}
		IOWR(LED_PIO_BASE, 0, IORD(SW_PIO_BASE, 0));
		usleep(1000);
	}
	return 0;
}
static void handle_button_interrupts(void* context){
	// cast context pointer into volatile pointer
	// to avoid compiler optimalization
	volatile int* edge_capture_ptr = (volatile int*)context;

	// puts the interrupt edge capture into edge_capture memory
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(INTERRUPT_PIO_BASE);

	// resets edge capture register to 0
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(INTERRUPT_PIO_BASE,0);
	// checks on updated edge capture
	int thigh;
	switch(edge_capture){
		case 0x1:
			printf("key1 pressed\n");
			read_ctrl = 1; // enable temp reading
			break;
		case 0x2:
			printf("key2 pressed\n");
			read_ctrl = 0; // disable temp reading
			break;
		case 0x4:
			printf("key3 pressed\n");
			thigh = read_from_i2c_device(TMP175_ADDR, TMP175_THIGH_REG, I2C_TWO_BYTE);
			if(thigh == (25<<8)){
				// set a large temperature value in thigh register that the alrt signal will not be sent
				write_to_i2c_device(TMP175_ADDR, TMP175_THIGH_REG,I2C_TWO_BYTE,(80<<8));
			}else{
				// initiate alert signal when temperature exceeds 25 degrees
				write_to_i2c_device(TMP175_ADDR, TMP175_THIGH_REG, 2,(25<<8));
			}
			break;
		case 0x8:
			printf("i2c alert\n");
			break;
		default:
			printf("illegal interrupt value\n");
	}
}

static void init_button_pio(){

	void* edge_capture_ptr = (void*) &edge_capture;
	// enables the 4 button interrupts
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(INTERRUPT_PIO_BASE,0x0f);

	// resets edge capture register to 0
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(INTERRUPT_PIO_BASE,0);

	// register the interrupt handler
	alt_ic_isr_register(INTERRUPT_PIO_IRQ_INTERRUPT_CONTROLLER_ID, INTERRUPT_PIO_IRQ, handle_button_interrupts, edge_capture_ptr, 0x0);

}

static void initTMP175(){
	// sets config register to 12 bit resolution:
	write_to_i2c_device(TMP175_ADDR, TMP175_CONFIG_REG, 0x1, 0x60);
	// Resets temperature register
	write_to_i2c_device(TMP175_ADDR, TMP175_TEMP_REG, 0x0, 0x0);

	write_to_i2c_device(TMP175_ADDR, TMP175_THIGH_REG, 0x2, 0x1E0 << 4);
}
