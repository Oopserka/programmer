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



