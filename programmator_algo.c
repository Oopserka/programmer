#include "main.h"
#include "spi.h"
uint8_t shift = 0;



void power_and_reset_sequence (void) {
POWER0;
RS0;
CLK0;
POWER1;
HAL_Delay (100);
RS1;
HAL_Delay (100);
RS0;		
}

void reset_sequence (void) {
POWER1;
RS0;
HAL_Delay (100);
RS1;
HAL_Delay (100);
RS0;		
}

void page (int in) {
in = in*8;
shift = in >>8;
in = 0xFF & in; 	
char page[4] = {0x4C, shift, in, 0x00};				
SPI_Write (page,4);
}



