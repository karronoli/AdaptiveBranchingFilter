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
 *  \brief �I�[�f�B�I�R�[���o�b�N�֐�
 *  \author �A�i���O�E�f�o�C�Z�Y�������
 *  \version 1.0
 *  \date    2006.Sep.8
 *
 * ���̃t�@�C���́A�I�[�f�B�I�����̃R�[���o�b�N�֐����܂ށB
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
/** �R�[���o�b�N�֐��̏�����
    *
    * afw::processData()�֐����ĂԑO�ɏ������̕K�v������ꍇ�́A���̊֐������ōs���B
*/  
    //FILE *fp;
    fract* myu_buffer;
	void initProcessData(void)
	{
        // ������processData�p�̏�������K�v�ɉ����čs��
        //fp = fopen("/tmp/b.txt", "rw");    
        //dblLPF  = new IIR_Direct2<float, ORDER>(amD, bmD);  // using double type  
        //abf = new AdaptiveBranchingFilter<float>;
        
        abf = new AdaptiveBranchingFilter<fract>;
	}
	
    
/** �I�[�f�B�I�E�R�[���o�b�N�֐�
   *
   * @param leftIn ���`�����l���̓��̓f�[�^�o�b�t�@
   * @param rightIn �E�`�����l���̓��̓f�[�^�o�b�t�@
   * @param leftOut ���`�����l���̏o�̓f�[�^�o�b�t�@
   * @param rightOut �E�`�����l���̏o�̓f�[�^�o�b�t�@
   * @param count �e�o�b�t�@�Ɋi�[����Ă���I�[�f�B�I�f�[�^�̐�
   *
   * ���̊֐���DMA����M�o�b�t�@�������ς��ɂ��邽�тɊ��荞�݌o�R�ŌĂ΂��B
   * �����Ƃ��ēn�����͎̂�M�f�[�^���܂ޓ��̓o�b�t�@�̂ق��A���M�f�[�^��
   * �i�[���邽�߂Ɏg���o�̓o�b�t�@�ƃf�[�^���ł���B
   * ���[�U�[�͊֐������œ��̓o�b�t�@�̃f�[�^�����H���āA�o�̓o�b�t�@�ɑ��M���ׂ�
   * �f�[�^�������B�Ăяo���ɐ旧���ď��������K�v�ȏꍇ�́A afw::initProcessData()
   * �֐��ōs���B
   *
   * �z���shortfract�^�ł���B����16bit�Œ菬���_�^��short�Ɠ����傫����
   * �����A�l���͌Œ菬���_�̈�Œ�`����Ă���B
   * 
   * for���͏�ɕK�v�Ȃ̂ŁA��������ꍇ��for���̓�������������������B����
   * �ł���z��̊e�v�f�́A�I�[�f�B�I�̃T���v���f�[�^�����ԂɊi�[���ꂽ����
   * �ł���B���������āA���������Ԃɏ������邱�ƂŁA�I�[�f�B�I����������
   * �ł���B
   * 
   * ���荞�݃\�[�X�̃N���A�Ȃǂ́A���̊֐��̌Ăяo�����ōs���̂ŁA�֐�����
   * ��������K�v�͂Ȃ��B
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
        for ( int i=0; i<count; i++ ){      // �����z��̂��ׂẴf�[�^����������B
            // �����ŐM���������s���B
            
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

