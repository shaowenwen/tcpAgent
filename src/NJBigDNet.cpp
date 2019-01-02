//============================================================================
// Name        : NJBigDNet.cpp
// Author      : sw
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "protocol_binary.h"


//============================================================================
// Name        : tcpAgent.cpp
// Author      :
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================


#include <evpp/timestamp.h>
#include <evpp/tcp_server.h>
#include <evpp/buffer.h>
#include <evpp/tcp_conn.h>

#include "winmain-inl.h"

#include "binary_codec.h"
#include "appimp.h"


#include "memcache_server.h"

zlog_category_t* g_server_cat;
zlog_category_t* g_server_cat_count;

gTcpMap g_map_fd_TcpNLMsg;




int main(int argc, char* argv[]) {

//	memset(&p_tcp_nl_msg,0,sizeof(TCP_NL_MSG));
//	appMan->init_para();

    //std::string addr = "0.0.0.0:18888";

	g_map_fd_TcpNLMsg.clear();
    std::string addr = "0.0.0.0:12104";
    int thread_num = 200;
    evpp::EventLoop loop;
    evpp::TCPServer server(&loop, addr, "TCPEchoServer", thread_num);



    MemcacheServer *appServer = new MemcacheServer(&loop,&server);

    appServer->init_para();

    loop.Run();

    delete appServer;

	printf("0k  o\n");
    return 0;
}




