
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f401_discovery.h"
#include "stm32f401_discovery_audio.h"
#include "main.h"


char get_lcd_embd_h(char);
char get_lcd_embd_l(char);
void send_char_to_lcd(char);
UART_HandleTypeDef EspInit(void);
void clear_lcd();
void cursor_to_home();
void init_lcd();

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef esp;

#define LCD_ADD     	0x27 // Адрес LCD на шине I2C
#define LCD_ADD_W 	0x7E // Адрес LCD на шине I2C для записи
#define LCD_ADD_R 	0x7F // Адрес LCD на шине I2C для чтения


char ans[25] = {0};


// ----- main() ---------------------------------------------------------------
// Точка входа в программу
int
main(int argc, char* argv[])
{

  	HAL_Init(); // инициализация библиотеки HAL 
  	SystemClock_Config(); // настройка таймера (необходимо для задания задержек)
	// инициализация входов/выходов общего назначения для передачи на дисплей по I2C 	
	MX_GPIO_Init(); 
   	I2c_Init(&hi2c1); // настройка передачи по I2C
	init_lcd(); // инициализация дисплея
   	esp = EspInit(); // инициалзация wifi модуля (приемник)

	// настройка wifi модуля
	// AT+CWMODE=3 - режим модуля Station (server) и AP 
	HAL_UART_Transmit(&esp, "AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"), 5000);
    	HAL_Delay(50); // для нормальной инициализации нужна небольшая задержка
	send_char_to_lcd('c');
	// AT+CIPMODE=1 - обычный режим пережачи
	HAL_UART_Transmit(&esp, "AT+CIPMODE=0\r\n", strlen("AT+CIPMODE=0\r\n"), 5000);
	cursor_to_home();
	// AT+CIPMUX=1 - поддерживание множественного подключения к сети
	HAL_UART_Transmit(&esp, "AT+CIPMUX=1\r\n", strlen("AT+CIPMUX=1\r\n"), 5000);
	cursor_to_home();
	send_char_to_lcd('c');
	// AT+CIPSERVER=1,88 - стартуем TCP сервер на 88 порту (по нему и будем стучаться)
	HAL_UART_Transmit(&esp, "AT+CIPSERVER=1,88\r\n", strlen("AT+CIPSERVER=1,88\r\n"), 5000);
	cursor_to_home();
	send_char_to_lcd('s');
	// инициализируем прерывания по приему на UART
	HAL_UART_Receive_IT(&esp, ans, 1);
	__HAL_UART_ENABLE_IT(&esp, UART_IT_RXNE);
	while(1){
		// Ждем когда будем выставлен флаг SET (прием каких-то данных по UART)
	    	if(UartReady == SET){
			// проверяем, была ли в принят. данных искомая посл-ть
	    		if(strstr(ans, "+IPD:0,")){
				cursor_to_home();
	    			for(int i = 12; i < 25; i++)
	    		       		send_char_to_lcd(ans[i]);
	    		       	send_char_to_lcd(' ');
	    		}
	    		memset(ans, 0, 25);
	    		UartReady = RESET;
	    		HAL_UART_Receive_IT(&esp, ans, 1);
	    	}	
	    }

 }


// ----- EspInit() ---------------------------------------------------------------
// Инициализация wi-fi модуля приемника
UART_HandleTypeDef EspInit(void){
	// инициализируем тактирование шины портов
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3; // выбираем 2 и 3 порты (PA2, PA3)
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // выбираем альтернативную функцию (UART)
	GPIO_InitStruct.Pull = GPIO_PULLUP; // подтягиваем к питанию выводы (необходимо для норм. работы(см. дз. 2))
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH; // задаем скорость работы
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2; // выбираем UART2
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); 

	UART_HandleTypeDef huart1;

	huart1.Instance = USART2; // работаем с UART2
	huart1.Init.BaudRate = 115200; // выбираем 115200 бод (см. значения в дз)
	huart1.Init.WordLength = UART_WORDLENGTH_8B; // задаем длину слова
	huart1.Init.StopBits = UART_STOPBITS_1; // задаем один стоп бит
	huart1.Init.Parity = UART_PARITY_NONE; // задаем, что не будет бита четности
	huart1.Init.Mode = UART_MODE_TX_RX; // режим прием/передача
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
 
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0); // задаем приоритет прерываний для UART (наивысший)
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	return huart1;
}

// ----- get_lcd_embd_h(char) ---------------------------------------------------------------
/*
	Получение четырех старших битов для отправки. Кодирование данных 
	проиводим в соответсв. с таблицей кодирования из
	документации на LCD 
*/
char get_lcd_embd_h(char a){
	switch (a) {
	case '0' ... '9':
		return 0b0011;
	case 'A' ... 'O':
		return 0b0100;
	case 'P' ... 'Z':
		return 0b0101;
	case 'a' ... 'o':
		return 0b0110;
	case 'p' ... 'z':
		return 0b0111;
	case '.':
		return 0b0010;
	case ' ':
			return 0b0001;
	default:
		return 0b1101;
	}
}

// ----- get_lcd_embd_l(char) ---------------------------------------------------------------
/*
	Получение четырех младших битов для отправки. Кодирование данных 
	проиводим в соответсв. с таблицей кодирования из
	документации на LCD 
*/
char get_lcd_embd_l(char a){
	switch(a){
	case '0' ... '9':
		return a-'0';
	case 'A' ... 'O':
		return a -'A' + 1;
	case 'P' ... 'Z':
		return a-'P';
	case 'a' ... 'o':
		return a -'a' + 1;
	case 'p' ... 'z':
		return a-'p';
	case '.':
		return 0b1110;
	case ' ':
		return 0b0000;
	default:
		return 0b1111;
	}
}


// ----- send_char_to_lcd(char) ---------------------------------------------------------------
/*
	Отправка полноценного пакета с данными.
	Т.к. в нашей схеме придусмотрен расширитель портов, поэтому
	необходимо задать режим передачи по 4 бита, а затем
	слать стачала 4 старших, за котороми 4 младших бита.
	Формирование битов производим в соотв. с таблицей
	кодирования данных из документации на LCD.
*/
void send_char_to_lcd(char to_send){
	char h_part = (get_lcd_embd_h(to_send) << 4) + 0b1101;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &h_part, 1, 0x100);
	char l_part = (get_lcd_embd_l(to_send) << 4) + 0b1101;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &l_part, 1, 0x100);
}


// ----- clear_lcd(char) ---------------------------------------------------------------
/*
	Фун-ия для очистки экрана. 
	По док-ии команда для очистки имеет вид 00000001
	Т.к. мы шлем по 4 бита, поэтому разбеляем всю последовательность
	на две части: сначала шлем старшие, а за ними младшие.
	Остальные 4 бита (1100) задают режим работы LCD (выключать ли подскетку и т.д.)
	Подробнее см. отчет.
*/
void clear_lcd(){
	char cmd = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
	cmd = 0b00011100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
}


// ----- cursor_to_home() ---------------------------------------------------------------
/*
	Фун-ия для перевода курсора в начало экрана. 
	По док-ии команда для очистки имеет вид 0000001x (x - don't care value)
	Т.к. мы шлем по 4 бита, поэтому разбеляем всю последовательность
	на две части: сначала шлем старшие, а за ними младшие.
	Остальные 4 бита (1100) задают режим работы LCD (выключать ли подскетку и т.д.)
	Подробнее см. отчет.
*/
void cursor_to_home(){
	char cmd = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
	cmd = 0b00101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
}


// ----- init_lcd() ---------------------------------------------------------------
/*
	Инициализация LCD.
	Для задания корректной работы LCD предумотрен ряд команд 
	в документации, которые очищают экран, явно задают режим (в нашем случае - 
	передача по 4 бита) и т.д. (подробнее см. документацию на LCD1602A)
	Передачу производим посредством протокола I2C, по которому работает 
	расширитель портов, который припаян к LCD.
*/
void init_lcd(){
  	HAL_Delay(50);
  	uint8_t cmd[1];
	cmd[0] = 0b00111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(6);
	cmd[0] = 0b00111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(1);
	cmd[0] = 0b00111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b10001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00011100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b01101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	cmd[0] = 0b11111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
}

// ----- HAL_UART_RxCpltCallback(UART_HandleTypeDef*) ---------------------------------------------------------------
/*
	Обработчки прерывания приема данных на LCD.
*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  	UartReady = SET;
  	if (ans[0]){
		if (HAL_UART_Receive(&esp, ans, 25, 1) != HAL_OK){
			HAL_UART_Receive_IT(&esp, ans, 1); // явно задаем прием на 1 байт
		}
  	}
}

// ----- MX_GPIO_Init() ---------------------------------------------------------------
/*
	Задание выводов общего назначение для работы
	по протоколу I2C.
*/
static void MX_GPIO_Init(void){

	/**I2C1 GPIO Configuration
	 PB6     ------> I2C1_SCL
	 PB7     ------> I2C1_SDA
	 */
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	// Задаем режим альтеративной функции. Для протокола I2C это должен быть OP (Open Drain/Открытый коллектор)
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; 
	GPIO_InitStruct.Pull = GPIO_PULLUP; // Подтягиваем вывода к питанию
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH; // Задаем частоту работы
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1; // Выбираем альтернативную фун-ию как работу по I2C1
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // Инициализируем выбранные выше параметры
	  __HAL_RCC_I2C1_CLK_ENABLE();
  	/* Задаем тактирование шины портов */
  	__HAL_RCC_GPIOC_CLK_ENABLE(); 
  	__HAL_RCC_GPIOH_CLK_ENABLE();
  	__HAL_RCC_GPIOB_CLK_ENABLE();
}


// ----- I2c_Init() ---------------------------------------------------------------
/*
	Инициализация для работы I2C.
*/
I2C_HandleTypeDef I2c_Init(void){
	__HAL_RCC_I2C1_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	I2C_HandleTypeDef i2c;

	i2c.Instance = I2C1;
	i2c.Init.ClockSpeed = 100000;
	i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c.Init.OwnAddress1 = 0x15;
	i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c.Init.OwnAddress2 = 0x00;
	i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

	gpio.Pin = GPIO_PIN_9 | GPIO_PIN_6;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Pull = GPIO_NOPULL;
	gpio.Speed = GPIO_SPEED_FREQ_HIGH;
	gpio.Alternate = GPIO_AF4_I2C1;

	HAL_GPIO_Init(GPIOB, &gpio);

	__HAL_RCC_I2C1_FORCE_RESET();
	__HAL_RCC_I2C1_RELEASE_RESET();

	HAL_I2C_Init(&i2c);

	uint8_t ctrl_reg_byte = 0xE0;

	return i2c;
}
#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------

