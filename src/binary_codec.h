#ifndef BINARY_CODEC_H_
#define BINARY_CODEC_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include "evpp/buffer.h"
#include "evpp/tcp_conn.h"
#include "evpp/timestamp.h"
#include "zlog.h"

#include "protocol_binary.h"



using namespace std;






class BinaryCodec {
public:

    BinaryCodec();
    virtual ~BinaryCodec();

    void init_para();
	void OnPreDealMessage(const evpp::TCPConnPtr& conn,evpp::Buffer* buf);
	bool veriftyHeaderMsg(protocol_receive_data_header &headers,uint8_t* msg,UINT16 &msgType,UINT32 &code);
	bool veriftyPostionMsg(protocol_receive_msg_postion_body &traj,uint8_t*  msg,int &len);
	bool veriftyDailyOperMsg(PackDailyOper &dailyOper,uint8_t*  msg);
	bool veriftyMonthlyOperMsg(PackMonthlyOper &monthlyOper,uint8_t*  msg);
	bool veriftyDriverCardOperMsg(PackDriverCard &driverCard,uint8_t*  msg);
	bool veriftyDriverViolateOperMsg(PackDriverViolate &driverViolate,uint8_t*  msg);
	UINT32 veriftyVehOperateMsg(PackOperator &vehOper,uint8_t*  msg);
	bool getLNSYRetMsg(protocol_receive_data_header headers, byte *ret,int &retmsglen);


	bool veriftyADASMsg(PackADASAlarm &ADASMsg,uint8_t*  msg,int &len);


   // void OnProcessLNSYCZData(const evpp::TCPConnPtr& conn,uint8_t* msgbody,int RecMsgType,protocol_receive_data_header LNSYheader,protocol_send_data_header send_header);

	char getLNSYMsgCode();
	void setLNSYMsgCode(char res);
	void getsubDailyOperMsg(UINT32 &value,char *submsg);
	void getLNSYMsgHeader(protocol_receive_data_header &head);
	short getLNSYRetMsgCRC(byte* ret,int len);

    int crc16_ccitt(byte ptr[], int len);
    short getCrc(byte data[]) ;
    int hex_char_value(char c);


    int GetModuleFileName(char* name, int size);


    //static BinaryCodec* instance();









private:
    // noncopyable
    //BinaryCodec(const BinaryCodec&);
//    void DecodePrefixGetPacket(const protocol_binary_response_header& resp,
//                               evpp::Buffer* buf, std::string& key, CommandPtr& cmd);
//    const BinaryCodec& operator=(const BinaryCodec&);
//
//    void OnResponsePacket(const protocol_binary_response_header& resp,
//                          evpp::Buffer* buf);
private:

    //static BinaryCodec* codec;

    char   result;

    UrbanCount  last_send_count;
    UrbanCount  send_count;
    UrbanCount  rec_count;
    UINT32 GetTickCount();
    UINT32 g_szCheckLastTime ;
    void urbanVehCount(int recive_sn,int recive_code);

};

#endif



