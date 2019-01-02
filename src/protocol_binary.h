/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * Copyright (c) <2008>, Sun Microsystems, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of the  nor the
 *       names of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY SUN MICROSYSTEMS, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL SUN MICROSYSTEMS, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
/*
 * Summary: Constants used by to implement the binary protocol.
 *
 * Copy: See Copyright for the status of this software.
 *
 * Author: Trond Norbye <trond.norbye@sun.com>
 */

#ifndef PROTOCOL_BINARY_H
#define PROTOCOL_BINARY_H


#include <stdint.h>
#include <map>



/**
 * \addtogroup Protocol
 * @{
 */

/**
 * This file contains definitions of the constants and packet formats
 * defined in the binary specification. Please note that you _MUST_ remember
 * to convert each multibyte field to / from network byte order to / from
 * host order.
 */
#ifdef __cplusplus
extern "C"
{
#endif

	#define MAX_MSG_LEN 165535

	using namespace std;


	typedef  unsigned int       UINT32;
	typedef  unsigned short     UINT16;
	typedef  unsigned char      byte;

	// - - - CRC -16 CITT --- //

	static int crcTable[] = { 0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7, 0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF, 0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6, 0x9339,
				0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE, 0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485, 0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D, 0x3653, 0x2672, 0x1611, 0x0630, 0x76D7,
				0x66F6, 0x5695, 0x46B4, 0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC, 0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823, 0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B, 0x5AF5,
				0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12, 0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A, 0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41, 0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A,
				0xBD0B, 0x8D68, 0x9D49, 0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70, 0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78, 0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F, 0x1080,
				0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067, 0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E, 0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256, 0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E,
				0xE54F, 0xD52C, 0xC50D, 0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405, 0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C, 0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634, 0xD94C,
				0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB, 0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3, 0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A, 0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1,
				0x1AD0, 0x2AB3, 0x3A92, 0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9, 0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1, 0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8, 0x6E17,
				0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0 };

    /**
     * Definition of the legal "magic" values used in a packet.
     * See section 3.1 Magic byte
     */
    typedef enum {
        PROTOCOL_FRIM_LN_SY_CZ_RM = 0x10EEFFF1,
		PROTOCOL_FRIM_LN_SY_CZ_RZ = 0x10EEFFF2,
		PROTOCOL_FRIM_XJ_HM_CZ_RZ = 0x10EEEEF1,
		PROTOCOL_FRIM_QG_SJ_RZ    = 0x10EEEEF2,
		PROTOCOL_FRIM_LN_SY_CZ_SJ = 0x30EEEEF1,
		PROTOCOL_FRIM_TEST        = 0xFFFFFFFF

    } protocol_binary_magic;



    typedef struct  _TCP_NL_MSG
    {
        unsigned char g_recv_buff[2*MAX_MSG_LEN];
        int g_recv_len;
        int flag_in_NL_proc;
    }TCP_NL_MSG;


    typedef map<int, TCP_NL_MSG> 	gTcpMap;



    typedef enum {
            UP_SUM_MSG_CITY_DAY_REQ        =    0x2201,
            UP_SUM_MSG_CITY_DAY_RESP       =    0x8201,
            UP_SUM_MSG_CAR_MONTH_REQ       =    0x2202,
            UP_SUM_MSG_CAR_MONTH_RESP      =    0x8201,
            UP_EXG_MSG_REAL_RUN_REQ        =    0x2103,
            UP_EXG_MSG_REAL_RUN_RESP       =    0x8103,
            UP_EXG_MSG_REAL_LOCATION_REQ   =    0x2101,
            UP_EXG_MSG_REAL_LOCATION_RESP  =    0x8101,
            UP_EXG_MSG_LOGON_STATUS_REQ    =    0x2185,
            UP_EXG_MSG_LOGON_STATUS_RESP   =    0x8185,
            UP_EXG_MSG_ILLEGAL_REQ         =    0x2186,
            UP_EXG_MSG_ILLEGAL_RESP        =    0x8186,
			UP_EXG_MSG_ADAS_REQ            =    0x2187,
			UP_EXG_MSG_ADAS_RESP           =    0x8187
    } protocol_service_code;

    /**
     * Definition of the data types in the packet
     * See section 3.4 Data Types
     */
    typedef enum {
        PROTOCOL_BINARY_RAW_BYTES = 0x00
    } protocol_binary_datatypes;

    /**
     * Definition of the header structure for a request packet.
     * See section 2
     */

    enum  RelayRet
    {
    	  error=0,
    	  success,
    	  crcerror
    };


    typedef struct _SYSTECOUNT {
    	UINT32 lnsyczrz;
    	UINT32 lnsyczrm;
    	UINT32 lnsyczsj;
    	UINT32 xjhmczrz;
    	UINT32 hldqwh;
    	UINT32 bjbjcz;
    }UrbanCount;

    typedef struct _SYSTEMTIME {
        UINT16 wYear;
        UINT16 wMonth;
        UINT16 wDayOfWeek;
        UINT16 wDay;
        UINT16 wHour;
        UINT16 wMinute;
        UINT16 wSecond;
        UINT16 wMilliseconds;
    }SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;



    typedef union {
        struct {
				byte	  frim[8];			//
				byte      area_code[8];
				byte      type[32];
				byte      command[32];
				byte      prov[64];
				byte      city[64];
        } send_data_header;
        uint8_t bytes[64];
    }protocol_send_data_header;





    // - - - 26 bytes msg basic postion   - - - //
    typedef union {
		struct tagPackBasicPostion{
			byte    encrypt;
			UINT32  alarm_state;
			UINT32  veh_state;
			UINT32  latitude;
			UINT32  longitude;
			UINT16  speed;
			byte    direction_angle;
			byte    stamp[6];
		}basic_postion;
		uint8_t bytes[26];
    }protocol_receive_basic_postion_body;

    typedef union {
        struct {
				UINT32		msg_length;			//
				UINT32      msg_sn;
				UINT32      msg_id;
				UINT32      msg_gnsscenterid;
				byte        version_flag[3];
				byte        encrypt_flag;
				UINT32      encrypt_key;
        } receive_data_header;
        uint8_t bytes[22];
    }protocol_receive_data_header;

    typedef union {
            struct {
            	protocol_receive_basic_postion_body basiclocal;
				UINT16  altitude;
				byte    plate[12];
				byte    imei[21];
				byte    platecolor;
            }sub_postion;
            uint8_t bytes[51];
    }protocol_receive_sub_postion_body;


    // - - - msg postion - - - //
    typedef union {
                struct {
				UINT16  service_module;
				UINT32  datalen;
				protocol_receive_sub_postion_body subTraj[100];

			}msg_postion;
			uint8_t bytes[51*100+6];
		}protocol_receive_msg_postion_body;



	// - - - msg postion - - - //
	typedef union {
				struct {
				UINT16  service_module;
				UINT32  datalen;
				protocol_receive_sub_postion_body subTraj[100];
			}msg_postion;
			uint8_t bytes[51*100+6];
		}protocol_receive_msg_adas_body;



	// ----63bytes----/
	typedef struct tagPackSubOperate{
		byte      plate[12];
		byte      platecolor;
		byte      encrypt;
		UINT32    getin_lat;
		UINT32    getin_lon;
		byte      getin_time[7];
		UINT32    getout_lat;
		UINT32    getout_lon;
		byte      getout_time[7];
		byte      cert_number[19];
		byte      eval_opin_code;
		UINT16    eval_opin_extend;
		UINT16    oper_on_mile;
		UINT16    oper_off_mile;
		UINT16    fuel_charge;
		byte      wait_time[2];
		UINT16    amount;
		byte      amount_code;
		byte      imei[11];
	}PackSubOperator;

	// - - - msg veh operator - - - //
	typedef struct tagPackOperate{
		UINT16    service_module;
		UINT32    datalen;
		PackSubOperator  subOperator[10];
	}PackOperator;


	// - - - msg daily operate - - - //
	typedef struct tagPackDailyOper{
		UINT16      service_module;
		UINT32      datalen;
		UINT32        count_date;
		UINT32      area_code;
		UINT32      taxi_num;
		UINT32      online_num;
		UINT32      oper_num;
		UINT32      oper_time;
		UINT32      oper_day;
		UINT32      oper_night;
		UINT32      oper_freq;
		UINT32      oper_mile;
		UINT32      offoper_mile;
		UINT32      all_utiliz;
		UINT32      offpeak_utiliz;
		UINT32      peak_utiliz;
		UINT32      lowspeed_on;
		UINT32      lowspeed_off;
		UINT32      car_income;
		UINT32      eval_times;
		UINT32      verysatisfy;
		UINT32      satisfy;
		UINT32      nosatisfy;
		UINT32      call_times;
		UINT32      call_success;
		UINT32      call_fail;
	}PackDailyOper;


	// - - - msg monthly operator - - - //
	typedef struct tagPackMonthlyOper{
		UINT16      service_module;
		UINT32      datalen;
		UINT32        area_code;
		UINT32        count_interval;
		UINT32        veh_color;
		UINT32        veh_plate;
		UINT32        online_time;
		UINT32        oper_time;
		UINT32        oper_day;
		UINT32        oper_night;
		UINT32        operate_freq;
		UINT32        oper_on_mile;
		UINT32        oper_off_mile;
		UINT32        oper_on_lowspeed;
		UINT32        oper_off_lowspeed;
		UINT32        car_income;
		UINT32        card_income;
		UINT32        eval_times;
		UINT32        verysatisfy;
		UINT32        satisfy;
		UINT32        nosatisfy;
		UINT32        call_success;
		UINT32        call_fail;

	}PackMonthlyOper;




	// - - - msg DriverCard- - - //
	typedef struct tagPackdDriverCard{
		UINT16        service_module;
		byte          veh_plate[12];
		protocol_receive_msg_postion_body      drivercardpos;
		byte          reg_state;
		byte          reg_time[5];
		byte          driver_num;
		byte          driver_name[32];
		byte          driver_cert_number[20];
		byte          driver_iden_number[20];
		byte          comp_cert_number[17];
		byte          inst_cert_num;
		byte          inst_cert_name[256+1];
		byte          cert_startdate[4];
		byte          cert_deadline[4];
		byte          comp_num;
		byte          comp_name[64];
		byte          imei[11];

	}PackDriverCard;

	// - - - msg DriverViolate - - - //
	typedef struct tagPackDriverViolate{
		UINT16        service_module;
		byte          veh_plate[12];
		protocol_receive_msg_postion_body      driverviolate;
		byte          driver_num;
		byte          driver_name[32];
		byte          comp_num;
		byte          comp_name[64];
		byte          driver_iden_number[19 + 1];
		byte          alarm_type;
		byte          imei[11];
	}PackDriverViolate;





	// - - - msg adas alarm - - - //
	typedef struct tagPackSubADASAlarm{
		UINT32        msg_single_len;
		byte          veh_plate[12];
		byte          plate_color;
		byte          imei[21];
		protocol_receive_basic_postion_body      adas_pos;
		byte          adas_type;
		byte          para_one_type;
		byte          para_one_len;
		byte          para_one_value[64];
		byte          para_two_type;
		byte          para_two_len;
		byte          para_two_value[64];
	}PackSubADASAlarm;

	// - - - msg adas alarm - - - //
	typedef struct tagPackADASAlarm{
		UINT16        service_module;
		UINT32        msg_total_len;
		PackSubADASAlarm   adas_alarm[21];
	}PackADASAlarm;

    /**


    /**
     * @}
     */

#ifdef __cplusplus
}
#endif
#endif /* PROTOCOL_BINARY_H */
