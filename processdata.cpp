#include "afw.h"
//#include <stdio.h>
//#include <stdlib.h>
//#include <shortfract>
//extern fract Myu;
fract Myu;
#include <btc.h>
/*
fract plotBuffer[3000];
BTC_MAP_BEGIN
BTC_MAP_ENTRY("plotBuffer", (long)&plotBuffer, sizeof(plotBuffer))
BTC_MAP_ENTRY("Myu", (long)&Myu, sizeof(Myu))
BTC_MAP_END
*/

#include "AdaptiveBranchingFilter.cpp"
//#include "classIIR_Direct2.cpp"
//#include "classIIR_Direct2_Branch.cpp"

/** \file processdata.cpp
 *  \brief オーディオコールバック関数
 *  \author アナログ・デバイセズ株式会社
 *  \version 1.0
 *  \date    2006.Sep.8
 *
 * このファイルは、オーディオ処理のコールバック関数を含む。
 */
namespace afw{
    /*
    const int ORDER = 7;
    const float amD[ORDER] =
        {  6.4824931281E+00, -1.8158778276E+01,  2.8482078515E+01,
        -2.7008235419E+01,  1.5479818828E+01, -4.9646327290E+00,
        6.8723598287E-01};
        
    const float bmD[ORDER+1] =
        {  5.8004454915E-03, -2.6712907186E-02,  4.5606643993E-02,
        -2.4684197588E-02, -2.4684197588E-02,  4.5606643993E-02,
        -2.6712907186E-02,  5.8004454915E-03};
    IIR_Direct2<float, ORDER>* dblLPF;
    */
    
    AdaptiveBranchingFilter<fract>* abf;
    //AdaptiveBranchingFilter<float>* abf;
/** コールバック関数の処理化
    *
    * afw::processData()関数を呼ぶ前に初期化の必要がある場合は、この関数内部で行う。
*/  
    //FILE *fp;
    fract* myu_buffer;
	void initProcessData(void)
	{
        // ここでprocessData用の初期化を必要に応じて行う
        //fp = fopen("/tmp/b.txt", "rw");    
        //dblLPF  = new IIR_Direct2<float, ORDER>(amD, bmD);  // using double type  
        //abf = new AdaptiveBranchingFilter<float>;
        
        abf = new AdaptiveBranchingFilter<fract>;
	}
	
    
/** オーディオ・コールバック関数
   *
   * @param leftIn 左チャンネルの入力データバッファ
   * @param rightIn 右チャンネルの入力データバッファ
   * @param leftOut 左チャンネルの出力データバッファ
   * @param rightOut 右チャンネルの出力データバッファ
   * @param count 各バッファに格納されているオーディオデータの数
   *
   * この関数はDMAが受信バッファをいっぱいにするたびに割り込み経由で呼ばれる。
   * 引数として渡されるのは受信データを含む入力バッファのほか、送信データを
   * 格納するために使う出力バッファとデータ個数である。
   * ユーザーは関数内部で入力バッファのデータを加工して、出力バッファに送信すべき
   * データをおく。呼び出しに先立って初期化が必要な場合は、 afw::initProcessData()
   * 関数で行う。
   *
   * 配列はshortfract型である。この16bit固定小数点型はshortと同じ大きさを
   * 持つが、四則は固定小数点領域で定義されている。
   * 
   * for文は常に必要なので、改造する場合はfor文の内側だけを書き換える。引数
   * である配列の各要素は、オーディオのサンプルデータが順番に格納されたもの
   * である。したがって、これらを順番に処理することで、オーディオ処理を実装
   * できる。
   * 
   * 割り込みソースのクリアなどは、この関数の呼び出し側で行うので、関数内で
   * 処理する必要はない。
   */
    
    void processData( 
                      const shortfract leftIn[], 
                      const shortfract rightIn[], 
                      shortfract leftOut[], 
                      shortfract rightOut[],
                      int count
    				 )
    {    
        
        
        myu_buffer = new fract[count];
        
  
        //float tmp;
        shortfract tmp;
        fract myu;
        for ( int i=0; i<count; i++ ){      // 引数配列のすべてのデータを処理する。
            // ここで信号処理を行う。
            
            tmp = leftIn[i];
            
            Myu = abf->getMyu();
            myu_buffer[i] = abf->getMyu();
            abf->Execute(fr16_to_fr32(tmp));
            abf->Adapt();
            //fwrite(&myu, sizeof(myu), 1, fp);
            //fwrite("\n", sizeof("\n"), 1, fp);    
       
	        switch (param::Type) {
	         case 0:
	           //leftOut[i] = tmp;
	           //rightOut[i] = leftIn[i];
	           leftOut[i] = abf->getLow();
	           //leftOut[i] = abf->getHigh();
	           rightOut[i] = abf->getHigh();
	           //rightOut[i] = abf->getLow();
	         break;
	         case 1:
	           leftOut[i] = abf->getLow() + abf->getHigh();
	           rightOut[i] = tmp;
	         break;
	         case 2:
	           leftOut[i] = abf->getLow();
	           rightOut[i] = tmp;
	         break;
	         case 3:
	           leftOut[i] = abf->getHigh();
	           rightOut[i] = tmp;
	         break;
	         case 4:
	           rightOut[i] = tmp;
	           tmp = abf->getHigh();
	           leftOut[i] = tmp * tmp;
	         break;
	         default:
	           tmp = abf->getLow();
	           leftOut[i] = tmp * tmp;
	           tmp = abf->getHigh();
	           rightOut[i] = tmp * tmp;
	         break;
	        }
            
	        
	        
        }
        
        //btc_write_value(0, (unsigned int *)myu_buffer, sizeof(myu_buffer));
        delete[] myu_buffer;
        //fclose(fp);	        
        //delete abf;
    }   // processData

};  // namespace afw

