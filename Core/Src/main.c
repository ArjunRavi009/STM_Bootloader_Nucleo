/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
CAN_HandleTypeDef hcan1;
CAN_HandleTypeDef hcan2;

CRC_HandleTypeDef hcrc;

UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USER CODE BEGIN PV */
#define D_UART   &huart3
#define C_UART   &huart2

#define CRC_VALIDATE_OK 		1U
#define CRC_VALIDATE_NOT_OK 	0U

#define BL_ACK 		0xA5
#define BL_NACK 	0x7F

uint8_t b1_rx_buffer[200];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_CAN1_Init(void);
static void MX_CAN2_Init(void);
static void MX_CRC_Init(void);
/* USER CODE BEGIN PFP */
static void printmsg(char *format,...);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static void bootloader_send_ACK(uint8_t followlen);
static void bootloader_send_NACK(void);
static uint8_t bootloader_Validate_Crc(uint8_t* PData, uint32_t datalen, uint32_t host_crc);
static uint8_t get_bootloader_version(void);
static void bootloader_uart_write_data(uint8_t *pBuffer,uint32_t len);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_CAN1_Init();
  MX_CAN2_Init();
  MX_CRC_Init();
  /* USER CODE BEGIN 2 */

  if ( HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
  {
	  /*Entering Boot Loader*/
	  printmsg("BL_DEBUG_MSG:Button is pressed .. going to BL mode\n\r");

	  /*Call BootLoader Handler function*/
	  bootloader_Uart_ReadData();
  }
  else
  {

	  printmsg("BL_DEBUG_MSG:Button is NOT pressed .. going to APPL mode\n\r");

	  /*Jump to Application Handler function*/
	  bootloader_Jump_UserAppl();
  }
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
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

/**
  * @brief CAN1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 16;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/**
  * @brief CAN2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_CAN2_Init(void)
{

  /* USER CODE BEGIN CAN2_Init 0 */

  /* USER CODE END CAN2_Init 0 */

  /* USER CODE BEGIN CAN2_Init 1 */

  /* USER CODE END CAN2_Init 1 */
  hcan2.Instance = CAN2;
  hcan2.Init.Prescaler = 16;
  hcan2.Init.Mode = CAN_MODE_NORMAL;
  hcan2.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan2.Init.TimeSeg1 = CAN_BS1_1TQ;
  hcan2.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan2.Init.TimeTriggeredMode = DISABLE;
  hcan2.Init.AutoBusOff = DISABLE;
  hcan2.Init.AutoWakeUp = DISABLE;
  hcan2.Init.AutoRetransmission = DISABLE;
  hcan2.Init.ReceiveFifoLocked = DISABLE;
  hcan2.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN2_Init 2 */

  /* USER CODE END CAN2_Init 2 */

}

/**
  * @brief CRC Initialization Function
  * @param None
  * @retval None
  */
static void MX_CRC_Init(void)
{

  /* USER CODE BEGIN CRC_Init 0 */

  /* USER CODE END CRC_Init 0 */

  /* USER CODE BEGIN CRC_Init 1 */

  /* USER CODE END CRC_Init 1 */
  hcrc.Instance = CRC;
  if (HAL_CRC_Init(&hcrc) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CRC_Init 2 */

  /* USER CODE END CRC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* Function to handle the BootLoader service requests */
void bootloader_Uart_ReadData(void)
{
	uint8_t rcv_len = 0;

	 printmsg("BL_DEBUG_MSG:Start polling for BOOTLOADER service requests\n\r");


	/* once enter to the CUSTOM BOOTLOADER, the control will be here until next RESET */
	while(1)
	{

		/*Reset the rx_buffer*/
		memset(b1_rx_buffer, 0, sizeof(b1_rx_buffer));
		HAL_UART_Receive(C_UART,b1_rx_buffer,1,HAL_MAX_DELAY);
		rcv_len = b1_rx_buffer[0];
		HAL_UART_Receive(C_UART, &b1_rx_buffer[1],rcv_len,HAL_MAX_DELAY);

		switch(b1_rx_buffer[1])
		{
			case BL_GET_VER:
				bootloaderHandle_Get_Ver_Cmd(b1_rx_buffer);

			case BL_GET_HELP:
				bootloaderHandle_Get_Help_Cmd();

			case BL_GET_CID:
				bootloaderHandle_Get_ChipId_Cmd();

			case BL_GET_RDP_STATUS:
				bootloaderHandle_Get_RDPStatus_Cmd();

		}

	}

}

/* Function to handle the jump to application from BootLoader */
void bootloader_Jump_UserAppl(void)
{

   //just a function pointer to hold the address of the reset handler of the user application.
    void (*app_reset_handler)(void);

    printmsg("BL_DEBUG_MSG:bootloader_jump_to_user_app\n\r");


    // 1. configure the MSP by reading the value from the base address of the sector 2
    uint32_t msp_value = *(volatile uint32_t *)FLASH_SECTOR2_BASE_ADDRESS;
    printmsg("BL_DEBUG_MSG:MSP value : %#x\n\r",msp_value);

    __disable_irq();

    SCB->VTOR = FLASH_SECTOR2_BASE_ADDRESS;

    printmsg("BL_DEBUG_MSG:VTOR Set value : %#x\n\r",SCB->VTOR);

    //This function comes from CMSIS.
    __set_MSP(msp_value);

    __enable_irq();

    /* 2. Now fetch the reset handler address of the user application
     * from the location FLASH_SECTOR2_BASE_ADDRESS+4
     */
    uint32_t resethandler_address = *(volatile uint32_t *) (FLASH_SECTOR2_BASE_ADDRESS + 4);

    app_reset_handler = (void*) resethandler_address;

    printmsg("BL_DEBUG_MSG: app reset handler addr : %#x\n\r",app_reset_handler);

    //3. jump to reset handler of the user application
    app_reset_handler();

}
/* prints formatted string to console over UART */
 void printmsg(char *format,...)
 {
	char str[80];
#ifdef BL_DEBUG_MSG
	/*Extract the the argument list using VA apis */
	va_list args;
	va_start(args, format);
	vsnprintf(str,sizeof(str),format,args);
	va_end(args);
	HAL_UART_Transmit(C_UART,(uint8_t *)str, strlen(str),HAL_MAX_DELAY);
#endif
 }


 /*****BOOTLOADER Service function implementations*****/

 /* BOOTLOADER service to read the Version Number of the BOOTLOADER SW */
void bootloaderHandle_Get_Ver_Cmd(uint8_t *rx_buffer)
{
	uint8_t bl_version;
	uint32_t commandpacketlength;
	uint32_t host_crc;

	commandpacketlength = rx_buffer[0]+1;
	host_crc = *((uint32_t*) (rx_buffer+commandpacketlength-4));

	/*DO the CRC check ::
	 * check to transmit ACK / NACK */
	if ( CRC_VALIDATE_OK==
		bootloader_Validate_Crc(rx_buffer, commandpacketlength-4,host_crc))
	{
		 printmsg("BL_DEBUG_MSG:checksum success !!\n");

		/*0xA5*/
		bootloader_send_ACK(1);

        bl_version=get_bootloader_version();
        printmsg("BL_DEBUG_MSG:BL_VER : %d %#x\n",bl_version,bl_version);
        bootloader_uart_write_data(&bl_version,1);
	}
	else
	{
		/*0x7F*/
		bootloader_send_NACK();
	}

}

void bootloaderHandle_Get_Help_Cmd(void){}

void bootloaderHandle_Get_ChipId_Cmd(void){}

void bootloaderHandle_Get_RDPStatus_Cmd(void){}


/* Static functions */
static void bootloader_send_ACK(uint8_t followlen)
{
	uint8_t ACKBuff[2];

	ACKBuff[0] = BL_ACK;
	ACKBuff[1] = followlen;
	HAL_UART_Transmit(C_UART,ACKBuff,2,HAL_MAX_DELAY);

}
static void bootloader_send_NACK(void)
{
	uint8_t NACKBuff;

	NACKBuff = BL_NACK;
	HAL_UART_Transmit(C_UART,&NACKBuff,1,HAL_MAX_DELAY);

}
static uint8_t bootloader_Validate_Crc(uint8_t *PData, uint32_t datalen, uint32_t host_crc)
{
	uint8_t retVal = CRC_VALIDATE_NOT_OK;
	uint32_t calc_crc = 0xff;
	uint32_t i_data;

	for(uint8_t i=0; i< datalen; i++)
	{
		i_data = PData[i];
		calc_crc = HAL_CRC_Accumulate(&hcrc,&i_data,1);
	}

	 /* Reset CRC Calculation Unit */
	__HAL_CRC_DR_RESET(&hcrc);

	if(calc_crc == host_crc)
	{
		retVal = CRC_VALIDATE_OK;
	}

	return retVal;
}

static uint8_t get_bootloader_version(void)
{

	uint8_t retval;
	retval = BL_VERSION_INFO;
	return retval;
}

static void bootloader_uart_write_data(uint8_t *pBuffer,uint32_t len)
{
    /*you can replace the below ST's USART driver API call with your MCUs driver API call */
	HAL_UART_Transmit(C_UART,pBuffer,len,HAL_MAX_DELAY);

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
