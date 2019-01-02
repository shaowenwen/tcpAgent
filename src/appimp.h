#ifndef APPIMP_H
#define APPIMP_H

#include "zlog.h"
#include <string.h>
#include <stdlib.h>
#include <iostream>


#include "protocol_binary.h"
#include "PkgDataToJson.h"
#include "ckafkaproducer.h"
#include "binary_codec.h"


class appManager;
class CKafkaProducer;

extern zlog_category_t* g_server_cat;
extern zlog_category_t* g_server_cat_count;
using namespace std;


class appManager
{
public:
    appManager();

    static appManager* instance();

    void init_para();

    int pushKafkaData(char * recmsg,protocol_send_data_header &head);

    UrbanCount  last_send_count;
    UrbanCount  send_count;
    UrbanCount  rec_count;
    UINT32 GetTickCount();
    void urbanVehCount(protocol_send_data_header head,protocol_receive_data_header rec_head);

    BinaryCodec *codec;
    PkgDataToJson * pkgJson;

private:
    static appManager* appMan;


    CKafkaProducer *kp_gps;
    CKafkaProducer *kp_alarm;
    CKafkaProducer *kp_vehService;
    CKafkaProducer *kp_dailyService;
    CKafkaProducer *kp_monthlyService;
    CKafkaProducer *kp_driverCard;
    CKafkaProducer *kp_driverViolate;

    UINT32 g_szCheckLastTime ;





};


#endif // APPIMP_H
