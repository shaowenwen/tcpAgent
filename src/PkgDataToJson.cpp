/*
 * PkgDataToJson.cpp
 *
 *  Created on: 2018-11-6
 *      Author: root
 */

#include "PkgDataToJson.h"



const char * notify_type[20][3] = {{"front_car_distance","front_car_speed","front_car_distance"},{"lane_departure","error","lane_departure_distance"},{"pedestrian_collision","error","pedestrian_collision_distance"},{"front_car_start","",""},\
								{"left_direction_lamp","",""},{"right_direction_lamp","",""},{"left_right_direction_lamp","",""},\
								{"brakes","",""},{"shaking","",""},{"front_car_start_remove","",""},{"others","",""},{"","",""},{"","",""},{"","",""},{"","",""},{"","",""}};

PkgDataToJson::PkgDataToJson() {
	// TODO Auto-generated constructor stub

}

PkgDataToJson::~PkgDataToJson() {
	// TODO Auto-generated destructor stub
}


void PkgDataToJson::getgpsStamp(byte *inVal,byte *outVal)
{
    char stamp_temp[32];
    byte stamp_orig[7];
    memcpy(stamp_orig,(char* )inVal,6);
    for(int j=6;j>0;j--)
    {
    	stamp_orig[j] = stamp_orig[j-1];
    }
    stamp_orig[0] =0x20;
    Hex2Str((char*)stamp_orig,stamp_temp,7);
    string strStamp = stamp_temp;
    string szstamp = strStamp.substr(0,4).append("-").append(strStamp.substr(4,2)).append("-").append( \
    		strStamp.substr(6,2)).append(" ").append(strStamp.substr(8,2)).append(":").append(\
    				strStamp.substr(10,2)).append(":").append(strStamp.substr(12,2));

    memcpy(outVal,(char*)szstamp.c_str(),szstamp.length());
    outVal[szstamp.length()] ='\0';
}


void PkgDataToJson::gettaxiStamp(byte *inVal,byte *outVal)
{
    char stamp_temp[32];
    byte stamp_orig[7];
    memcpy(stamp_orig,(char* )inVal,7);
    Hex2Str((char*)stamp_orig,stamp_temp,7);
    string strStamp = stamp_temp;
    string szstamp = strStamp.substr(0,4).append("-").append(strStamp.substr(4,2)).append("-").append( \
    		strStamp.substr(6,2)).append(" ").append(strStamp.substr(8,2)).append(":").append(\
    				strStamp.substr(10,2)).append(":").append(strStamp.substr(12,2));


    memcpy(outVal,(char*)szstamp.c_str(),szstamp.length());
    outVal[szstamp.length()] ='\0';
}


void PkgDataToJson::Hex2Str( const char *sSrc,  char sDest[], int nSrcLen )
{
    int  i;
    char szTmp[3];

    for( i = 0; i < nSrcLen; i++ )
    {
        sprintf( szTmp, "%02x", (unsigned char) sSrc[i] );
        memcpy( &sDest[i * 2], szTmp, 2 );
    }
    return ;
}


void PkgDataToJson::Hex2Str_zero( const char *sSrc,  char sDest[], int nSrcLen )
{
    int index =0;

    for( int i = 0; i < nSrcLen; i++ )
    {
    	if(sSrc[i] != '0' )
    	{
    		index = i;
    		break;
    	}
    }

    memcpy( &sDest[0], &sSrc[index], nSrcLen - index );
    sDest[nSrcLen - index] = '\0';



    return ;
}

int PkgDataToJson::code_convert(char *from_charset,char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
        iconv_t cd;
        int rc;
        char **pin = &inbuf;
        char **pout = &outbuf;

        cd = iconv_open(to_charset,from_charset);
        if (cd==0)
                return -1;
        memset(outbuf,0,outlen);
        if (iconv(cd,pin,&inlen,pout,&outlen) == -1)
        {
        	iconv_close(cd);
        	return -1;
        }
        iconv_close(cd);
        return 0;
}



int PkgDataToJson::u2g(char *inbuf,int inlen,char *outbuf,int outlen)
{
        return code_convert("utf-8","gb2312",inbuf,inlen,outbuf,outlen);
}

int PkgDataToJson::g2u(char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
      //  return code_convert("gb2312","utf-8",inbuf,inlen,outbuf,outlen);
      return code_convert("gbk","utf-8",inbuf,inlen,outbuf,outlen);
}

void  PkgDataToJson::getregStamp(byte *inVal,byte *outVal)
{
    char stamp_temp[32];
    byte stamp_orig[6];
    memcpy(stamp_orig,(char* )inVal,5);
    for(int j=5;j>0;j--)
    {
    	stamp_orig[j] = stamp_orig[j-1];
    }
    stamp_orig[0] =0x20;
    Hex2Str((char*)stamp_orig,stamp_temp,6);
    string strStamp = stamp_temp;
    string szstamp = strStamp.substr(0,4).append("-").append(strStamp.substr(4,2)).append("-").append( \
    		strStamp.substr(6,2)).append(" ").append(strStamp.substr(8,2)).append(":").append(\
    				strStamp.substr(10,2)).append(":").append("00");

    memcpy(outVal,(char*)szstamp.c_str(),szstamp.length());
    outVal[szstamp.length()] ='\0';
}


int PkgDataToJson::getADASParaValue(byte *inVal,int len)
{
    byte sztemp[64];
    int ret= 0;
    memcpy(&sztemp[0],(char* )inVal,len);
    for(int j=len-1;j>=0;j--)
    {
    	ret = ret + int(sztemp[j]) * 256   ;
    }
    printf("ret:%d\n",ret);
    return ret;

}


void PkgDataToJson::getg2uvalue(byte *inVal,byte *outVal)
{
	char gbk_plate[256];
	memcpy(gbk_plate,&inVal[0],128);
	memset(outVal,0x00,sizeof(outVal));
	g2u(gbk_plate,128,(char*)outVal,256);
}


void PkgDataToJson::PkgPostionAlarmStateToJson(cJSON* root,UINT32 Val)
{

	cJSON_AddNumberToObject(root, "a0",  (Val&0x1));
	cJSON_AddNumberToObject(root, "a1", ((Val>>1)&0x01));
	cJSON_AddNumberToObject(root, "a2", ((Val>>2)&0x01));
	cJSON_AddNumberToObject(root, "a3", ((Val>>3)&0x01));
	cJSON_AddNumberToObject(root, "a4", ((Val>>4)&0x01));
	cJSON_AddNumberToObject(root, "a5", ((Val>>5)&0x01));
	cJSON_AddNumberToObject(root, "a6", ((Val>>6)&0x01));
	cJSON_AddNumberToObject(root, "a7", ((Val>>7)&0x01));
	cJSON_AddNumberToObject(root, "a8", ((Val>>8)&0x01));
	cJSON_AddNumberToObject(root, "a9", ((Val>>9)&0x01));
	cJSON_AddNumberToObject(root, "a10", ((Val>>10)&0x01));
	cJSON_AddNumberToObject(root, "a11", ((Val>>11)&0x01));
	cJSON_AddNumberToObject(root, "a12", ((Val>>12)&0x01));
	cJSON_AddNumberToObject(root, "a13", ((Val>>13)&0x01));
	cJSON_AddNumberToObject(root, "a14", ((Val>>14)&0x01));
	cJSON_AddNumberToObject(root, "a15", ((Val>>15)&0x01));
	cJSON_AddNumberToObject(root, "a16", ((Val>>16)&0x01));
	cJSON_AddNumberToObject(root, "a17", ((Val>>17)&0x01));
	cJSON_AddNumberToObject(root, "a18", ((Val>>18)&0x01));
	cJSON_AddNumberToObject(root, "a19", ((Val>>19)&0x01));
	cJSON_AddNumberToObject(root, "a20", ((Val>>20)&0x01));
	cJSON_AddNumberToObject(root, "a21", ((Val>>21)&0x01));
	cJSON_AddNumberToObject(root, "a22", ((Val>>22)&0x01));
	cJSON_AddNumberToObject(root, "a23", ((Val>>23)&0x01));
	cJSON_AddNumberToObject(root, "a24", ((Val>>24)&0x01));
	cJSON_AddNumberToObject(root, "a25", ((Val>>25)&0x01));
	cJSON_AddNumberToObject(root, "a26", ((Val>>26)&0x01));
	cJSON_AddNumberToObject(root, "a27", ((Val>>27)&0x01));
	cJSON_AddNumberToObject(root, "a28", ((Val>>28)&0x01));

}


void PkgDataToJson::PkgPostionVehStateToJson(cJSON* root,UINT32 Val)
{

	cJSON_AddNumberToObject(root, "b0",  (Val&0x1));
	cJSON_AddNumberToObject(root, "b1", ((Val>>1)&0x01));
	cJSON_AddNumberToObject(root, "b2", ((Val>>2)&0x01));
	cJSON_AddNumberToObject(root, "b3", ((Val>>3)&0x01));
	cJSON_AddNumberToObject(root, "b4", ((Val>>4)&0x01));
	cJSON_AddNumberToObject(root, "b5", ((Val>>5)&0x01));
	cJSON_AddNumberToObject(root, "b6", ((Val>>6)&0x01));
	cJSON_AddNumberToObject(root, "b7", ((Val>>7)&0x01));
	cJSON_AddNumberToObject(root, "b8", ((Val>>8)&0x01));
	cJSON_AddNumberToObject(root, "b9", ((Val>>9)&0x01));
	cJSON_AddNumberToObject(root, "b10", ((Val>>10)&0x01));
	cJSON_AddNumberToObject(root, "b11", ((Val>>11)&0x01));
	cJSON_AddNumberToObject(root, "b12", ((Val>>12)&0x01));
	cJSON_AddNumberToObject(root, "b13", ((Val>>13)&0x01));
	cJSON_AddNumberToObject(root, "b14", ((Val>>14)&0x01));

}


void PkgDataToJson::GetLocalTime(SYSTEMTIME* lpSystemTime)
{
    struct timeval tv;
    gettimeofday (&tv , NULL);

    tm tm_now ;
    localtime_r(&tv.tv_sec,&tm_now);

    lpSystemTime->wYear = tm_now.tm_year+1900;
    lpSystemTime->wMonth = tm_now.tm_mon+1;
    lpSystemTime->wDay = tm_now.tm_mday;
    lpSystemTime->wDayOfWeek = (tm_now.tm_wday+1)%7 ;
    lpSystemTime->wHour =tm_now.tm_hour;
    lpSystemTime->wMinute = tm_now.tm_min;
    lpSystemTime->wSecond = tm_now.tm_sec;
    lpSystemTime->wMilliseconds = tv.tv_usec/1000;
}



void  PkgDataToJson::PkgPostionToJson(protocol_send_data_header rechead,protocol_receive_msg_postion_body packtraj,int len,string *out_str)
{
    char* in_str;
    byte stamp_value[128];
    byte plate_u_value[256];
    char _timestamp[64];

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);

    for(int i =0;i<len;i++)
    {

    	memset(stamp_value,0,sizeof(stamp_value));
    	memset(plate_u_value,0,sizeof(plate_u_value));
        cJSON* root = cJSON_CreateObject();
        cJSON* pJsonsub_alarm = cJSON_CreateObject();
        cJSON* pJsonsub_veh = cJSON_CreateObject();


        char * _frim =(char*)rechead.send_data_header.frim;
        char * _prov =(char*)rechead.send_data_header.prov;
        char * _city =(char*)rechead.send_data_header.city;
        char * _area_code =(char*)rechead.send_data_header.area_code;
        char * _type =(char*)rechead.send_data_header.type;
        char * _command =(char*)rechead.send_data_header.command;



        byte _imei_temp[21];
        byte _imei[21];
        Hex2Str((char*)packtraj.msg_postion.subTraj[i].sub_postion.imei,(char*)_imei_temp,20);
        Hex2Str_zero((char*)_imei_temp,(char*)_imei,20);
//    	_imei[20]='\0';

        cJSON_AddStringToObject(root, "timestamp",_timestamp);
        cJSON_AddStringToObject(root, "frim",_frim);
        cJSON_AddStringToObject(root, "prov",_prov);
        cJSON_AddStringToObject(root, "city",_city);
        cJSON_AddStringToObject(root, "area_code",_area_code);
        cJSON_AddStringToObject(root, "car_type",_type);
        cJSON_AddStringToObject(root, "command",_command);

        cJSON_AddStringToObject(root, "imei",(char*)_imei);

        cJSON_AddNumberToObject(root, "latitude", packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.latitude/600000.0);
        cJSON_AddNumberToObject(root, "longitude",packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.longitude/600000.0);
        cJSON_AddNumberToObject(root, "speed",packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.speed/10);
        cJSON_AddNumberToObject(root, "altitude",packtraj.msg_postion.subTraj[i].sub_postion.altitude);
        cJSON_AddNumberToObject(root, "direction_angle",packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.direction_angle*2);
//
        getgpsStamp(packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.stamp,stamp_value);
        cJSON_AddStringToObject(root, "stamp",(char*)stamp_value);


        getg2uvalue(packtraj.msg_postion.subTraj[i].sub_postion.plate,plate_u_value);
        cJSON_AddStringToObject(root, "plate",(char*)plate_u_value);

        char _plate_color[2];
        sprintf( _plate_color, "%d", int(packtraj.msg_postion.subTraj[i].sub_postion.platecolor) );

        cJSON_AddStringToObject(root, "plate_color",(char*)_plate_color);

//
        PkgPostionAlarmStateToJson(pJsonsub_alarm,packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.alarm_state);
        PkgPostionVehStateToJson(pJsonsub_veh,packtraj.msg_postion.subTraj[i].sub_postion.basiclocal.basic_postion.veh_state);
        cJSON_AddItemToObject(root, "alarm_state", pJsonsub_alarm);
        cJSON_AddItemToObject(root, "veh_state", pJsonsub_veh);

//
        in_str = cJSON_PrintUnformatted(root);
        out_str[i] = in_str;
        if(in_str != nullptr)
        {
            free(in_str);
            in_str =nullptr;
        }

        cJSON_Delete(root);

    }

}




void  PkgDataToJson::PkgADASToJson(protocol_send_data_header rechead,PackADASAlarm packadas,int len,string *out_str)
{
    char* in_str;
    byte stamp_value[128];
    byte plate_u_value[256];
    char _timestamp[64];

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);

    for(int i =0;i<len;i++)
    {

    	printf("ok:%d\n",i);
    	memset(stamp_value,0,sizeof(stamp_value));
    	memset(plate_u_value,0,sizeof(plate_u_value));
        cJSON* root = cJSON_CreateObject();
        cJSON* pJsonsub_alarm = cJSON_CreateObject();
        cJSON* pJsonsub_veh = cJSON_CreateObject();


        char * _frim =(char*)rechead.send_data_header.frim;
        char * _prov =(char*)rechead.send_data_header.prov;
        char * _city =(char*)rechead.send_data_header.city;
        char * _area_code =(char*)rechead.send_data_header.area_code;
        char * _type =(char*)rechead.send_data_header.type;
        char * _command =(char*)rechead.send_data_header.command;

        byte _imei_temp[21];
        byte _imei[21];
        Hex2Str((char*)packadas.adas_alarm[i].imei,(char*)_imei_temp,20);
        Hex2Str_zero((char*)_imei_temp,(char*)_imei,20);

        cJSON_AddStringToObject(root, "timestamp",_timestamp);
        cJSON_AddStringToObject(root, "frim",_frim);
        cJSON_AddStringToObject(root, "prov",_prov);
        cJSON_AddStringToObject(root, "city",_city);
        cJSON_AddStringToObject(root, "area_code",_area_code);
        cJSON_AddStringToObject(root, "car_type",_type);
        cJSON_AddStringToObject(root, "command",_command);

        cJSON_AddStringToObject(root, "imei",(char*)_imei);

        cJSON_AddNumberToObject(root, "latitude", packadas.adas_alarm[i].adas_pos.basic_postion.latitude/600000.0);
        cJSON_AddNumberToObject(root, "longitude",packadas.adas_alarm[i].adas_pos.basic_postion.longitude/600000.0);
        cJSON_AddNumberToObject(root, "speed",packadas.adas_alarm[i].adas_pos.basic_postion.speed/10);
        cJSON_AddNumberToObject(root, "altitude",packadas.adas_alarm[i].adas_pos.basic_postion.latitude);
        cJSON_AddNumberToObject(root, "direction_angle",packadas.adas_alarm[i].adas_pos.basic_postion.direction_angle*2);

        getgpsStamp(packadas.adas_alarm[i].adas_pos.basic_postion.stamp,stamp_value);
        cJSON_AddStringToObject(root, "stamp",(char*)stamp_value);

        getg2uvalue(packadas.adas_alarm[i].veh_plate,plate_u_value);
        cJSON_AddStringToObject(root, "plate",(char*)plate_u_value);



        char _plate_color[2];
        sprintf( _plate_color, "%d", int(packadas.adas_alarm[i].plate_color) );

        cJSON_AddStringToObject(root, "plate_color",(char*)_plate_color);

       // cJSON_AddNumberToObject(root, "plate_color",int(packadas.adas_alarm[i].plate_color));
        cJSON_AddStringToObject(root, "notify_type",notify_type[int(packadas.adas_alarm[i].adas_type) - 1][0]);


        if(packadas.adas_alarm[i].para_one_type == 1)
        {
        	//int _one_value =atoi((char*)packadas.adas_alarm[i].para_one_value);

        	int _one_value  = getADASParaValue((byte*)packadas.adas_alarm[i].para_one_value,packadas.adas_alarm[i].para_one_len);
        	cJSON_AddNumberToObject(root, (char*)notify_type[int(packadas.adas_alarm[i].adas_type) - 1][1],_one_value/1000.0);
        }
        if(packadas.adas_alarm[i].para_two_type == 1)
		{
        	int _two_value = getADASParaValue((byte*)packadas.adas_alarm[i].para_two_value,packadas.adas_alarm[i].para_two_len);
        	cJSON_AddNumberToObject(root, (char*)notify_type[int(packadas.adas_alarm[i].adas_type) - 1][1],_two_value/1000.0);
		}

        if(packadas.adas_alarm[i].para_one_type == 2)
        {
        	int _one_value = getADASParaValue((byte*)packadas.adas_alarm[i].para_one_value,packadas.adas_alarm[i].para_one_len);
        	cJSON_AddNumberToObject(root, (char*)notify_type[int(packadas.adas_alarm[i].adas_type) - 1][2],_one_value/1000.0);
        }
        if(packadas.adas_alarm[i].para_two_type == 2)
		{
        	int _two_value = getADASParaValue((byte*)packadas.adas_alarm[i].para_two_value,packadas.adas_alarm[i].para_two_len);
        	cJSON_AddNumberToObject(root, (char*)notify_type[int(packadas.adas_alarm[i].adas_type) - 1][2],_two_value/1000.0);
		}
//
        PkgPostionAlarmStateToJson(pJsonsub_alarm,packadas.adas_alarm[i].adas_pos.basic_postion.alarm_state);
        PkgPostionVehStateToJson(pJsonsub_veh,packadas.adas_alarm[i].adas_pos.basic_postion.veh_state);
        cJSON_AddItemToObject(root, "alarm_state", pJsonsub_alarm);
        cJSON_AddItemToObject(root, "veh_state", pJsonsub_veh);

//
        in_str = cJSON_PrintUnformatted(root);
        out_str[i] = in_str;
        if(in_str != nullptr)
        {
            free(in_str);
            in_str =nullptr;
        }
        cJSON_Delete(root);

    }

}



/* Parse the input body VEHOperator msg , and populate the result into body json. */
void PkgDataToJson::PkgVEHOperatorToJson(protocol_send_data_header rechead,PackOperator& packoperator,int len,string * out_str)
{
	char* in_str;
    byte stamp_getin_value[128];
    byte stamp_getout_value[128];
    byte plate_u_value[256];
    char _timestamp[64];

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);
    for(int i =0;i<len;i++)
    {
    	memset(stamp_getin_value,0,sizeof(stamp_getin_value));
    	memset(stamp_getout_value,0,sizeof(stamp_getout_value));
    	memset(plate_u_value,0,sizeof(plate_u_value));

        byte _imei_temp[21];
        byte _imei[21];
        Hex2Str((char*)packoperator.subOperator[i].imei,(char*)_imei_temp,20);
        Hex2Str_zero((char*)_imei_temp,(char*)_imei,20);

    	char * _cert_number =(char*)packoperator.subOperator[i].cert_number;
        char * _frim =(char*)rechead.send_data_header.frim;
        char * _prov =(char*)rechead.send_data_header.prov;
        char * _city =(char*)rechead.send_data_header.city;
        char * _area_code =(char*)rechead.send_data_header.area_code;
        char * _type =(char*)rechead.send_data_header.type;
        char * _command =(char*)rechead.send_data_header.command;


		cJSON* root = cJSON_CreateObject();

		cJSON_AddStringToObject(root, "timestamp",_timestamp);

        cJSON_AddStringToObject(root, "frim",_frim);
        cJSON_AddStringToObject(root, "prov",_prov);
        cJSON_AddStringToObject(root, "city",_city);
        cJSON_AddStringToObject(root, "area_code",_area_code);
        cJSON_AddStringToObject(root, "car_type",_type);
        cJSON_AddStringToObject(root, "command",_command);

		getg2uvalue(packoperator.subOperator[i].plate,plate_u_value);
		cJSON_AddStringToObject(root, "plate",(char*)plate_u_value);
		cJSON_AddStringToObject(root, "imei",(char*)_imei);

        char _plate_color[2];
        sprintf( _plate_color, "%d", int(packoperator.subOperator[i].platecolor) );

        cJSON_AddStringToObject(root, "plate_color",(char*)_plate_color);


		//cJSON_AddNumberToObject(root, "plate_color",packoperator.subOperator[i].platecolor);
		cJSON_AddNumberToObject(root, "getin_lat",packoperator.subOperator[i].getin_lat/600000.0);
		cJSON_AddNumberToObject(root, "getin_lon",packoperator.subOperator[i].getin_lon/600000.0);
		gettaxiStamp(packoperator.subOperator[i].getin_time,stamp_getin_value);
		cJSON_AddStringToObject(root, "getin_time",(char*)stamp_getin_value);
		cJSON_AddNumberToObject(root, "getout_lat",packoperator.subOperator[i].getout_lat/600000.0);
		cJSON_AddNumberToObject(root, "getout_lon",packoperator.subOperator[i].getout_lon/600000.0);
		gettaxiStamp(packoperator.subOperator[i].getout_time,stamp_getout_value);
		cJSON_AddStringToObject(root, "getout_time",(char*)stamp_getout_value);

		cJSON_AddStringToObject(root, "cert_number",(char*)_cert_number);

		cJSON_AddNumberToObject(root, "eval_opin_code",int(packoperator.subOperator[i].eval_opin_code ));

	    char _eval_opin_code[2];
		sprintf( _eval_opin_code, "%d", int(packoperator.subOperator[i].eval_opin_code) );

		cJSON_AddStringToObject(root, "eval_opin_code",(char*)_eval_opin_code);

//		cJSON_AddNumberToObject(root, "eval_opin_code",packoperator.subOperator[i].eval_opin_code);
		cJSON_AddNumberToObject(root, "eval_opin_extend",packoperator.subOperator[i].eval_opin_extend);
		cJSON_AddNumberToObject(root, "oper_on_mile",packoperator.subOperator[i].oper_on_mile);
		cJSON_AddNumberToObject(root, "oper_off_mile",packoperator.subOperator[i].oper_off_mile);
		cJSON_AddNumberToObject(root, "fuel_charge",packoperator.subOperator[i].fuel_charge);
		byte wait_time[5];
		Hex2Str((char*)packoperator.subOperator[i].wait_time,(char*)wait_time,4);
		//memcpy(wait_time,&packoperator.subOperator[i].wait_time[0],3);
		wait_time[4]='\0';
		cJSON_AddStringToObject(root, "wait_time",(char*)wait_time);
		cJSON_AddNumberToObject(root, "amount",packoperator.subOperator[i].amount);

	    char _amount_code[2];
		sprintf( _amount_code, "%d", int(packoperator.subOperator[i].amount_code) );

		cJSON_AddStringToObject(root, "amount_code",(char*)_amount_code);

		//cJSON_AddNumberToObject(root, "amount_code",packoperator.subOperator[i].amount_code);

		in_str = cJSON_PrintUnformatted(root);
		out_str[i] = in_str;
        if(in_str != nullptr)
        {
            free(in_str);
            in_str =nullptr;
        }
		cJSON_Delete(root);
    }


}

/* Parse the input body DailyOperator msg , and populate the result into body json. */
void PkgDataToJson::PkgDailyOperatorToJson(protocol_send_data_header& rechead,PackDailyOper& packdailyoperator,string *out_str)
{

	char* in_str;
	char _timestamp[64];

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);

	//char *out_str = malloc(1000);
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "timestamp",_timestamp);
    char * _frim =(char*)rechead.send_data_header.frim;
    char * _prov =(char*)rechead.send_data_header.prov;
    char * _city =(char*)rechead.send_data_header.city;
    char * _area_code =(char*)rechead.send_data_header.area_code;
    char * _type =(char*)rechead.send_data_header.type;
    char * _command =(char*)rechead.send_data_header.command;

    cJSON_AddStringToObject(root, "frim",_frim);
    cJSON_AddStringToObject(root, "prov",_prov);
    cJSON_AddStringToObject(root, "city",_city);
    cJSON_AddStringToObject(root, "area_code",_area_code);
    cJSON_AddStringToObject(root, "car_type",_type);
    cJSON_AddStringToObject(root, "command",_command);



    cJSON_AddNumberToObject(root, "count_date",packdailyoperator.count_date);
    cJSON_AddNumberToObject(root, "areaCode",packdailyoperator.area_code);
    cJSON_AddNumberToObject(root, "taxi_num",packdailyoperator.taxi_num);
    cJSON_AddNumberToObject(root, "online_num",packdailyoperator.online_num);
    cJSON_AddNumberToObject(root, "oper_num",packdailyoperator.oper_num);
    cJSON_AddNumberToObject(root, "oper_time",packdailyoperator.oper_time);
    cJSON_AddNumberToObject(root, "oper_day",packdailyoperator.oper_day);
    cJSON_AddNumberToObject(root, "oper_night",packdailyoperator.oper_night);
    cJSON_AddNumberToObject(root, "oper_freq",packdailyoperator.oper_freq);
    cJSON_AddNumberToObject(root, "oper_mile",packdailyoperator.oper_mile);
    cJSON_AddNumberToObject(root, "offoper_mile",packdailyoperator.offoper_mile);
    cJSON_AddNumberToObject(root, "all_utiliz",packdailyoperator.all_utiliz);
    cJSON_AddNumberToObject(root, "offpeak_utiliz",packdailyoperator.offpeak_utiliz);
    cJSON_AddNumberToObject(root, "peak_utiliz",packdailyoperator.peak_utiliz);
    cJSON_AddNumberToObject(root, "lowspeed_on",packdailyoperator.lowspeed_on);
//    cJSON_AddStringToObject(msgbody, "lowspeed_off",(char*)int_to_char(packdailyoperator.lowspeed_off));
    cJSON_AddNumberToObject(root, "car_income",packdailyoperator.car_income);
    cJSON_AddNumberToObject(root, "eval_times",packdailyoperator.eval_times);
    cJSON_AddNumberToObject(root, "verysatisfy",packdailyoperator.verysatisfy);
    cJSON_AddNumberToObject(root, "satisfy",packdailyoperator.satisfy);
    cJSON_AddNumberToObject(root, "nosatisfy",packdailyoperator.nosatisfy);
    cJSON_AddNumberToObject(root, "call_times",packdailyoperator.call_times);
    cJSON_AddNumberToObject(root, "call_success",packdailyoperator.call_success);
    cJSON_AddNumberToObject(root, "call_fail",packdailyoperator.call_fail);

	in_str = cJSON_PrintUnformatted(root);
	out_str[0] = in_str;
	//memcpy(&out_str,&in_str,sizeof((char*)in_str));
    if(in_str != nullptr)
    {
        free(in_str);
        in_str =nullptr;
    }

    cJSON_Delete(root);


}

/* Parse the input body MonthlyOperator msg , and populate the result into body json. */
void PkgDataToJson::PkgMonthlyOperatorToJson(protocol_send_data_header& rechead,PackMonthlyOper& packmonthlyoperator,string out_str)
{
	char* in_str;
	char _timestamp[64];

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);
    cJSON* root = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "timestamp",_timestamp);

    cJSON_AddNumberToObject(root, "area_code",packmonthlyoperator.area_code);
    cJSON_AddNumberToObject(root, "count_interval",packmonthlyoperator.count_interval);
    cJSON_AddNumberToObject(root, "veh_color",packmonthlyoperator.veh_color);
    cJSON_AddNumberToObject(root, "veh_plate",packmonthlyoperator.veh_plate);
    cJSON_AddNumberToObject(root, "online_time",packmonthlyoperator.online_time);
    cJSON_AddNumberToObject(root, "oper_time",packmonthlyoperator.oper_time);
    cJSON_AddNumberToObject(root, "oper_day",packmonthlyoperator.oper_day);
    cJSON_AddNumberToObject(root, "oper_night",packmonthlyoperator.oper_night);
    cJSON_AddNumberToObject(root, "operate_freq",packmonthlyoperator.operate_freq);
    cJSON_AddNumberToObject(root, "oper_on_mile",packmonthlyoperator.oper_on_mile);
    cJSON_AddNumberToObject(root, "oper_off_mile",packmonthlyoperator.oper_off_mile);
    cJSON_AddNumberToObject(root, "car_income",packmonthlyoperator.car_income);
    cJSON_AddNumberToObject(root, "card_income",packmonthlyoperator.card_income);
    cJSON_AddNumberToObject(root, "eval_times",packmonthlyoperator.eval_times);
    cJSON_AddNumberToObject(root, "verysatisfy",packmonthlyoperator.verysatisfy);
    cJSON_AddNumberToObject(root, "satisfy",packmonthlyoperator.satisfy);
    cJSON_AddNumberToObject(root, "nosatisfy",packmonthlyoperator.nosatisfy);
    cJSON_AddNumberToObject(root, "call_success",packmonthlyoperator.call_success);
    cJSON_AddNumberToObject(root, "call_fail",packmonthlyoperator.call_fail);

	in_str = cJSON_PrintUnformatted(root);
	out_str = in_str;
	free(in_str);
	in_str =nullptr;
    cJSON_Delete(root);


}

/* Parse the input body DriverCard msg , and populate the result into body json. */
void  PkgDataToJson::PkgDriverCardToJson(protocol_send_data_header& rechead,PackDriverCard& packdrivercard,string * out_str)
{
	char* in_str;
	char _timestamp[64];

    char * _frim =(char*)rechead.send_data_header.frim;
    char * _prov =(char*)rechead.send_data_header.prov;
    char * _city =(char*)rechead.send_data_header.city;
    char * _area_code =(char*)rechead.send_data_header.area_code;
    char * _type =(char*)rechead.send_data_header.type;
    char * _command =(char*)rechead.send_data_header.command;

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);

    byte plate_u_value[256];
    byte stamp_value[128];
    byte reg_value[128];
    byte info_driver_value[256];
    byte info_cert_value[256];
    byte info_comp_value[256];

	memset(plate_u_value,0,sizeof(plate_u_value));
	memset(stamp_value,0,sizeof(stamp_value));
	memset(reg_value,0,sizeof(reg_value));
	memset(info_driver_value,0,sizeof(info_driver_value));
	memset(info_cert_value,0,sizeof(info_cert_value));
	memset(info_comp_value,0,sizeof(info_comp_value));


    byte _imei_temp[21];
    byte _imei[21];
    Hex2Str((char*)packdrivercard.imei,(char*)_imei_temp,20);
    Hex2Str_zero((char*)_imei_temp,(char*)_imei,20);

	char * _cert_number =(char*)packdrivercard.driver_cert_number;
	char * _iden_number =(char*)packdrivercard.driver_iden_number;
	char * _comp_cert_number =(char*)packdrivercard.comp_cert_number;


    cJSON* root = cJSON_CreateObject();
    cJSON* pJsonsub_alarm = cJSON_CreateObject();
    cJSON* pJsonsub_veh = cJSON_CreateObject();

    cJSON_AddStringToObject(root, "timestamp",_timestamp);
    cJSON_AddStringToObject(root, "frim",_frim);
    cJSON_AddStringToObject(root, "prov",_prov);
    cJSON_AddStringToObject(root, "city",_city);
    cJSON_AddStringToObject(root, "area_code",_area_code);
    cJSON_AddStringToObject(root, "car_type",_type);
    cJSON_AddStringToObject(root, "command",_command);


    getg2uvalue(packdrivercard.veh_plate,plate_u_value);
    cJSON_AddStringToObject(root, "veh_plate",(char*)plate_u_value);
    cJSON_AddStringToObject(root, "imei",(char*)_imei);
    cJSON_AddNumberToObject(root, "reg_state",packdrivercard.reg_state);

    getregStamp(packdrivercard.reg_time,reg_value);
    cJSON_AddStringToObject(root, "reg_time",(char*)reg_value);
    cJSON_AddNumberToObject(root, "driver_num",packdrivercard.driver_num/2);

    getg2uvalue(packdrivercard.driver_name,info_driver_value);
    cJSON_AddStringToObject(root, "driver_name",(char*)info_driver_value);

    cJSON_AddStringToObject(root, "driver_cert_number",(char*)_cert_number);

    cJSON_AddStringToObject(root, "driver_iden_number",(char*)_iden_number);

    cJSON_AddStringToObject(root, "comp_cert_number",(char*)_comp_cert_number);

    cJSON_AddNumberToObject(root, "inst_cert_num",packdrivercard.inst_cert_num/2);

//	byte inst_cert_name[packdrivercard.inst_cert_num];
//	memcpy(inst_cert_name,&packdrivercard.inst_cert_name[0],packdrivercard.inst_cert_num);

	getg2uvalue(packdrivercard.inst_cert_name,info_cert_value);
	cJSON_AddStringToObject(root, "inst_cert_name",(char*)info_cert_value);





    byte start_date[9];
	Hex2Str((char*)packdrivercard.cert_startdate,(char*)start_date,8);
	start_date[8]='\0';
    cJSON_AddStringToObject(root, "cert_startdate",(char*)start_date);

    byte dead_line[9];
	Hex2Str((char*)packdrivercard.cert_deadline,(char*)dead_line,8);
	dead_line[8]='\0';
    cJSON_AddStringToObject(root, "cert_deadline",(char*)dead_line);

    cJSON_AddNumberToObject(root, "comp_num",packdrivercard.comp_num/2);
    getg2uvalue(packdrivercard.comp_name,info_comp_value);
    cJSON_AddStringToObject(root, "comp_name",(char*)info_comp_value);

    getgpsStamp(packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.stamp,stamp_value);
    cJSON_AddStringToObject(root, "stamp",(char*)stamp_value);
    cJSON_AddNumberToObject(root, "latitude",packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude/600000.0);
    cJSON_AddNumberToObject(root, "longitude",packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude/600000.0);
    cJSON_AddNumberToObject(root, "speed",packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed);
    cJSON_AddNumberToObject(root, "altitude",packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.altitude);
    cJSON_AddNumberToObject(root, "direction_angle",packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.direction_angle*2);


    PkgPostionAlarmStateToJson(pJsonsub_alarm,packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state);
    PkgPostionVehStateToJson(pJsonsub_veh,packdrivercard.drivercardpos.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state);
    cJSON_AddItemToObject(root, "alarm_state", pJsonsub_alarm);
    cJSON_AddItemToObject(root, "veh_state", pJsonsub_veh);


	in_str = cJSON_PrintUnformatted(root);

	out_str[0] = in_str;
	//memcpy(&out_str,&in_str,sizeof((char*)in_str));

    if(in_str != nullptr)
    {
        free(in_str);
        in_str =nullptr;
    }
    cJSON_Delete(root);



}

/* Parse the input body DriverViolate msg , and populate the result into body json. */
void PkgDataToJson::PkgDriverViolateToJson(protocol_send_data_header& rechead,PackDriverViolate& packdriverviolate,string *out_str)
{
	char* in_str;
	char _timestamp[64];

	SYSTEMTIME _systime;
	GetLocalTime(&_systime);
	sprintf((char *)_timestamp,"%04d-%02d-%02d %02d:%02d:%02d",_systime.wYear,_systime.wMonth,_systime.wDay,_systime.wHour,_systime.wMinute,_systime.wSecond);

    byte stamp_value[128];
    byte plate_u_value[256];
    byte info_comp_value[256];
    byte info_driver_value[256];
    cJSON* root = cJSON_CreateObject();
    cJSON* pJsonsub_alarm = cJSON_CreateObject();
    cJSON* pJsonsub_veh = cJSON_CreateObject();

	memset(plate_u_value,0,sizeof(plate_u_value));
	memset(info_driver_value,0,sizeof(info_driver_value));
	memset(info_comp_value,0,sizeof(info_comp_value));
	memset(stamp_value,0,sizeof(stamp_value));


    byte _imei_temp[21];
    byte _imei[21];
    Hex2Str((char*)packdriverviolate.imei,(char*)_imei_temp,20);
    Hex2Str_zero((char*)_imei_temp,(char*)_imei,20);

	char * _driver_cert_number =(char*)packdriverviolate.driver_iden_number;
    char * _frim =(char*)rechead.send_data_header.frim;
    char * _prov =(char*)rechead.send_data_header.prov;
    char * _city =(char*)rechead.send_data_header.city;
    char * _area_code =(char*)rechead.send_data_header.area_code;
    char * _type =(char*)rechead.send_data_header.type;
    char * _command =(char*)rechead.send_data_header.command;

	cJSON_AddStringToObject(root, "timestamp",_timestamp);
    cJSON_AddStringToObject(root, "frim",_frim);
    cJSON_AddStringToObject(root, "prov",_prov);
    cJSON_AddStringToObject(root, "city",_city);
    cJSON_AddStringToObject(root, "area_code",_area_code);
    cJSON_AddStringToObject(root, "car_type",_type);
    cJSON_AddStringToObject(root, "command",_command);

    getg2uvalue(packdriverviolate.veh_plate,plate_u_value);
    cJSON_AddStringToObject(root, "veh_plate",(char*)plate_u_value);
    cJSON_AddStringToObject(root, "imei",(char*)_imei);
    getgpsStamp(packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.stamp,stamp_value);
    cJSON_AddStringToObject(root, "stamp",(char*)stamp_value);
    cJSON_AddNumberToObject(root, "latitude",packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.latitude/600000.0);
    cJSON_AddNumberToObject(root, "longitude",packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.longitude/600000.0);
    cJSON_AddNumberToObject(root, "speed",packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.speed);
    cJSON_AddNumberToObject(root, "altitude",packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.altitude);
    cJSON_AddNumberToObject(root, "direction_angle",packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.direction_angle*2);
    cJSON_AddNumberToObject(root, "driver_num",packdriverviolate.driver_num/2);
    getg2uvalue(packdriverviolate.driver_name,info_driver_value);
    cJSON_AddStringToObject(root, "driver_name",(char*)info_driver_value);
    cJSON_AddNumberToObject(root, "comp_num",packdriverviolate.comp_num/2);
    getg2uvalue(packdriverviolate.comp_name,info_comp_value);
    cJSON_AddStringToObject(root, "comp_name",(char*)info_comp_value);

    cJSON_AddStringToObject(root, "driver_cert_number",(char*)_driver_cert_number);
    cJSON_AddNumberToObject(root, "alarm_type",packdriverviolate.alarm_type);


    PkgPostionAlarmStateToJson(pJsonsub_alarm,packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.alarm_state);
    PkgPostionVehStateToJson(pJsonsub_veh,packdriverviolate.driverviolate.msg_postion.subTraj[0].sub_postion.basiclocal.basic_postion.veh_state);
    cJSON_AddItemToObject(root, "alarm_state", pJsonsub_alarm);
    cJSON_AddItemToObject(root, "veh_state", pJsonsub_veh);


	in_str = cJSON_PrintUnformatted(root);

	out_str[0] = in_str;
	//memcpy(&out_str,&in_str,sizeof((char*)in_str));
    if(in_str != nullptr)
    {
        free(in_str);
        in_str =nullptr;
    }
    cJSON_Delete(root);


}
