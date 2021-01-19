
#include "main.h"

#define RS0 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_3, 0);
#define RS1 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_3, 1);

#define CLK0 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_0, 0);
#define CLK1 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_0, 1);

#define MOSI0 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, 0);
#define MOSI1 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_1, 1);

#define POWER0 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_2, 0);
#define POWER1 HAL_GPIO_WritePin (GPIOA, GPIO_PIN_2, 1);


#define SPI_Delay_time 1000 //us delay for SPI

extern I2C_HandleTypeDef hi2c1;
void SPI_Write(unsigned char value[], uint16_t size);
uint8_t SPI_Read(void);
uint8_t SPI_Read_Write (unsigned char value[], uint16_t size, uint8_t reading_byte_num);
void power_and_reset_sequence (void);
void page (int in);
void reset_sequence (void);
