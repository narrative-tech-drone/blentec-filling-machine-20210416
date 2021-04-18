/****************************************
    GT module : GTWV070C3A00PA(GT800X480A-C903PA)
    CPU board : DS20-010(ATmega328P/3.3V/8MHz)
                Arduino Pro mini(ATmega328P/3.3/8MHz)互換

    非同期シリアル接続(GTのジャンパーJ5,J6をショート)
    通信速度：38400bps
    -------------------------------------------------------------
    FROM2に以下の画像データを登録する。
    address    image file           X    Y   format
    02000000h  b_0.bmp              70   70  Color16HSDither(91h)*
    02002648h  b_0_on.bmp           70   70  Color16HSDither(91h)
    02004C90h  b_1.bmp              70   70  Color16HSDither(91h)*
    020072D8h  b_1_on.bmp           70   70  Color16HSDither(91h)
    02009920h  b_2.bmp              70   70  Color16HSDither(91h)*
    0200BF68h  b_2_on.bmp           70   70  Color16HSDither(91h)
    0200E5B0h  b_3.bmp              70   70  Color16HSDither(91h)*
    02010BF8h  b_3_on.bmp           70   70  Color16HSDither(91h)
    02013240h  b_4.bmp              70   70  Color16HSDither(91h)*
    02015888h  b_4_on.bmp           70   70  Color16HSDither(91h)
    02017ED0h  b_5.bmp              70   70  Color16HSDither(91h)*
    0201A518h  b_5_on.bmp           70   70  Color16HSDither(91h)
    0201CB60h  b_6.bmp              70   70  Color16HSDither(91h)*
    0201F1A8h  b_6_on.bmp           70   70  Color16HSDither(91h)
    020217F0h  b_7.bmp              70   70  Color16HSDither(91h)*
    02023E38h  b_7_on.bmp           70   70  Color16HSDither(91h)
    02026480h  b_8.bmp              70   70  Color16HSDither(91h)*
    02028AC8h  b_8_on.bmp           70   70  Color16HSDither(91h)
    0202B110h  b_9.bmp              70   70  Color16HSDither(91h)*
    0202D758h  b_9_on.bmp           70   70  Color16HSDither(91h)
    0202FDA0h  b_c.bmp              70   70  Color16HSDither(91h)*
    020323E8h  b_c_on.bmp           70   70  Color16HSDither(91h)
    02034A30h  b_e.bmp              70   70  Color16HSDither(91h)*
    02037078h  b_e_on.bmp           70   70  Color16HSDither(91h)
    020396C0h  keypad_210x280.bmp  210  280  Color16HSDither(91h)
    02056220h  keypad_210x350.bmp  210  350  Color16HSDither(91h)
    ※image_keypad_2000000h.binをメモリツールの「ファイル→GT」機能で登録する。
    -------------------------------------------------------------
*****************************************/

#define IMAGE_ADDR_PAGE0  0x00000000
#define IMAGE_ADDR_PAGE1  0x00D7E880
#define IMAGE_ADDR_PAGE1A 0x006E7080
#define IMAGE_ADDR_PAGE1B 0x00CC3080
#define IMAGE_ADDR_PAGE1C 0x00C07880

#define IMAGE_ADDR_PAGE2  0x000BB800
#define IMAGE_ADDR_PAGE3  0x002EE000
#define IMAGE_ADDR_PAGE4  0x003A9800
#define IMAGE_ADDR_PAGE5  0x000BB800
#define IMAGE_ADDR_PAGE6  0x00177000
#define IMAGE_ADDR_PAGE7  0x00232800
#define IMAGE_ADDR_PAGE8  0x00B4C080 //
#define IMAGE_ADDR_PAGE9  0x0207A058 //


//変数
volatile uint32_t keypaddata  = 0;        //キーパッド入力値

byte bg_mode = 0;           //バックグラウンド表示選択(0～3)
byte incomingByte1 = 0;     //タッチデータ読み取り変数
byte incomingByte2 = 0;
byte incomingByte3 = 0;

//作成した関数のプロトタイプ（省略可能）
void gt_put(unsigned char onebyte);                         //1byte送信
void gt_print(unsigned char message[], int L);              //文字列送信
void gt_clr();                                              //画面クリア
void gt_setCursor(int X, int Y);                            //カーソルセット
void gt_WriteScreenMode(byte mode);                         //書き込み画面モード選択
void gt_fontsize(byte m);                                   //フォントサイズ選択
void gt_OutlineFontSize(int yl, int ys, int xs, int bo);    //アウトラインフォントサイズ指定
void gt_OutlineFontTypeSelect(byte n);                      //アウトライン フォント タイプ選択
void gt_fontmag(int X, int Y);                              //キャラクタ拡大
void gt_scrhon(byte n);                                     //横スクロールモード指定＋横スクロールモード速度指定
void gt_scroff();                                           //横スクロールモード解除（オーバーライトモード）
void gt_reverse(byte n);                                    //リバース指定・解除
void gt_wait(byte t);                                       //ウェイト
void gt_uwindow(byte a, int xP, int yP, int xS, int yS);    //ユーザーウィンドウ定義
void gt_cwindow(byte a);                                    //カレントウィンドウ選択
void gt_kanjiOn();                                          //漢字表示の設定
void gt_kanjiOff();                                         //漢字表示の解除
void gt_CharacterColor(byte R, byte G, byte B);             //キャラクタ表示色指定
void gt_BackgroundColor(byte R, byte G, byte B);            //背景表示色指定
void gt_BackgroundOnOff(byte b);                            //背景表示 ON/OFF 指定
void gt_WriteMixtureMode(byte mode);                        //表示書き込み合成モード指定
void gt_tableput(const uint8_t tablename[], int n);         //テーブルデータ送信
void gt_realtimebitimage1(const uint8_t tablename[], int x, int y);     //リアルタイムビットイメージ表示/1bitモノクロ
void gt_realtimebitimage6(const uint8_t tablename[], int x, int y);     //リアルタイムビットイメージ表示/6bitカラー
void gt_BoxDrawing(byte mode, byte pen, int x1, int y1, int xs, int ys);//ボックス描画
void gt_CoordinatesMode(byte ch);                                       //タッチパネル制御モード定義・座標モード
void gt_SwitchMatrixMode(byte ch, byte nx, byte cx, byte ny, byte cy);  //タッチパネル制御モード定義・マトリクススイッチモード
void gt_CustomSwitchMode(byte ch, byte sn);                             //タッチパネル制御モード定義・カスタムスイッチモード（ヘッダ部のみ）
void gt_CustomSwitch(int px, int py, int sx, int sy);                   //タッチパネル制御モード定義・カスタムスイッチモード（各スイッチデータ）
void gt_TouchDataTransmit(byte m);                                      //タッチパネル制御データ送信許可
void gt_TouchModeSelect(byte n);                                        //タッチモード選択（シングルタッチモード／マルチタッチモード）
void gt_ThresholdSet(byte thr);                                         //タッチパラメータ制御（スレッショルド値設定）
void gt_TouchChannelSelect(byte ch);                                    //タッチパネル制御チャンネル選択
void gt_CopyFrom(long f2add, int xS, int x, int y, byte fmt);           //定義済みビットイメージ表示(FROM2)
void gt_CopyVram(long vx, long vy, int vxs, int vys, int vramx);        //定義済みビットイメージ表示(VRAMからコピー)
void gt_PortSetting(byte n, byte a);                                    //ポート入出力選択
void gt_PortOutput(byte n, byte a);                                     //ポート出力
void gt_dispnum5(uint32_t ndata, int nx, int ny);
void gt_dispnum7(uint32_t ndata, int nx, int ny);
uint16_t disp_keypad(int kx, int ky);



////////////////////////////////////////////////////
// setup（リセット後に１回だけ実行されるプログラム）
////////////////////////////////////////////////////
void gtUartSetup()
{
    //ピン初期設定
    pinMode(GT_DTR, INPUT);
    pinMode(GT_DSR, OUTPUT);
    pinMode(GT_TRDY, INPUT);
    digitalWrite(GT_DSR, LOW);
    //ボーレート設定
    Serial2.begin(38400);            //38400bps(GTモジュールの初期設定に合わせる)
    //RESET(念のため)
    pinMode(GT_RESET, OUTPUT);
    digitalWrite(GT_RESET, LOW);
    delay(100);
    digitalWrite(GT_RESET, HIGH);
    pinMode(GT_RESET, INPUT);       //RESET接続ポートがオープンドレイン出力でない場合は入力(HighZ)にしておく
    delay(200);

    //最初にプログラム名を1秒間表示(識別用)
    gt_fontsize(4);                 //フォントサイズ8x16
    Serial2.print("Ver.1.0.2");
    delay(1000);    gt_clr();

    //スレッショルド設定（ここで設定しない場合はメモリスイッチの設定で動作します）
    //gt_ThresholdSet(0x30);        //初期値は50h･･･設置状態(カバー厚など)により調整してください

    //アウトラインフォント設定
    gt_OutlineFontTypeSelect(0);    //アウトライン フォント タイプ選択(日本語)
    gt_fontsize(00);                //フォントサイズ選択(アウトラインフォント)

    //初期画面(非表示エリアに表示してから表示エリアにコピーする)
    //背景描画
    gt_setCursor(0, 0);
    gt_CopyFrom(IMAGE_ADDR_PAGE0, 800, 800, 480, 0x91);   //p01_800x480.jpg ～ p03_800x480.jpg
    delay(2000);
}

///////////////////////////////////////////////////
// キーパッド表示/タッチ入力/キーパッド消去
//   kx:キーパッド表示位置(左上)x座標
//   ky:キーパッド表示位置(左上)y座標
//   戻り値：入力値keypaddata
///////////////////////////////////////////////////
uint16_t disp_keypad(int kx, int ky)
{
    int k_mode = 0;                         //動作モード(1:Eタッチあり)

    //背景退避
    gt_setCursor(800, 0);                   //裏画面(800,0)
    gt_CopyVram(kx, ky, 210, 350, 1600);    //定義済みビットイメージ表示(VRAMからコピー)

    //キーパッド表示
    gt_setCursor(kx, ky);
    gt_CopyFrom(0x02056220, 210, 210, 350, 0x91);   //定義済みビットイメージ表示(key_210x350.png)

    //アウトラインフォント設定
    gt_OutlineFontTypeSelect(0);                    //アウトライン フォント タイプ選択(日本語)
    gt_fontsize(00);                                //フォントサイズ選択(アウトラインフォント)
    gt_OutlineFontSize(70, 0, 0, 0);                //アウトラインフォントサイズ指定
    gt_dispnum5(keypaddata, kx + 40, ky);            //5桁数値表示(初期値)

    //タッチパネル制御モード定義(カスタムスイッチモード)
    gt_CustomSwitchMode(1, 12);                     //ch=1,sn=12
    gt_CustomSwitch(kx      , ky + 210, 70, 70);    //sw1(1)
    gt_CustomSwitch(kx +  70, ky + 210, 70, 70);    //sw2(2)
    gt_CustomSwitch(kx + 140, ky + 210, 70, 70);    //sw3(3)
    gt_CustomSwitch(kx,       ky + 140, 70, 70);    //sw4(4)
    gt_CustomSwitch(kx +  70, ky + 140, 70, 70);    //sw5(5)
    gt_CustomSwitch(kx + 140, ky + 140, 70, 70);    //sw6(6)
    gt_CustomSwitch(kx,       ky +  70, 70, 70);    //sw7(7)
    gt_CustomSwitch(kx +  70, ky +  70, 70, 70);    //sw8(8)
    gt_CustomSwitch(kx + 140, ky +  70, 70, 70);    //sw9(9)
    gt_CustomSwitch(kx,       ky + 280, 70, 70);    //sw10(0)
    gt_CustomSwitch(kx +  70, ky + 280, 70, 70);    //sw11(C/clear)
    gt_CustomSwitch(kx + 140, ky + 280, 70, 70);    //sw12(E/enter)

    gt_TouchChannelSelect(1);           //タッチパネル制御チャンネル選択(1)
    gt_TouchModeSelect(1);              //マルチタッチモード(sn=1)

touch_keypad:
    //受信バッファが3になったらデータ読込
    while (Serial2.available() < 3) {}
    incomingByte1 = Serial2.read();  //ヘッダ(=10h)
    incomingByte2 = Serial2.read();  //識別子(=30h:リリース/31h:タッチ)
    incomingByte3 = Serial2.read();  //スイッチ番号
    if (incomingByte2 == 0x31) {    //タッチ時
        switch (incomingByte3) {
            case 1:
                keypaddata = (keypaddata * 10 + 1) % 100000;      //入力値更新(最大99999)
                gt_dispnum5(keypaddata, kx + 40, ky);            //5桁数値表示
                gt_setCursor(kx, ky + 210);                     //キー"1"の左上座標
                gt_CopyFrom(0x020072D8, 70, 70, 70, 0x91);      //定義済みビットイメージ表示(b_1_on.bmp)
                break;
            case 2:
                keypaddata = (keypaddata * 10 + 2) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 70, ky + 210);
                gt_CopyFrom(0x0200BF68, 70, 70, 70, 0x91);      //b_2_on.bmp
                break;
            case 3:
                keypaddata = (keypaddata * 10 + 3) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 140, ky + 210);
                gt_CopyFrom(0x02010BF8, 70, 70, 70, 0x91);      //b_3_on.bmp
                break;
            case 4:
                keypaddata = (keypaddata * 10 + 4) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx, ky + 140);
                gt_CopyFrom(0x02015888, 70, 70, 70, 0x91);      //b_4_on.bmp
                break;
            case 5:
                keypaddata = (keypaddata * 10 + 5) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 70, ky + 140);
                gt_CopyFrom(0x0201A518, 70, 70, 70, 0x91);      //b_5_on.bmp
                break;
            case 6:
                keypaddata = (keypaddata * 10 + 6) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 140, ky + 140);
                gt_CopyFrom(0x0201F1A8, 70, 70, 70, 0x91);      //b_6_on.bmp
                break;
            case 7:
                keypaddata = (keypaddata * 10 + 7) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx, ky + 70);
                gt_CopyFrom(0x02023E38, 70, 70, 70, 0x91);      //b_7_on.bmp
                break;
            case 8:
                keypaddata = (keypaddata * 10 + 8) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 70, ky + 70);
                gt_CopyFrom(0x02028AC8, 70, 70, 70, 0x91);      //b_8_on.bmp
                break;
            case 9:
                keypaddata = (keypaddata * 10 + 9) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 140, ky + 70);
                gt_CopyFrom(0x0202D758, 70, 70, 70, 0x91);      //b_9_on.bmp
                break;
            case 10:
                keypaddata = (keypaddata * 10) % 100000;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx, ky + 280);
                gt_CopyFrom(0x02002648, 70, 70, 70, 0x91);      //b_0_on.bmp
                break;
            case 11:
                keypaddata = 0;
                gt_dispnum5(keypaddata, kx + 40, ky);
                gt_setCursor(kx + 70, ky + 280);
                gt_CopyFrom(0x020323E8, 70, 70, 70, 0x91);      //b_c_on.bmp
                break;
            case 12:
                gt_setCursor(kx + 140, ky + 280);
                gt_CopyFrom(0x02037078, 70, 70, 70, 0x91);      //b_e_on.bmp
                k_mode = 1;
                break;
        }
    }

    if (incomingByte2 == 0x30) {    //リリース時
        if (k_mode == 1) {
            //キーパッド消去(退避した背景を戻す)
            gt_setCursor(kx, ky);
            gt_CopyVram(800, 0, 210, 350, 1600);            //定義済みビットイメージ表示(VRAMからコピー)
            return keypaddata;                               //RETURN
        } else {
            gt_setCursor(kx, ky + 70);
            gt_CopyFrom(0x020396C0, 210, 210, 280, 0x91);   //keypad_210x280.bmp
        }
    }
    goto touch_keypad;   //繰り返し
}

///////////////////////////////////////////////////
// 7桁 小数点数値表示(0.0～99,999.9)
// uint32_t nDataは百倍値(0~9,999,999)
// ゼロサプレス：文字色を背景と同じ黒に設定して0表示
// （数字0～9は等幅であるが空白20hは等幅ではないため使えない）
///////////////////////////////////////////////////
void gt_dispdecimal(uint32_t ndata, int nx, int ny)
{
    gt_setCursor(nx, ny);                       //カーソルセット

    ndata *= 10;

    if (ndata < 10000000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100000000) / 10000000 + 0x30);    //100,000位

    if (ndata < 1000000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 10000000) / 1000000 + 0x30);    //10,000位

    if (ndata < 100000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 1000000) / 100000 + 0x30);    //1,000位

    
    if (ndata < 10000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100000) / 10000 + 0x30);    //100位

    if (ndata < 1000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 10000) / 1000 + 0x30);      //10位

    if (ndata < 100) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
        //gt_put((ndata % 1000) / 100 + 0x30);        //1位
    }


    if (ndata < 10) {
        //gt_CharacterColor(0, 0, 0);             //文字色：黒
        gt_CharacterColor(255, 255, 255);       //文字色：白
        gt_put('0');
        gt_put('.');
        gt_put(ndata + 0x30);        //少数第一位
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
        gt_put((ndata % 1000) / 100 + 0x30);        //1位
        gt_put('.');
        gt_put((ndata % 100) / 10 + 0x30);        //少数第一位
    }
}

///////////////////////////////////////////////////
// 7桁 数値表示(0～9999999)
// ゼロサプレス：文字色を背景と同じ黒に設定して0表示
// （数字0～9は等幅であるが空白20hは等幅ではないため使えない）
///////////////////////////////////////////////////
void gt_dispnum7(uint32_t ndata, int nx, int ny)
{
    gt_setCursor(nx, ny);                       //カーソルセット

    if (ndata < 1000000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 10000000) / 1000000 + 0x30);    //1,000,000位

    if (ndata < 100000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 1000000) / 100000 + 0x30);      //100,000位

    if (ndata < 10000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100000) / 10000 + 0x30);      //10,000位

    if (ndata < 1000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 10000) / 1000 + 0x30);      //1,000位

    if (ndata < 100) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 1000) / 100 + 0x30);        //100位

    if (ndata < 10) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100) / 10 + 0x30);          //10位

    gt_CharacterColor(255, 255, 255);           //文字色：白
    gt_put(ndata % 10 + 0x30);                  //1位
}

///////////////////////////////////////////////////
// 5桁 数値表示(0～99999)
// ゼロサプレス：文字色を背景と同じ黒に設定して0表示
// （数字0～9は等幅であるが空白20hは等幅ではないため使えない）
///////////////////////////////////////////////////
void gt_dispnum5(uint32_t ndata, int nx, int ny)
{
    gt_setCursor(nx, ny);                       //カーソルセット

    if (ndata < 10000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100000) / 10000 + 0x30);    //10000位

    if (ndata < 1000) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 10000) / 1000 + 0x30);      //1000位

    if (ndata < 100) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 1000) / 100 + 0x30);        //100位

    if (ndata < 10) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100) / 10 + 0x30);          //10位

    gt_CharacterColor(255, 255, 255);           //文字色：白
    gt_put(ndata % 10 + 0x30);                  //1位
}

///////////////////////////////////////////////////
// 2桁 数値表示(0～99)
// ゼロサプレス：文字色を背景と同じ黒に設定して0表示
// （数字0～9は等幅であるが空白20hは等幅ではないため使えない）
///////////////////////////////////////////////////
void gt_dispnum2(int ndata, int nx, int ny)
{
    gt_setCursor(nx, ny);                       //カーソルセット

    if (ndata < 10) {
        gt_CharacterColor(0, 0, 0);             //文字色：黒
    } else {
        gt_CharacterColor(255, 255, 255);       //文字色：白
    }
    gt_put((ndata % 100) / 10 + 0x30);          //10位

    gt_CharacterColor(255, 255, 255);           //文字色：白
    gt_put(ndata % 10 + 0x30);                  //1位
}


/********************** 関数 **********************/

////////////////////////////////////////////////////
// 1byte送信(非同期シリアル)
////////////////////////////////////////////////////
void gt_put(unsigned char onebyte)
{
    while ( digitalRead(GT_DTR) == HIGH ) {}    //busycheck
    Serial2.write(onebyte);
}
////////////////////////////////////////////////////
// 文字列送信
////////////////////////////////////////////////////
void gt_print(unsigned char message[], int L)
{
    int i;
    for (i = 0; i < L; i++)
    {
        gt_put(message[i]);
    }
}
////////////////////////////////////////////////////
// 画面クリア
////////////////////////////////////////////////////
void gt_clr()
{
    gt_put(0x0C);  //CLR
}
////////////////////////////////////////////////////
// カーソルセット(X:1ドット単位,Y:1ドット単位)
////////////////////////////////////////////////////
void gt_setCursor(int X, int Y)
{
    gt_put(0x1f);
    gt_put(0x24);
    gt_put(X % 0x100);      //X下位バイト
    gt_put(X / 0x100);      //X上位バイト
    gt_put(Y % 0x100);      //Y下位バイト
    gt_put(Y / 0x100);      //Y上位バイト
}
////////////////////////////////////////////////////
// キャラクタコード指定(n=0:USA,1:カタカナ,･･･)
////////////////////////////////////////////////////
void gt_charCode(byte n)
{
    gt_put(0x1b);
    gt_put(0x74);
    gt_put(n);
}
////////////////////////////////////////////////////
// フォントサイズ選択
////////////////////////////////////////////////////
void gt_fontsize(byte m)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x01);
    gt_put(m);          //0(アウトライン),1(6x8),2(8x16),3(12x24),4(16x32)
}
///////////////////////////////////////////////////
// アウトラインフォントサイズ指定
//  yl:Yサイズ
//  ys:文字「サイズ」Y
//  xs:文字「サイズ」X
//  bo:カーソル位置からベースラインのY-オフセット
//  (yLのみ指定、ys=xs=bo=0なら自動計算)
///////////////////////////////////////////////////
void gt_OutlineFontSize(int yl, int ys, int xs, int bo)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x06);
    gt_put(yl % 0x100);     //Yサイズ（高さ）、ピクセル単位（x1h）
    gt_put(yl / 0x100);     //Yサイズ（高さ）、ピクセル単位（x100h）
    gt_put(ys % 0x100);     //文字「サイズ」Y（高さ）、ピクセル単位（x1h）
    gt_put(ys / 0x100);     //文字「サイズ」Y（高さ）、ピクセル単位（x100h）
    gt_put(xs % 0x100);     //文字「サイズ」X（幅）、ピクセル単位（x1h）
    gt_put(xs / 0x100);     //文字「サイズ」X（幅）、ピクセル単位（x100h）
    gt_put(bo % 0x100);     //カーソル位置からベースラインのY-オフセット、ピクセル単位（x1h）
    gt_put(bo / 0x100);     //カーソル位置からベースラインのY-オフセット、ピクセル単位（x100h）
}
///////////////////////////////////////////////////
// アウトライン フォント タイプ選択
//  n=0(日本語),1(韓国語),2(中国簡体字),3(中国繁体字)
///////////////////////////////////////////////////
void gt_OutlineFontTypeSelect(byte n)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x08);
    gt_put(n);
}
////////////////////////////////////////////////////
// キャラクタ拡大（x:xサイズ,y:yサイズ）
////////////////////////////////////////////////////
void gt_fontmag(int x, int y)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x40);
    gt_put(x);      //x=1-4
    gt_put(y);      //y=1-4
}
////////////////////////////////////////////////////
// キャラクタ幅選択
//    m=0:固定幅
//    m=2:プロポーショナル1
//    m=3:プロポーショナル2
//    m=4:プロポーショナル3
////////////////////////////////////////////////////
void gt_fontwidth(byte m)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x04);
    gt_put(m);
}
////////////////////////////////////////////////////
// 横スクロールモード指定＋横スクロールモード速度指定
//  n:スクロール速度(ooh～1Fh)
////////////////////////////////////////////////////
void gt_scrhon(byte n)
{
    gt_put(0x1f);   //横スクロールモード指定
    gt_put(0x03);
    gt_put(0x1f);   //横スクロールモード速度指定
    gt_put(0x73);
    gt_put(n);      //速度
}
////////////////////////////////////////////////////
// 横スクロールモード解除（オーバーライトモード）
////////////////////////////////////////////////////
void gt_scroff()
{
    gt_put(0x1f);   //オーバーライトモード
    gt_put(0x01);
}
////////////////////////////////////////////////////
// リバース指定・解除
////////////////////////////////////////////////////
void gt_reverse(byte n)
{
    gt_put(0x1f);
    gt_put(0x72);
    gt_put(n);      //1:指定、0:解除
}
////////////////////////////////////////////////////
// ウェイト
////////////////////////////////////////////////////
void gt_wait(byte t)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x61);
    gt_put(0x01);
    gt_put(t);      //tx0.47sec
}
////////////////////////////////////////////////////
// ユーザーウィンドウ定義
////////////////////////////////////////////////////
void gt_uwindow(byte a, int xP, int yP, int xS, int yS)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x77);
    gt_put(0x02);
    gt_put(a);              //定義ウィンドウ(1～4)
    gt_put(0x01);           //1:定義
    gt_put(xP % 0x100);     //xPL:ウィンドウ左位置x下位バイト（1ピクセル単位）
    gt_put(xP / 0x100);     //xPH:ウィンドウ左位置x上位バイト（1ピクセル単位
    gt_put(yP % 0x100);     //yPL:ウィンドウ上位置y下位バイト（1ピクセル単位）
    gt_put(yP / 0x100);     //yPH:ウィンドウ上位置y上位バイト（1ピクセル単位）
    gt_put(xS % 0x100);     //xSL:ウィンドウXサイズ下位バイト（1ピクセル単位）
    gt_put(xS / 0x100);     //xSH:ウィンドウXサイズ上位バイト（1ピクセル単位）
    gt_put(yS % 0x100);     //ySL:ウィンドウYサイズ下位バイト（1ピクセル単位）
    gt_put(yS / 0x100);     //ySH:ウィンドウYサイズ上位バイト（1ピクセル単位）
}
////////////////////////////////////////////////////
// カレントウィンドウ選択
////////////////////////////////////////////////////
void gt_cwindow(byte a)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x77);
    gt_put(0x01);
    gt_put(a);      //ウィンドウNo.(0～4)
}
///////////////////////////////////////////////////
// 漢字表示の設定
///////////////////////////////////////////////////
void gt_kanjiOn()
{
    gt_put(0x1f);   //漢字モード指定
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x02);
    gt_put(0x01);
    gt_put(0x1f);   //2バイト文字タイプ選択(日本語)
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x0f);
    gt_put(0x00);
}
///////////////////////////////////////////////////
// 漢字表示の解除
///////////////////////////////////////////////////
void gt_kanjiOff()
{
    gt_put(0x1f);   //漢字モード解除
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x02);
    gt_put(0x00);
}
///////////////////////////////////////////////////
// キャラクタ表示色指定
///////////////////////////////////////////////////
void gt_CharacterColor(byte R, byte G, byte B)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x50);
    gt_put(R);      //R(0-FFh)
    gt_put(G);      //G(0-FFh)
    gt_put(B);      //B(0-FFh)
}
///////////////////////////////////////////////////
// 背景表示色指定
///////////////////////////////////////////////////
void gt_BackgroundColor(byte R, byte G, byte B)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x51);
    gt_put(R);      //R(0-FFh)
    gt_put(G);      //G(0-FFh)
    gt_put(B);      //B(0-FFh)
}
///////////////////////////////////////////////////
// 背景表示 ON/OFF 指定
///////////////////////////////////////////////////
void gt_BackgroundOnOff(byte b)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x67);
    gt_put(0x58);
    gt_put(b);      //1:ON 0:OFF
}
///////////////////////////////////////////////////
// 表示書き込み合成モード指定
///////////////////////////////////////////////////
void gt_WriteMixtureMode(byte mode)
{
    gt_put(0x1f);
    gt_put(0x77);
    gt_put(0x10 + mode);    //0:背景上書き有効(初期値)/1:背景上書き無効
}
////////////////////////////////////////////////////
// テーブルデータ送信
// （n:データ数）
////////////////////////////////////////////////////
void gt_tableput(const uint8_t tablename[], int n)
{
    int k = 0;
    while (k < n ) {
        gt_put (pgm_read_byte_near(tablename + k));
        k++;
    }
}
////////////////////////////////////////////////////
// リアルタイムビットイメージ表示/1bitモノクロ
// （x:1ドット単位, y:1ドット単位）
////////////////////////////////////////////////////
void gt_realtimebitimage1(const uint8_t tablename[], int x, int y)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x66);
    gt_put(0x11);
    gt_put(x % 0x100);  //xL:
    gt_put(x / 0x100);  //xH:
    gt_put(y % 0x100);  //yL:
    gt_put(y / 0x100);  //yH:
    gt_put(0x81);
    int k = 0;
    while (k < (x / 8 * y) ) {
        gt_put (pgm_read_byte_near(tablename + k));
        k++;
    }
}
////////////////////////////////////////////////////
// リアルタイムビットイメージ表示/6bitカラー
// （x:1ドット単位, y:1ドット単位）
////////////////////////////////////////////////////
void gt_realtimebitimage6(const uint8_t tablename[], int x, int y)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x66);
    gt_put(0x11);
    gt_put(x % 0x100);  //xL:
    gt_put(x / 0x100);  //xH:
    gt_put(y % 0x100);  //yL:
    gt_put(y / 0x100);  //yH:
    gt_put(0x86);
    int k = 0;
    while (k < (x * y) ) {
        gt_put (pgm_read_byte_near(tablename + k));
        k++;
    }
}
///////////////////////////////////////////////////
// ボックス描画　
///////////////////////////////////////////////////
void gt_BoxDrawing(byte mode, byte pen, int x1, int y1, int sx, int sy)
{
    int x2 = x1 + sx - 1;
    int y2 = y1 + sy - 1;
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x64);
    gt_put(0x11);
    gt_put(mode);           //mode=0(line),1(box),2(boxfull)
    gt_put(pen);            //pen=0(ピクセル表示ON),1(ピクセル表示OFF)
    gt_put(x1 % 0x100);
    gt_put(x1 / 0x100);
    gt_put(y1 % 0x100);
    gt_put(y1 / 0x100);
    gt_put(x2 % 0x100);
    gt_put(x2 / 0x100);
    gt_put(y2 % 0x100);
    gt_put(y2 / 0x100);
}
///////////////////////////////////////////////////
// タッチパネル制御モード定義
//  座標モード
///////////////////////////////////////////////////
void gt_CoordinatesMode(byte ch)
{
    gt_put(0x1f);
    gt_put(0x50);
    gt_put(0x10);
    gt_put(ch);     //ch=0～3
    gt_put(0x00);
}
///////////////////////////////////////////////////
// タッチパネル制御モード定義
//  マトリクススイッチモード
///////////////////////////////////////////////////
void gt_SwitchMatrixMode(byte ch, byte nx, byte cx, byte ny, byte cy)
{
    gt_put(0x1f);
    gt_put(0x50);
    gt_put(0x10);
    gt_put(ch);         //ch=0～3
    gt_put(0x01);
    gt_put(nx);         //x方向スイッチ数(1～16)
    gt_put(cx);         //x方向クリアランスドット数(1～16)
    gt_put(ny);         //y方向スイッチ数(1～16)
    gt_put(cy);         //y方向クリアランスドット数(1～16)
}
///////////////////////////////////////////////////
// タッチパネル制御モード定義
//  カスタムスイッチモード（ヘッダ部のみ）
///////////////////////////////////////////////////
void gt_CustomSwitchMode(byte ch, byte sn)
{
    gt_put(0x1f);
    gt_put(0x50);
    gt_put(0x10);
    gt_put(ch);         //ch=0～3
    gt_put(0x02);
    gt_put(sn);         //sn=1～32
}
///////////////////////////////////////////////////
// タッチパネル制御モード定義
//  カスタムスイッチモード（スイッチデータ部）
///////////////////////////////////////////////////
void gt_CustomSwitch(int px, int py, int sx, int sy)
{
    gt_put(px % 0x100);
    gt_put(px / 0x100);
    gt_put(py % 0x100);
    gt_put(py / 0x100);
    gt_put(sx % 0x100);
    gt_put(sx / 0x100);
    gt_put(sy % 0x100);
    gt_put(sy / 0x100);
}
///////////////////////////////////////////////////
// タッチパネル制御データ送信許可
///////////////////////////////////////////////////
void gt_TouchDataTransmit(byte m)
{
    gt_put(0x1F);
    gt_put(0x50);
    gt_put(0x20);
    gt_put(m);      //m=0(禁止),1(許可)
}

///////////////////////////////////////////////////
// タッチモード選択
//  シングルタッチモード／マルチタッチモード
///////////////////////////////////////////////////
void gt_TouchModeSelect(byte n)
{
    gt_put(0x1F);
    gt_put(0x50);
    gt_put(0x01);
    gt_put(n);      //n=0(シングルタッチモード),1(マルチタッチモード)
}

///////////////////////////////////////////////////
// タッチパラメータ制御
//  スレッショルド値設定
///////////////////////////////////////////////////
void gt_ThresholdSet(byte thr)
{
    gt_put(0x1F);
    gt_put(0x4B);
    gt_put(0x70);
    gt_put(0x00);
    gt_put(thr);    //thr=00h-FFh(初期値50h)
}

///////////////////////////////////////////////////
// タッチパネル制御チャンネル選択
///////////////////////////////////////////////////
void gt_TouchChannelSelect(byte ch)
{
    gt_put(0x1F);
    gt_put(0x50);
    gt_put(0x21);
    gt_put(ch);     //ch=0-3
}

///////////////////////////////////////////////////
// 定義済みビットイメージ表示(FROM2)
//   f2add : FROM2ビットイメージデータ表示アドレス
//   xS : 定義済みビットイメージXサイズ(1ピクセル単位)
//   xD : ビットイメージ表示Xサイズxサイズ(1ピクセル単位)
//   yD : ビットイメージ表示Xサイズyサイズ(1ピクセル単位)
//   fmt : イメージ フォーマット(81h:モノクロ/90h:16ビットカラー･･･ほか)
///////////////////////////////////////////////////
void gt_CopyFrom(long f2add, int xS, int x, int y, byte fmt)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x66);
    gt_put(0x10);
    gt_put(0x10 + f2add / 0x1000000);   //m(10h～1Fh)
    gt_put(f2add % 0x100);              //aL
    gt_put((f2add / 0x100) % 0x100);    //aH
    gt_put((f2add / 0x10000) % 0x100);  //aE
    gt_put(xS % 0x100);                 //xSL
    gt_put(xS / 0x100);                 //xSH
    gt_put(x % 0x100);                  //xL
    gt_put(x / 0x100);                  //yL
    gt_put(y % 0x100);                  //yL
    gt_put(y / 0x100);                  //yH
    gt_put(fmt);                        //fmt
}

///////////////////////////////////////////////////
// 定義済みビットイメージ表示(VRAMからコピー)
//   vx  : コピー元のx座標(0～959/1599)
//   vy  : コピー元のy座標(0～544/959)
//   vxs : 画像xサイズ
//   vys : 画像yサイズ
//   vramx : VRAMのXサイズ(GT800X480=1600,GT480X272=960)
///////////////////////////////////////////////////
void gt_CopyVram(long vx, long vy, int vxs, int vys, int vramx)
{
    long vadd = vy * vramx + vx;    //VRAMビットイメージデータ表示アドレス
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x66);
    gt_put(0x10);
    gt_put(0x02);                   //m=02h ：表示メモリ内ビットイメージ
    gt_put(vadd % 0x100);           //aL
    gt_put((vadd / 0x100) % 0x100); //aH
    gt_put(vadd / 0x10000);         //aE
    gt_put(vramx % 0x100);          //xSL
    gt_put(vramx / 0x100);          //xSH
    gt_put(vxs % 0x100);            //xL
    gt_put(vxs / 0x100);            //yL
    gt_put(vys % 0x100);            //yL
    gt_put(vys / 0x100);            //yH
    gt_put(0x98);                   //fmt=98h: 24ビット カラーフォーマット(表示メモリからの読み出し)
}

///////////////////////////////////////////////////
// 書き込み画面モード選択
//  mode = 0:表示画面モード(初期値) 1:全画面モード
///////////////////////////////////////////////////
void gt_WriteScreenMode(byte mode)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x77);
    gt_put(0x10);
    gt_put(mode);
}

///////////////////////////////////////////////////
// ポート入出力選択
//  n=ポートNo. , a=入出力設定(bitHigh出力)
///////////////////////////////////////////////////
void gt_PortSetting(byte n, byte a)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x70);
    gt_put(0x01);
    gt_put(n);
    gt_put(a);
}

///////////////////////////////////////////////////
// ポート出力
//  n=ポートNo. , a=出力
///////////////////////////////////////////////////
void gt_PortOutput(byte n, byte a)
{
    gt_put(0x1f);
    gt_put(0x28);
    gt_put(0x70);
    gt_put(0x10);
    gt_put(n);
    gt_put(a);
}