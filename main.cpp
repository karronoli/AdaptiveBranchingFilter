/** \mainpage
* \Author �A�i���O�E�f�o�C�Z�Y�������
* \date 2006.Sep.8
*
* ���̃v���O�����́AADSP-BF537�𓋍ڂ���]����AEZ-KIT BF537�̃f���E�v���O�����ł���B
*
* ::main �֐������s����ƁA�e��̏��������s���A���荞�݃n���h�� afw::rxISR��o�^������A
* ���荞�݂�DMA�������ăI�[�f�B�I�M���̑���M���s���B�I�[�f�B�I�M����SPORT0�o�R�œ���
* ���ADMA�ɂ���ă�������Ɋi�[�����B2����DMA�̊e�s���I��邽�тɃI�[�f�B�I�M����M���荞��
* ���������A afw::rxISR���Ăяo�����B
*
* afw::rxISR�͊��荞�݂��N���A���A�g���v���o�b�t�@�̐�����s���B�����Ă�����ׂ��f�[�^��
* DMA�̎�M�o�b�t�@������o���ƁA�g���₷���悤���ׂȂ����ă��[�U�[��`�̃R�[���o�b�N�֐�
* afw::processData���Ăяo���B
*
* afw::processData�͎��ۂɐM���������s���ꏊ�ł��邪�A���̃A�v���P�[�V�����̓t���[�����[�N��
* ����A�����̕����͋�ɂȂ��Ă���B���[�U�[�͂��̋�̕��������R�ɏ��������Ď��g��
* �A�v���P�[�V�����ɕς���B afw::processData �͑��M�������f�[�^������Ȃ�Ώo�̓o�b�t�@�Ɋi�[���ČĂяo���֐��ɖ߂�B
* 
* �Ăяo�����֐��ł��� afw::rxISR�͏o�̓f�[�^����בւ��đR��ׂ�DMA�o�b�t�@��
* �i�[����B
*
* EZ-KIT Lite BF-537�̃f�B�b�v�X�C�b�`�̐ݒ�͈ȉ��̂Ƃ���
   - SW1 : ���ׂ�OFF
   - SW7 : ���ׂ�ON
   - SW8 : 1-2��ON�A�c��͂��ׂ�OFF
*/

/** \file main.cpp
 *  \brief ���C���֐���`�t�@�C��
 *  \author �A�i���O�E�f�o�C�Z�Y�������
 *  \version 1.0
 *  \date    2006.Sep.8
 *
 * ���̃t�@�C����VisualDSP++ 4.5�̃T���v���E�A�v���P�[�V�����ATalkTrhough����
 * �h�����������̂ł���BEZ-KIT BF537�̃��\�[�X�����������A48Ksample/S�̃I�[�f�B�I
 * �t���[�����[�N���N������B��M�f�[�^�̓R�[���o�b�N�֐� afw::processData �ɂ����
 * ���������B
 */
#include <ccblkfn.h>
#include "afw.h"
#include "utilbf537.h"

#include <cdefBF53x.h>                  // ���W�X�^�E�A�h���X�̃C���N���[�h
#include <signal.h>                     // �����݊֘A�̃C���N���[�h

#include <climits>
#include <stdio.h>

//const int led_offset = 6;
//const int led_all = 0x3F << led_offset;
const int sw10 = (1<<5);
const int sw11 = (1<<4);
extern void printParameter(int signal);
extern void changeParameter(int signal);


/** ���C���֐�
   * 
   * �e�y���t�F��������������A���荞�ݑ҂��ɓ���B
   */
//#include <btc.h>

void main(void)
{
    
	utilbf537::initPLL();   // PLL��ݒ肷��

    afw::init();            // �I�[�f�B�I�E�t���[�����[�N������
    afw::startAudio();      // �I�[�f�B�I�����J�n

    // afw::init()��FER,DIR,INEN�͏�������
    //*pPORTF_FER    &= ~sw10 & ~sw11 & led_all;
    // EZ-KIT Lite��SW10�������ݓ���,���͂ɐݒ�. LED1&2���X�e�[�^�X�\���Ɏg��,�o�͂ɐݒ�
    //*pPORTFIO_DIR &= ~sw10 & ~sw11 & led_all;
    //*pPORTFIO_INEN |=  sw10 | sw11;   // ���̓o�b�t�@��L���ɂ���
    //*pPORTFIO_CLEAR =  sw10 | sw11 | led_all;   // �O�̂��ߊ����ݗv�����N���A���Ă���&LED������

    *pPORTFIO_EDGE |=  sw10 | sw11;   // �G�b�W���荞�݂ɐݒ�
    *pPORTFIO_MASKA_SET = sw10;         // ������A��I��
    // ���荞�݂̋��BPORTF���荞��A�̏��ʂ̓f�t�H���g��12(IVG12)
    *pSIC_IMASK |= IRQ_PFA_PORTFG;      // SIC_IMASK��PORTF������A��������
    interrupt(SIGIVG12, &printParameter);   // �����݃n���h���o�^.����12(IVG12)
    *pPORTFIO_MASKB_SET = sw11;         // ������B��I��
    // ���荞�݂̋��BPORTF���荞��B�̏��ʂ̓f�t�H���g13(IVG13)
    *pSIC_IMASK |= IRQ_PFB_PORTF ;      // SIC_IMASK��PORTF������B��������
    interrupt(SIGIVG13, &changeParameter); // �����݃n���h���o�^.����13(IVG13)
    
    while(1)
        idle();             // ���荞�ݑ҂�
    // BF537�p��libbtc532.dlb�������N
    //btc_init();
    //while(1)
        //btc_poll();    
}


    // �����݃n���h��
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
    // LED�̏�Ԃ𔽓]
	//*pPORTFIO_TOGGLE = 4 << led_offset;            // Flag�s���̃g�O��
    printf("Count:%d, Myu:%f\n", param::Count, (float)param::Myu);
	    // ���荞�݂��N���A
	*pPORTFIO_CLEAR  = sw10;            // ���荞�݂̃N���A
	ssync();                            // �m���ɃN���A�����܂ő҂�
}


void changeParameter(int signal)
{
  const int led_offset = 6;
  const int number_of_output_types = 6;
  param::Type = (param::Type+1) % number_of_output_types;
  //*pPORTFIO = param::Type << led_offset; // �Ȃ����o�͂��Ȃ��Ȃ�
  *pPORTFIO_CLEAR  = sw11;            // ���荞�݂̃N���A   
  ssync();                            // �m���ɃN���A�����܂ő҂�
}
