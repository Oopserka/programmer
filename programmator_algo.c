#include "main.h"
#include "spi.h"

void power_and_reset_sequence (void) {
POWER0;
RS0;
CLK0;
POWER1;
HAL_Delay (10);
RS1;
HAL_Delay (1);
RS0;		
}

void reset_sequence (void) {

RS0;
HAL_Delay (10);
RS1;
HAL_Delay (1);
RS0;		
}

void page (int in) {
	in = in*8;
char page[4] = {0x4C, 0x00, in, 0x00};				
SPI_Write (page,4);
}



