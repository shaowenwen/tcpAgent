#include "binary_codec.h"

extern zlog_category_t* g_server_cat;
extern zlog_category_t* g_server_cat_count;
using namespace std;




BinaryCodec::BinaryCodec()
{


	result = 0;

}


BinaryCodec::~BinaryCodec(){

//	delete kp_gps;
//	delete kp_alarm;
//	delete kp_vehService;
//	delete kp_dailyService;
//	delete kp_monthlyService;
//	delete kp_driverCard;
//	delete kp_driverViolate;
//
//	delete

}

void BinaryCodec::init_para()
{
    cout << "BinaryCodec::init_para()" <<endl;



	printf("0k\n");

}


char BinaryCodec::getLNSYMsgCode()
{
	return result;
}


void BinaryCodec::setLNSYMsgCode(char res)
{
	result = res;
}


//BinaryCodec* BinaryCodec::instance() {
//    return BinaryCodec::codec;
//}


void BinaryCodec::getLNSYMsgHeader(protocol_receive_data_header &head)
{

	head.receive_data_header.msg_length = htonl(0x1b);
	head.receive_data_header.msg_sn=htonl(head.receive_data_header.msg_sn);
	head.receive_data_header.msg_id=htons((head.receive_data_header.msg_id&0xDFFF)|0x8000);
	head.receive_data_header.msg_gnsscenterid=htonl(head.receive_data_header.msg_gnsscenterid);
	memcpy(&head.receive_data_header.version_flag,&head.receive_data_header.version_flag,3);
	head.receive_data_header.encrypt_key=htonl(head.receive_data_header.encrypt_key);

}





bool BinaryCodec::getLNSYRetMsg(protocol_receive_data_header headers, byte *ret,int &retmsglen)
{
	byte retmsg[32];
	byte outbBuf[32];
	char strretCode =getLNSYMsgCode();
	printf("strretCode:%d,\n",strretCode);
	getLNSYMsgHeader(headers);
	retmsg[0]=0x5b;
	memcpy(&retmsg[1],&headers.receive_data_header.msg_length,4);
	memcpy(&retmsg[5],&headers.receive_data_header.msg_sn,4);
	memcpy(&retmsg[9],&headers.receive_data_header.msg_id,2);
	memcpy(&retmsg[11],&headers.receive_data_header.msg_gnsscenterid,4);
	memcpy(&retmsg[15],&headers.receive_data_header.version_flag,3);
	memcpy(&retmsg[18],&headers.receive_data_header.encrypt_flag,1);
	memcpy(&retmsg[19],&headers.receive_data_header.encrypt_key,4);



	printf("sizeof(retHead):%d,\n",sizeof(headers));
	retmsg[23] = strretCode;

	//crc
	byte retBodyMsg[28];
	memcpy(retBodyMsg,&retmsg[1],23);

	int crc_value = crc16_ccitt(retBodyMsg, 23);

//	printf("crc_value:%x",crc_value);

//	char crc_low;
//	sprintf(crc_low,"%x",intValue);
//
//	char crc_high;
//	sprintf(crc_high,"%x",intValue);

	retmsg[23 + 1] = (crc_value>>8)&0x00ff ;
	retmsg[23 + 2] = crc_value&0x00ff;
	retmsg[23 + 3] =0x5d;
	retmsg[23 + 4] ='\0';

//	printf("crc_value:%x,%x",retmsg[24],retmsg[25]);
	int k =1;
	int transferredFlag = 0;

	outbBuf[0] = 0x5b;
	for(int i =1;i<26;i++)
	{
		if((retmsg[i] == 0x5b))
		{
			outbBuf[k] = 0x5a;
			outbBuf[k+1] = 0x01;
			k++;

		}
		else if((retmsg[i] == 0x5a))
		{
			outbBuf[k] = 0x5a;
			outbBuf[k+1] = 0x02;
			k++;
		}
		else if((retmsg[i] == 0x5d ))
		{
			outbBuf[k] = 0x5e;
			outbBuf[k+1] = 0x01;
			k++;
		}
		else if((retmsg[i] == 0x5e))
		{
			outbBuf[k] = 0x5e;
			outbBuf[k+1] = 0x02;
			k++;
		}
		else
		{
			outbBuf[k] = retmsg[i];
		}

		k++;
	}
	outbBuf[k] = 0x5d;


	retmsglen = k + 1;
	memcpy(ret,outbBuf,retmsglen);

	char logData[26];
	for(int j =0;j<27;j++)
	{
		logData[j]=retmsg[j];
	}
	char logtemp[26*2 + 1];
	char szTmp[3];

	 for( int i = 0; i < 27; i++ )
	 {
		sprintf( szTmp, "%02x", (unsigned char) logData[i] );
		memcpy( &logtemp[i * 2], szTmp, 2 );
	 }

	logtemp[26*2+ 1] = '\0';


	zlog_info(g_server_cat,"LNSY Data Recvice :\n%s\n",logtemp);

}


bool BinaryCodec::veriftyHeaderMsg(protocol_receive_data_header &headers,uint8_t* msg,UINT16 &msgType,UINT32 &code)
{
	try
	{
		protocol_receive_data_header LNSYhead ;
		std::string p;
		byte head_flag;
		byte tail_flag;
		memset(&LNSYhead,0,sizeof(protocol_receive_data_header));

		byte RecHeader[500];

	//	//msg vaild
		memcpy(&head_flag,&msg,1);

		memcpy(&RecHeader,&msg[1],sizeof(LNSYhead) -1);
		memcpy(&LNSYhead.receive_data_header.msg_length ,&RecHeader[0],4);
		memcpy(&LNSYhead.receive_data_header.msg_sn ,&RecHeader[4],4);
		memcpy(&LNSYhead.receive_data_header.msg_id ,&RecHeader[8],2);
		memcpy(&LNSYhead.receive_data_header.msg_gnsscenterid ,&RecHeader[10],4);
		memcpy(&LNSYhead.receive_data_header.version_flag ,&RecHeader[14],3);
		memcpy(&LNSYhead.receive_data_header.encrypt_flag ,&RecHeader[17],1);
		memcpy(&LNSYhead.receive_data_header.encrypt_key ,&RecHeader[18],4);
		memcpy(&tail_flag,&msg[ntohl(LNSYhead.receive_data_header.msg_length) -1],1);


		if(tail_flag != 0x5d)
		{
			zlog_error(g_server_cat,"tail error \n");
			return false;
		}

		LNSYhead.receive_data_header.msg_length =ntohl(LNSYhead.receive_data_header.msg_length);			//
		LNSYhead.receive_data_header.msg_sn =ntohl(LNSYhead.receive_data_header.msg_sn);
		LNSYhead.receive_data_header.msg_id=ntohs(LNSYhead.receive_data_header.msg_id);
		LNSYhead.receive_data_header.msg_gnsscenterid=ntohl(LNSYhead.receive_data_header.msg_gnsscenterid);
		LNSYhead.receive_data_header.encrypt_key=ntohl(LNSYhead.receive_data_header.encrypt_key);



		code  = LNSYhead.receive_data_header.msg_gnsscenterid;

		memcpy(&headers,&LNSYhead,sizeof(protocol_receive_data_header));
		memcpy(&msgType,&msg[23],2);

		msgType=ntohs(msgType);

		byte bodyMsg_generate_crc[1000];
		short bodyMsg_crc_value;
		memcpy(&bodyMsg_generate_crc[0],&msg[1],headers.receive_data_header.msg_length -4);
		memcpy(&bodyMsg_crc_value,&msg[headers.receive_data_header.msg_length -3],2);
		int  crcValue = crc16_ccitt(bodyMsg_generate_crc, headers.receive_data_header.msg_length -4);

		// 0x5b ,0x5d

		if(crcValue!=ntohs(bodyMsg_crc_value))
		{
			result =2;
			zlog_error(g_server_cat,"crc error \n");
			return false;
		}

		zlog_debug(g_server_cat,"crc right: %x  , %x \n",ntohs(bodyMsg_crc_value),crcValue);
		result = 0;
		return true;

	}
	catch(exception e)
	{
		result = 1;
		zlog_error(g_server_cat,"verifty header failed:%s\n",e.what());
		return false;
	}

}


short BinaryCodec::getCrc(byte data[]) {
        int crc = 0xFFFF;
        int length = strlen((char*)data);
        for (int i = 0; i < length; i++) {
            crc = ((crc << 8) & 0xFFFF) ^ crcTable[(crc >> 8) ^ data[i] & 0xFF];
        }
        return (short) crc;
    }

int BinaryCodec::crc16_ccitt(byte ptr[], int len) {
        int crc = 0xFFFF;
        for (int i = 0; i < len; i++) {
            crc = (crc << 8 & 0xFFFF) ^ crcTable[(crc >> 8 & 0xFF) ^ ptr[i]];

        }
        return crc;
    }




/* appManage veriftyPostionMsg*/
bool BinaryCodec::veriftyPostionMsg(protocol_receive_msg_postion_body &TrajPos,uint8_t*  msg,int &len)
{
	try
	{
			byte RecTraj[500];
			memset(&TrajPos,0,sizeof(protocol_receive_msg_postion_body));

		//	// msg body
			memcpy(RecTraj,msg ,sizeof(RecTraj) -1);
			memcpy(&TrajPos.msg_postion.service_module ,&RecTraj[0],2);
			memcpy(&TrajPos.msg_postion.datalen ,&RecTraj[2],4);
		//
			TrajPos.msg_postion.datalen = ntohl(TrajPos.msg_postion.datalen);
			int  signleMsgLen = 49;

			len = (TrajPos.msg_postion.datalen/signleMsgLen);

			printf("len : %d,%d,%d\n",len,TrajPos.msg_postion.datalen,signleMsgLen);

			for(int i=0;i<len;i++)
			{
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.plate ,&RecTraj[6 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.plate));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.imei ,&RecTraj[18 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.imei));
				TrajPos.msg_postion.subTraj[i].sub_postion.imei[sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.imei)-1] = '\0';
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.platecolor ,&RecTraj[28 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.platecolor));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.encrypt ,&RecTraj[29 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.encrypt));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.alarm_state ,&RecTraj[30 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.alarm_state));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.veh_state ,&RecTraj[34 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.veh_state));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.latitude ,&RecTraj[38 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.latitude));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.longitude ,&RecTraj[42 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.longitude));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.speed ,&RecTraj[46 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.speed));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.direction_angle ,&RecTraj[48 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.direction_angle));
				memcpy(&TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.stamp ,&RecTraj[49 + signleMsgLen*i],sizeof(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.stamp));



				TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.alarm_state = ntohl(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.alarm_state);
				TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.veh_state =   ntohl(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.veh_state);
				TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.latitude =    ntohl(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.latitude);
				TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.longitude =   ntohl(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.longitude);
				TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.speed =       ntohs(TrajPos.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.speed);


			}

//			memcpy(&traj,&TrajPos,sizeof(PackTraj));

	}
	catch(exception e)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty postion failed :%s\n",e.what());
	}


}

/* appManage veriftyPostionMsg*/
bool BinaryCodec::veriftyADASMsg(PackADASAlarm &ADASMsg,uint8_t*  msg,int &len)
{
	try
	{
		    int msg_index =0;
			int msg_pos = 0;
			byte RecADAS[5000];
			int total_single_msg = 0;
			memset(&ADASMsg,0,sizeof(PackADASAlarm));

		//	// msg body
			memcpy(RecADAS,msg ,sizeof(RecADAS) -1);
			memcpy(&ADASMsg.service_module ,&RecADAS[0],2);
			memcpy(&ADASMsg.msg_total_len ,&RecADAS[2],4);

			msg_pos  = msg_pos + 6;


			ADASMsg.msg_total_len = ntohl(ADASMsg.msg_total_len);

			printf("ADASMsg.msg_total_len :%d\n",ADASMsg.msg_total_len );


			while(ADASMsg.msg_total_len > msg_pos)
			{

				memcpy(&ADASMsg.adas_alarm[msg_index].msg_single_len ,&RecADAS[msg_pos],4);
				printf("msg_pos:%d\n",msg_pos );
				msg_pos  = msg_pos + 4;

				ADASMsg.adas_alarm[msg_index].msg_single_len = ntohl(ADASMsg.adas_alarm[msg_index].msg_single_len );

				total_single_msg = total_single_msg + ADASMsg.adas_alarm[msg_index].msg_single_len;
				printf("total_single_msg:%d|%d\n",total_single_msg,ADASMsg.adas_alarm[msg_index].msg_single_len);

				memcpy(&ADASMsg.adas_alarm[msg_index].veh_plate ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].veh_plate));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].veh_plate));


				memcpy(&ADASMsg.adas_alarm[msg_index].imei ,&RecADAS[msg_pos],(sizeof(ADASMsg.adas_alarm[msg_index].imei)-1)/2);
				msg_pos  = msg_pos + int((sizeof(ADASMsg.adas_alarm[msg_index].imei) - 1)/2);


				memcpy(&ADASMsg.adas_alarm[msg_index].plate_color ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].plate_color));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].plate_color));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.encrypt ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.encrypt));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.encrypt));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.alarm_state ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.alarm_state));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.alarm_state));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.veh_state ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.veh_state));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.veh_state));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.latitude ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.latitude));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.latitude));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.longitude ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.longitude));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.longitude));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.speed ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.speed));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.speed));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.direction_angle ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.direction_angle));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.direction_angle));

				memcpy(&ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.stamp ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.stamp));
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.stamp));


				memcpy(&ADASMsg.adas_alarm[msg_index].adas_type ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].adas_type));

				//printf("ADASMsg.adas_alarm[msg_index].adas_type:%d|%d\n",ADASMsg.adas_alarm[msg_index].adas_type,msg_pos);
				msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].adas_type));


				ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.alarm_state = ntohl(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.alarm_state);
				ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.veh_state = ntohl(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.veh_state);
				ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.latitude = ntohl(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.latitude);
				ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.longitude = ntohl(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.longitude);
				ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.speed = ntohs(ADASMsg.adas_alarm[msg_index].adas_pos.basic_postion.speed);


				if((msg_pos - (10 + msg_index*4))<  total_single_msg )
				{
					memcpy(&ADASMsg.adas_alarm[msg_index].para_one_type ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].para_one_type));
					msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].para_one_type));

					memcpy(&ADASMsg.adas_alarm[msg_index].para_one_len ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].para_one_len));
					msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].para_one_len));



					memcpy(&ADASMsg.adas_alarm[msg_index].para_one_value[0] ,&RecADAS[msg_pos],int(ADASMsg.adas_alarm[msg_index].para_one_len));
					msg_pos  = msg_pos + int(ADASMsg.adas_alarm[msg_index].para_one_len);

					if((msg_pos - (10 + msg_index*4))<  total_single_msg )
					{
						memcpy(&ADASMsg.adas_alarm[msg_index].para_two_type ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].para_two_type));
						msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].para_two_type));

						memcpy(&ADASMsg.adas_alarm[msg_index].para_two_len ,&RecADAS[msg_pos],sizeof(ADASMsg.adas_alarm[msg_index].para_two_len));
						msg_pos  = msg_pos + int(sizeof(ADASMsg.adas_alarm[msg_index].para_two_len));

						memcpy(&ADASMsg.adas_alarm[msg_index].para_two_value[0] ,&RecADAS[msg_pos],int(ADASMsg.adas_alarm[msg_index].para_two_len));
						msg_pos  = msg_pos + int(ADASMsg.adas_alarm[msg_index].para_two_len);
					}

				}

				msg_index = msg_index + 1;
			}


			len = msg_index;
		//



	}
	catch(exception e)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty ADAS failed :%s\n",e.what());
	}


}




int BinaryCodec::GetModuleFileName(char* name, int size)
{
    FILE* stream = fopen("/proc/self/cmdline", "r");
    fgets(name, size, stream);
    fclose(stream);

    return strlen(name);
}




/* appManage veriftyVehOperateMsg*/
UINT32 BinaryCodec::veriftyVehOperateMsg(PackOperator &vehOper,uint8_t*  msg)
{
	try
	{
		PackOperator  Operator;
		byte RecVehOper[700];
		memset(&Operator,0,sizeof(PackOperator));
		// msg body
		memcpy(RecVehOper,msg,sizeof(RecVehOper) -1);
		memcpy(&Operator.service_module ,&RecVehOper[0],2);
		memcpy(&Operator.datalen ,&RecVehOper[2],4);

		Operator.datalen =ntohl(Operator.datalen);

		int signleMsgLen = 86;

		for(int i=0;i<(int(Operator.datalen)/signleMsgLen);i++)
		{
			memcpy(&Operator.subOperator[i].plate ,&RecVehOper[6 + signleMsgLen*i],12);
			memcpy(&Operator.subOperator[i].imei ,&RecVehOper[18+ signleMsgLen*i],10);
			Operator.subOperator[i].imei[10] = '\0';
			memcpy(&Operator.subOperator[i].platecolor ,&RecVehOper[28 + signleMsgLen*i],1);
			memcpy(&Operator.subOperator[i].encrypt ,&RecVehOper[29 + signleMsgLen*i],1);
			memcpy(&Operator.subOperator[i].getin_lat ,&RecVehOper[30 + signleMsgLen*i],4);
			memcpy(&Operator.subOperator[i].getin_lon ,&RecVehOper[34 + signleMsgLen*i],4);
			memcpy(&Operator.subOperator[i].getin_time ,&RecVehOper[38 + signleMsgLen*i],7);
			memcpy(&Operator.subOperator[i].getout_lat ,&RecVehOper[45 + signleMsgLen*i],4);
			memcpy(&Operator.subOperator[i].getout_lon ,&RecVehOper[49 + signleMsgLen*i],4);
			memcpy(&Operator.subOperator[i].getout_time ,&RecVehOper[53 + signleMsgLen*i],7);
			memcpy(&Operator.subOperator[i].cert_number ,&RecVehOper[60 + signleMsgLen*i],18);
			Operator.subOperator[i].cert_number[18]='\0';
			memcpy(&Operator.subOperator[i].eval_opin_code ,&RecVehOper[78 + signleMsgLen*i],1);
			memcpy(&Operator.subOperator[i].eval_opin_extend ,&RecVehOper[79 + signleMsgLen*i],2);
			memcpy(&Operator.subOperator[i].oper_on_mile ,&RecVehOper[81 + signleMsgLen*i],2);
			memcpy(&Operator.subOperator[i].oper_off_mile ,&RecVehOper[83 + signleMsgLen*i],2);
			memcpy(&Operator.subOperator[i].fuel_charge ,&RecVehOper[85 + signleMsgLen*i],2);
			memcpy(&Operator.subOperator[i].wait_time ,&RecVehOper[87 + signleMsgLen*i],2);
			memcpy(&Operator.subOperator[i].amount ,&RecVehOper[89 + signleMsgLen*i],2);
			memcpy(&Operator.subOperator[i].amount_code ,&RecVehOper[91 + signleMsgLen*i],1);



			Operator.subOperator[i].getin_lat = ntohl(Operator.subOperator[i].getin_lat);
			Operator.subOperator[i].getin_lon =   ntohl(Operator.subOperator[i].getin_lon);
			Operator.subOperator[i].getout_lat = ntohl(Operator.subOperator[i].getout_lat);
			Operator.subOperator[i].getout_lon =   ntohl(Operator.subOperator[i].getout_lon);
			Operator.subOperator[i].eval_opin_extend =ntohs(Operator.subOperator[i].eval_opin_extend);
			Operator.subOperator[i].oper_on_mile =ntohs(Operator.subOperator[i].oper_on_mile);
			Operator.subOperator[i].oper_off_mile =ntohs(Operator.subOperator[i].oper_off_mile);
			Operator.subOperator[i].fuel_charge =ntohs(Operator.subOperator[i].fuel_charge);
			//Operator.subOperator[i].wait_time =ntohs(Operator.subOperator[i].wait_time);
			Operator.subOperator[i].amount =ntohs(Operator.subOperator[i].amount);
		}

		memcpy(&vehOper,&Operator,sizeof(PackOperator));
		return Operator.datalen/signleMsgLen;
	}
	catch(exception e)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty vehservice failed:%s\n",e.what());
		return 0;
	}



}
int BinaryCodec::hex_char_value(char c)
{
    if(c >= '0' && c <= '9')
        return c - '0';
    else if(c >= 'a' && c <= 'f')
        return (c - 'a' + 10);
    else if(c >= 'A' && c <= 'F')
        return (c - 'A' + 10);
    //assert(0);
    return 0;
}

/* appManage veriftyDailyOperMsg*/
void BinaryCodec::getsubDailyOperMsg(UINT32 &value,char *submsg)
{
	int k =strlen((char*)submsg);
	for(int j=0;j<strlen((char*)submsg);j++)
	{
		if(k>1)
		{
			value = value + int(hex_char_value(submsg[j]))*(pow(10,k-1-j));
		}
		else if(k ==1)
		{
			value  = hex_char_value(submsg[j]);
		}
		else
		{
			value =0;
		}
	}

}
/* appManage veriftyDailyOperMsg*/
bool BinaryCodec::veriftyDailyOperMsg(PackDailyOper &dailyOper,uint8_t*  msg)
{
	try
	{
		PackDailyOper  DailyOper;
		byte RecDailyOper[500];
		memset(&DailyOper,0,sizeof(PackDailyOper));
		// msg body
		memcpy(RecDailyOper,msg,sizeof(RecDailyOper) -1);
		memcpy(&DailyOper.service_module ,&RecDailyOper[0],2);
		memcpy(&DailyOper.datalen ,&RecDailyOper[2],4);

		DailyOper.datalen = ntohl(DailyOper.datalen);

		byte submsg[24][16];
		//get value


		int subdatalen =1;
		int index =0;
		for(int i=0;i<DailyOper.datalen;i++)
		{
			if(index<9)
			{
				if((RecDailyOper[6+i] == 0x4e) && (RecDailyOper[6+i+1] == index+1+0x30) &&  (RecDailyOper[6+i+2] == 0x3a) )
				{
					while(RecDailyOper[6+i] !=0x3b)
					{
						subdatalen ++;
						i++;

					}
					memcpy(&submsg[index][0],&RecDailyOper[6 + i - subdatalen +5],(subdatalen-5));
					submsg[index][subdatalen-5]='\0';

					subdatalen = 1;
					index ++;
				}
				else{

				}
			}
			else
			{
				//printf("index1:%x,%x\n",((index+1)/10 + 0x30),((index+1)%10 +0x30));
				if((RecDailyOper[6+i] == 0x4e) && (RecDailyOper[6+i+1] == ((index+1)/10 + 0x30)) &&  (RecDailyOper[6+i+2] == ((index+1)%10 +0x30)) &&  (RecDailyOper[6+i+3] == 0x3a) )
				{
					while(RecDailyOper[6+i] !=0x3b)
					{
						subdatalen ++;
						i++;
					}
					memcpy(&submsg[index][0],&RecDailyOper[6 + i - subdatalen +6],subdatalen-6);
					submsg[index][subdatalen-6]='\0';
					subdatalen = 1;
					index ++;
				}
			}
		}

		getsubDailyOperMsg(DailyOper.count_date,(char*)submsg[0]);
		getsubDailyOperMsg(DailyOper.area_code,(char*)submsg[1]);
		getsubDailyOperMsg(DailyOper.taxi_num,(char*)submsg[2]);
		getsubDailyOperMsg(DailyOper.online_num,(char*)submsg[3]);
		getsubDailyOperMsg(DailyOper.oper_num,(char*)submsg[4]);
		getsubDailyOperMsg(DailyOper.oper_time,(char*)submsg[5]);
		getsubDailyOperMsg(DailyOper.oper_day,(char*)submsg[6]);
		getsubDailyOperMsg(DailyOper.oper_night,(char*)submsg[7]);
		getsubDailyOperMsg(DailyOper.oper_freq,(char*)submsg[8]);
		getsubDailyOperMsg(DailyOper.oper_mile,(char*)submsg[9]);
		getsubDailyOperMsg(DailyOper.offoper_mile,(char*)submsg[10]);
		getsubDailyOperMsg(DailyOper.all_utiliz,(char*)submsg[11]);
		getsubDailyOperMsg(DailyOper.offpeak_utiliz,(char*)submsg[12]);
		getsubDailyOperMsg(DailyOper.peak_utiliz,(char*)submsg[13]);
		getsubDailyOperMsg(DailyOper.lowspeed_on,(char*)submsg[14]);
	//	getsubDailyOperMsg(DailyOper.lowspeed_off,(char*)submsg[15]);
		getsubDailyOperMsg(DailyOper.car_income,(char*)submsg[15]);
		getsubDailyOperMsg(DailyOper.eval_times,(char*)submsg[16]);
		getsubDailyOperMsg(DailyOper.verysatisfy,(char*)submsg[17]);
		getsubDailyOperMsg(DailyOper.satisfy,(char*)submsg[18]);
		getsubDailyOperMsg(DailyOper.nosatisfy,(char*)submsg[19]);
		getsubDailyOperMsg(DailyOper.call_times,(char*)submsg[20]);
		getsubDailyOperMsg(DailyOper.call_success,(char*)submsg[21]);
		getsubDailyOperMsg(DailyOper.call_fail,(char*)submsg[22]);



		memcpy(&dailyOper,&DailyOper,sizeof(PackDailyOper));
	}
	catch(exception e)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty daily vehservice failed:%s\n",e.what());
	}

}
/* appManage veriftyMonthlyOperMsg*/
bool BinaryCodec::veriftyMonthlyOperMsg(PackMonthlyOper &monthlyOper,uint8_t*  msg)
{
	try
	{
		PackMonthlyOper MonthlyOper;
		byte RecMonthlyOper[300];
		memset(&MonthlyOper,0,sizeof(PackMonthlyOper));
		// msg body
		memcpy(RecMonthlyOper,msg ,sizeof(RecMonthlyOper) -1);
		memcpy(&MonthlyOper.service_module ,&RecMonthlyOper[0],2);
		memcpy(&MonthlyOper.datalen ,&RecMonthlyOper[2],4);

		MonthlyOper.datalen = ntohl(MonthlyOper.datalen);


		byte submsg[24][16];
		//get value

		int subdatalen =1;
		int index =0;
		for(int i=0;i<MonthlyOper.datalen;i++)
		{
			if(index<9)
			{
				if((RecMonthlyOper[6+i] == 0x4e) && (RecMonthlyOper[6+i+1] == index+1+0x30) &&  (RecMonthlyOper[6+i+2] == 0x3a) )
				{
					while(RecMonthlyOper[6+i] !=0x3b)
					{
						subdatalen ++;
						i++;

					}
					memcpy(&submsg[index][0],&RecMonthlyOper[6 + i - subdatalen +5],(subdatalen-5));
					submsg[index][subdatalen-5]='\0';

					subdatalen = 1;
					index ++;
				}
				else{

				}
			}
			else
			{
				if((RecMonthlyOper[6+i] == 0x4e) && (RecMonthlyOper[6+i+1] == ((index+1)/10 + 0x30)) &&  (RecMonthlyOper[6+i+2] == ((index+1)%10 +0x30)) &&  (RecMonthlyOper[6+i+3] == 0x3a) )
				{
					while(RecMonthlyOper[6+i] !=0x3b)
					{
						subdatalen ++;
						i++;

					}
					memcpy(&submsg[index][0],&RecMonthlyOper[6 + i - subdatalen +6],subdatalen-6);
					submsg[index][subdatalen-6]='\0';
					subdatalen = 1;
					index ++;
				}

			}
		}


		getsubDailyOperMsg(MonthlyOper.area_code,(char*)submsg[0]);
		getsubDailyOperMsg(MonthlyOper.count_interval,(char*)submsg[1]);
		getsubDailyOperMsg(MonthlyOper.veh_color,(char*)submsg[2]);
		getsubDailyOperMsg(MonthlyOper.veh_plate,(char*)submsg[3]);
		getsubDailyOperMsg(MonthlyOper.online_time,(char*)submsg[4]);
		getsubDailyOperMsg(MonthlyOper.oper_time,(char*)submsg[5]);
		getsubDailyOperMsg(MonthlyOper.oper_day,(char*)submsg[6]);
		getsubDailyOperMsg(MonthlyOper.oper_night,(char*)submsg[7]);
		getsubDailyOperMsg(MonthlyOper.operate_freq,(char*)submsg[8]);
		getsubDailyOperMsg(MonthlyOper.oper_on_mile,(char*)submsg[9]);
		getsubDailyOperMsg(MonthlyOper.oper_off_mile,(char*)submsg[10]);
		getsubDailyOperMsg(MonthlyOper.oper_on_lowspeed,(char*)submsg[11]);
		getsubDailyOperMsg(MonthlyOper.oper_off_lowspeed,(char*)submsg[12]);
		getsubDailyOperMsg(MonthlyOper.car_income,(char*)submsg[13]);
		getsubDailyOperMsg(MonthlyOper.card_income,(char*)submsg[14]);
		getsubDailyOperMsg(MonthlyOper.eval_times,(char*)submsg[15]);
		getsubDailyOperMsg(MonthlyOper.verysatisfy,(char*)submsg[16]);
		getsubDailyOperMsg(MonthlyOper.satisfy,(char*)submsg[17]);
		getsubDailyOperMsg(MonthlyOper.nosatisfy,(char*)submsg[18]);
		getsubDailyOperMsg(MonthlyOper.call_success,(char*)submsg[19]);
		getsubDailyOperMsg(MonthlyOper.call_fail,(char*)submsg[20]);

		memcpy(&monthlyOper,&MonthlyOper,sizeof(PackMonthlyOper));

	}
	catch(...)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty Monthly vehservice failed\n");
	}

}


/* appManage veriftyDriverCardOperMsg*/
bool BinaryCodec::veriftyDriverCardOperMsg(PackDriverCard &driverCard,uint8_t*  msg)
{
	try
	{
		PackDriverCard  DriverCard;
		byte RecDriverCard[500];
		memset(&DriverCard,0,sizeof(PackDriverCard));
		// msg body
		memcpy(RecDriverCard,msg ,sizeof(RecDriverCard) -1);

		memcpy(&DriverCard.service_module ,&RecDriverCard[0],2);
		memcpy(&DriverCard.veh_plate ,&RecDriverCard[2],12); //
		memcpy(&DriverCard.imei ,&RecDriverCard[14],10);
		DriverCard.imei[10] = '\0';
		//memcpy(&DriverCard.drivercardpos.subTraj[0].basiclocal.encrypt ,&RecDriverCard[14],1);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state ,&RecDriverCard[24],4);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state ,&RecDriverCard[28],4);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude ,&RecDriverCard[32],4);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude ,&RecDriverCard[36],4);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed,&RecDriverCard[40],2);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.direction_angle ,&RecDriverCard[42],1);
		memcpy(&DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.stamp,&RecDriverCard[43],6); //
		memcpy(&DriverCard.reg_state ,&RecDriverCard[49],1);
		memcpy(&DriverCard.reg_time ,&RecDriverCard[50],5); //
		memcpy(&DriverCard.driver_num ,&RecDriverCard[55],1);

		memcpy(&DriverCard.driver_name ,&RecDriverCard[56],DriverCard.driver_num); //

		DriverCard.driver_name[DriverCard.driver_num]='\0';//
		memcpy(&DriverCard.driver_cert_number ,&RecDriverCard[DriverCard.driver_num + 56],19); //
		DriverCard.driver_cert_number[19]='\0';
		memcpy(&DriverCard.driver_iden_number ,&RecDriverCard[DriverCard.driver_num + 75],19); //
		DriverCard.driver_iden_number[19]='\0';
		memcpy(&DriverCard.comp_cert_number ,&RecDriverCard[DriverCard.driver_num + 94],16);  //
		DriverCard.comp_cert_number[16]='\0';
		memcpy(&DriverCard.inst_cert_num ,&RecDriverCard[DriverCard.driver_num + 110],1);

		memcpy(&DriverCard.inst_cert_name ,&RecDriverCard[DriverCard.driver_num + 111],DriverCard.inst_cert_num);

		DriverCard.inst_cert_name[DriverCard.inst_cert_num]='\0';
		memcpy(&DriverCard.cert_startdate ,&RecDriverCard[DriverCard.driver_num + DriverCard.inst_cert_num + 111],4);
		memcpy(&DriverCard.cert_deadline ,&RecDriverCard[DriverCard.driver_num + DriverCard.inst_cert_num + 115],4);
		memcpy(&DriverCard.comp_num ,&RecDriverCard[DriverCard.driver_num + DriverCard.inst_cert_num + 119],1);
		memcpy(&DriverCard.comp_name ,&RecDriverCard[DriverCard.driver_num + DriverCard.inst_cert_num + 120],DriverCard.comp_num); //



		DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state = ntohl(DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state);
		DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state = ntohl(DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state);
		DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude = ntohl(DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude);
		DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude = ntohl(DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude);
		DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed = ntohs(DriverCard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed);
		//DriverCard.cert_startdate = ntohl(DriverCard.cert_startdate);
		//DriverCard.cert_deadline = ntohl(DriverCard.cert_deadline);

		memcpy(&driverCard,&DriverCard,sizeof(PackDriverCard));

	}
	catch(exception e)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty DriverCard vehservice failed:%s\n",e.what());
	}

}
/* appManage veriftyDriverViolateOperMsg*/
bool BinaryCodec::veriftyDriverViolateOperMsg(PackDriverViolate &driverViolate,uint8_t*  msg)
{
	try
	{
		PackDriverViolate  DriverViolate;
		byte RecDriverViolate[500];
		memset(&DriverViolate,0,sizeof(PackDriverViolate));
		// msg body
		memcpy(RecDriverViolate,msg,sizeof(RecDriverViolate) -1);

		memcpy(&DriverViolate.service_module ,&RecDriverViolate[0],2);
		memcpy(&DriverViolate.veh_plate ,&RecDriverViolate[2],12); //
		memcpy(&DriverViolate.imei ,&RecDriverViolate[14],10);
		DriverViolate.imei[10]='\0';
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.encrypt ,&RecDriverViolate[24],1);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state ,&RecDriverViolate[25],4);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state ,&RecDriverViolate[29],4);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude ,&RecDriverViolate[33],4);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude ,&RecDriverViolate[37],4);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed,&RecDriverViolate[41],2);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.direction_angle ,&RecDriverViolate[43],1);
		memcpy(&DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.stamp,&RecDriverViolate[44],6); //
		memcpy(&DriverViolate.driver_num ,&RecDriverViolate[50],1);

		memcpy(&DriverViolate.driver_name ,&RecDriverViolate[51],DriverViolate.driver_num); //


		DriverViolate.driver_name[DriverViolate.driver_num] = '\0';
		memcpy(&DriverViolate.comp_num ,&RecDriverViolate[51 + DriverViolate.driver_num],1);

		memcpy(&DriverViolate.comp_name ,&RecDriverViolate[52 + DriverViolate.driver_num ],DriverViolate.comp_num); //

		DriverViolate.comp_name[DriverViolate.comp_num] = '\0';
		memcpy(&DriverViolate.driver_iden_number ,&RecDriverViolate[52 + DriverViolate.driver_num + DriverViolate.comp_num],19); //
		DriverViolate.driver_iden_number[19] = '\0';
		memcpy(&DriverViolate.alarm_type ,&RecDriverViolate[52 + 19 + DriverViolate.driver_num + DriverViolate.comp_num],1);


		DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state = ntohl(DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state);
		DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state = ntohl(DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state);
		DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude = ntohl(DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude);
		DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude = ntohl(DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude);
		DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed = ntohs(DriverViolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed);

		memcpy(&driverViolate,&DriverViolate,sizeof(PackDriverViolate));

	}
	catch(exception e)
	{
		result = 2;
		zlog_error(g_server_cat,"verifty DriverViolate failed:%s\n",e.what());
	}

}

