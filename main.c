#include "main.h"
#include "spi.h"
#include "data.h"
#include "main.h"
#include <string.h>
#include "ssd1306.h"
#include "fonts.h"


I2C_HandleTypeDef hi2c1;
const uint8_t APBPrescTable[8]  = {0, 0, 0, 0, 1, 2, 3, 4};
uint8_t program_enable_byte;
unsigned int i;
uint8_t reload_counter;
uint8_t signature[3];	
unsigned int string_structure[];
int convert (char x, char y);
int o;



int main(void)
{

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
	MX_I2C1_Init();
  ssd1306_Init (&hi2c1);

	
power_and_reset_sequence ();
program_enable_byte = SPI_Read_Write (program_enable, 4, 3);

signature[0] = SPI_Read_Write (read_signature1, 4, 4);
signature[1] = SPI_Read_Write (read_signature2, 4, 4);	
signature[2] = SPI_Read_Write (read_signature3, 4, 4);

	
	SPI_Write (chip_erase,4);
	
if (program_enable_byte == 0x53) {
//Stage1_OK ();
}
else 
{
	power_and_reset_sequence ();
//	Stage1_NOK ();
	reload_counter = reload_counter +1;
}
if (reload_counter >= 3) NVIC_SystemReset();

if ((signature[0] == 0x1E) && (signature[1] == 0x93) && (signature[1] == 0x1E))
{
	// LCD_Print ("Atmega 88PA");
}
//else if 

for (int u = 0; u <= sizeof (main_program); u++) {
string_structure[0] = convert (main_program[u][0], main_program[u][1]); // ????????? ????????? ???? ? ??????
string_structure[1] = convert (main_program[u][2], main_program[u][3]);
string_structure[2] = convert (main_program[u][4], main_program[u][5]); // 2 ????? ?????????? ?????? ??????
string_structure[3] = convert (main_program[u][6], main_program[u][7]); // 1 ???? ?????? ? ??????? ??????
	
SPI_Write ((uint8_t*) 0x4D, 1);
SPI_Write ((uint8_t*) 0x00, 1);
SPI_Write ((uint8_t*)string_structure[2], 1);
SPI_Write ((uint8_t*) 0x00, 1);
int addr = 0;	
	
for (o = 4; o <= (string_structure[0] + 4); o++) {
string_structure[o] = convert (main_program[u][o+3], main_program[u][o+4]); // ??????? ?? ????????? ????? ??????, ??????? ?????????? ?????????
SPI_Write ((uint8_t*) 0x40, 1);
SPI_Write ((uint8_t*) 0x00, 1);
SPI_Write ((uint8_t*) addr , 1);	
SPI_Write ((uint8_t*) string_structure[o] , 1);	 	

SPI_Write ((uint8_t*) 0x48, 1);
SPI_Write ((uint8_t*) 0x00, 1);
SPI_Write ((uint8_t*) addr , 1);	
SPI_Write ((uint8_t*) string_structure[o+1] , 1);	 	
	addr = addr +1;
}
SPI_Write (write_page, 4);
}
power_and_reset_sequence ();
  while (1)
{


}
}

void delay_us(uint32_t us)
{
	uint32_t us_count_tick =  us * (SystemCoreClock/1000000);
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT_CYCCNT  = 0;
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk; 
	while(DWT_CYCCNT < us_count_tick);
	DWT_CONTROL &= ~DWT_CTRL_CYCCNTENA_Msk;
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
	
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}




static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE(); 

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_7, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_5|GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  GPIO_InitStruct.Pin = GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

static void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
}



int convert (char x, char y) {
int out1, out2, out, v,v1;
	
out1 = (x - '0');

if (out1 == 22) out1 = 15;	
if (out1 == 21) out1 = 14;
if (out1 == 20) out1 = 13;
if (out1 == 19) out1 = 12;
if (out1 == 18) out1 = 11;
if (out1 == 17) out1 = 10;

out2 = (y - '0');
if (out2 == 22) out2 = 15;	
if (out2 == 21) out2 = 14;
if (out2 == 20) out2 = 13;
if (out2 == 19) out2 = 12;
if (out2 == 18) out2 = 11;
if (out2 == 17) out2 = 10;

out = out1 << 4  | out2;
return out;
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
		
  }

}