#include "main.h"
#include "stm32f1xx_hal.h"
#include "fatfs.h"
#include <string.h>
#include "ssd1306.h"
#include "fonts.h"
#include "spi.h"
#include "data.h"

I2C_HandleTypeDef hi2c1;
SPI_HandleTypeDef hspi1;
////////////////////////////////////////////////////
// FATFS реализован на базе стандартной библиотеки от СТМ.
FATFS fs;
FATFS *pfs;
FIL fil;
FRESULT fres;
DWORD fre_clust;
uint32_t total, free;
/////////////////////////////////////////////////////
// Блок переменных
void delay_us(uint32_t us);
uint8_t program_enable_byte;
unsigned int i;
uint8_t reload_counter;
uint8_t signature[3];	
unsigned int string_structure[100];
void write_string (char* string);
int convert (char x, char y);
int o;

		int x, y;
		int i2, res = -1;
char buffer[1000];
int addr = 0;	
////////////////////////////////////////////////
int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_I2C1_Init();
  MX_FATFS_Init(); 
  ssd1306_Init (&hi2c1);	
	
if (f_mount(&fs, "", 0) == FR_OK) {                     // Монтируем карту
 ssd1306_Fill (0);		
 ssd1306_SetCursor (0,0);		
 ssd1306_WriteString ("SD MOUNT OK", Font_7x10, 1);	//Вывод информации об успешном монтировании на дисплей
 ssd1306_UpdateScreen (&hi2c1);
 HAL_Delay (1000);
}
else {
 ssd1306_Fill (0);		
 ssd1306_SetCursor (0,0);		
 ssd1306_WriteString ("SD MOUNT NE OK", Font_7x10, 1);	//Вывод информации о неудаче на дисплей
 ssd1306_UpdateScreen (&hi2c1);	
 HAL_Delay (1000);
}
	
if (f_open(&fil, "firmware.hex", FA_READ) == FR_OK) { //Открываем файл, содержащий прошивку. Для удобства в дальнейшем можно сделать возможность выбора файла, из которого в дальнейшембудут считываться данные
 ssd1306_Fill (0);		
 ssd1306_SetCursor (0,0);	
 ssd1306_WriteString ("SD OPEN OK", Font_7x10, 1); //Вывод информации на дисплей об успешном открытии файла
 ssd1306_UpdateScreen (&hi2c1);
 HAL_Delay (1000);
}
else {
 ssd1306_Fill (0);		
 ssd1306_SetCursor (0,0);		
 ssd1306_WriteString ("SD OPEN NE OK", Font_7x10, 1);//Вывод информации о неудаче на дисплей		
 ssd1306_UpdateScreen (&hi2c1);	
 HAL_Delay (1000);	
}
///////////////////////////////////////////////////////////////////  

power_and_reset_sequence (); //Выполняем перезагрузку и подготовку камня к прошивке
program_enable_byte = SPI_Read_Write (program_enable, 4, 3); // Вводим АВРку в режим программирования, попутно считывая байт
	
	if (program_enable_byte == 0x53) {
	ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Progr. mode enabled", Font_7x10, 1); // Режим программирования успешно включен
	ssd1306_UpdateScreen (&hi2c1);
	HAL_Delay (1000);
}
else 
{

	ssd1306_Fill (0);
	ssd1306_SetCursor (0,0);
	ssd1306_WriteString ("Progr. mode failed", Font_7x10, 1); //Режим программирования успешно не включился.
	ssd1306_UpdateScreen (&hi2c1);
	HAL_Delay (1000);
	reload_counter = reload_counter +1;
}
if (reload_counter >= 3) NVIC_SystemReset(); // в случае 3-х промахов перезагружаем управляющий контроллер.


signature[0] = SPI_Read_Write (read_signature1, 4, 4);
signature[1] = SPI_Read_Write (read_signature2, 4, 4);	
signature[2] = SPI_Read_Write (read_signature3, 4, 4);//Считываем идентификатор модели АВРки


if ((signature[0] == 0x1E) && (signature[1] == 0x93) && (signature[2] == 0x0F))
{
 ssd1306_Fill (0);
 ssd1306_SetCursor (0,0);
 ssd1306_WriteString ("Atmega88-PA", Font_7x10, 1); //Потом можно будет добавить и другие АВРки
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
	
	
	
SPI_Write (chip_erase,4); //Очищаем АВРку
ssd1306_Fill (0);
ssd1306_SetCursor (0,0);
ssd1306_WriteString ("Chip is clear", Font_7x10, 1);
ssd1306_UpdateScreen (&hi2c1);
HAL_Delay (1000);

	
/*
Чтение данных с карты можно реализовать различными способами. 
Изначально я хотел считать весь файл в какую-нибудь переменную. Учитывая, что у 88-й объем флеш-памяти равен 2байта*32слова*64Стр = 4кб.,
то внутренней памяти СТМки хватило бы с запасом. Если бы я пошел этим путем, то я бы прогнал весь массив данных, парсируя из него символы начала строки и символы сдвига каретки \r\n.
Так можно было бы узнать количество строк в файле, а также длинну этих строк, а затем просто впихнуть их в алгоритм прораммирования.
Но этот способ не очень практичный, учитывая, что атмеги бывают разные, а память у них отличается.

Поэтому был выбран другой способ: Данные достаются построчно в один и тот же буффер, который сразу же прогоняется по алгоритму программирования и записывает данные в память АВРки.
После того, как данные отправлены, буфер заполняется заново.

*/
while(f_gets((TCHAR*)buffer, 10000, &fil))
  {
for (i2 = 0; i2 <=100; i2++) {
	   if ((buffer[i2] != 10) && (buffer[i2] != 13)) { //убираем символы сдвига каретки из массива.		 		 
	      ssd1306_Fill (0);
	      ssd1306_SetCursor (0,0);
	      ssd1306_printInt (0, 20, i2, Font_7x10, 1);
	      ssd1306_UpdateScreen (&hi2c1);
	}
else {
                i2 = 100; //как только мы натыкаемся на символы сдвига каретки, мы выходим из цикла считывания строки.
		res = res +1;
     }
}

string_structure[0] = convert (buffer[1], buffer[2]); //Узнаем количество байт в строке.
string_structure[1] = convert (buffer[3], buffer[4]);
string_structure[2] = convert (buffer[5], buffer[6]); // Начальный адрес записи.
string_structure[3] = convert (buffer[7], buffer[8]); //Тип строки (запись, чтение, последняя строка и пр.)

if (string_structure[3] == 00) { //Учитывая, что мы только прошиваем камень и строки чтения нас не интересуют, мы отбрасываем другие строки. (К примеру, последнюю, которая не несет в себе ничего кроме данных о том, что она последняя).
int p = -2;//

for (o = 4; o <= (string_structure[0] + 3); o = o+2) {
 if (o == 4) {
	string_structure[4] = convert (buffer[9], buffer[10]); //Если у нас первый байт данных строки, то преобразуем его следующим образом:
	string_structure[5] = convert (buffer[11], buffer[12]); 	
}
else {
p = p+2;
string_structure[o] = convert (buffer[o+7+p], buffer[o+8+p]); // Если у нас последующие байты строки, то преобразуем их по формуле:
string_structure[o+1] = convert (buffer[o+9+p], buffer[o+10+p]);
}
}
/*
После того, как строка преобразована из текстового вида в вещественную переменную, мы можем начать ее записывать в память контроллера.
*/
	
for (int k = 4; k <= (string_structure[0] + 3); k=k+2) { //Ориентируемся на 1-й байт парсированной строки, в котором указано количество символов.
unsigned char str2[4] = 	{0x40, 0x00, addr, string_structure[k]}; //Запись младшего байта по адресу addr. Адрес должен инкеменироваться каждую новую итерацию, чтобы не записывать все в одну ячейк.
SPI_Write (str2, 4);
unsigned char str3[4] = 	{0x48, 0x00, addr, string_structure[k+1]}; //Старший байт слова
SPI_Write (str3, 4); 
addr = addr +1; // Адрес слова.
}
}
page (res); // Данная функция реализует запись загруженных данных на одну страницу. Переменная res содержит в себе данные о номере текущей страницы, на которую ведётся запись данных. 
//С адресацией слов и байт данных у АВР все очень непросто и я потратил несколько часов, чтобы нормально доработать эту функцию
}
/*
После того, как страница записана, цикл возвращается к работе с новой строкой hex-файла. И так до тех пор, пока не будут записаны все строки.
*/
	
reset_sequence;//Перезагрузка АВРки
ssd1306_Fill (0);
ssd1306_SetCursor (0,0);
ssd1306_WriteString ("Done", Font_7x10, 1);//Прошивка закончена.
ssd1306_UpdateScreen (&hi2c1);

//////////////////////////////////////////////////////////////////////

f_close(&fil); //Закрываем хекс
f_mount(NULL, "", 1);
	
  while (1)
  {
		
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


void delay_us(uint32_t us) //Задержка для SPI
{
	uint32_t us_count_tick =  us * (SystemCoreClock/1000000);
	SCB_DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT_CYCCNT  = 0;
	DWT_CONTROL |= DWT_CTRL_CYCCNTENA_Msk; 
	while(DWT_CYCCNT < us_count_tick);
	DWT_CONTROL &= ~DWT_CTRL_CYCCNTENA_Msk;
}



int convert (char x, char y) { //Функция перевода данных строки в вещественные переменные.
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


