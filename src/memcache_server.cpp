#include "memcache_server.h"


static const size_t kHeaderLen = sizeof(protocol_receive_data_header);

extern gTcpMap g_map_fd_TcpNLMsg;
std::mutex g_io_mutex;

appManager  *appMan = appManager::instance();

MemcacheServer::~MemcacheServer() {


}

void MemcacheServer::init_para()
{
	cout <<"MemcacheServer::init_para()"<<endl;
	//appMan = new appManager();

	appMan->init_para();

    tcp_server_->SetMessageCallback(OnMessage);
    tcp_server_->SetConnectionCallback(OnConnection);

    tcp_server_->Init();
    tcp_server_->Start();

}


void MemcacheServer::OnConnection(const evpp::TCPConnPtr& conn){
	cout << "OnConnection " << conn->fd()  << endl;
	TCP_NL_MSG p_tcp_nl_msg;
	memset(&p_tcp_nl_msg,0,sizeof(TCP_NL_MSG));
	if (conn->IsConnected()) {
		g_io_mutex.lock();
	    map<int, TCP_NL_MSG>::iterator iter = g_map_fd_TcpNLMsg.find(conn->fd());
	    if (iter == g_map_fd_TcpNLMsg.end())
	    {
	    	zlog_info(g_server_cat,"insert conn->fd():%d\n",conn->fd());
	    	g_map_fd_TcpNLMsg.insert(gTcpMap::value_type(conn->fd(),p_tcp_nl_msg));
	    }
	    g_io_mutex.unlock();

	} else {
		g_io_mutex.lock();
	    map<int, TCP_NL_MSG>::iterator iter = g_map_fd_TcpNLMsg.find(conn->fd());
	    if (iter == g_map_fd_TcpNLMsg.end())
	    {
	    	return;
	    }
	    zlog_info(g_server_cat,"erase conn->fd():%d\n",conn->fd());
		g_map_fd_TcpNLMsg.erase(iter);
		g_io_mutex.unlock();
	}

}

void MemcacheServer::OnMessage(const evpp::TCPConnPtr& conn,evpp::Buffer* msg){
	cout << "OnMessage " << endl;

	int fd = conn->fd();
    map<int, TCP_NL_MSG>::iterator iter = g_map_fd_TcpNLMsg.find(fd);
    if (iter == g_map_fd_TcpNLMsg.end())
    {
        return;
    }
    TCP_NL_MSG *p_tcp_nl_msg = &(iter->second);
    uint8_t recvbuf[MAX_MSG_LEN];
    memset(recvbuf,0,MAX_MSG_LEN);

    uint8_t recvtempbuf[MAX_MSG_LEN];
    memset(recvtempbuf,0,MAX_MSG_LEN);

    int recvtemplen = msg->size();
    memcpy(&recvtempbuf,msg->data(),msg->size());

    int recvlen =0;
    if(recvtemplen == 0)
    {
    	zlog_debug(g_server_cat,"uehandle_recv_pack() receive shutdown command from peer endpoint\n");
        return;
    }
    if (recvtemplen == -1)
    {
    	zlog_debug(g_server_cat,"uehandle_recv_pack() ret -1,errno=%d\n", errno);
        return;
    }

	char log_orgData[recvtemplen];
	for(int j =0;j<recvtemplen;j++)
	{
		log_orgData[j]=recvtempbuf[j];
	}
	char log_orgtemp[recvtemplen*2 + 2];
	char szorgTmp[3];

	 for( int i = 0; i < recvtemplen; i++ )
	 {
		sprintf( szorgTmp, "%02x", (unsigned char) log_orgData[i] );
		memcpy( &log_orgtemp[i * 2], szorgTmp, 2 );
	 }

//		Hex2Str((char*)logData,logtemp,msglen);
	 log_orgtemp[recvtemplen*2] = ':';
	 log_orgtemp[recvtemplen*2+1] = '\0';


	zlog_info(g_server_cat,"LNSY Data Recvice :\n%s\n",log_orgtemp);


    if (p_tcp_nl_msg->flag_in_NL_proc == 1)
	{

		memcpy(p_tcp_nl_msg->g_recv_buff+p_tcp_nl_msg->g_recv_len,recvtempbuf,recvtemplen);
		recvtemplen += p_tcp_nl_msg->g_recv_len;
		memcpy(recvtempbuf, p_tcp_nl_msg->g_recv_buff, recvtemplen);

		memset(p_tcp_nl_msg->g_recv_buff, 0, 2*MAX_MSG_LEN);
		p_tcp_nl_msg->g_recv_len = 0;
		p_tcp_nl_msg->flag_in_NL_proc = 0;

	}

    byte head_flag;
	byte tail_flag;
	uint32_t msgLen = 0;

	int rcvindex =0;
	int realmsglen = 0;
	int lastMsgLen = 0;
	printf("recvtemplen:%d\n",recvtemplen);

	int transferredFlag = 0;
	if (recvtemplen >= kHeaderLen)
	{
		for(int i=0;i<recvtemplen ;i++)
		{
			lastMsgLen = lastMsgLen + msgLen;
			memcpy(&head_flag,&recvtempbuf [realmsglen],1);
			memcpy(&msgLen ,&recvtempbuf[1+ realmsglen] ,4);
			msgLen = ntohl(msgLen);

				if(head_flag == 0x5b)
				{
					recvbuf[rcvindex] = 0x5b;
					i++;
					rcvindex++;
					while(!(recvtempbuf[i]== 0x5d ))
					{
						if((recvtempbuf[i] == 0x5a) && (recvtempbuf[i+1] == 0x01))
						{
							recvbuf[rcvindex] = 0x5b;
							transferredFlag = 1;
						}
						else if((recvtempbuf[i] == 0x5a) && (recvtempbuf[i+1] == 0x02))
						{
							recvbuf[rcvindex] = 0x5a;
							transferredFlag = 1;
						}
						else if((recvtempbuf[i] == 0x5e )&& (recvtempbuf[i+1] == 0x01))
						{
							recvbuf[rcvindex] = 0x5d;
							transferredFlag = 1;
						}
						else if((recvtempbuf[i] == 0x5e) && (recvtempbuf[i+1] == 0x02))
						{
							recvbuf[rcvindex] = 0x5e;
							transferredFlag = 1;
						}
						else
						{
							recvbuf[rcvindex] = recvtempbuf[i];
						}
						if(transferredFlag ==1)
						{
							i++;
							transferredFlag = 0;
						}


						if((i >= recvtemplen -1  ))
						{
							break;
						}
						i++;
						rcvindex++;

					}
					if(recvtempbuf[i] == 0x5d)
					{
						recvbuf[rcvindex] = recvtempbuf[i];

						realmsglen = i + 1;

						if((recvtemplen- realmsglen )< int(kHeaderLen) && ((recvtemplen- realmsglen) >0))
						{
							i = recvtemplen;
							zlog_debug(g_server_cat,"recvlen(%u) < sizeof(TcpMsgHead):%u\n", recvtemplen- realmsglen, int(kHeaderLen));
							memset(p_tcp_nl_msg->g_recv_buff, 0, MAX_MSG_LEN);
							memcpy(p_tcp_nl_msg->g_recv_buff, &recvtempbuf[realmsglen ], recvtemplen - realmsglen);
							p_tcp_nl_msg->g_recv_len = recvtemplen - realmsglen;
							p_tcp_nl_msg->flag_in_NL_proc = 1;

							g_map_fd_TcpNLMsg[conn->fd()] = *p_tcp_nl_msg;

							msg->Truncate(recvtemplen);
							msg->Reset();

							lastMsgLen = lastMsgLen + msgLen;
							break;
						}

					}
					else
					{
						if(msgLen > (rcvindex+1- lastMsgLen))
						{
							zlog_debug(g_server_cat,"sizeof(TcpMsgHead):%u < recvlen(%u) < msglen(%u)",int(kHeaderLen), recvtemplen- realmsglen, msgLen);
							memset(p_tcp_nl_msg->g_recv_buff, 0, MAX_MSG_LEN);
							memcpy(p_tcp_nl_msg->g_recv_buff, &recvtempbuf[realmsglen ], recvtemplen - realmsglen);
							p_tcp_nl_msg->g_recv_len = recvtemplen - realmsglen;
							p_tcp_nl_msg->flag_in_NL_proc = 1;

							g_map_fd_TcpNLMsg[conn->fd()] = *p_tcp_nl_msg;

							msg->Truncate(recvtemplen);
							msg->Reset();
							if(lastMsgLen == 0)
							{
								return;
							}

							break;

						}

					}

				}
				else
				{
					msg->Truncate(recvtemplen);
					msg->Reset();
					zlog_error(g_server_cat,"msg  format error \n");
					return ;
				}

			rcvindex++;
		}

	}
	else
	{
		zlog_debug(g_server_cat,"recvlen(%u) < sizeof(TcpMsgHead):%u\n", recvlen,int(kHeaderLen));
		memset(p_tcp_nl_msg->g_recv_buff, 0, MAX_MSG_LEN);
		memcpy(p_tcp_nl_msg->g_recv_buff, recvtempbuf, recvtemplen);
		p_tcp_nl_msg->g_recv_len = recvtemplen;
		p_tcp_nl_msg->flag_in_NL_proc = 1;

		g_map_fd_TcpNLMsg[conn->fd()] = *p_tcp_nl_msg;

		msg->Truncate(recvtemplen);
		msg->Reset();
		return;

	}


	if((recvbuf[rcvindex-1] == 0x5d) && (recvtempbuf[recvtemplen - 1] == 0x5d ))
	{
		printf("rcvindex:%d,%d,%d\n",rcvindex,recvtemplen,lastMsgLen);
		recvlen = rcvindex;
	}
	else
	{
		printf("rcvindex1:%d,%d,%d\n",rcvindex,recvtemplen,lastMsgLen);
		recvlen = lastMsgLen;
	}
	zlog_debug(g_server_cat,"recvlen  :%d  \n",recvlen);

    uint8_t*  buf =recvbuf;//(uint8_t*)msg->data();
    int  realmsgLen = 0;
	while(1)
	{
			//analyze  msg header and get msg id
		protocol_receive_data_header msgheader;
		UINT16 RecMsgType;
		UINT32 receiveCode;

		bool verFlag = appMan->codec->veriftyHeaderMsg(msgheader,buf,RecMsgType,receiveCode);
		if(verFlag != true)
		{
			msg->Truncate(recvtemplen);
			msg->Reset();
			zlog_error(g_server_cat,"veriftyHeaderMsg  error \n");
			return;
		}
		zlog_debug(g_server_cat,"LNSYheader.msg_length:%d,%x\n",msgheader.receive_data_header.msg_length,RecMsgType);
		uint32_t msglen = msgheader.receive_data_header.msg_length;


		char logData[msglen];
		for(int j =0;j<msglen;j++)
		{
			logData[j]=buf[j];
		}
		char logtemp[msglen*2 + 2];
		char szTmp[3];

		 for( int i = 0; i < msglen; i++ )
		 {
			sprintf( szTmp, "%02x", (unsigned char) logData[i] );
			memcpy( &logtemp[i * 2], szTmp, 2 );
		 }

//		Hex2Str((char*)logData,logtemp,msglen);
		logtemp[msglen*2] = ';';
		logtemp[msglen*2+1] = '\0';


		zlog_info(g_server_cat,"LNSY Data Recvice :\n%s\n",logtemp);



		protocol_send_data_header  send_header;
		if(PROTOCOL_FRIM_LN_SY_CZ_RM == receiveCode )
		{
			sprintf((char *)send_header.send_data_header.frim,"%s","rm");
			sprintf((char *)send_header.send_data_header.prov,"%s","LiaoNingSheng");
			sprintf((char *)send_header.send_data_header.city,"%s","ShenYangShi");
			sprintf((char *)send_header.send_data_header.area_code,"%s","210100");
			sprintf((char *)send_header.send_data_header.type,"%s","cz");

			OnProcessLNSYCZData(conn,&buf[23], RecMsgType,msgheader,send_header);

		}else if(PROTOCOL_FRIM_LN_SY_CZ_RZ  == receiveCode )  //PROTOCOL_FRIM_XJ_HM_CZ_RZ
		{
			sprintf((char *)send_header.send_data_header.frim,"%s","rz");
			sprintf((char *)send_header.send_data_header.prov,"%s","LiaoNingSheng");
			sprintf((char *)send_header.send_data_header.city,"%s","ShenYangShi");
			sprintf((char *)send_header.send_data_header.area_code,"%s","210100");
			sprintf((char *)send_header.send_data_header.type,"%s","cz");

			OnProcessLNSYCZData(conn,&buf[23], RecMsgType,msgheader,send_header);
		}else if(PROTOCOL_FRIM_XJ_HM_CZ_RZ  == receiveCode )  //
		{
			sprintf((char *)send_header.send_data_header.frim,"%s","rz");
			sprintf((char *)send_header.send_data_header.prov,"%s","XinJiangWeiWuErZiZhiQu");
			sprintf((char *)send_header.send_data_header.city,"%s","HaMiDiQu");
			sprintf((char *)send_header.send_data_header.area_code,"%s","650500");
			sprintf((char *)send_header.send_data_header.type,"%s","cz");

			OnProcessLNSYCZData(conn,&buf[23], RecMsgType,msgheader,send_header);
		}else if(PROTOCOL_FRIM_TEST  == receiveCode )  //
		{
			sprintf((char *)send_header.send_data_header.frim,"%s","test");
			sprintf((char *)send_header.send_data_header.prov,"%s","test");
			sprintf((char *)send_header.send_data_header.city,"%s","test");
			sprintf((char *)send_header.send_data_header.type,"%s","test");

			OnProcessLNSYCZData(conn,&buf[23], RecMsgType,msgheader,send_header);
		}




		if (recvlen == msglen)
		{
			msg->Truncate(recvtemplen);
			msg->Reset();
			break;
		}
		recvlen -= msglen;
		buf += msglen;

	}

}

void MemcacheServer::Hex2Str( const char *sSrc,  char sDest[], int nSrcLen )
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



void MemcacheServer::OnProcessLNSYCZData(const evpp::TCPConnPtr& conn,uint8_t* msgbody,int RecMsgType,protocol_receive_data_header LNSYheader,protocol_send_data_header send_header)
{
	appMan->urbanVehCount(send_header,LNSYheader);
	evpp::Buffer retMsg;

	// deal different msg id
	switch(RecMsgType)
	{
		case UP_EXG_MSG_REAL_LOCATION_REQ:
		{
			protocol_receive_msg_postion_body    LNSYTraj;
			sprintf((char *)send_header.send_data_header.command,"%s","track");

			int trajlen =0;
			appMan->codec->veriftyPostionMsg(LNSYTraj,msgbody,trajlen);
//			 realy msg
			byte retmsg[32];
			int retlen =0;
			memset(&retmsg,0,sizeof(retmsg));
			appMan->codec->getLNSYRetMsg(LNSYheader,retmsg,retlen);
			retMsg.Write(retmsg,retlen);
			conn->Send(&retMsg);

//			// produce msg kafka
			string reqInfo[trajlen];
			appMan->pkgJson->PkgPostionToJson(send_header,LNSYTraj,trajlen,reqInfo);

			for(int i=0;i<trajlen;i++)
			{
				appMan->pushKafkaData((char*)reqInfo[i].c_str(),send_header);
				zlog_debug(g_server_cat,"reqInfo :%s  ",(char*)reqInfo[i].c_str());
			}

			break;
		}
		case UP_EXG_MSG_REAL_RUN_REQ:
		{

			PackOperator  LNSYOperator;
			sprintf((char *)send_header.send_data_header.command,"%s","veh_service");
			int operlen = appMan->codec->veriftyVehOperateMsg(LNSYOperator,msgbody);

			// realy msg
			byte retmsg[32];
			int retlen =0;
			memset(&retmsg,0,sizeof(retmsg));
			appMan->codec->getLNSYRetMsg(LNSYheader,retmsg,retlen);
			retMsg.Write(retmsg,27);
			conn->Send(&retMsg);

			// produce msg kafka
			string reqInfo[operlen] ;
			appMan->pkgJson->PkgVEHOperatorToJson(send_header,LNSYOperator,operlen,reqInfo);
			for(int i=0;i<operlen;i++)
			{
				appMan->pushKafkaData((char*)reqInfo[i].c_str(),send_header);
				zlog_debug(g_server_cat,"reqInfo :%s  ",(char*)reqInfo[i].c_str());
			}

			break;
		}

		case UP_SUM_MSG_CITY_DAY_REQ:
		{

				PackDailyOper  LNSYDailyOper;
				sprintf((char *)send_header.send_data_header.command,"%s","taxi_daily_service");
				appMan->codec->veriftyDailyOperMsg(LNSYDailyOper,msgbody);
				// realy msg
				byte retmsg[32];
				int retlen =0;
				memset(&retmsg,0,sizeof(retmsg));
				appMan->codec->getLNSYRetMsg(LNSYheader,retmsg,retlen);
				retMsg.Write(retmsg,retlen);
				conn->Send(&retMsg);
				// produce msg kafka
				string  reqInfo[1];
				appMan->pkgJson->PkgDailyOperatorToJson(send_header,LNSYDailyOper,reqInfo);
				zlog_debug(g_server_cat,"reqInfo :%s  ",reqInfo[0].c_str());
				appMan->pushKafkaData((char*)reqInfo[0].c_str(),send_header);

			break;
		}

		case UP_SUM_MSG_CAR_MONTH_REQ:
		{
//			sprintf((char *)send_header.send_data_header.command,"%s","taxi_monthly_service");
//			PackMonthlyOper LNSYMonthlyOper;
//			appMan->ucodec->veriftyMonthlyOperMsg(LNSYMonthlyOper,msgbody);
//			// realy msg
//			byte retmsg[32];
//			memset(&retmsg,0,sizeof(retmsg));
//			appMan->ucodec->getLNSYRetMsg(LNSYheader,retmsg);
//			retMsg.Write(retmsg,27);
//			conn->Send(&retMsg);
//			// produce msg kafka
//			string reqInfo ;
//			appMan->ucodec->pkgJson->PkgMonthlyOperatorToJson(send_header,LNSYMonthlyOper,reqInfo);
//			zlog_debug(g_server_cat,"reqInfo :%s  ",reqInfo);
//			pushKafkaData((char*)reqInfo.c_str(),send_header);

			break;
		}

		case UP_EXG_MSG_LOGON_STATUS_REQ:
		{
			PackDriverCard  LNSYDriverCard;
			sprintf((char *)send_header.send_data_header.command,"%s","driver_card");
			appMan->codec->veriftyDriverCardOperMsg(LNSYDriverCard,msgbody);
			// realy msg
			byte retmsg[32];
			int retlen =0;
			memset(&retmsg,0,sizeof(retmsg));
			appMan->codec->getLNSYRetMsg(LNSYheader,retmsg,retlen);
			retMsg.Write(retmsg,retlen);
			conn->Send(&retMsg);
			// produce msg kafka
			string reqInfo[1];
			appMan->pkgJson->PkgDriverCardToJson(send_header,LNSYDriverCard,reqInfo);
			zlog_debug(g_server_cat,"reqInfo :%s  ",reqInfo[0].c_str());
			appMan->pushKafkaData((char*)(reqInfo[0].c_str()),send_header);



			break;
		}

		case UP_EXG_MSG_ILLEGAL_REQ:
		{
			PackDriverViolate  LNSYDriverViolate;
			sprintf((char *)send_header.send_data_header.command,"%s","driver_violate");
			appMan->codec->veriftyDriverViolateOperMsg(LNSYDriverViolate,msgbody);
			// realy msg
			byte retmsg[32];
			int retlen =0;
			memset(&retmsg,0,sizeof(retmsg));
			appMan->codec->getLNSYRetMsg(LNSYheader,retmsg,retlen);
			retMsg.Write(retmsg,retlen);
			conn->Send(&retMsg);
			// produce msg kafka
			string reqInfo[1];
			appMan->pkgJson->PkgDriverViolateToJson(send_header,LNSYDriverViolate,reqInfo);
			zlog_debug(g_server_cat,"reqInfo :%s  ",reqInfo[0].c_str());
			appMan->pushKafkaData((char*)reqInfo[0].c_str(),send_header);


			break;
		}


		case UP_EXG_MSG_ADAS_REQ:
		{
			PackADASAlarm  ADASAlarm;
			sprintf((char *)send_header.send_data_header.command,"%s","alarm");
			int single_adas =0;
			printf("UP_EXG_MSG_ADAS_REQ\n");
			appMan->codec->veriftyADASMsg(ADASAlarm,msgbody,single_adas);
			// realy msg
			byte retmsg[32];
			int retlen =0;
			memset(&retmsg,0,sizeof(retmsg));
			appMan->codec->getLNSYRetMsg(LNSYheader,retmsg,retlen);
			retMsg.Write(retmsg,retlen);
			conn->Send(&retMsg);
			// produce msg kafka
			string reqInfo[single_adas] ;
			appMan->pkgJson->PkgADASToJson(send_header,ADASAlarm,single_adas,reqInfo);
			for(int i=0;i<single_adas;i++)
			{
				appMan->pushKafkaData((char*)reqInfo[i].c_str(),send_header);
				zlog_debug(g_server_cat,"reqInfo :%s  ",(char*)reqInfo[i].c_str());
			}


			break;
		}

		default:
			break;


	}

}





