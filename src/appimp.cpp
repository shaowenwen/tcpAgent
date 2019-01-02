#include "appimp.h"



appManager* appManager::appMan = new appManager();

appManager::appManager()
{

	kp_gps = NULL;
	kp_alarm = NULL;
	kp_vehService = NULL;
	kp_dailyService = NULL;
	kp_monthlyService = NULL;
	kp_driverCard = NULL;
	kp_driverViolate = NULL;

}




void appManager::init_para()
{
	// init log

    char szFileName[260] = {0};
    FILE* stream = fopen("/proc/self/cmdline", "r");
	fgets(szFileName, 260, stream);
	fclose(stream);

    int nIndex = strlen(szFileName);
	while (nIndex > 0 && szFileName[nIndex - 1] != '/')
	{nIndex--;};
	szFileName[nIndex- 1] = 0;

	while (nIndex > 0 && szFileName[nIndex - 2] != '/')
	{nIndex--;};
	szFileName[nIndex -1] = 0;


//	printf("szFileName:%s\n",szFileName);
	strncat(szFileName, "./conf/RecData_log.conf", 260);

	printf("szFileName:%s\n",szFileName);


	int ret = zlog_init(szFileName);///mnt/d/Projects/workspace/NJBigDNet/conf/"/opt/Projects/njitsAgent/tcpAgent/conf/RecData_log.conf"
	if (ret != 0)
	{
		printf("Initlize zlog faield\n");
		exit(EXIT_FAILURE);
	}

	g_server_cat = zlog_get_category("server_cat");
	if (!g_server_cat)
	{
	   printf("zlog_get_category failed\n");
		exit(EXIT_FAILURE);
	}

	g_server_cat_count = zlog_get_category("server_cat_count");
	if (!g_server_cat_count)
	{
	   printf("zlog_get_category failed\n");
		exit(EXIT_FAILURE);
	}
	// init kafka


	//char *brokers = "192.168.44.102:9092,192.168.44.103:9092,192.168.44.104:9092";
	//char *brokers = "192.168.44.161:9092,192.168.44.162:9092,192.168.44.163:9092";
    int partition = RD_KAFKA_PARTITION_UA;

	const char * topic_gps = "com.itsp.taxi.gps.sourcedata";
	const char * topic_alarm = "com.itsp.taxi.alarm.sourcedata";
	const char * topic_vehicleservice = "com.itsp.taxi.vehicleservice.sourcedata";
	const char * topic_dailyservice = "com.itsp.taxi.dailyservice.sourcedata";
	const char * topic_monthlyservice = "com.itsp.taxi.monthlyservice.sourcedata";
	const char * topic_drivercard = "com.itsp.taxi.drivercard.sourcedata";
	const char * topic_driverviolate = "com.itsp.taxi.driverviolate.sourcedata";


    //char *brokers = "192.168.44.102:9092,192.168.44.103:9092,192.168.44.104:9092";
	char *brokers = "127.0.0.1:9092";

    kp_gps = new CKafkaProducer();
    kp_alarm = new CKafkaProducer();
    kp_vehService = new CKafkaProducer();
    kp_dailyService = new CKafkaProducer();
    kp_monthlyService = new CKafkaProducer();
    kp_driverCard = new CKafkaProducer();
    kp_driverViolate = new CKafkaProducer();



    printf("0k\n");
    ret = kp_gps->init(const_cast<char*>(topic_gps), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: kp_gps.init(): ret=%d;\n", ret);}

    ret = kp_alarm->init(const_cast<char*>(topic_alarm), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: kp_alarm.init(): ret=%d;\n", ret);}

    ret = kp_vehService->init(const_cast<char*>(topic_vehicleservice), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: kp_vehService.init(): ret=%d;\n", ret);}

    ret = kp_dailyService->init(const_cast<char*>(topic_dailyservice), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: taxi_daily_service.init(): ret=%d;\n", ret);}

    ret = kp_monthlyService->init(const_cast<char*>(topic_monthlyservice), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: kp_monthlyService.init(): ret=%d;\n", ret);}

    ret = kp_driverCard->init(const_cast<char*>(topic_drivercard), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: kp_driverCard.init(): ret=%d;\n", ret);}

    ret = kp_driverViolate->init(const_cast<char*>(topic_driverviolate), brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: driver_violate.init(): ret=%d;\n", ret);}


    g_szCheckLastTime = GetTickCount();
	memset(&rec_count,0,sizeof(UrbanCount));
	memset(&send_count,0,sizeof(UrbanCount));
	memset(&last_send_count,0,sizeof(UrbanCount));


	codec = new BinaryCodec();

	pkgJson = new PkgDataToJson();


}



/* appManage Forwarding different messages to kafka  according to the header type*/
int appManager::pushKafkaData(char * recmsg,protocol_send_data_header &head)
{
    int ret =0;
    if(strcmp("track",(char*)head.send_data_header.command) == 0)
    {
        ret = kp_gps->sendMessage(recmsg, strlen(recmsg));
    }
    else if(strcmp("alarm",(char*)head.send_data_header.command) == 0)
    {
        ret = kp_alarm->sendMessage(recmsg, strlen(recmsg));
    }
    else if(strcmp("veh_service",(char*)head.send_data_header.command) == 0)
	{
		ret = kp_vehService->sendMessage(recmsg, strlen(recmsg));
	}
    else if(strcmp("taxi_daily_service",(char*)head.send_data_header.command) == 0)
	{
		ret = kp_dailyService->sendMessage(recmsg, strlen(recmsg));
	}
    else if(strcmp("taxi_monthly_service",(char*)head.send_data_header.command) == 0)
	{
		ret = kp_monthlyService->sendMessage(recmsg, strlen(recmsg));
	}
    else if(strcmp("driver_card",(char*)head.send_data_header.command) == 0)
	{
		ret = kp_driverCard->sendMessage(recmsg, strlen(recmsg));
	}
    else if(strcmp("driver_violate",(char*)head.send_data_header.command) == 0)
	{
		ret = kp_driverViolate->sendMessage(recmsg, strlen(recmsg));
	}
    if(ret != 0)
    {
        zlog_error(g_server_cat,"Error: kp.sendMessage(): ret=%d;\n", ret);
        return ret;
    }
    return ret;
}

UINT32 appManager::GetTickCount()
{
    struct timeval tv;
    if(gettimeofday(&tv, NULL) != 0)
        return 0;
    return (tv.tv_sec * 1000) + (tv.tv_usec / 1000);
}


void appManager::urbanVehCount(protocol_send_data_header head,protocol_receive_data_header rec_head)
{
	unsigned int m_ncurrentEXTime = GetTickCount();
	unsigned int m_nintervalTime = (m_ncurrentEXTime - g_szCheckLastTime)/1000;


	if((strcmp((char*)head.send_data_header.frim , "rm" ) == 0)&& (strcmp((char*)head.send_data_header.city,"ShenYangShi") ==0 )&& (strcmp((char*)head.send_data_header.prov ,"LiaoNingSheng" ))==0)
	{
		rec_count.lnsyczrm++;
		send_count.lnsyczrm = rec_head.receive_data_header.msg_sn - last_send_count.lnsyczrm  ;
	}else if((strcmp((char*)head.send_data_header.frim, "rz" )== 0)&& (strcmp((char*)head.send_data_header.city , "ShenYangShi" )== 0)&& (strcmp((char*)head.send_data_header.prov , "LiaoNingSheng" )== 0))
	{
		rec_count.lnsyczrz++;
		send_count.lnsyczrz = rec_head.receive_data_header.msg_sn - last_send_count.lnsyczrz ;
	}else if((strcmp((char*)head.send_data_header.frim , "rz")== 0 )&& (strcmp((char*)head.send_data_header.city ,"HaMiDiQu" )== 0)&& (strcmp((char*)head.send_data_header.prov , "XinJiangWeiWuErZiZhiQu" )== 0))
	{
		rec_count.xjhmczrz++;
		send_count.xjhmczrz = rec_head.receive_data_header.msg_sn - last_send_count.xjhmczrz ;
	}

    if(m_nintervalTime > 60)   //send the data to NodeSMonitorSrv per 10 second
    {
    	g_szCheckLastTime = m_ncurrentEXTime;
    	memcpy(&last_send_count,&send_count,sizeof(UrbanCount));

    	zlog_info(g_server_cat_count,"urbanVeharea:lnsyczrm,rec_count:%d,send_count:%d\n",rec_count.lnsyczrm,send_count.lnsyczrm);
    	zlog_info(g_server_cat_count,"urbanVeharea:lnsyczrz,rec_count:%d,send_count:%d\n",rec_count.lnsyczrz,send_count.lnsyczrz);
		zlog_info(g_server_cat_count,"urbanVeharea:xjhmczrz,rec_count:%d,send_count:%d\n",rec_count.xjhmczrz,send_count.xjhmczrz);

    	memset(&rec_count,0,sizeof(UrbanCount));
    	memset(&send_count,0,sizeof(UrbanCount));
    }
}

appManager* appManager::instance() {
    return appManager::appMan;
}
