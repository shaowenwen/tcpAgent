/*
 * PkgDataToJson.h
 *
 *  Created on: 2018-11-6
 *      Author: root
 */

#ifndef PKGDATATOJSON_H_
#define PKGDATATOJSON_H_

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include <stdio.h>

#include <string>
#include "protocol_binary.h"
#include "cJSON.h"
#include <time.h>
#include <sys/time.h>

#include <iconv.h>
using namespace std;




class PkgDataToJson {
public:
	PkgDataToJson();
	virtual ~PkgDataToJson();

	void getgpsStamp(byte *inVal,byte *outVal);
	void gettaxiStamp(byte *inVal,byte *outVal);
	void getregStamp(byte *inVal,byte *outVal);
	void PkgPostionToJson(protocol_send_data_header rechead,protocol_receive_msg_postion_body packtraj,int len,string *out_str);
	void PkgPostionAlarmStateToJson(cJSON* root,UINT32 Val);
	void PkgPostionVehStateToJson(cJSON* root,UINT32 Val);
	void PkgVEHOperatorToJson(protocol_send_data_header rechead,PackOperator& packoperator,int len,string * out_str);
	void PkgDailyOperatorToJson(protocol_send_data_header& rechead,PackDailyOper& packdailyoperator,string * out_str);
	void PkgMonthlyOperatorToJson(protocol_send_data_header& rechead,PackMonthlyOper& packmonthlyoperator,string out_str);
	void PkgDriverCardToJson(protocol_send_data_header& rechead,PackDriverCard& packdrivercard,string * out_str);
	void PkgDriverViolateToJson(protocol_send_data_header& rechead,PackDriverViolate& packdriverviolate,string * out_str);
	void PkgADASToJson(protocol_send_data_header rechead,PackADASAlarm packadas,int len,string *out_str);

	int code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen);
	int u2g(char *inbuf,int inlen,char *outbuf,int outlen);
	int g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen);
	void Hex2Str( const char *sSrc,  char sDest[], int nSrcLen );
	void Hex2Str_zero( const char *sSrc,  char sDest[], int nSrcLen );
	void getg2uvalue(byte *inVal,byte *outVal);

	void GetLocalTime(SYSTEMTIME* lpSystemTime);

	int getADASParaValue(byte *inVal,int len);

};

#endif /* PKGDATATOJSON_H_ */
