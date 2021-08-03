#include <Noritake_GT_uart.h>
#include <EEPROM.h> 

#define L_CUSTOM_SW gt_CustomSwitch(  0, 430, 280,  50)
#define R_CUSTOM_SW gt_CustomSwitch(520, 430, 280,  50)
#define M_CUSTOM_SW gt_CustomSwitch(300, 430, 280,  50)
#define MENU_SW     gt_CustomSwitch(650,   0, 150,  50)
#define LOCK_SW     gt_CustomSwitch(  0,   0, 150,  50)

#define DISP_NUM_W5   230 
#define DISP_NUM_H5   110
#define DISP_NUM_W7   330 
#define DISP_NUM_H7   110

#define X_PAGE2_SW1   110
#define Y_PAGE2_SW1   73
#define X_PAGE2_SW2   110
#define Y_PAGE2_SW2   203
#define X_PAGE2_SW3   500
#define Y_PAGE2_SW3   100
#define PAGE2_SW3_H   80
#define X_PAGE2_SW4   180
#define Y_PAGE2_SW4   435
#define PAGE2_SW4_H   40
#define PAGE2_SW4_W   40
#define X_PAGE2_SW5   400
#define Y_PAGE2_SW5   435
#define PAGE2_SW5_H   40
#define PAGE2_SW5_W   40

#define X_PAGE3_SW1    50
#define Y_PAGE3_SW1    95
#define X_PAGE3_SW2    50
#define Y_PAGE3_SW2   275

#define X_PAGE4_SW1    50
#define Y_PAGE4_SW1    95
#define X_PAGE4_SW2    50
#define Y_PAGE4_SW2   275
#define X_PAGE4_SW3   468
#define Y_PAGE4_SW3    90

#define X_PAGE5_SW1    50
#define Y_PAGE5_SW1    95
#define X_PAGE5_SW2    50
#define Y_PAGE5_SW2   275

#define X_PAGE6_SW1    50
#define Y_PAGE6_SW1    95
#define X_PAGE6_SW2   450
#define Y_PAGE6_SW2    95

#define X_PAGE7_SW1    50
#define Y_PAGE7_SW1    95
#define X_PAGE7_SW2    50
#define Y_PAGE7_SW2   275


uint32_t gt_keyanddisp5(int sw_x, int sw_y);
void gt_dispnumbox7(uint32_t inputdata , int sw_x, int sw_y);
void gt_dispselect();
void stop_filling();
void gt_clockdisp(int sw_x, int sw_y);
void gt_page00();
void gt_page01();
void gt_page02();
void gt_page13();
void gt_page04();
void gt_page05();
void gt_page06();
void gt_page07();
void gt_page08();

void lock_unlock();

bool emg_on_state = false;

void lock_unlock(){
    if(lock_flag == true){
        if(gt_keyanddisp5(0, 430) == 1111){
            lock_flag = false;
        }
    }else{
        lock_flag = true;
    }
}

void gt_dispselect(){
    if(digitalRead(EMG_ON) == LOW){
        pageNum = 8;
        emg_on_state = true;
    }else{
        if(emg_on_state == true){
//            pageNum = 1;
//            emg_on_state = false;
//      Serial.print("HIGH");
        }
    }
    switch(pageNum){
        case 0:
            //gt_page00();
            break;
        case 1:
            gt_page01();            //MENU -- キャリブレーションの済か否かで2パターンあり
            break;
        case 2:
            gt_page02();            //キャリブレーションモード
            break;
        case 3:
            gt_page06();            //充填モード
            break;
        case 4:
            gt_page04();
            break;
        case 5:
            gt_page05();          //キャリブレーションモード　充填
            break;
        case 6:
            gt_page06();          //充填モード　連続
            break;
        case 7:
            gt_page07();            //累積・エラー表示画面
            break;
        case 8:
            gt_page08();            //非常停止画面
            break;
        case 13:
            gt_page13();
    }
}


//MENU -- キャリブレーション済か否かで2パターンあり*****************************************************************
void gt_page01(){
    if(pgch_flag == true){
    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(0, 5);              //ch=0,sn=5
    gt_CustomSwitch( 40, 100, 300, 110);    //SW1(キャリブレーション)
    gt_CustomSwitch(460, 100, 300, 110);    //SW2(充填)
    gt_CustomSwitch( 40, 235, 300, 110);    //SW3(洗浄)
    gt_CustomSwitch(460, 235, 300, 110);    //SW4(積算、エラー確認)
    gt_CustomSwitch( 40, 370, 300, 110);    //SW5(手動)

    gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
    gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
    if(_eeprom.calib_flag == false){
        if(error_flag == true){
            gt_CopyFrom(IMAGE_ADDR_PAGE1_4, 800, 800, 480, 0x91);
        }else{
            gt_CopyFrom(IMAGE_ADDR_PAGE1_2, 800, 800, 480, 0x91);
        }
    }else{
        if(error_flag == true){
            gt_CopyFrom(IMAGE_ADDR_PAGE1_4, 800, 800, 480, 0x91);
        }else{
            gt_CopyFrom(IMAGE_ADDR_PAGE1_2, 800, 800, 480, 0x91);
        }
    }

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    pgch_flag = false;
    gt_clockdisp(520, 0);
    }


    //受信バッファが3になったらデータ読込
    if (Serial2.available() < 3) {
        gt_clockdisp(520, 0);
    }else{
        incomingByte1 = Serial2.read();  //ヘッダ(=10h)
        incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
        incomingByte3 = Serial2.read();  //スイッチ番号
        if (incomingByte2 == 0x31) {
            switch (incomingByte3) {
                case 1:
                    pageNum = 2;
                    digitalWrite(SOLV_PIN_PRESSURE,LOW);
                    solvON = false;
                    pgch_flag = true;
                    break;
                case 2:
                    pageNum = 3;
                    digitalWrite(SOLV_PIN_PRESSURE,LOW);
                    solvON = false;
                    pgch_flag = true;
                    break;
                case 3:
                    pageNum = 4;
                    digitalWrite(SOLV_PIN_PRESSURE,LOW);
                    solvON = false;
                    pgch_flag = true;
                    break;
                case 4:
                    pageNum = 7;
                    digitalWrite(SOLV_PIN_PRESSURE,LOW);
                    solvON = false;
                    pgch_flag = true;
                    break;
                case 5:
                    pageNum = 13;
                    digitalWrite(SOLV_PIN_PRESSURE,LOW);
                    solvON = false;
                    pgch_flag = true;
                    break;
            }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
        }
    }
}

void start_fill(){
    if((digitalRead(FOOT_SW_PIN) == LOW) && (pulsecount_flag == false)){
            digitalWrite(FD_INPUT2,HIGH);
            delay(20);
            valve_open_Hp();
            if((pageNum == 6) || (pageNum == 3)){
                if(filling_num_now > 0){
                    filling_num_now--;
                }
            }
            //FlexiTimer2::start();
            pulseP    = 0;
            pulseN    = 0;
            ad_vol = 0;
            pulsecount_flag = true;

            //ad_vol_flag = true;  //パルス　アナログミックス時のみ使用
            
            digitalWrite(FD_INPUT2,LOW);
    }
}

//キャリブレーションモード****************************************************************************************
void gt_page02(){
    if(pulsecount_flag == true){
     //入力値初期表示(0)
        gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
        gt_dispdecimal(filling_vol_now, X_PAGE2_SW2 + 30, (Y_PAGE2_SW2 + 130) +5); 
    }

    if(pgch_flag == true){
        //タッチパネル制御モード定義(カスタムスイッチモード)
        gt_CustomSwitchMode(0, 8);              //ch=0,sn=5
        gt_CustomSwitch( X_PAGE2_SW1, Y_PAGE2_SW1, DISP_NUM_W7, DISP_NUM_H7);    //1 低速切り替え目標値
        gt_CustomSwitch( X_PAGE2_SW2, Y_PAGE2_SW2, DISP_NUM_W7, DISP_NUM_H7);    //2 停止目標値
        gt_CustomSwitch( X_PAGE2_SW3, Y_PAGE2_SW3, DISP_NUM_W5, PAGE2_SW3_H);    //3 充填目標値
        gt_CustomSwitch( X_PAGE2_SW4, Y_PAGE2_SW4, PAGE2_SW4_W, PAGE2_SW4_H);    //4 低速のみ供給
        gt_CustomSwitch( X_PAGE2_SW5, Y_PAGE2_SW5, PAGE2_SW5_W, PAGE2_SW5_H);    //5 分解能選択
        R_CUSTOM_SW;                            //6   累積リセット
        LOCK_SW;                                //7   LOCK
        MENU_SW;                                //8   MENU
        

        gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
        gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

        //初期画面(非表示エリアに表示してから表示エリアにコピーする)
        //背景描画
        gt_setCursor(800, 0);
        if(lock_flag == true){
            gt_CopyFrom(IMAGE_ADDR_PAGE5A, 800, 800, 480, 0x91);
        }else{
            gt_CopyFrom(IMAGE_ADDR_PAGE5, 800, 800, 480, 0x91);
        }
 

        //数値表示
        //gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定
        gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
        //gt_dispnum5(_eeprom.prestop_offset1, X_PAGE2_SW1 + 30 + 800, Y_PAGE2_SW1 +5);          //入力値初期表示(0)
        gt_dispdecimal(_eeprom.prestop_offset1, X_PAGE2_SW1 + 30 + 800, Y_PAGE2_SW1 +5);          //入力値初期表示(0)
        //gt_dispnum5(_eeprom.prestop_offset2, X_PAGE2_SW2 + 30 + 800, Y_PAGE2_SW2 +5);         //入力値初期表示(0)
        gt_dispdecimal(_eeprom.prestop_offset2, X_PAGE2_SW2 + 30 + 800, Y_PAGE2_SW2 +5);         //入力値初期表示(0)
        gt_dispdecimal(filling_vol_now, X_PAGE2_SW2 + 30 + 800, (Y_PAGE2_SW2 + 130) +5);         //入力値初期表示(0)

        gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
        gt_dispdecimal(_eeprom.filling_vol_goal, 500 + 800, 90);          //入力値初期表示(0)
        gt_dispnum7(_eeprom.filling_vol_accum, 500 + 800, 210);         //入力値初期表示(0)
        gt_dispnum7(_eeprom.filling_num_accum, 500 + 800, 330);         //入力値初期表示(0)

        //非表示エリアから表示エリアにコピー
        gt_setCursor(0, 0);
        gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

        //アウトラインフォント設定
        gt_OutlineFontSize(50, 0, 0, 0);               //アウトラインフォントサイズ指定
        gt_OutlineFontTypeSelect(0);    //アウトライン フォント タイプ選択(日本語)
        gt_fontsize(00);                //フォントサイズ選択(アウトラインフォント)

        if(_eeprom.calib_flag == true){
            //gt_setCursor(300, 420);
            //Serial2.print("fix");
        }

        
        //文字表示
        //低速のみ供給設定表示
        gt_setCursor(X_PAGE2_SW4, Y_PAGE2_SW4-10);
        gt_OutlineFontSize(50, 0, 0, 0);               //アウトラインフォントサイズ指定
        if(lp_only_flag == false){
            Serial2.print("OFF");
        }else{
            Serial2.print("ON");
        }

        //分解の表示
        gt_setCursor(X_PAGE2_SW5, Y_PAGE2_SW5-10);
        if(low_reso_flag == true){
            Serial2.print("1.0");
        }else{
            Serial2.print("0.1");
        }
        pgch_flag = false;
    }


    //受信バッファが3になったらデータ読込
    if(Serial2.available() < 3) {
        start_fill();
        /*
        if((digitalRead(FOOT_SW_PIN) == LOW) && (pulsecount_flag == false)){
            digitalWrite(FD_INPUT2,HIGH);
            delay(20);
            valve_open_Hp();
            FlexiTimer2::start();
            pulseP    = 0;
            pulseN    = 0;
            ad_vol = 0;
            pulsecount_flag = true;
            ad_vol_flag = true;  //パルス　アナログミックス時のみ使用
            digitalWrite(FD_INPUT2,LOW);
            pgch_flag = true;
        }
        */
    }else{
        incomingByte1 = Serial2.read();  //ヘッダ(=10h)
        incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
        incomingByte3 = Serial2.read();  //スイッチ番号

        if (incomingByte2 == 0x31) {
            if(lock_flag == false){
            switch (incomingByte3) {
                case 1:
                    _eeprom.calib_flag = false; 
                    _eeprom.prestop_offset1 = gt_keyanddisp5(X_PAGE2_SW1, Y_PAGE2_SW1);
                    if(_eeprom.prestop_offset1 >= _eeprom.filling_vol_goal){
                        _eeprom.prestop_offset1 = _eeprom.filling_vol_goal;
                        lp_only_flag = true;
                    }
                    EEPROM.put(0x00,_eeprom);
                    _eeprom.calib_flag = true; 
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 2:
                    _eeprom.calib_flag = false; 
                    _eeprom.prestop_offset2   = gt_keyanddisp5(X_PAGE2_SW2, Y_PAGE2_SW2);
                    if(_eeprom.prestop_offset2 >= _eeprom.prestop_offset1){
                        _eeprom.prestop_offset2 = _eeprom.prestop_offset1;
                    }
                    EEPROM.put(0x00,_eeprom);
                    _eeprom.calib_flag = true; 
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 3: // 充填目標値
                    _eeprom.calib_flag = false; 
                    _eeprom.filling_vol_goal = gt_keyanddisp5(X_PAGE2_SW3, Y_PAGE2_SW3);
                    EEPROM.put(0x00,_eeprom);
                    SD_logging_settingdata(_eeprom.filling_vol_goal);
                    _eeprom.calib_flag = true; 
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 4: // 低速のみ供給
                    //トグル動作
                    if ( false == lp_only_flag){
                        lp_only_flag = true;
                    }else{
                        lp_only_flag = false;
                    }
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 5: // 分解能選択
                    //トグル動作
                    if ( false == low_reso_flag){
                        low_reso_flag = true;
                    }else{
                        low_reso_flag = false;
                    }
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 6://累積リセット                                         //R_CUSTOM_SW;
                    _eeprom.calib_flag = false;                                //累積リセット  
                    _eeprom.filling_vol_accum = 0;
                    _eeprom.filling_num_accum = 0;
                    EEPROM.put(0x00,_eeprom);
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 7://LOCK(元fix表示)                                         //L_CUSTOM_SW;
                    lock_unlock();
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                case 8://MENU                                         //MENU_SW
                    pageNum = 1;                                //page01に移動
                    digitalWrite(SOLV_PIN_PRESSURE, LOW);
                    digitalWrite(SOLV_PIN_OPEN, LOW);
                    digitalWrite(SOLV_PIN_CLOSE, LOW);
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
            }
            }else{
                switch (incomingByte3) {
                case 7://LOCK(元fix表示)                                         //L_CUSTOM_SW;
                    lock_unlock();
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                }
            }
            gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
        }
    }
}



//充填モード****************************************************************************************
void gt_page13(){
    if(pgch_flag == true){
    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(0, 4);              //ch=0,sn=5
    gt_CustomSwitch( 80, 230, 280, 130);    //SW1(充填目標値)
    gt_CustomSwitch(450, 230, 280, 130);    //SW2(充填回数)
    MENU_SW;                                //menu
    LOCK_SW;                                //4 LOCK

    gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
    gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
        if(digitalRead(SOLV_PIN_OPEN) == LOW){
            if(lock_flag == true){
            gt_CopyFrom(IMAGE_ADDR_PAGE13B, 800, 800, 480, 0x91);
        }else{
            gt_CopyFrom(IMAGE_ADDR_PAGE13A, 800, 800, 480, 0x91);
        }
    }else{
        gt_CopyFrom(IMAGE_ADDR_PAGE13, 800, 800, 480, 0x91);
    }

    //数値表示
    gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    pgch_flag = false;
    }

    //受信バッファが3になったらデータ読込
    if(Serial2.available() < 3) {

    }else{

    incomingByte1 = Serial2.read();  //ヘッダ(=10h)
    incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
    incomingByte3 = Serial2.read();  //スイッチ番号

    if(_eeprom.calib_flag == true){
        conFilling_flag = true;
    }else{
        conFilling_flag = false;
    }

    if (incomingByte2 == 0x31) {
        if(lock_flag == false){
        switch (incomingByte3) {
            case 1:
                digitalWrite(SOLV_PIN_OPEN,HIGH);
                pgch_flag = true;
                break;
            case 2:
                digitalWrite(SOLV_PIN_OPEN,LOW);
                pgch_flag = true;
                break;
            case 3:                                         //MENU_SW;
                pageNum = 1;                                //MENUに移動
                pgch_flag = true;
                digitalWrite(SOLV_PIN_OPEN,LOW);
                break;
            case 4://LOCK
                lock_unlock();
                pgch_flag = true;
                pulsecount_flag = false;
                break;
            }
        }else{
                switch (incomingByte3) {
                case 4://LOCK
                    lock_unlock();
                    pgch_flag = true;
                    pulsecount_flag = false;
                    break;
                }
            }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
    }
    }
}

void washing_start(){
    if(washing_num_now > 0){
        washing_num_now--;
    }else{
        return;
    }
    digitalWrite(FD_INPUT2,HIGH);
    delay(20);
    valve_open_Hp();
//    FlexiTimer2::start();
    pulseP    = 0;
    pulseN    = 0;
//    ad_vol = 0;
    washing_flag = true;
//    ad_vol_flag = true;  //パルス　アナログミックス時のみ使用
    pulsecount_flag = true;
    digitalWrite(FD_INPUT2,LOW);
    pgch_flag = true;
}

void washing_stop(){
    wash_val_close();
    washing_flag = false;
    pulsecount_flag = false;
    pgch_flag = true;
}

//洗浄モード****************************************************************************************
void gt_page04(){
    if(pgch_flag == true){
    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(0, 8);              //ch=0,sn=8
    gt_CustomSwitch( X_PAGE4_SW1, Y_PAGE4_SW1, DISP_NUM_W5, DISP_NUM_H5);    //SW1(洗浄量)
    gt_CustomSwitch( X_PAGE4_SW2, Y_PAGE4_SW2, DISP_NUM_W5, DISP_NUM_H5);    //SW2(洗浄回数)
    gt_CustomSwitch( X_PAGE4_SW3, Y_PAGE4_SW3, DISP_NUM_W5, DISP_NUM_H5);    //SW3(洗浄間隔)
    R_CUSTOM_SW;                            //洗浄累積リセット
    L_CUSTOM_SW;                            //start washing
    MENU_SW;                                //menu
    M_CUSTOM_SW;
    LOCK_SW;

    gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
    gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
    if(lock_flag == true){
        gt_CopyFrom(IMAGE_ADDR_PAGE4A, 800, 800, 480, 0x91);
    }else{
        gt_CopyFrom(IMAGE_ADDR_PAGE4, 800, 800, 480, 0x91);
    }

    //数値表示
    gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum5(_eeprom.washing_vol_goal, 70 + 800, Y_PAGE4_SW1 +5);          //入力値初期表示(0)

    gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum5(_eeprom.washing_time_interval, 550 + 800, Y_PAGE4_SW3 -10);          //入力値初期表示(0)
    gt_dispnum5(washing_num_now, 100 + 800, Y_PAGE4_SW2 -10);         //入力値初期表示(0)
    gt_dispnum7(_eeprom.washing_num_accum, 500 + 800, 200);         //入力値初期表示(0)
    gt_dispnum7(_eeprom.washing_vol_accum, 500 + 800, 320);         //入力値初期表示(0)

    gt_OutlineFontSize(30, 0, 0, 0);
    gt_dispnum5(_eeprom.washing_num_goal, 230 + 800, Y_PAGE4_SW2 +75);         //入力値初期表示(0)

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    pgch_flag = false;
    }

    //受信バッファが3になったらデータ読込
    if(Serial2.available() < 3) {
        if((digitalRead(FOOT_SW_PIN) == LOW) && (pulsecount_flag == false)){
            if(_eeprom.washing_num_goal > 0){
                washing_start();
            }
        }
    }else{
    incomingByte1 = Serial2.read();  //ヘッダ(=10h)
    incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
    incomingByte3 = Serial2.read();  //スイッチ番号

    if (incomingByte2 == 0x31) {
        if(lock_flag == false){
        switch (incomingByte3) {
            case 1:
                _eeprom.washing_vol_goal = gt_keyanddisp5(X_PAGE4_SW1, Y_PAGE4_SW1);
                EEPROM.put(0x00,_eeprom);
                washing_stop();
                break;
            case 2:
                _eeprom.washing_num_goal   = gt_keyanddisp5(X_PAGE4_SW2, Y_PAGE4_SW2);
                washing_num_now = _eeprom.washing_num_goal;
                EEPROM.put(0x00,_eeprom);
                washing_stop();
                break;
            case 3:
                _eeprom.washing_time_interval   = gt_keyanddisp5(X_PAGE4_SW3, Y_PAGE4_SW3);
                EEPROM.put(0x00,_eeprom);
                washing_stop();
                break;
            case 4:
                _eeprom.washing_num_accum = 0;    //洗浄累積リセット
                _eeprom.washing_vol_accum = 0;
                EEPROM.put(0x00,_eeprom);
                washing_stop();
                break;
            case 5:                                         //洗浄開始
                if( (_eeprom.washing_num_goal > 0)  && (pulsecount_flag == false)){
                    //_eeprom.washing_num_goal--;
                    washing_start();
                }
                pgch_flag = true;
                break;
            case 6:                                         //MENU_SW
                pageNum = 1;                                //page01に移動
                washing_stop();
                break;
            case 7:                                         //STOP
                washing_stop();
                break;
            case 8:                                         //LOCK_SW
                lock_unlock();
                washing_stop();
                pulsecount_flag = false;
                break;
        }
        }else{
            switch (incomingByte3) {
                case 8:                                         //LOCK_SW
                lock_unlock();
                gt_OutlineFontSize(10, 0, 0, 0); 
                pgch_flag = true;
                washing_flag = false;
                pulsecount_flag = false;
                wash_val_close();
                break;
            }
        }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
    }
    }
}

//キャリブレーションモード　充填*****************************************************************
void gt_page05(){
    if(pgch_flag == true){
      //タッチパネル制御モード定義(カスタムスイッチモード)
      gt_CustomSwitchMode(0, 5);              //ch=0,sn=5
      gt_CustomSwitch( X_PAGE5_SW1, Y_PAGE5_SW1, DISP_NUM_W5, DISP_NUM_H5);    //SW1(充填目標値)
      gt_CustomSwitch( X_PAGE5_SW2, Y_PAGE5_SW2, DISP_NUM_W5, DISP_NUM_H5);    //SW2(充填回数)
      R_CUSTOM_SW;                            //洗浄累積リセット
      L_CUSTOM_SW;                            //start washing
      MENU_SW;                                //menu

      gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
      gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
      gt_setCursor(800, 0);
      gt_CopyFrom(IMAGE_ADDR_PAGE5, 800, 800, 480, 0x91);

    //数値表示
      gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定
      gt_dispnum5(filling_vol_history, 70 + 800, Y_PAGE5_SW1 +5);          //入力値初期表示(0)
      gt_dispnum2(filling_vol_input, 70 + 800, Y_PAGE5_SW2 +5);         //入力値初期表示(0)

      gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
      gt_dispnum5(_eeprom.filling_vol_goal, 550 + 800, 90);          //入力値初期表示(0)
      gt_dispnum5(_eeprom.filling_vol_accum, 550 + 800, 210);         //入力値初期表示(0)
      gt_dispnum5(_eeprom.filling_num_accum, 550 + 800, 330);         //入力値初期表示(0)
    
    //非表示エリアから表示エリアにコピー
      gt_setCursor(0, 0);
      gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

      pgch_flag = false;
    }

    //受信バッファが3になったらデータ読込
    if(Serial2.available() < 3) {
        start_fill();
    }else{
      incomingByte1 = Serial2.read();  //ヘッダ(=10h)
      incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
      incomingByte3 = Serial2.read();  //スイッチ番号

      if (incomingByte2 == 0x31) {
        switch (incomingByte3) {
            case 1:
                filling_vol_history = gt_keyanddisp5(X_PAGE5_SW1, Y_PAGE5_SW1);
                pgch_flag = true;
                break;
            case 2:
                _eeprom.filling_num_goal  = gt_keyanddisp5(X_PAGE5_SW2, Y_PAGE5_SW2);
                EEPROM.put(0x00,_eeprom);
                pgch_flag = true;
                break;
            case 3:                                         //R_CUSTOM_SW; 補正値リセット
                
                pgch_flag = true;
                break;
            case 4:                                         //L_CUSTOM_SW;　補正値確定
                

                break;
            case 5:                                         //MENU_SW
                pageNum = 1;                                //page01に移動
                pgch_flag = true;
                break;
        }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
      }
    }
}

//充填モード　連続*****************************************************************
void gt_page06(){
//    if(pulsecount_flag == true){
    if(false){
    //数値表示
    gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispdecimal(filling_vol_now, 70 , 270 +5);         //入力値初期表示(0)
    gt_dispnum5(filling_num_now, 550 , 70);          //入力値初期表示(0)
    //gt_dispnum7(_eeprom.filling_vol_accum, 510 , 210);         //入力値初期表示(0)
    
    //非表示エリアから表示エリアにコピー
    //gt_setCursor(0, 0);
    //gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    }else if(true){
    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(0, 5);              //ch=0,sn=5
    MENU_SW;                                //menu
    gt_CustomSwitch( X_PAGE6_SW1, Y_PAGE6_SW1, DISP_NUM_W5, DISP_NUM_H5);    //SW1(充填目標値)
    gt_CustomSwitch( X_PAGE6_SW2, Y_PAGE6_SW2, DISP_NUM_W5, DISP_NUM_H5);    //SW2(充填回数)
    R_CUSTOM_SW;                            //累積リセット
    LOCK_SW;                                //LOCK

    gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
    gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
    if(pgch_flag == true){
        if(lock_flag == true){
            gt_CopyFrom(IMAGE_ADDR_PAGE6A, 800, 800, 480, 0x91);
        }else{
            gt_CopyFrom(IMAGE_ADDR_PAGE6, 800, 800, 480, 0x91);
        }
    }

    //数値表示
    
    gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispdecimal(_eeprom.filling_vol_goal, 70 + 800, Y_PAGE6_SW1 +5);          //入力値初期表示(0)
    gt_OutlineFontSize(80, 0, 0, 0);  
    gt_dispdecimal(filling_vol_now, 70 + 800,270 +5);         //入力値初期表示(0)
    gt_OutlineFontSize(80, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum5(filling_num_now, 550 + 800, 70);          //入力値初期表示(0)
    gt_OutlineFontSize(80, 0, 0, 0);  
    gt_dispnum7(_eeprom.filling_vol_accum, 510 + 800, 210);         //入力値初期表示(0)
    gt_OutlineFontSize(80, 0, 0, 0);  
    gt_dispnum7(_eeprom.filling_num_accum, 510 + 800, 330);         //入力値初期表示(0)

    gt_OutlineFontSize(30, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum5(_eeprom.filling_num_goal, 650 + 800, 155);         //入力値初期表示(0)
    

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    pgch_flag = false;
    }

    //受信バッファが3になったらデータ読込
    if(Serial2.available() < 3) {
        start_fill();
    }else{
    incomingByte1 = Serial2.read();  //ヘッダ(=10h)
    incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
    incomingByte3 = Serial2.read();  //スイッチ番号

    if (incomingByte2 == 0x31) {
        if(lock_flag == false){
        switch (incomingByte3) {
            case 1:                                         //MENU_SW
                pageNum = 1;                                //page01に移動
                digitalWrite(SOLV_PIN_PRESSURE, LOW);
                digitalWrite(SOLV_PIN_OPEN, LOW);
                digitalWrite(SOLV_PIN_CLOSE, LOW);
                pgch_flag = true;
                pulsecount_flag = false;
                break;
            case 2:
                _eeprom.filling_vol_goal = gt_keyanddisp5(X_PAGE6_SW1, Y_PAGE6_SW1);
                SD_logging_settingdata(_eeprom.filling_vol_goal);
                EEPROM.put(0x00,_eeprom);
                pgch_flag = true;
                pulsecount_flag = false;
                break;
            case 3:
                filling_num_now = gt_keyanddisp5(X_PAGE6_SW2, Y_PAGE6_SW2);
                _eeprom.filling_num_goal = filling_num_now;
                EEPROM.put(0x00,_eeprom);
                pgch_flag = true;
                pulsecount_flag = false;
                break;
            case 4://累積リセット                                         //R_CUSTOM_SW;
                _eeprom.filling_vol_accum = 0;
                _eeprom.filling_num_accum = 0;
                EEPROM.put(0x00,_eeprom);
                pgch_flag = true;
                pulsecount_flag = false;
                break;
            case 5://LOCK
                lock_unlock();
                pgch_flag = true;
                pulsecount_flag = false;
                break;
            }
        }else{
            switch (incomingByte3) {
            case 5:
                lock_unlock();
                pgch_flag = true;
                pulsecount_flag = false;
                break;
        }
        }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
        }
    }
}

//累積・エラー表示画面****************************************************************************************
void gt_page07(){
    if(pgch_flag == true){
    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(0, 4);              //ch=0,sn=3
    R_CUSTOM_SW;                            //エラーリセット
    L_CUSTOM_SW;                            //累積リセット
    MENU_SW;                                //menu
    LOCK_SW;                                //LOCK

    gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
    gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
    gt_setCursor(800, 0);
    if(lock_flag == true){
        gt_CopyFrom(IMAGE_ADDR_PAGE7A, 800, 800, 480, 0x91);
    }else{
        gt_CopyFrom(IMAGE_ADDR_PAGE7, 800, 800, 480, 0x91);
    }

    //数値表示
    gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum7(_eeprom.filling_vol_accum, 70 + 800, Y_PAGE7_SW1 +5);          //入力値初期表示(0)
    gt_dispnum7(_eeprom.filling_num_accum, 70 + 800, Y_PAGE7_SW2 +5);         //入力値初期表示(0)

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    

    //画面表示追加

    //アウトラインフォント設定
    gt_OutlineFontSize(50, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_OutlineFontTypeSelect(0);    //アウトライン フォント タイプ選択(日本語)
    gt_fontsize(00);                //フォントサイズ選択(アウトラインフォント)

    if(error_sd == true){
        gt_setCursor(550, 200);
        Serial2.print("SD card error");
    }
    if(error_sensor == true){
        gt_setCursor(550, 280);
        Serial2.print("Sensor error");
    }

    pgch_flag = false;
    }

    //受信バッファが3になったらデータ読込
    if(Serial2.available() < 3) {

    }else{
    incomingByte1 = Serial2.read();  //ヘッダ(=10h)
    incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
    incomingByte3 = Serial2.read();  //スイッチ番号

      if (incomingByte2 == 0x31) {
          if(lock_flag == false){
        switch (incomingByte3) {
            case 1:                                         //R_CUSTOM_SW;
                error_flag = false;                                //エラーリセット
                if(error_sd == true){
                    error_sd = false;
                    SD_initialize();
                }
                error_sensor =false;
                if(digitalRead(XA_OUT2) == HIGH ){
                    error_sensor = true;
                }
                pgch_flag = true;
                break;
            case 2:                                         //L_CUSTOM_SW;
                _eeprom.filling_num_accum = 0;                      //累積データをリセット
                _eeprom.filling_vol_accum = 0;
                EEPROM.put(0x00,_eeprom);
                pgch_flag = true;
                break;
            case 3:                                         //MENU_SW
                pageNum = 1;                                //page01に移動
                pgch_flag = true;
                break;
            case 4:
                lock_unlock();
                pulsecount_flag = false;
                pgch_flag = true;
                break;
        }
        }else{
            switch (incomingByte3) {
            case 4:
                lock_unlock();
                pulsecount_flag = false;
                pgch_flag = true;
                break;
        }
        }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
      }
    }
}


//非常停止画面 -- 非常停止ボタンが押された場合に表示*****************************************************************
void gt_page08(){
    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
    gt_CopyFrom(IMAGE_ADDR_PAGE8, 800, 800, 480, 0x91);

    //数値表示
    gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

    washing_flag = false;
    pulsecount_flag = false;
    digitalWrite(SOLV_PIN_PRESSURE, LOW);
    digitalWrite(SOLV_PIN_OPEN, LOW);
    digitalWrite(SOLV_PIN_CLOSE, LOW);
    
    while(digitalRead(EMG_ON) == LOW){};
    pageNum = 1;
    pgch_flag = true;
}

uint32_t gt_keyanddisp5(int sw_x, int sw_y){
    gt_CharacterColor(255, 0, 0);               //キャラクタ表示色指定(赤)
    gt_BoxDrawing(1, 1, sw_x, sw_y, DISP_NUM_W5, DISP_NUM_H5);      //キー入力表示枠
    keypaddata = 0;
    uint32_t inputdata = disp_keypad(540, 80);        //キーパッド表示→入力値取得
    gt_CharacterColor(255, 255, 255);           //キャラクタ表示色指定(白)
    gt_BoxDrawing(1, 1, sw_x, sw_y, DISP_NUM_W5, DISP_NUM_H5);      //キー入力表示枠
//    gt_OutlineFontSize(100, 0, 0, 0);           //アウトラインフォントサイズ指定
//    gt_dispnum5(inputdata, 70, sw_y +5);            //入力値1表示
    return inputdata;
}

void gt_dispnumbox7(uint32_t inputdata, int sw_x, int sw_y){
    gt_CharacterColor(255, 0, 0);               //キャラクタ表示色指定(赤)
    gt_BoxDrawing(1, 1, sw_x, sw_y, DISP_NUM_W7, DISP_NUM_H7);      //キー入力表示枠
    gt_CharacterColor(255, 255, 255);           //キャラクタ表示色指定(白)
    gt_OutlineFontSize(100, 0, 0, 0);           //アウトラインフォントサイズ指定
//    gt_dispnum7(inputdata, 70, sw_y +5);            //入力値1表示
}

void gt_clockdisp(int sw_x, int sw_y){
    gt_OutlineFontSize(50, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum5(rtcclock.RTCyear,   sw_x , sw_y);         //年　表示(0)
    gt_put('/');
    gt_dispnum2(rtcclock.RTCmonth,  sw_x +103 , sw_y);         //月　表示(0)
    gt_put('/');
    gt_dispnum2(rtcclock.RTCday,    sw_x +152 , sw_y);         //日　表示(0)
    gt_put('-');
    gt_dispnum2(rtcclock.RTChour,   sw_x +199 , sw_y);         //時間表示(0)
    gt_put(':');
    gt_dispnum2(rtcclock.RTCminute, sw_x +244 , sw_y);         //分　表示(0)
    delay(100);
}

////////////////////////////////////////////////////
// loop（繰り返し実行されるプログラム）
////////////////////////////////////////////////////
void gtUartLoop()
{
    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(0, 4);              //ch=0,sn=4
    gt_CustomSwitch( 50,  50, 230, 110);    //SW1(box1)
    gt_CustomSwitch( 50, 185, 230, 110);    //SW2(box2)
    gt_CustomSwitch( 50, 320, 230, 110);    //SW3(box3)
    gt_CustomSwitch(650, 330, 150, 150);    //SW4(背景変更)

    gt_TouchModeSelect(1);                  //マルチタッチモード(sn=1)
    gt_TouchDataTransmit(1);                //タッチパネル制御データ送信許可

demo_st:
    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(800, 0);
    gt_CopyFrom(0x0207A058 + 0xBB800 * bg_mode, 800, 800, 480, 0x91);   //p01_800x480.jpg ～ p03_800x480.jpg

    //数値表示ボックス描画
    gt_BoxDrawing(2, 0, 50 + 800, 50, 230, 110);    //Boxfull(黒)
    gt_BoxDrawing(1, 1, 50 + 800, 50, 230, 110);    //Box(白)
    gt_BoxDrawing(2, 0, 50 + 800, 185, 230, 110);   //Boxfull(黒)
    gt_BoxDrawing(1, 1, 50 + 800, 185, 230, 110);   //Box(白)
    gt_BoxDrawing(2, 0, 50 + 800, 320, 230, 110);   //Boxfull(黒)
    gt_BoxDrawing(1, 1, 50 + 800, 320, 230, 110);   //Box(白)

    //数値表示
    gt_OutlineFontSize(100, 0, 0, 0);               //アウトラインフォントサイズ指定
    gt_dispnum5(_eeprom.filling_vol_goal, 70 + 800, 55);          //入力値初期表示(0)
    gt_dispnum5(_eeprom.filling_num_goal, 70 + 800, 190);         //入力値初期表示(0)
    gt_dispnum5(_eeprom.calib_num_goal, 70 + 800, 325);         //入力値初期表示(0)

    //非表示エリアから表示エリアにコピー
    gt_setCursor(0, 0);
    gt_CopyVram(800, 0, 800, 480, 1600);            //定義済みビットイメージ表示(VRAMからコピー)

touch_read1:
    //受信バッファが3になったらデータ読込
    while (Serial2.available() < 3) {}

    incomingByte1 = Serial2.read();  //ヘッダ(=10h)
    incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
    incomingByte3 = Serial2.read();  //スイッチ番号

    if (incomingByte2 == 0x31) {
        switch (incomingByte3) {
            case 1:
                gt_CharacterColor(255, 0, 0);               //キャラクタ表示色指定(赤)
                gt_BoxDrawing(1, 1, 50, 50, 230, 110);      //キー入力表示枠
                keypaddata = 0;
                _eeprom.filling_vol_goal = disp_keypad(540, 80);        //キーパッド表示→入力値取得
                gt_CharacterColor(255, 255, 255);           //キャラクタ表示色指定(白)
                gt_BoxDrawing(1, 1, 50, 50, 230, 110);      //キー入力表示枠
                gt_OutlineFontSize(100, 0, 0, 0);           //アウトラインフォントサイズ指定
                gt_dispnum5(_eeprom.filling_vol_goal, 70, 55);            //入力値1表示
                break;
            case 2:
                gt_CharacterColor(255, 0, 0);               //キャラクタ表示色指定(赤)
                gt_BoxDrawing(1, 1, 50, 185, 230, 110);     //キー入力表示枠
                keypaddata = 0;
                _eeprom.filling_num_goal = disp_keypad(540, 80);          //キーパッド表示→入力値取得
                gt_CharacterColor(255, 255, 255);           //キャラクタ表示色指定(白)
                gt_BoxDrawing(1, 1, 50, 185, 230, 110);     //キー入力表示枠
                gt_OutlineFontSize(100, 0, 0, 0);           //アウトラインフォントサイズ指定
                gt_dispnum5(_eeprom.filling_num_goal, 70, 190);           //入力値2表示
                break;
            case 3:
                gt_CharacterColor(255, 0, 0);               //キャラクタ表示色指定(赤)
                gt_BoxDrawing(1, 1, 50, 320, 230, 110);     //キー入力表示枠
                keypaddata = 0;
                _eeprom.calib_num_goal = disp_keypad(540, 80);          //キーパッド表示→入力値取得
                gt_CharacterColor(255, 255, 255);           //キャラクタ表示色指定(白)
                gt_BoxDrawing(1, 1, 50, 320, 230, 110);     //キー入力表示枠
                gt_OutlineFontSize(100, 0, 0, 0);           //アウトラインフォントサイズ指定
                gt_dispnum5(_eeprom.calib_num_goal, 70, 325);           //入力値3表示
                break;
            case 4:
                bg_mode = bg_mode + 1;                      //背景画像変更
                if (bg_mode > 3) {
                    bg_mode = 0;
                }
                goto demo_st;
                break;
        }
        gt_TouchChannelSelect(0);           //タッチパネル制御チャンネル選択
    }
    goto touch_read1;   //繰り返し
}