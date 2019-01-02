#ifndef MEMCACHE_SERVER_H
#define MEMCACHE_SERVER_H

#include <queue>
#include <map>
#include <mutex>

#include "evpp/tcp_conn.h"
#include "evpp/tcp_server.h"

#include "evpp/event_watcher.h"
#include "evpp/event_loop.h"
#include "evpp/event_loop_thread_pool.h"

#include "zlog.h"

#include "protocol_binary.h"

#include "appimp.h"
using namespace std;


class appManager;


class MemcacheServer : public std::enable_shared_from_this<MemcacheServer> {
public:
	MemcacheServer(evpp::EventLoop* evloop, evpp::TCPServer *tcp_server):tcp_server_(tcp_server),
	evloop_(evloop){

    }
	MemcacheServer(){

	}
    virtual ~MemcacheServer();
    void init_para();

    static void OnConnection(const evpp::TCPConnPtr& conn);//(const evpp::TCPConnPtr& conn,evpp::Buffer* msg);
    static void OnMessage(const evpp::TCPConnPtr& conn,evpp::Buffer* msg);
    static void OnProcessLNSYCZData(const evpp::TCPConnPtr& conn,uint8_t* msgbody,int RecMsgType,protocol_receive_data_header LNSYheader,protocol_send_data_header send_header);
	void Hex2Str( const char *sSrc,  char sDest[], int nSrcLen );


private:
    // noncopyable
    MemcacheServer(const MemcacheServer&);
    const MemcacheServer& operator=(const MemcacheServer&);
private:
    evpp::TCPServer *tcp_server_;
    evpp::EventLoop *evloop_;

//    appManager  *appMan ;



};

#endif

