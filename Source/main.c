/*!
 * @file        main.c
 *
 * @brief       Main program body
 *
 * @version     V1.0.1
 *
 * @date        2021-07-01
 *
 * @attention
 *
 *  Copyright (C) 2020-2022 Geehy Semiconductor
 *
 *  You may not use this file except in compliance with the
 *  GEEHY COPYRIGHT NOTICE (GEEHY SOFTWARE PACKAGE LICENSE).
 *
 *  The program is only for reference, which is distributed in the hope
 *  that it will be usefull and instructional for customers to develop
 *  their software. Unless required by applicable law or agreed to in
 *  writing, the program is distributed on an "AS IS" BASIS, WITHOUT
 *  ANY WARRANTY OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the GEEHY SOFTWARE PACKAGE LICENSE for the governing permissions
 *  and limitations under the License.
 */

#include "Board.h"
#include "stdio.h"
#include "apm32f0xx_fmc.h"
#include "apm32f0xx_iwdt.h"

/** printf function configs to USART2*/
#define DEBUG_USART  USART2

/** Delay */
void Delay(void);

/*!
 * @brief       Main program
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
int main(void)
{
	uint32_t i;
    uint32_t addr ;

    APM_MINI_COMInit(COM2);
	
    printf("test\r\n");
	
    
//	if ( ((*(uint32_t *)0x1FFFF804) & 0xff)!= 0X55)
//	{
//		printf("the 0x1FFFF804 != 0XAA\r\n");
//		
//		/** Unlock Flash for Clear or Write*/
//	    FMC_Unlock();
//	    FMC_UnlockOptionByte(); 
//		FMC_EraseOptionByte();
//	    FMC_ProgramOptionByteData(0x1FFFF804,0x56);		
//		
//		//强制选项字节重装载，生成系统复位  read protection level 1
//		//FMC_LaunchOptionByte();  
//        /** Lock Flash*/
//        FMC_Lock();
//		FMC_LockOptionByte();		
//	}
//	else
//	{
//	   printf("the 0x1FFFF804 = 0XAA\r\n");
//	}
	
    
	if( (FMC_ReadOptionByteUser() & 0x01) == 1)
	{
		  FMC_STATE_T state;
		  FMC_Unlock();
		 FMC_UnlockOptionByte(); 
		
		//FMC_EraseOptionByte();
		
	    state = FMC_WriteOptionByteUser(FMC_OB_IWDT_HW);
		
		
		printf("the hardware watchdog state is %d\r\n",state);
		
		FMC_LaunchOptionByte();  
		  FMC_Lock();
		FMC_LockOptionByte();	
		
		printf("the hardware watchdog \r\n");
	}
	    /** clear IWDTRST Flag*/
    if(RCM_ReadStatusFlag(RCM_FLAG_IWDTRST) != RESET)
    {
        RCM_ClearStatusFlag();
    }
    /** set IWDT Write Access */
    IWDT_EnableWriteAccess();

    /** set IWDT Divider*/
    IWDT_ConfigDivider(IWDT_DIV_64);

    /** set IWDT Reloader*/
    IWDT_ConfigReload(40000/32);

    /** Refresh*/
    IWDT_Refresh();
	
    for (;;)
    {
        i ++;
        if (i > 1000000)
		{
		    i = 0;
			printf("the systerm run \r\n");  
		}			
		// IWDT_Refresh();
    }
}

/*!
 * @brief       Delay
 *
 * @param       None
 *
 * @retval      None
 *
 * @note
 */
void Delay(void)
{
    volatile uint32_t delay = 0xff5;

    while (delay--);
}
/*!
* @brief       Redirect C Library function printf to serial port.
*              After Redirection, you can use printf function.
*
* @param       ch:  The characters that need to be send.
*
* @param       *f:  pointer to a FILE that can recording all information
*              needed to control a stream
*
* @retval      The characters that need to be send.
*
* @note
*/
int fputc(int ch, FILE* f)
{
    /** send a byte of data to the serial port */
    USART_TxData(DEBUG_USART, (uint8_t)ch);

    /** wait for the data to be send  */
    while (USART_ReadStatusFlag(DEBUG_USART, USART_FLAG_TXBE) == RESET);

    return (ch);
}
