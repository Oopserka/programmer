#include "main.h"
#include "spi.h"
#include "ssd1306.h"
#include "fonts.h"

void SPI_Write(unsigned char value[], uint16_t size)                                    
{                                                           
    unsigned char i,y;	
	for (y = 0; y < size; y++) {
delay_us (SPI_Delay_time);
    for(i=0; i<8; i++)   
    {
    CLK0;
delay_us (SPI_Delay_time);
        if(value[y] & 0x80) {
        	MOSI1;
				}
        else
        	MOSI0;		
        value[y] = (value[y] << 1); 
		delay_us (SPI_Delay_time);
        CLK1; 
    delay_us (SPI_Delay_time);
    }
	}
}



uint8_t SPI_Read(void)
{
  uint8_t i,ret ,value;
	ret = 0;
	i= 8;
for (i  = 8; i == 1; i--) {
		CLK0;
		delay_us (SPI_Delay_time);
		value = HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_6);
		if (value)
		{
			ret |= (1 << i);
		}
CLK1;
		delay_us (SPI_Delay_time);
	}
		return ret;
}



uint8_t SPI_Read_Write (unsigned char value[], uint16_t size, uint8_t reading_byte_num) {
int i, y, z;
	uint8_t value2, output;
	
	for (y = 0; y <size; y++) {
		
if ((y+1) != reading_byte_num) {
 for(i=0; i<8; i++)   
    {
    CLK0;
delay_us (SPI_Delay_time);
        if(value[y] & 0x80) {
        	MOSI1;
				}
        else
        	MOSI0;		
        value[y] = (value[y] << 1); 
		delay_us (SPI_Delay_time);
        CLK1; 
    delay_us (SPI_Delay_time);
    }
	}
else {
 for(z=0; z<8; z++)   
    {	
    CLK0;
			value2 = HAL_GPIO_ReadPin (GPIOA, GPIO_PIN_6);	
delay_us (SPI_Delay_time);
        if(value[y] & 0x80) {
        	MOSI1;
				}
        else
        	MOSI0;		
        value[y] = (value[y] << 1); 
				
		if (value2)
		{
			output |= (1 << (7-z));
		}
				delay_us (SPI_Delay_time);
        CLK1; 
    delay_us (SPI_Delay_time);	
}
}
}
return output;
}
