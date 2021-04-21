/*
Keyence FD-XA1
*/

volatile uint32_t pulseP = 0;
volatile uint32_t pulseN = 0;
//volatile uint16_t prestop_offset1 = 70;
//volatile uint16_t prestop_offset2 = 5;
volatile uint16_t ad_FD = 0;
volatile uint32_t ad_vol = 0;
volatile bool ad_vol_flag = false;
volatile uint8_t ad_vol_cg_offset = 10;

volatile bool test_pulse_flag = false;

void valve_open_Hp()
{
  digitalWrite(SOLV_PIN_PRESSURE, LOW);
  digitalWrite(SOLV_PIN_OPEN, HIGH);
}

void valve_open_Lp()
{
  digitalWrite(SOLV_PIN_PRESSURE, HIGH);
  digitalWrite(SOLV_PIN_OPEN, HIGH);
}

void valve_close_Hp()
{
  digitalWrite(SOLV_PIN_OPEN, LOW);
  delay(200);
}

void valve_close_Lp()
{
  //
  digitalWrite(SOLV_PIN_OPEN, LOW);
  digitalWrite(SOLV_PIN_CLOSE, HIGH);
  delay(200);
  digitalWrite(SOLV_PIN_CLOSE, LOW);
}

void wash_val_close()
{
  valve_close_Hp();
  delay(200);
  digitalWrite(SOLV_PIN_PRESSURE, LOW);
  delay(350);
  washing_flag = false;
  //ad_vol_flag = false;
  FlexiTimer2::stop();
}

//calcurate washing volume
void vol_cal_wash()
{
  if (washing_flag == false)
  {
    washing_vol_now = 5 * (pulseP - pulseN) + ad_vol / AD_CALIB;
    return;
  }
  uint16_t vol = 5 * (pulseP - pulseN) + ad_vol / AD_CALIB;

  if (_eeprom.washing_vol_goal <= vol)
  {
    wash_val_close();
    _eeprom.washing_vol_accum = _eeprom.washing_vol_accum + vol;
    _eeprom.washing_num_accum++;
    EEPROM.put(0x00, _eeprom);
    pgch_flag = true;
    washing_continue_flag = true;
  }
}

//calcurate washing volume
void vol_cal_wash_new()
{
  if (washing_flag == false)
  {
    washing_vol_now = 5 * (pulseP - pulseN);
    return;
  }

  uint16_t vol; //流量一時値
  if (low_reso_flag == true){
    vol = 5 * (pulseP - pulseN);
  }
  else{
    vol = (pulseP - pulseN) / 2;
  }

  Serial.print(vol);

  if (_eeprom.washing_vol_goal <= vol)
  {
    wash_val_close();
    _eeprom.washing_vol_accum = _eeprom.washing_vol_accum + vol;
    _eeprom.washing_num_accum++;
    EEPROM.put(0x00, _eeprom);
    pgch_flag = true;
    washing_continue_flag = true;
  }
}

//not used
void vol_cal_mix()
{
  //パルスで計算しないときはAD値を返す
  if (pulsecount_flag == false)
  {
    filling_vol_now = 5 * (pulseP - pulseN) + ad_vol / AD_CALIB;
    return;
  }

  uint16_t vol = 5 * (pulseP - pulseN) + ad_vol / AD_CALIB;

  //目標値以下の時は計算を行う
  if ((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol)
  {
    valve_close_Lp();
    Serial.println("closeLP");
    Serial.println(vol);
    delay(200);
    digitalWrite(SOLV_PIN_PRESSURE, LOW);
    delay(350);
    pulsecount_flag = false;
    lowpressure_flag = false;
    ad_vol_flag = false;
    FlexiTimer2::stop();
    Serial.println(vol);
    _eeprom.filling_vol_accum = _eeprom.filling_vol_accum + filling_vol_now;
    SD_logging_data(filling_vol_now);
    _eeprom.filling_num_accum++;
    EEPROM.put(0x00, _eeprom);
    pulse_stop_flag = true;
    delay(450);
    pgch_flag = true;
  }
  else if (((_eeprom.filling_vol_goal - _eeprom.prestop_offset1) <= vol) && (lowpressure_flag == false))
  {
    lowpressure_flag = true;
    Serial.println("closeHP");
    valve_close_Hp();
    delay(100);
    Serial.println("openLP");
    valve_open_Lp();
  }
  filling_vol_now = vol;
}

//not used
void vol_cal()
{
  if (pulsecount_flag == false)
  {
    filling_vol_now = ad_vol / AD_CALIB;
    return;
  }
  uint16_t vol = ad_vol / AD_CALIB;
  if ((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol)
  {
    valve_close_Lp();
    delay(200);
    digitalWrite(SOLV_PIN_PRESSURE, LOW);
    delay(350);
    pulsecount_flag = false;
    lowpressure_flag = false;
  }
  else if (((_eeprom.filling_vol_goal - _eeprom.prestop_offset1) <= vol) && (lowpressure_flag == false))
  {
    lowpressure_flag = true;
    valve_close_Hp();
    delay(100);
    valve_open_Lp();
  }
  filling_vol_now = vol;
}

//not used
void vol_cal_ad()
{
  if (pulsecount_flag == false)
  {
    filling_vol_now = ad_vol / AD_CALIB;
    return;
  }
  uint16_t vol = ad_vol / AD_CALIB;
  if ((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol)
  {
    valve_close_Lp();
    delay(200);
    digitalWrite(SOLV_PIN_PRESSURE, LOW);
    delay(350);
    pulsecount_flag = false;
    lowpressure_flag = false;
    Serial.println(vol);
  }
  else if (((_eeprom.filling_vol_goal - _eeprom.prestop_offset1) <= vol) && (lowpressure_flag == false))
  {
    lowpressure_flag = true;
    valve_close_Hp();
    delay(100);
    valve_open_Lp();
  }
  filling_vol_now = vol;
}

//calcurate filling volume
void vol_cal_pulse()
{
  uint16_t vol; //流量一時値
  if (low_reso_flag == true){
    vol = (pulseP - pulseN) * 5;
  }else{
    vol = (pulseP - pulseN) / 2;
  }
  //バルブを操作していないとき（充填していないとき）は現在値を計算するだけ
  if (pulsecount_flag == false)
  {
    filling_vol_now = vol;
    return;
  }

  if (lp_only_flag == true){
    //低圧の時
    if ((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol){
      valve_close_Lp();
      //kito 追加
      Serial.println("closeLP");
      Serial.println(vol);
      //kito ここまで
      delay(200);
      digitalWrite(SOLV_PIN_PRESSURE, LOW);
      delay(350);
      lowpressure_flag = false;
      //kito 追加
      FlexiTimer2::stop();
      Serial.println(vol);
      _eeprom.filling_vol_accum = _eeprom.filling_vol_accum + filling_vol_now / 10;
      SD_logging_data(filling_vol_now);
      _eeprom.filling_num_accum++;
      EEPROM.put(0x00, _eeprom);
      pulsecount_flag = false;
      pulse_stop_flag = true;
      delay(450);
      pgch_flag = true;
      //kito ここまで
    } //高圧の時
    else if(lowpressure_flag == false){
      lowpressure_flag = true;
      valve_close_Hp();
      Serial.println("closeHP");
      delay(100);
      valve_open_Lp();
      Serial.println("openLP");
    }
  }
  else{
    //低圧の時
    if ((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol){
      valve_close_Lp();
      //kito 追加
      Serial.println("closeLP");
      Serial.println(vol);
      //kito ここまで
      delay(200);
      digitalWrite(SOLV_PIN_PRESSURE, LOW);
      delay(350);
      lowpressure_flag = false;
      //kito 追加
      FlexiTimer2::stop();
      Serial.println(vol);
      _eeprom.filling_vol_accum = _eeprom.filling_vol_accum + filling_vol_now / 10;
      SD_logging_data(filling_vol_now);
      _eeprom.filling_num_accum++;
      EEPROM.put(0x00, _eeprom);
      pulsecount_flag = false;
      pulse_stop_flag = true;
      delay(450);
      pgch_flag = true;
      //kito ここまで
    } //高圧の時
    else if (((_eeprom.filling_vol_goal - _eeprom.prestop_offset1) <= vol) && (lowpressure_flag == false)){
      lowpressure_flag = true;
      valve_close_Hp();
      Serial.println("closeHP");
      delay(100);
      valve_open_Lp();
      Serial.println("openLP");
    }
  }

  filling_vol_now = vol;
}

//not used New
void vol_cal_pulse_old()
{

  uint16_t vol; //流量一時値
  if (low_reso_flag == true)
  {
    vol = (pulseP - pulseN) * 5;
  }
  else
  {
    vol = (pulseP - pulseN) / 2;
  }

  //バルブを操作していないとき（充填していないとき）は現在値を計算するだけ
  if (pulsecount_flag == false)
  {
    filling_vol_now = vol;
    return;
  }

  //低圧の時
  if ((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol)
  {
    valve_close_Lp();
    //kito 追加
    Serial.println("closeLP");
//    Serial.println(vol);
    //kito ここまで
    delay(200);
    digitalWrite(SOLV_PIN_PRESSURE, LOW);
    delay(350);
    pulsecount_flag = false;
    lowpressure_flag = false;
    //kito 追加
    FlexiTimer2::stop();
//    Serial.println(vol);
    _eeprom.filling_vol_accum = _eeprom.filling_vol_accum + filling_vol_now / 10;
    SD_logging_data(filling_vol_now);
    _eeprom.filling_num_accum++;
    EEPROM.put(0x00, _eeprom);
    pulse_stop_flag = true;
    delay(450);
    pgch_flag = true;
    //kito ここまで
  } //高圧の時
  else if (((_eeprom.filling_vol_goal - _eeprom.prestop_offset1) <= vol) && (lowpressure_flag == false))
  {
    lowpressure_flag = true;
    valve_close_Hp();
    Serial.println("closeHP");
    delay(100);
    valve_open_Lp();
    Serial.println("openLP");
  }
  filling_vol_now = vol;
}

//not used old
/*
void vol_cal_pulse(){
  if(pulsecount_flag == false){
    filling_vol_now = 5 * (pulseP - pulseN);
    return;
  }
  uint16_t vol = 5 * (pulseP - pulseN);
  if((_eeprom.filling_vol_goal - _eeprom.prestop_offset2) <= vol){
    valve_close_Lp();
    delay(200);
    digitalWrite(SOLV_PIN_PRESSURE,LOW);
    delay(350);
    pulsecount_flag = false;
    lowpressure_flag = false;
    Serial.println(vol);
  }else if( ((_eeprom.filling_vol_goal - _eeprom.prestop_offset1) <= vol) && (lowpressure_flag == false) ){
    lowpressure_flag = true;
    valve_close_Hp();
    delay(100);
    valve_open_Lp();
  }
  filling_vol_now = vol;
}
*/

//new
void Ppulse()
{
  if (pulsecount_flag == false)
  {
    return;
  }

  if (pulsecount_flag == true)
  {
    pulseP++;
    Serial.print("P");
    Serial.println(pulseP);
  }
  else if (washing_flag == true)
  {
    pulseP++;
  }
}

#if 0 //old
void Ppulse() {
  if( (ad_vol_flag == true) && (pulsecount_flag == false) ){
    return;
  }else if(pulsecount_flag == true){
    if(pulseP == 0){
      if(ad_vol_flag == false){
        pulseP = 1;
      }else{
        ad_vol_flag = false;
      }
    }else{
      pulseP ++;
    }
    if((_eeprom.filling_vol_goal - _eeprom.prestop_offset2 - ad_vol_cg_offset) <= filling_vol_now){
      detachInterrupt(digitalPinToInterrupt(XA_OUT1));
      detachInterrupt(digitalPinToInterrupt(XA_OUT2));
      ad_vol_flag = true;
    }
    Serial.println("P");
//    int i = vol_cal();
  }else if(washing_flag == true){
    if(pulseP == 0){
      ad_vol_flag = false;
    }
    pulseP ++;
    if((_eeprom.washing_vol_goal - ad_vol_cg_offset) <= washing_vol_now){
      ad_vol_flag = true;
    }
  }
}
#endif

void Ppulse_test()
{
  if (test_pulse_flag == false)
  {
    Serial.println("P");
    test_pulse_flag = true;
    ad_vol_flag = true;
    ad_vol = 0;
    FlexiTimer2::start();
  }
  else
  {
    FlexiTimer2::stop();
    test_pulse_flag = false;
    Serial.println(ad_vol);
  }
}

void Npulse()
{
  if (pulsecount_flag == true)
  {
    pulseN++;
    Serial.println("N");
    //    int i = vol_cal();
  }
  else
  {
    //    pulseN = 0;
  }
}

void setupFD()
{
  attachInterrupt(digitalPinToInterrupt(XA_OUT1), Ppulse, CHANGE);
  attachInterrupt(digitalPinToInterrupt(XA_OUT2), Npulse, CHANGE);
}
