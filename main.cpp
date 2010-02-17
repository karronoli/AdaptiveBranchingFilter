/** \mainpage
* \Author アナログ・デバイセズ株式会社
* \date 2006.Sep.8
*
* このプログラムは、ADSP-BF537を搭載する評価基板、EZ-KIT BF537のデモ・プログラムである。
*
* ::main 関数を実行すると、各種の初期化を行い、割り込みハンドラ afw::rxISRを登録した後、
* 割り込みとDMAを許可してオーディオ信号の送受信を行う。オーディオ信号はSPORT0経由で入力
* し、DMAによってメモリ上に格納される。2次元DMAの各行が終わるたびにオーディオ信号受信割り込み
* が発生し、 afw::rxISRが呼び出される。
*
* afw::rxISRは割り込みをクリアし、トリプルバッファの制御を行う。そしてしかるべきデータを
* DMAの受信バッファから取り出すと、使いやすいよう並べなおしてユーザー定義のコールバック関数
* afw::processDataを呼び出す。
*
* afw::processDataは実際に信号処理を行う場所であるが、このアプリケーションはフレームワークで
* あり、処理の部分は空になっている。ユーザーはこの空の部分を自由に書き直して自身の
* アプリケーションに変える。 afw::processData は送信したいデータがあるならば出力バッファに格納して呼び出し関数に戻る。
* 
* 呼び出した関数である afw::rxISRは出力データを並べ替えて然るべきDMAバッファに
* 格納する。
*
* EZ-KIT Lite BF-537のディップスイッチの設定は以下のとおり
   - SW1 : すべてOFF
   - SW7 : すべてON
   - SW8 : 1-2をON、残りはすべてOFF
*/

/** \file main.cpp
 *  \brief メイン関数定義ファイル
 *  \author アナログ・デバイセズ株式会社
 *  \version 1.0
 *  \date    2006.Sep.8
 *
 * このファイルはVisualDSP++ 4.5のサンプル・アプリケーション、TalkTrhoughから
 * 派生させたものである。EZ-KIT BF537のリソースを初期化し、48Ksample/Sのオーディオ
 * フレームワークを起動する。受信データはコールバック関数 afw::processData によって
 * 処理される。
 */
#include <ccblkfn.h>
#include "afw.h"
#include "utilbf537.h"

#include <cdefBF53x.h>                  // レジスタ・アドレスのインクルード
#include <signal.h>                     // 割込み関連のインクルード

#include <climits>
#include <stdio.h>

//const int led_offset = 6;
//const int led_all = 0x3F << led_offset;
const int sw10 = (1<<5);
const int sw11 = (1<<4);
extern void printParameter(int signal);
extern void changeParameter(int signal);


/** メイン関数
   * 
   * 各ペリフェラルを初期化後、割り込み待ちに入る。
   */
//#include <btc.h>

void main(void)
{
    
	utilbf537::initPLL();   // PLLを設定する

    afw::init();            // オーディオ・フレームワーク初期化
    afw::startAudio();      // オーディオ処理開始

    // afw::init()でFER,DIR,INENは初期化済
    //*pPORTF_FER    &= ~sw10 & ~sw11 & led_all;
    // EZ-KIT LiteのSW10を割込み入力,入力に設定. LED1&2をステータス表示に使う,出力に設定
    //*pPORTFIO_DIR &= ~sw10 & ~sw11 & led_all;
    //*pPORTFIO_INEN |=  sw10 | sw11;   // 入力バッファを有効にする
    //*pPORTFIO_CLEAR =  sw10 | sw11 | led_all;   // 念のため割込み要求をクリアしておく&LEDを消灯

    *pPORTFIO_EDGE |=  sw10 | sw11;   // エッジ割り込みに設定
    *pPORTFIO_MASKA_SET = sw10;         // 割込みAを選択
    // 割り込みの許可。PORTF割り込みAの順位はデフォルトで12(IVG12)
    *pSIC_IMASK |= IRQ_PFA_PORTFG;      // SIC_IMASKのPORTF割込みAを許可する
    interrupt(SIGIVG12, &printParameter);   // 割込みハンドラ登録.順位12(IVG12)
    *pPORTFIO_MASKB_SET = sw11;         // 割込みBを選択
    // 割り込みの許可。PORTF割り込みBの順位はデフォルト13(IVG13)
    *pSIC_IMASK |= IRQ_PFB_PORTF ;      // SIC_IMASKのPORTF割込みBを許可する
    interrupt(SIGIVG13, &changeParameter); // 割込みハンドラ登録.順位13(IVG13)
    
    while(1)
        idle();             // 割り込み待ち
    // BF537用にlibbtc532.dlbをリンク
    //btc_init();
    //while(1)
        //btc_poll();    
}


    // 割込みハンドラ
#include "myparam.cpp"

fract param::Alpha = 0;
unsigned long param::Count = 0;
unsigned short param::Type = 0;
fract param::Psi = 0;
fract param::Myu = 0;
fract param::In = 0;
fract param::outLow = 0;
fract param::outHigh = 0;
fract param::apf = 0.0r;

void printParameter(int signal)
{
    // LEDの状態を反転
	//*pPORTFIO_TOGGLE = 4 << led_offset;            // Flagピンのトグル
    printf("Count:%d, Myu:%f\n", param::Count, (float)param::Myu);
	    // 割り込みをクリア
	*pPORTFIO_CLEAR  = sw10;            // 割り込みのクリア
	ssync();                            // 確実にクリアされるまで待つ
}


void changeParameter(int signal)
{
  const int led_offset = 6;
  const int number_of_output_types = 6;
  param::Type = (param::Type+1) % number_of_output_types;
  //*pPORTFIO = param::Type << led_offset; // なぜか出力がなくなる
  *pPORTFIO_CLEAR  = sw11;            // 割り込みのクリア   
  ssync();                            // 確実にクリアされるまで待つ
}
