#include "main.h"
#include "stm32f1xx_hal.h"
#include "fatfs.h"
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "fonts.h"
#include "spi.h"
#include "data.h"
#include "ff.h"


I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;

FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
FILINFO fsize, fname;
DIR dir;
uint32_t total, free;

void delay_us(uint32_t us);
uint8_t program_enable_byte;
unsigned int i;
uint8_t reload_counter;
uint8_t signature[3];	
unsigned int string_structure[100];
void write_string (char* string);
int convert (char x, char y);
void delay_us(uint32_t us);
uint8_t find_key (char key[], char array[]);
uint8_t read_fuses (char array[100]);
int o;
		int x, y, m;
		int i2, res = -1;


char buffer[1000];
char batbuffer[100];


int addr = 0;	
char model [15];
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
	MX_I2C1_Init();
  MX_FATFS_Init();
	ssd1306_Init (&hi2c1);	
	

	if (f_mount(&fs, "", 0) == FR_OK) {
ssd1306_Fill (0);		
ssd1306_SetCursor (0,0);		
ssd1306_WriteString ("SD MOUNT OK", Font_7x10, 1);		
ssd1306_UpdateScreen (&hi2c1);
HAL_Delay (1000);
	}
	else {
ssd1306_Fill (0);		
ssd1306_SetCursor (0,0);		
ssd1306_WriteString ("SD MOUNT NE OK", Font_7x10, 1);		
ssd1306_UpdateScreen (&hi2c1);	
HAL_Delay (1000);
	}
	
	
if (f_open(&fil, "prog4.bat", FA_READ) == FR_OK) {
ssd1306_Fill (0);		
ssd1306_SetCursor (0,0);	
ssd1306_WriteString ("SD OPEN OK", Font_7x10, 1);		
ssd1306_UpdateScreen (&hi2c1);
HAL_Delay (1000);
}
	else {
ssd1306_Fill (0);		
ssd1306_SetCursor (0,0);		
ssd1306_WriteString ("SD OPEN NE OK", Font_7x10, 1);		
ssd1306_UpdateScreen (&hi2c1);	
HAL_Delay (1000);	
	}


while(f_gets((TCHAR*)buffer, 10000, &fil))
  {
for (i2 = 0; i2 <= 100; i2++) {
if ((buffer[i2] != 13) && (buffer [i2] != 10))	 {
	batbuffer[m++] = buffer[i2];
}
else {
	i2 = 100;
	m = 0;
}
}
read_fuses(batbuffer);
	ssd1306_Fill (0);
  ssd1306_printInt (0,0,cksel, Font_7x10, 1);
ssd1306_SetCursor (0,20);
ssd1306_WriteString (batbuffer, Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);

}

	/*
///////////////////////////////////////////////////////////////////  
	
	power_and_reset_sequence ();
  program_enable_byte = SPI_Read_Write (program_enable, 4, 3);
	
	if (program_enable_byte == 0x53) {
	ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Progr. mode enabled", Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);
		HAL_Delay (1000);
}
else 
{
	power_and_reset_sequence ();
  ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Progr. mode failed", Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);
	HAL_Delay (1000);
	reload_counter = reload_counter +1;
}
if (reload_counter >= 3) NVIC_SystemReset();


signature[0] = SPI_Read_Write (read_signature1, 4, 4);
signature[1] = SPI_Read_Write (read_signature2, 4, 4);	
signature[2] = SPI_Read_Write (read_signature3, 4, 4);


if ((signature[0] == 0x1E) && (signature[1] == 0x93) && (signature[2] == 0x0F))
{
  ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Atmega88-PA", Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);
	HAL_Delay (1000);
}	


else
{
	ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Signature Failed", Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);
	HAL_Delay (1000);	
}	
	
SPI_Write (chip_erase,4);	 
ssd1306_Fill (0);
ssd1306_SetCursor (0,0);
ssd1306_WriteString ("Chip is clear", Font_7x10, 1);
ssd1306_UpdateScreen (&hi2c1);
HAL_Delay (1000);


while(f_gets((TCHAR*)buffer, 10000, &fil))
  {
for (i2 = 0; i2 <=1000; i2++) {
	   if ((buffer[i2] != 10) && (buffer[i2] != 13)) {
	ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_printInt (0, 20, i2, Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);
		 }
		 else {
			 if (i2 >= 16) res = res +1;
			 i2 = 1000;
		 }
}

string_structure[0] = convert (buffer[1], buffer[2]);
string_structure[1] = convert (buffer[3], buffer[4]);
string_structure[2] = convert (buffer[5], buffer[6]);
string_structure[3] = convert (buffer[7], buffer[8]);
if (string_structure[3] == 00) {
int p = -2;

for (o = 4; o <= (string_structure[0] + 3); o = o+2) {
if (o == 4) {
	string_structure[4] = convert (buffer[9], buffer[10]); // ??????? ?? ????????? ????? ??????, ??????? ?????????? ?????????
	string_structure[5] = convert (buffer[11], buffer[12]); 	
}
else {
	p = p+2;
	string_structure[o] = convert (buffer[o+7+p], buffer[o+8+p]); // ??????? ?? ????????? ????? ??????, ??????? ?????????? ?????????
	string_structure[o+1] = convert (buffer[o+9+p], buffer[o+10+p]);
}
}

for (int k = 4; k <= (string_structure[0] + 3); k=k+2) {
unsigned char str2[4] = 	{0x40, 0x00, addr, string_structure[k]}; //low byte
SPI_Write (str2, 4);
unsigned char str3[4] = 	{0x48, 0x00, addr, string_structure[k+1]}; //high byte
SPI_Write (str3, 4); 
addr = addr +1; // word addr
}
}
page (res);
	}
	

	void reset_sequence (void);
  ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Done", Font_7x10, 1);
	ssd1306_UpdateScreen (&hi2c1);

//////////////////////////////////////////////////////////////////////


*/

f_close(&fil);
f_mount(NULL, "", 1);

  while (1)
  {
		
  }
}

uint8_t find_key (char key[10], char array[100]) {
char *istr = strstr (array,key);
	
   if ( istr == NULL) {
return 0;
	 }
   else {
		 uint8_t pos = istr-batbuffer+1;
return pos;
}
}

uint8_t read_fuses (char array[100]) {

uint8_t pos = find_key ("bodlevel=", array);
	if (pos!=0) {
bodlevel = (array[pos+8] - '0');
pos = 0;
	}
	
pos = find_key ("rstdisbl=", array);
	if (pos!=0) {	
rstdisbl = array[pos+8] - '0';
pos = 0;	
	}		
	
pos = find_key ("dwen=", array);
	if (pos!=0) {
dwen = array[pos+4] - '0';
pos = 0;
	}
	
pos = find_key ("wdton=", array);
		if (pos!=0) {
wdton = array[pos+5]- '0';
pos = 0;
		}
				
pos = find_key ("eesave=", array);
			if (pos!=0) {
eesave = (array[pos+6]) - '0';
pos = 0;
			}
				
pos = find_key ("ckdiv8=", array);
			if (pos!=0) {
ckdiv8 = (array[pos+6]) - '0';
pos = 0;
			}
				
pos = find_key ("ckout=", array);
			if (pos!=0) {
ckout = (array[pos+5]) - '0';
pos = 0;
			}			
				
pos = find_key ("sut=", array);
			if (pos!=0) {
sut = (array[pos+3]) - '0';
pos = 0;
			}			
							
pos = find_key ("cksel=", array);
			if (pos!=0) {
cksel = (array[pos+5]) - '0';
pos = 0;
			}
			
pos = find_key ("bootsz=", array);
			if (pos!=0) {
bootsz = (array[pos+6]) - '0';
pos = 0;
			}
			
pos = find_key ("bootrst=", array);
			if (pos!=0) {
bootrst = (array[pos+7]) - '0';
pos = 0;
			}
			
}





void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}


static void MX_SPI1_Init(void)
{
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_8;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

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
    _Error_Handler(__FILE__, __LINE__);
  }
}


static void MX_GPIO_Init(void)
{

  GPIO_InitTypeDef GPIO_InitStruct;

  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
	
	  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_0|GPIO_PIN_3|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4;// SCK1, MOSI1, POWER, RST1, CS2
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  GPIO_InitStruct.Pin = GPIO_PIN_11; // MISO1
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}




void _Error_Handler(char * file, int line)
{
  while(1) 
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

out = out1 << 4 | out2;
return out;
}


