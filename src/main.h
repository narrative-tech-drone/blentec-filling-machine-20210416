/**
  ******************************************************************************
  * @file    Multi/Examples/IOLink/IOM001_Example/Inc/main.h
  * @author  IPC Rennes
  * @version V1.0.0
  * @date    January 8th, 2018
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2018 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MAIN_H
#define MAIN_H

//????????????
#define GT_RESET    7  //RESET
#define GT_DTR      6  //DTR
#define GT_DSR      5  //DSR
#define GT_TRDY     4   //TRDY

#define FOOT_SW_PIN A4
#define SOLV_PIN_PRESSURE    A3
#define SOLV_PIN_OPEN   A2
#define SOLV_PIN_CLOSE   A1

#define IOLINK_TXQ    8
#define IOLINK_TXC    18
#define IOLINK_RX     19

#define FD_ANALOG     A5
#define FD_INPUT2     A0

#define XA_OUT1       3
#define XA_OUT2       2

#define EMG_ON        14        

#define AD_CALIB      1970 //2????????????????????
//20.00L/min 159-162
//15.00L/min 324-327
//10.00L/min 490-492
// 5.00L/min 655-657
// 0.00L/min 819-822
// sensor error 139-141
#define SENSOR_CHK    80
#define AD_THRESHOLD  822


/* Includes ------------------------------------------------------------------*/
#include <Arduino.h>
#include "eeprom_update.h"
#include <FlexiTimer2.h>
#include <SPI.h>
#include <RTCada.h>
#include <SDada.h>


/* Exported types ------------------------------------------------------------*/
//??????
//volatile uint16_t filling_vol_goal;        //???????????????
//volatile uint16_t filling_num_goal;        //??????????????????
volatile uint32_t filling_vol_now;        //??????????????? ??????
volatile uint16_t filling_num_now;        //??????????????? ??????
volatile uint16_t washing_vol_now;        //??????????????? ??????
//volatile uint16_t washing_num_now;        //??????????????? ??????
//volatile uint16_t calib_num_goal;           //?????????????????????????????????
//volatile uint32_t filling_vol_accum;        //???????????????
//volatile uint32_t filling_num_accum;        //??????????????????
volatile uint16_t filling_vol_history;        //??????????????? ?????????
//volatile uint16_t washing_time_goal;        //?????????
//volatile uint16_t washing_num_goal;        //????????????
//volatile uint16_t washing_time_interval;        //????????????????????????
//volatile uint32_t washing_vol_accum;        //???????????????
//volatile uint32_t washing_num_accum;        //??????????????????
volatile uint16_t solv_open_min;            // ????????????????????????
//volatile int      calib_mean;             //????????????????????????????????????
//volatile bool     calib_flag = false;       //???????????????????????????????????????

volatile uint8_t  pageNum = 1;
volatile bool     solvON = false;       //SOLV-ON?????????
volatile int      sw_long = 0;              //?????????????????????
volatile bool     conFilling_flag = false;                    //???????????????????????????
volatile bool     pgch_flag = true;                //?????????????????????
//volatile bool     datach_flag = false;          //????????????????????????
volatile bool     wash_flag = false;            //???????????????

volatile bool error_flag = false;
volatile bool error_sensor = false;

volatile int     filling_vol_input = 0;
volatile bool     pulsecount_flag   = false;
volatile bool     lowpressure_flag = false;
volatile bool     washing_flag = false;
volatile bool     washing_continue_flag = false;
volatile bool     lp_only_flag = false;
volatile bool     low_reso_flag = true; //true????????????1ml
volatile bool     lock_flag = false;

volatile uint16_t washing_num_now;


/* Exported constants --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void SystemClock_Config(void);
void Error_Handler(uint16_t error);

#endif /* MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/


