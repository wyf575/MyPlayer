

#ifndef __ISO_MIFARE_H_
#define __ISO_MIFARE_H_

#include "nz3802_cfg.h"

/*! 
 * PCD command set.
 */
typedef enum
{
    MiCMD_REQIDL     = 0x26,     //Ѱ��������δ��������״̬
    MiCMD_REQALL     = 0x52,     //Ѱ��������ȫ����
    MiCMD_ANTICOLL1  = 0x93,      //����ײ
    MiCMD_ANTICOLL2  = 0x95,      //����ײ
    MiCMD_AUTHENT1A  = 0x60,      //��֤A��Կ
    MiCMD_AUTHENT1B  = 0x61,      //��֤B��Կ
    MiCMD_READ       = 0x30,      //����
    MiCMD_WRITE      = 0xA0,      //д��
    MiCMD_DECREMENT  = 0xC0,      //�ۿ�
    MiCMD_INCREMENT  = 0xC1,      //��ֵ
    MiCMD_RESTORE    = 0xC2,      //�������ݵ�������
    MiCMD_TRANSFER   = 0xB0,      //���滺����������
    MiCMD_HALT       = 0x50,      //����
    MiCMD_RESET      = 0xE0,      //��λ
}mifareCommand_t;


#define MIFARE_MAX_UID_LENGTH     10
#define MIFARE_TAG_TYPE_LENGTH     2

/*!< 
 * struct representing an MIFARE CARD as returned by
 * #MifareRequest.
 */
typedef struct  //_mifareProximityCard_t
{
    u8 Uid[MIFARE_MAX_UID_LENGTH]; /*<! UID of the MIFARE CARD */
    u8 UidLength;     /*!< UID length */
    u8 TagType[MIFARE_TAG_TYPE_LENGTH];/*!< Tag Type */
    u8 Sak;           /*!< SAK byte */
}mifareProximityCard_t;


/*!< 
 * struct mifare TranScive Buffer
 */
typedef struct //_mifareTranSceiveBuffer_t
{
    u8  MfCommand;
    u16 MfLength;
    u8  MfData[64];
}mifareTranSceiveBuffer_t;

extern u8 MifareInitialize(void);
extern u8 MifareInitialize_3v(void);
//Ѱ��
extern u8 MifareRequest(mifareCommand_t cmd, mifareProximityCard_t *MiCard);
//����ײ
extern u8 MifareAnticoll(u8 bcnt, mifareProximityCard_t *MiCard);
//ѡ��һ�ſ�
extern u8 MifareSelect(mifareProximityCard_t *miCard);
//��֤����Կ
extern u8 MifareAuthentication(mifareCommand_t cmd, mifareProximityCard_t *MiCard, u8 block, u8 *pKey);
//��mifare_one����һ��(block)����(16�ֽ�)
extern u8 MifareRead(u8 block, u8 *pData, u8 *pLen);
//д��MifareOne��һ������
extern u8 MifareWrite(u8 block, u8 *pData);
//��ʼ��ΪǮ��
extern u8 MifareInitPurse(u8 block, u8 *InitAmount);
//��ȡǮ��ֵ
extern u8 MifareReadPurse(u8 block, u8 *Amount);
//��ֵ
extern u8 MifareIncrement(u8 block, u8 *Amount);
//�ۿ�
extern u8 MifareDecrement(u8 block, u8 *Amount);
//��mifare_one��һ�����ݻش�
extern u8 MifareRestore(u8 block);
//���������������ݴ����ƶ��Ŀ�
extern u8 MifareTransfer(u8 block);
//����HALT״̬
extern u8 MifareHalt(void);


extern u8 MifarePresent(u8 value);

#endif

