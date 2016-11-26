#include "c_types.h"
#include "esp8266_auxrom.h"
#include "esp8266_rom.h"
#include "eagle_soc.h"
#include "ets_sys.h"
#include "gpio.h"
#include "nosdk8266.h"

//This is a little more bloated than PICO66.  It has useful stuff like PRINTF and can call anything in
//romlib.c 

#define call_delay_us( time ) { asm volatile( "mov.n a2, %0\n_call0 delay4clk" : : "r"(time*13) : "a2" ); }

extern volatile int isrs;

int main()
{
	int i = 0;
	romlib_init();

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO2_U,FUNC_GPIO2);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_GPIO5_U,FUNC_GPIO5);
	PIN_DIR_OUTPUT = _BV(2); //Enable GPIO2 light off.
	PIN_DIR_OUTPUT = _BV(5); //Enable GPIO2 light off.

	printf( "Trying a Flash test write.\n" );

	SPIEraseBlock(0x40000>>16);
	uint32_t stuff = 0xAABBCCDD;
	SPIWrite(0x40004, &stuff, 4);
	stuff = 0;
	SPIRead(0x40004, &stuff, 4);

	printf( "Checking to see if we can read from cache: %p / %p (both should be 0xaabbccdd)\n", *(void **)(0x40200000 + 0x40000 + 4), stuff );

	printf( "Starting I2S Engine (%p)\n",*(void**)0x400040bc);


	InitI2S();

	SendI2S();
	while(1)
	{
		printf( "Hello World %d / %d\n", i, isrs );
		PIN_OUT_SET = _BV(2); //Turn GPIO2 light off.
		call_delay_us( 500000 );
		PIN_OUT_CLEAR = _BV(2); //Turn GPIO2 light off.
		call_delay_us( 500000 );
		i++;
	}
}

