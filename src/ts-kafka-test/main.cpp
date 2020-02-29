#include <gtest/gtest.h>

#include <thread>

#include "ts-kafka/basic_consumer.hpp"
#include "ts-kafka/basic_producer.hpp"

#pragma comment(lib,"ts-kafka.lib")
#pragma comment(lib,"rdkafka.lib")
#pragma comment(lib,"gtest.lib")
#pragma comment(lib,"rdkafka++.lib")

std::string global_test_endpoint;
std::string global_test_user;
std::string global_test_passwd;
std::string global_test_topic="test_topic";

using namespace ts_kafka;


int main(int argc, char** argv) 
{

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

struct int_2
{
    int key;
    int ix;
};

void start_basic_cons_thread(int key, int nb_expected, basic_consumer & bc)
{
    {
        
        consumer_start_args args;

        args.start_offset = -1;//HEAD
        args.consume_block_time_ms = 1000;
        args.endpoint = global_test_endpoint;
        args.topic = global_test_topic;
        args.user = global_test_user;
        args.passwd = global_test_passwd;

        std::string err;
        int ix;
        bool s = bc.start(args, [&](tp t, const char* d, size_t sz)
        {
            int_2 * tmp = (int_2*)d;
            if (tmp->key != key)
                return;

            EXPECT_EQ(tmp->ix, ix);

            ++ix;

        }, err);

        bc.wait();

        EXPECT_TRUE(s);
    }
}

void start_basic_prod_thread(int key, int nb_expected)
{
    {
        basic_producer bc;

        def_arg args;

        args.endpoint = global_test_endpoint;
        args.user = global_test_user;
        args.passwd = global_test_passwd;
        args.endpoint = global_test_endpoint;
        args.topic = global_test_topic;

        std::string err;
        if (!bc.connect(args, err))
        {
            std::cerr << err << std::endl;
            EXPECT_TRUE(false);
        }

        timed_data_raw rd; 
        rd.object.resize(sizeof(int_2));
        int i = 0;
        int_2* i2 = (int_2*)rd.object.data();

        i2->key = key;
        while (i++ < nb_expected)
        {
            i2->ix = i-1;
            rd.ts = now();
            EXPECT_EQ(bc.push(rd), producer_status::ok);

        }

        EXPECT_EQ(i, nb_expected);
    }

    // force bc dtor before leaving
    EXPECT_TRUE(true);

}


TEST(prod_cons, basic) 
{
    int key = rand();
    
    int nbexpected = 10;
    basic_consumer bc;

    std::thread co(start_basic_cons_thread, key, nbexpected, std::ref(bc));
    std::thread pr(start_basic_prod_thread, key, nbexpected);

    pr.join();
    bc.stop();

    co.join();
    
    
}