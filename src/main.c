/*
 * This file is part of the µOS++ distribution.
 *   (https://github.com/micro-os-plus)
 * Copyright (c) 2014 Liviu Ionescu.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "math.h"
#include "stm32f401_discovery.h"
#include "stm32f401_discovery_audio.h"
#include "main.h"

#include "Timer.h"
#include "BlinkLed.h"

// ----------------------------------------------------------------------------
//
// Standalone STM32F4 led blink sample (trace via NONE).
//
// In debug configurations, demonstrate how to print a greeting message
// on the trace device. In release configurations the message is
// simply discarded.
//
// Then demonstrates how to blink a led with 1 Hz, using a
// continuous loop and SysTick delays.
//
// Trace support is enabled by adding the TRACE macro definition.
// By default the trace messages are forwarded to the NONE output,
// but can be rerouted to any device or completely suppressed, by
// changing the definitions required in system/src/diag/trace_impl.c
// (currently OS_USE_TRACE_ITM, OS_USE_TRACE_SEMIHOSTING_DEBUG/_STDOUT).
//

// ----- Timing definitions -------------------------------------------------

// Keep the LED on for 2/3 of a second.
#define BLINK_ON_TICKS  (TIMER_FREQUENCY_HZ * 3 / 4)
#define BLINK_OFF_TICKS (TIMER_FREQUENCY_HZ - BLINK_ON_TICKS)




// ----- main() ---------------------------------------------------------------

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


void ACCELERO_ReadAcc(void);
void GYRO_ReadAng(void);
void ACCELERO_MEMS_Test(void);
void GYRO_MEMS_Test(void);
void EXTILine0_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
char get_lcd_embd_h(char);
char get_lcd_embd_l(char);
void send_char_to_lcd(char);

I2C_HandleTypeDef I2c_Init(void);

extern __IO uint8_t UserPressButton;

/* Init af threshold to detect acceleration on MEMS */
int16_t ThresholdHigh = 1000;
int16_t ThresholdLow = -1000;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef esp;

#define LCD_ADD     0x27
#define LCD_ADD_W 0x7E
#define LCD_ADD_R 0x7F


__IO uint8_t UserPressButton = 0;

/* Wave Player Pause/Resume Status. Defined as external in waveplayer.c file */
__IO uint32_t PauseResumeStatus = IDLE_STATUS;

/* Counter for User button presses */
__IO uint32_t PressCount = 0;

UART_HandleTypeDef EspInit(void){
	__HAL_RCC_USART2_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	// pa2 = (tx)
	// pa3 = (rx)
	GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	UART_HandleTypeDef huart1;

	huart1.Instance = USART2;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;

	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART2_IRQn);

	if(HAL_UART_Init(&huart1) == HAL_OK)
		return huart1;
	return huart1;
}

//------------------------------------------------------
// ESPStart()
//------------------------------------------------------
int EspStart(UART_HandleTypeDef esp){
	/*
	 * TODO:
	 * 1. Delete all commands marked as "убрать"
	 * 2. Add answer test conditions
	 * 3. Be happy :-)
	 */
	char ans[20] = {0};

	HAL_Delay(1000);	//убрать

	HAL_UART_Transmit(&esp, ATCWMODE1, strlen(ATCWMODE1), 1000);

	HAL_Delay(30000);		//ГОВНО

	HAL_UART_Transmit(&esp, ATE0, strlen(ATE0), 1000);

	HAL_Delay(100);		//ГОВНО

	HAL_UART_Transmit(&esp, AT, strlen(AT), 1000);

	HAL_UART_Receive(&esp, ans, 5, 1000);

	if(!(ans[3] == 'O' && ans[4] == 'K')) return 1;

	HAL_UART_Transmit(&esp, ATCWJAP, strlen(ATCWJAP), 1000);

	HAL_Delay(1000);

	//Try to connect to TCP server

	//HAL_UART_Transmit(&esp, ATCIPSTART, strlen(ATCIPSTART), 1000); //священная команда

	//HAL_Delay(1000);
}
__IO ITStatus UartReady = RESET;


void trace_binary(char num){
	while (num) {
	    if (num & 1)
	    	trace_puts("1");
	    else
	    	trace_puts("0");

	    num >>= 1;
	}
	trace_puts("\n");
}

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

void send_char_to_lcd(char to_send){
	char h_part = (get_lcd_embd_h(to_send) << 4) + 0b1101;
	//trace_binary(h_part);
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &h_part, 1, 0x100);
	HAL_Delay(10);
	char l_part = (get_lcd_embd_l(to_send) << 4) + 0b1101;
	//trace_binary(l_part);
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &l_part, 1, 0x100);
	HAL_Delay(10);
}

void clear_lcd(){
	char cmd = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
	cmd = 0b00011100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
}

void cursor_to_home(){
	char cmd = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
	cmd = 0b00101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, &cmd, 1, 0x100);
	HAL_Delay(10);
}

void init_lcd(){
  	HAL_Delay(50);
  	uint8_t cmd[1];
	cmd[0] = 0b00111100;
	//trace_printf("\nGO\n");
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(6);
	cmd[0] = 0b00111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(1);
	cmd[0] = 0b00111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b00101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);


	cmd[0] = 0b00101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b11001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);

	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b10001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);

	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b00011100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);


	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b01101100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);

	cmd[0] = 0b00001100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b11111100;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);

}

char ans[25] = {0};

int
main(int argc, char* argv[])
{
	  trace_puts("Hello ARM World!");

	//HAL_Init();
	BSP_PB_Init(BUTTON_KEY, BUTTON_MODE_EXTI);
	  //EXTILine0_Config();


	// Send a greeting to the trace device (skipped on Release).
  trace_puts("Hello ARM World!");

  // At this stage the system clock should have already been configured
  // at high speed.
  trace_printf("System clock: %u Hz\n", SystemCoreClock);

  timer_start();

  trace_printf("LCD ADD: %u", LCD_ADD);


  HAL_Init();

/* Configure the system clock */
  SystemClock_Config();

  MX_GPIO_Init();
    MX_I2C1_Init();


    //HAL_UART_Transmit(&esp, "AT+GMR\r\n", strlen("AT+GMR\r\n"), 1000);
   // (EspStart(esp));
    init_lcd();
    esp = EspInit();


    /*##-2- Put UART peripheral in reception process ###########################*/
     // if(HAL_UART_Receive_IT(&esp, (uint8_t *)ans, 3) != HAL_OK)
      //{
       // Error_Handler();
      //}

      /*##-3- Wait for the end of the transfer ###################################*/
      //while (UartReady != SET)
      //{
      //}



    //HAL_UART_Transmit(&esp, "AT\r\n", strlen("AT\r\n"), 5000);
    //HAL_Delay(100);
    /*
     *  AT+CWMODE=3
		AT+CIPMODE=0
		AT+CIPMUX=1
		AT+CIPSERVER=1,88
     */
    //HAL_UART_Transmit(&esp, "AT\r\n", strlen("AT\r\n"), 5000);
    HAL_UART_Transmit(&esp, "AT+CWMODE=3\r\n", strlen("AT+CWMODE=3\r\n"), 5000);
    //HAL_UART_Receive_IT(&esp, ans, 9);
    HAL_Delay(500);
    trace_printf("1");
    send_char_to_lcd('c');
    send_char_to_lcd('w');
    send_char_to_lcd('m');
    send_char_to_lcd('o');
    send_char_to_lcd('d');
    send_char_to_lcd('e');
    send_char_to_lcd(' ');
    send_char_to_lcd('3');
    send_char_to_lcd(' ');

    HAL_UART_Transmit(&esp, "AT+CIPMODE=0\r\n", strlen("AT+CIPMODE=0\r\n"), 5000);
    HAL_Delay(500);
    cursor_to_home();
    send_char_to_lcd('c');
    send_char_to_lcd('i');
    send_char_to_lcd('p');
    send_char_to_lcd('m');
    send_char_to_lcd('o');
    send_char_to_lcd('d');
    send_char_to_lcd('e');
    send_char_to_lcd(' ');
    send_char_to_lcd('0');
    send_char_to_lcd(' ');
    trace_printf("2");
    HAL_UART_Transmit(&esp, "AT+CIPMUX=1\r\n", strlen("AT+CIPMUX=1\r\n"), 5000);
    HAL_Delay(500);
    cursor_to_home();
    send_char_to_lcd('c');
    send_char_to_lcd('i');
    send_char_to_lcd('p');
    send_char_to_lcd('m');
    send_char_to_lcd('u');
    send_char_to_lcd('x');
    send_char_to_lcd(' ');
    send_char_to_lcd('1');
    send_char_to_lcd(' ');
    trace_printf("4");
    HAL_UART_Transmit(&esp, "AT+CIPSERVER=1,88\r\n", strlen("AT+CIPSERVER=1,88\r\n"), 5000);

    cursor_to_home();
    send_char_to_lcd('s');
    send_char_to_lcd('e');
    send_char_to_lcd('r');
    send_char_to_lcd('v');
    send_char_to_lcd('e');
    send_char_to_lcd('r');
    send_char_to_lcd(' ');
    send_char_to_lcd('s');
    send_char_to_lcd('t');
    send_char_to_lcd('a');
    send_char_to_lcd('r');
    send_char_to_lcd('t');
    send_char_to_lcd(' ');



    HAL_Delay(500);

    //HAL_UART_Transmit(&esp, "AT+CWLAP\r\n", strlen("AT+CWLAP\r\n"), 5000);
    HAL_Delay(500);
    cursor_to_home();
    send_char_to_lcd('w');
    send_char_to_lcd('a');
    send_char_to_lcd('i');
    send_char_to_lcd('t');
    send_char_to_lcd(' ');
    send_char_to_lcd('f');
    send_char_to_lcd('o');
    send_char_to_lcd('r');
    send_char_to_lcd(' ');
    send_char_to_lcd('m');
    send_char_to_lcd('a');
    send_char_to_lcd('r');
    send_char_to_lcd('a');
    send_char_to_lcd('t');
    send_char_to_lcd(' ');


    HAL_UART_Receive_IT(&esp, ans, 1);
    __HAL_UART_ENABLE_IT(&esp, UART_IT_RXNE);
    while(1){
    	//HAL_UART_Transmit(&esp, "123\r\n", strlen("123\r\n"), 5000);
    	//trace_printf(" esp.RxXferCount =  %d \n", esp.RxXferCount);

    	if(/*esp.RxXferCount == 0 ||*/ UartReady == SET){
    		UartReady = RESET;
    		cursor_to_home();
    		//HAL_Delay(100);
    		if(ans[2]=='+'){
    			clear_lcd();
    		       			for(int i = 12; i < 25; i++)
    		       				send_char_to_lcd(ans[i]);

    		}trace_printf("\n ANSWER: %s", ans);
       		//send_char_to_lcd(ans[0]);
       		//send_char_to_lcd(ans[0]);
       		//HAL_Delay(50);

       		//HAL_Delay(50);

       		if (ans[0])
    		    		trace_printf("\n %d %c", 0, ans[0]);
    		    	if (ans[1])
    		    	    		trace_printf("\n %d %c ",1,  ans[1]);
    		    	if (ans[2])
    		    	    		trace_printf("\n %d %c ",2,  ans[2]);
    		    	if (ans[3])
    		    	    		trace_printf("\n %d  %c ",3,  ans[3]);
    		    	if (ans[4])
    		    	    		trace_printf("\n %d %c ",4,  ans[4]);
    		    	if (ans[5])
    		    	    		    	    		trace_printf("\n %d %c ",5,  ans[5]);
    		    	if (ans[6])
    		    	    		    	    		    	    		trace_printf("\n %d %c ",6,  ans[6]);
    		    	if (ans[7])
    		    	    		    	    		    	    		trace_printf("\n %d %c ",7,  ans[7]);
    		    	if (ans[8])
    		    	    		    	    		    	    		trace_printf("\n %d %c ",8,  ans[8]);
    		    	if (ans[9])
    		    	    		    	    		    	    		trace_printf("\n %d %c ",9,  ans[9]);

    		    	trace_printf("______________");




    		//HAL_UART_Transmit(&esp, ans, 1, 5000);
    		//HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    		//HAL_NVIC_EnableIRQ(USART2_IRQn);
    		//__HAL_UART_ENABLE_IT(&esp, UART_IT_RXNE);
    		//HAL_UART_Receive(&esp, ans, 1, 100);
    		//HAL_Delay(100);
    		    	memset(ans, 0, 25);
    		HAL_UART_Receive_IT(&esp, ans, 1);
    		//HAL_UART_Transmit(&esp, "AT\r\n", strlen("AT\r\n"), 5000);
    		//esp.RxXferCount = 4;
    	}
    	//HAL_Delay(100);
    	/*if (ans[0])
    		trace_printf(" %d %c ", 0, ans[0]);
    	if (ans[1])
    	    		trace_printf(" %d %c ",1,  ans[1]);
    	if (ans[2])
    	    		trace_printf(" %d %c ",2,  ans[2]);
    	if (ans[3])
    	    		trace_printf(" %d  %c ",3,  ans[3]);
    	if (ans[4])
    	    		trace_printf(" %d %c ",4,  ans[4]);*/
    	/*if (ans[5])
    	    		trace_printf(" %s ", ans[5]);
    	if (ans[6])
    	    		trace_printf(" %s ", ans[6]);
    	if (ans[7])
    	    		trace_printf(" %s ", ans[7]);
    	if (ans[8])
    	    		trace_printf(" %s ", ans[8]);

    	if (ans[9])
    		trace_printf(" %s ", ans[9]);*/
    	//HAL_Delay(1000);
    }

    /*
	send_char_to_lcd('1');
	send_char_to_lcd('2');
	send_char_to_lcd('3');
	send_char_to_lcd('4');
	send_char_to_lcd('5');
	send_char_to_lcd(' ');
	send_char_to_lcd('6');
	send_char_to_lcd('7');
	send_char_to_lcd('8');
	send_char_to_lcd('9');
	send_char_to_lcd('1');

	send_char_to_lcd('2');

	send_char_to_lcd('3');

	send_char_to_lcd('4');

	send_char_to_lcd('5');
	send_char_to_lcd('6');

	cursor_to_home();

	send_char_to_lcd('7');

	send_char_to_lcd('8');
	send_char_to_lcd('-');
	send_char_to_lcd('.');
	send_char_to_lcd('B');*/




	//DATA
	/*cmd[0] = 0b00111101;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b00011101;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);

	cmd[0] = 0b00001101;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);
	cmd[0] = 0b11111101;
	HAL_I2C_Mem_Write(&hi2c1, LCD_ADD_W, 0x00, 1, cmd, 1, 0x100);
	HAL_Delay(10);*/


 }

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
  /* Set transmission flag: transfer complete*/
  UartReady = SET;
  //send_char_to_lcd('S');
  trace_printf("HERE");
  //HAL_Delay(1000);
  if (ans[0] != 'A' && ans[0]){
	  if (HAL_UART_Receive(&esp, ans, 25, 1) == HAL_OK){
	  //cursor_to_home();
	  /*send_char_to_lcd(ans[16]);
	  send_char_to_lcd(ans[17]);
	  send_char_to_lcd(ans[18]);
	  send_char_to_lcd(ans[19]);
	  send_char_to_lcd(ans[20]);
	  send_char_to_lcd(ans[21]);
	  send_char_to_lcd(ans[22]);
	  send_char_to_lcd(ans[23]);
	  send_char_to_lcd(ans[24]);*/
	  //send_char_to_lcd(ans[25]);
		  for(int i = 0; i < 25;i++){
			  //send_char_to_lcd(ans[i]);
			  trace_printf(" %c ", ans[i]);
		  }
	  }else{
		  HAL_UART_Receive_IT(&esp, ans, 1);

	  }
  /*HAL_UART_Receive(&esp, ans, 4, 1000);
  for(int i = 0; i < 4;i++)
  	  trace_printf(" %c ", ans[i]);*/
  }
  trace_printf("\nEXIT\n");
	  /* Turn LED4 on: Transfer in reception process is correct */
  //BSP_LED_On(LED4);
}

/* I2C1 init function */
static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  __HAL_RCC_I2C1_FORCE_RESET();
  	__HAL_RCC_I2C1_RELEASE_RESET();

  HAL_I2C_Init(&hi2c1);

  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    //_Error_Handler(__FILE__, __LINE__);
  }

}

/** Configure pins as
        * Analog
        * Input
        * Output
        * EVENT_OUT
        * EXTI
*/
static void MX_GPIO_Init(void)
{

	/**I2C1 GPIO Configuration
	 PB6     ------> I2C1_SCL
	 PB7     ------> I2C1_SDA
	 */
	  __HAL_RCC_I2C1_CLK_ENABLE();
	  __HAL_RCC_GPIOB_CLK_ENABLE();
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	  __HAL_RCC_I2C1_CLK_ENABLE();
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  //__HAL_RCC_I2C1_CLK_DISABLE();

    /**I2C1 GPIO Configuration
    PB6     ------> I2C1_SCL
    PB7     ------> I2C1_SDA
    */
    //HAL_GPIO_DeInit(GPIOB, GPIO_PIN_6|GPIO_PIN_7);

}

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

	//HAL_StatusTypeDef htd = HAL_I2C_Mem_Write(&i2c, BAR_ADR_W, CTRL_REG1,
	//										 I2C_MEMADD_SIZE_8BIT,
	//										 &ctrl_reg_byte, 1, I2C_MAX_TIMEOUT);

	return i2c;
}

/**
  * @brief  Test ACCELERATOR MEMS Hardware.
  *         The main objective of this test is to check acceleration on 2 axes X and Y
  * @param  None
  * @retval None
  */
void ACCELERO_MEMS_Test(void)
{
  trace_printf("\n\n\nACCELERO_MEMS_Test\n");
  /* Init Accelerometer MEMS */
  if(BSP_ACCELERO_Init() != HAL_OK)
  {
    /* Initialization Error */
    //Error_Handler();
  }

  UserPressButton = 0;
  while(!UserPressButton)
  {
    ACCELERO_ReadAcc();
  }
}

/**
  * @brief  Read Acceleration data.
  * @param  None
  * @retval None
  */
void ACCELERO_ReadAcc(void)
{
  int16_t buffer[3] = {0};
  int16_t xval, yval = 0x00;

  /* Read Acceleration */
  BSP_ACCELERO_GetXYZ(buffer);

  xval = buffer[0];
  yval = buffer[1];

  if((abs(xval))>(abs(yval)))
  {
    if(xval > ThresholdHigh)
    {
      /* LED5 On */
    	BSP_LED_On(LED5);
    	//blink_led_on(14);
    	timer_sleep(BLINK_ON_TICKS);
    }
    else if(xval < ThresholdLow)
    {
      /* LED4 On */
    	BSP_LED_On(LED4);
    	//blink_led_on(12);
    	timer_sleep(BLINK_ON_TICKS);
    }
    else
    {
    	timer_sleep(BLINK_ON_TICKS);
    }
  }
  else
  {
    if(yval < ThresholdLow)
    {
      /* LED6 On */
    	BSP_LED_On(LED6);
    	//blink_led_on(15);
    	timer_sleep(BLINK_ON_TICKS);
    }
    else if(yval > ThresholdHigh)
    {
      /* LED3 On */
    	BSP_LED_On(LED3);
    	//blink_led_on(13);
      timer_sleep(BLINK_ON_TICKS);
    }
    else
    {
    	timer_sleep(BLINK_ON_TICKS);
    }
  }

  trace_printf("xval = %d\t", xval);
  trace_printf("yval = %d	\n", yval);


  blink_led_off(12);
  blink_led_off(13);
  blink_led_off(14);
  blink_led_off(15);
}

void GYRO_MEMS_Test(void)
{
	trace_printf("\n\n\nGYRO_MEMS_Test\n");
	//trace_printf("\n\n\n\t\tHello!!!\n\t\tITS GYRO\n");
  /* Init Gyroscope MEMS */
  if(BSP_GYRO_Init() != HAL_OK)
  {
	 trace_printf("\n\n\n\t\tNOT OK\n\t\tHAL_OK\n");
    /* Initialization Error */
   // Error_Handler();
  }
	//trace_printf("\n\n\n\t\tYEAH\n\t\tGET\n");
  UserPressButton = 0;
  while(!UserPressButton)
  {
    GYRO_ReadAng();
  }
}

/**
  * @brief  Read Gyroscope Angular data.
  * @param  None
  * @retval None
  */
void GYRO_ReadAng(void)
{
  /* Gyroscope variables */
  float Buffer[3];
  float Xval, Yval = 0x00;

  //trace_printf("\n\n\n\t\tREAD\n\t\tANG\n");


  /* Read Gyroscope Angular data */
  BSP_GYRO_GetXYZ(Buffer);

  Xval = abs((Buffer[0]));
  Yval = abs((Buffer[1]));

  if(Xval>Yval)
  {
    if(Buffer[0] > 5000.0f)
    {
      /* LED5 On */
    	BSP_LED_On(LED5);
    	//blink_led_on(14);
    	timer_sleep(BLINK_ON_TICKS);
    	//HAL_Delay(10);
    }
    /*else*/ if(Buffer[0] < -5000.0f)
    {
      /* LED4 On */
    	BSP_LED_On(LED4);
    	//blink_led_on(12);
    	timer_sleep(BLINK_ON_TICKS);
      //BSP_LED_On(LED4);
      //HAL_Delay(10);
    }
    else
    {
    	timer_sleep(BLINK_ON_TICKS);
      //HAL_Delay(10);
    }
  }
  /*else*/
  {
    if(Buffer[1] < -5000.0f)
    {
      /* LED6 On */
      //BSP_LED_On(LED6);
    	BSP_LED_On(LED6);
    	//blink_led_on(15);
    	timer_sleep(BLINK_ON_TICKS);
      //HAL_Delay(10);
    }
    /*else*/ if(Buffer[1] > 5000.0f)
    {
      /* LED3 On */
    	BSP_LED_On(LED3);
    	//blink_led_on(13);
    	timer_sleep(BLINK_ON_TICKS);
      //HAL_Delay(10);
    }
    else
    {
    	timer_sleep(BLINK_ON_TICKS);
      //HAL_Delay(10);
    }
  }
  trace_printf("Xval = %g\t", Buffer[0]);
  trace_printf("Yval = %g\n", Buffer[1]);

  blink_led_off(12);
  blink_led_off(13);
  blink_led_off(14);
  blink_led_off(15);
}



/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin: Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	//trace_printf("\nHAL_GPIO_EXTI_Callback = 1\n");
  if (KEY_BUTTON_PIN == GPIO_Pin)
  {
    while (BSP_PB_GetState(BUTTON_KEY) != RESET);
    UserPressButton = 1;
   // trace_printf("\nUserPressButton = 1\n");
  }

  if(ACCELERO_INT1_PIN == GPIO_Pin)
  {
    if (PressCount == 1)
    {
      PauseResumeStatus = RESUME_STATUS;
      PressCount = 0;
    }
    else
    {
      PauseResumeStatus = PAUSE_STATUS;
      PressCount = 1;
    }
  }
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* Turn LED5 on */
  BSP_LED_On(LED5);
  while(1)
  {
  }
}


#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
