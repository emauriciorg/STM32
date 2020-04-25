#ifndef __PN532_H__
#define __PN532_H__


typedef struct {
		uint8_t start_frame[3];     /*0x00 0x00 0xff*/
		uint8_t len;                /*no of bytes  tfi , from PD0 to PDn  */
		uint8_t lcs;                /*length checksum  Lower byte of [LEN + LCS] = 0x00*/
		uint8_t tfi;                /*Frame identifier: d4 from host to PN532 , d5 from pn532 to host*/
		uint8_t payload;             /*LEN-1 bytes packet data, the first byte is the command code CC*/
		uint8_t dcs;                /*datacheck sum, byte that satifaces the relation  lowbyte of[TFI+PD0+PD1+PDn+ DCS] =0X00*/
		uint8_t post_amble; /*0x00*/
//65


}pn532_message_t;  /*more formats page 29*/

void get_fw_version(void);
void pn532_listener(void);
void pn532_init(void);
void pn532_isr(UART_HandleTypeDef *huart);
void pn532_wake_up(void);
void get_fw_version_wk(void);

void get_status(void);
void pn532_list_targets(void);
void pn532_sam_config(void);



//typedef struct{
	/*0x00  0x00  0xff  0x00  0xff 0x00  ACK*/
	/*0x00  0x00  0xff  0xff  0x00 0x00  NACK*/
	/*0x00  0x00  0xff  0x01  0xff 0x7f 0x81 0x00  0x error frame*/
	/*page 31*/
//}ack_frame_t;

//00 FF 02 FE D4 02 2A
//LEN 2
//LCS FE
//pd0 D4
//pd1 02
//  0x02+ 0xd4 --> 212 + 0x02 = 214 , d6

#endif /* __PN532_H__ */
