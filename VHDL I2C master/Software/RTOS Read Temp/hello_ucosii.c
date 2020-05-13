#include <stdio.h>
#include "includes.h"
#include <string.h>
#include "system.h"
#include "altera_avalon_pio_regs.h"
#include "HAL/inc/alt_types.h"
#include "HAL/inc/io.h"
#include "HAL/inc/sys/alt_irq.h"
#include "i2c_avalon_wrapper.h"
#include "unistd.h"

/* Definition of Task Stacks */
#define   TASK_STACKSIZE       2048
OS_STK task1_stk[TASK_STACKSIZE];
OS_STK task2_stk[TASK_STACKSIZE];
OS_STK task_temp_stk[TASK_STACKSIZE];
OS_STK task_alert_stk[TASK_STACKSIZE];
OS_STK task_interrupt_stk[TASK_STACKSIZE];

/* Definition of Task Priorities */
#define TASK_INTERRUPT_PRIORITY 1
#define TASK1_PRIORITY      	2
#define TASK2_PRIORITY      	3
#define TASK_TEMP_PRIORITY 		4

/* Definition of semaphores & mailbox */
static OS_EVENT *shared_jtag_sem;
static OS_EVENT *key1_sem;
static OS_EVENT *delay_mbox;

/* Definitions for calculation*/
#define RESOLU 0.0625

/* function heads  */
static void initTMP175();
static void init_button_pio();
static void handle_button_interrupts(void* context);
static float readTemp(void);
static int readSw(void);
/* ISR argument */
static volatile int edge_capture;

void task1(void* pdata) {
	char txt[] = "Hello from task1\n";
	INT8U error_code = OS_NO_ERR;
	int t_start, t_end, t_prew = 0;
	int i;
	while (1) {

		t_start = OSTimeGet();
		OSSemPend(shared_jtag_sem, 0, &error_code);
		// ask/ wait for semaphore
		for (i = 0; i < strlen(txt); i++) {
			putchar(txt[i]);
		}
		t_end = OSTimeGet();
		printf("T1: (Start,End,Ex.T.,P): ( %d , %d , %d , %d )\n", t_start,
				t_end, t_end - t_start, t_start - t_prew);
		OSSemPost(shared_jtag_sem); // release semaphore
		t_prew = t_start;
		OSTimeDlyHMSM(0, 0, 1, 20);
		// Context switch to next task
		// Task will go to the ready state
		// After the specified delay
	}
}
/* Prints "Hello World" and sleeps for three seconds */
void task2(void* pdata) {
	char txt[] = "Hello from task2\n";
	INT8U error_code = OS_NO_ERR;
	int t_start, t_end, t_prew = 0;
	int i;
	while (1) {

		t_start = OSTimeGet();
		OSSemPend(shared_jtag_sem, 0, &error_code);
		// ask/ wait for semaphore
		for (i = 0; i < strlen(txt); i++) {
			putchar(txt[i]);
		}
		t_end = OSTimeGet();
		printf("T2: (Start,End,Ex.T.,P): ( %d , %d , %d , %d )\n", t_start,
				t_end, t_end - t_start, t_start - t_prew);
		OSSemPost(shared_jtag_sem); // release semaphore
		t_prew = t_start;
		OSTimeDlyHMSM(0, 0, 1, 4);
		// Context switch to next task
		// Task will go to the ready state
		// After the specified delay
	}
}

void task_temperature(void* pdata) {
	initTMP175();
	INT8U error_code = OS_NO_ERR;
	float tmp;
	int t_start, t_end;
	int *mbox_msg;
	int delay = 5 * 1000;

	while (1) {
		t_start = OSTimeGet();
		mbox_msg = (int *) OSMboxPend(delay_mbox, delay, &error_code);
		if (mbox_msg) {
			delay = *mbox_msg;
		} else {
			tmp = readTemp();
			tmp = tmp * RESOLU;
			printf("Temperature: %f C \n", tmp);

		}
		t_end = OSTimeGet();
		printf("Delta time = %d ms\n", t_end - t_start);
	}
}

void task_interrupt(void* pdata) {
	init_button_pio();
	INT8U error_code = OS_NO_ERR;
	int dt;
	while (1) {
		OSSemPend(key1_sem, 0, &error_code);
		dt = readSw() * 100;
		OSMboxPostOpt(delay_mbox, &dt,
		OS_POST_OPT_BROADCAST);
		printf("New delaytime : %d ms\n", dt);
		OSTimeDlyHMSM(0, 0, 1, 4);
	}
}

/* The main function creates two task and starts multi-tasking */
int main(void) {

	shared_jtag_sem = OSSemCreate(1);
	key1_sem = OSSemCreate(0);
	delay_mbox = OSMboxCreate((void*)NULL);

	 OSTaskCreateExt(task1,
	 NULL, (void *) &task1_stk[TASK_STACKSIZE - 1],
	 TASK1_PRIORITY,
	 TASK1_PRIORITY, task1_stk,
	 TASK_STACKSIZE,
	 NULL, 0);

	 OSTaskCreateExt(task2,
	 NULL, (void *) &task2_stk[TASK_STACKSIZE - 1],
	 TASK2_PRIORITY,
	 TASK2_PRIORITY, task2_stk,
	 TASK_STACKSIZE,
	 NULL, 0);

	OSTaskCreateExt(task_temperature,
	NULL, (void *) &task_temp_stk[TASK_STACKSIZE - 1],
	TASK_TEMP_PRIORITY,
	TASK_TEMP_PRIORITY, task_temp_stk,
	TASK_STACKSIZE,
	NULL, 0);

	OSTaskCreateExt(task_interrupt,
	NULL, (void *) &task_interrupt_stk[TASK_STACKSIZE - 1],
	TASK_INTERRUPT_PRIORITY,
	TASK_INTERRUPT_PRIORITY, task_interrupt_stk,
	TASK_STACKSIZE,
	NULL, 0);

	OSStart();
	return 0;
}

static void initTMP175() {
	// sets config register to 12 bit resolution:
	write_to_i2c_device(TMP175_ADDR, TMP175_CONFIG_REG, 0x1, 0x60);
	// Resets temperature register
	write_to_i2c_device(TMP175_ADDR, TMP175_TEMP_REG, 0x0, 0x0);

	write_to_i2c_device(TMP175_ADDR, TMP175_THIGH_REG, 0x2, 0x1E0 << 4);
}

static float readTemp(void) {
	float tmp = (read_from_i2c_device(TMP175_ADDR, TMP175_TEMP_REG,
	I2C_TWO_BYTE) >> 4);
	return tmp;
}

static void handle_button_interrupts(void* context) {
	// cast context pointer into volatile pointer
	// to avoid compiler optimalization
	volatile int* edge_capture_ptr = (volatile int*) context;

	// puts the interrupt edge capture into edge_capture memory
	*edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(INTERRUPT_PIO_BASE);

	// resets edge capture register to 0
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(INTERRUPT_PIO_BASE, 0);
	// checks on updated edge capture
	switch (edge_capture) {
	case 0x1:
		OSSemPost(key1_sem);
		break;
	case 0x2:
		break;

	case 0x4:

		break;

	case 0x8:

		break;
	}
}

static void init_button_pio() {

	void* edge_capture_ptr = (void*) &edge_capture;
	// enables the 4 button interrupts
	IOWR_ALTERA_AVALON_PIO_IRQ_MASK(INTERRUPT_PIO_BASE, 0x0f);

	// resets edge capture register to 0
	IOWR_ALTERA_AVALON_PIO_EDGE_CAP(INTERRUPT_PIO_BASE, 0);

	// register the interrupt handler
	alt_ic_isr_register(INTERRUPT_PIO_IRQ_INTERRUPT_CONTROLLER_ID,
	INTERRUPT_PIO_IRQ, handle_button_interrupts, edge_capture_ptr, 0x0);

}

static int readSw(void) {
	/* Read 32 bits, mask 10 bits (10 switches) */
	int sw = IORD(SW_PIO_BASE,0) & 0x3FF; // bit value of sample time
	return sw;
}

// we can see that the task1 interrupts task2, due to higher priority which will corrupt the output
