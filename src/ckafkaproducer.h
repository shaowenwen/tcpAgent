#ifndef CKAFKAPRODUCER_H
#define CKAFKAPRODUCER_H

#include "librdkafka/rdkafka.h"
#include "zlog.h"
#include <string.h>

extern zlog_category_t* g_server_cat;

enum TopicType
{
    itsptaxi_gps = 0,
    itsptaxi_alarm,
    itsptaxi_vehicleservice,
    itsptaxi_dailyservice,
    itsptaxi_monthlyservice,
    itsptaxi_drivercard,
    itsptaxi_driverviolate
};

class CKafkaProducer
{
public:
    rd_kafka_t *                         m_kafka_handle;
    rd_kafka_topic_t *                   m_kafka_topic;
    rd_kafka_conf_t *                    m_kafka_conf;
    rd_kafka_topic_conf_t *              m_kafka_topic_conf;
    rd_kafka_topic_partition_list_t *    m_kafka_topic_partition_list;

    int m_partition;

public:
    CKafkaProducer();
    ~CKafkaProducer();

    int init(char *topic, char *brokers, int partition); //topic="my_test"; brokers="192.168.1.42:9092"; partition=0;
    int sendMessage(char *str, int len);

    static void err_cb(rd_kafka_t *rk, int err, const char *reason, void *opaque);
    static void throttle_cb(rd_kafka_t *rk, const char *broker_name, int32_t broker_id, int throttle_time_ms, void *opaque);
    static void offset_commit_cb(rd_kafka_t *rk, rd_kafka_resp_err_t err, rd_kafka_topic_partition_list_t *offsets, void *opaque);
    static int stats_cb(rd_kafka_t *rk, char *json, size_t json_len, void *opaque);
};
#endif // CKAFKAPRODUCER_H
