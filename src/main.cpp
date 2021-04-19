/**
  ******************************************************************************
  * @file    Multi/Examples/IOLink/IOM001_Example/Src/main.c 
  * @author  IPC Rennes
  * @version V1.0.0
  * @date    January 8th, 2018
  * @brief   This example shows how to use one STEVAL-IOM001V1 expansion board  
  * to evaluate L6360, an IO-Link communication master transceiver IC
  * The demo sequence starts when the user button is pressed.
  * Each time, the user button is pressed, the demo step is changed
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


/* Includes ------------------------------------------------------------------*/
#include "main.h"

/** @defgroup IOM001_Example
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
 // The struct of eeprom format
struct eeprom_format {
  volatile uint32_t filling_vol_goal;              //充填目標値
  volatile uint16_t filling_num_goal;              //充填目標回数
  volatile uint16_t calib_num_goal;                //キャリブレーション回数
  volatile uint32_t filling_vol_accum;             //累積充填量
  volatile uint32_t filling_num_accum;             //累積充填回数
  volatile uint32_t washing_vol_goal;              //洗浄供給量目標値
  volatile uint16_t washing_time_goal;              //洗浄量
  volatile uint16_t washing_num_goal;              //洗浄回数
  volatile uint16_t washing_time_interval;         //洗浄インターバル
  volatile uint32_t washing_vol_accum;             //累積洗浄量
  volatile uint32_t washing_num_accum;            //累積洗浄回数
  volatile uint32_t prestop_offset1;
  volatile uint32_t prestop_offset2;
  volatile uint8_t  ad_vol_cg_offset;
  volatile bool     calib_flag;                    //キャリブレーション済か否か
//  volatile uint8_t  calib_mean;                  　//キャリブレーション補正値
};
// initialize one struct
eeprom_format _eeprom = {100,     //充填目標値
                          50,     //充填目標回数
                           5,     //キャリブレーション回数
                        1200,     //累積充填量
                        1200,     //累積充填回数
                        7000,     //洗浄供給量目標値
                        1200,     //洗浄量
                        1111,     //洗浄回数
                        2222,     //洗浄インターバル
                        1234,     //累積洗浄回数
                        1222,     //洗浄インターバル
                        70,
                        5,
                        20,
                        true};    //キャリブレーション済か否か

volatile bool pulse_stop_flag = false;


/* Private define ------------------------------------------------------------*/
#define MAX_STEPS (10U)
#define DEBUG00   0
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#include <FD-XA1.h>
#include <GT_disp_IOlink001.h>


volatile bool automode_flg  = false;
volatile bool measure_now   = false;
//volatile bool ad_vol_flag   = false;


volatile int counter_min_RTC = 0;
volatile char counter_adc = 0;

volatile uint16_t disp_update_flag = 0;
volatile uint32_t rtc_update_flag = 0;




void RTCtimer();


void footSw_on(){
  if(digitalRead(FOOT_SW_PIN) == LOW){
    digitalWrite(SOLV_PIN_PRESSURE,HIGH);
    digitalWrite(SOLV_PIN_OPEN,HIGH);
    delay(500);
    digitalWrite(SOLV_PIN_OPEN,LOW);
    digitalWrite(SOLV_PIN_CLOSE,HIGH);
    delay(100);
    digitalWrite(SOLV_PIN_CLOSE,LOW);
    digitalWrite(SOLV_PIN_PRESSURE,LOW);
  }else{
    digitalWrite(SOLV_PIN_PRESSURE,LOW);
    digitalWrite(SOLV_PIN_OPEN,LOW);
  }
}

void RTCtimer(){
  if(counter_min_RTC < 60){
    counter_min_RTC ++;
  }else{
    RTCloop();
    counter_min_RTC = 1;
  }
}

void checkClockHandler(){
  RTCtimer();
  if(wash_flag == true){
  
  }
}

/*
void timer_5ms(){
  if(pulsecount_flag == true){
    if(counter_adc >= 6){
      counter_adc = 1;
      uint16_t ad_buff = analogRead(FD_ANALOG);
      if(ad_buff <= 422){
        ad_FD = 422 - ad_buff;
      }
    }else {
      counter_adc++;
    }
  ad_vol = ad_vol + ad_FD;
  }
}
*/

void timer_1ms(){
  if(ad_vol_flag == true ){
    uint16_t ad_buff = analogRead(FD_ANALOG);
    //Serial.println(ad_buff);
    if(ad_buff <= AD_THRESHOLD){
      ad_FD = AD_THRESHOLD - ad_buff;
    }
  ad_vol = ad_vol + ad_FD;
//  Serial.println(ad_vol);
  }
}

void setup() {
  // INPUT  Pins
  pinMode(FOOT_SW_PIN,INPUT_PULLUP);
  pinMode(EMG_ON,INPUT_PULLUP);
  pinMode(XA_OUT1,INPUT_PULLUP);
  pinMode(XA_OUT2,INPUT_PULLUP);
  // SOLV Pins
  pinMode(SOLV_PIN_PRESSURE,OUTPUT);
  digitalWrite(SOLV_PIN_PRESSURE,LOW);
  pinMode(SOLV_PIN_OPEN,OUTPUT);
  digitalWrite(SOLV_PIN_OPEN,LOW);
  pinMode(SOLV_PIN_CLOSE,OUTPUT);
  digitalWrite(SOLV_PIN_CLOSE,LOW);
  //  OUTPUT Pins for FD-XA1
  pinMode(FD_INPUT2,OUTPUT);
  digitalWrite(FD_INPUT2,LOW);

  Serial.begin(9600);
//  Serial.begin(38400);
  SD_initialize();
  RTCsetup();
  RTCloop();
  delay(200);
  gtUartSetup();
  setupFD();
  EEPROM.get(0x00,_eeprom);
  washing_num_now = _eeprom.washing_num_goal;
  if(analogRead(FD_ANALOG) <= SENSOR_CHK){
    //error_sensor = true;
  }
  //Serial.print(analogRead(FD_ANALOG));
  FlexiTimer2::set(1, timer_1ms); // call every 1ms "ticks"
//  FlexiTimer2::set(2, 1.0/10000, timer_1ms); // call every 1ms "ticks"
}

void loop() {
  if(disp_update_flag <= 1000){
    disp_update_flag ++;
  }else{
    if( (error_sd == true) || (error_sensor == true)  ){
      error_flag = true;
    }
//    if(analogRead(FD_ANALOG) <= 70){
//      error_sensor = true;
//    }
    gt_dispselect();
    disp_update_flag = 0;
//    Serial.println(ad_vol);
//    Serial.println(filling_vol_now);
  }

//rtc clock update
  if(pageNum == 1){
    if(rtc_update_flag <= 300000){
      rtc_update_flag ++;
    }else{
      RTCloop();
      rtc_update_flag = 0;
    }
  }

  //kito 2021.04.17 動作関数変更
  //vol_cal_mix();
  vol_cal_pulse();

  if(pulse_stop_flag == true){
    setupFD();
  }
  vol_cal_wash();
  if(washing_continue_flag == true){
    washing_num_now --;
    washing_continue_flag = false;
    if(washing_num_now > 0){
      for(int i=1; i <= _eeprom.washing_time_interval ; i++){
        delay(1000);
      }
      washing_start();
      pgch_flag = true;
    }
  }
}