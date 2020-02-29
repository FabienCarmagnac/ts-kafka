#include <gtest/gtest.h>

#include <thread>

#include "ts-kafka/basic_consumer.hpp"
#include "ts-kafka/basic_producer.hpp"

#pragma comment(lib,"ts-kafka.lib")
#pragma comment(lib,"rdkafka.lib")
#pragma comment(lib,"gtest.lib")
#pragma comment(lib,"rdkafka++.lib")

using namespace ts_kafka;
using namespace std;

string global_test_endpoint;
string global_test_user;
string global_test_passwd;
string global_test_topic="test_topic";



int main(int argc, char** argv) 
{
    global_test_endpoint = argv[1];
    global_test_user = argv[2];
    global_test_passwd = argv[3];
    global_test_topic = argv[4];

    if (global_test_topic.empty())
    {
        cout << "topic is empty\n";
        return 0;
    }

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

        string err;
        int ix=0;
        bool s = bc.start(args, [&](tp t, const char* d, size_t sz)
        {
            cout << "received object " << endl;

            int_2 * tmp = (int_2*)d;
            if (tmp->key != key)
                return;

            EXPECT_EQ(tmp->ix, ix);

            ++ix;

        }, err);

        bc.wait();

        EXPECT_TRUE(s);
        EXPECT_EQ(nb_expected, ix);
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

        string err;
        cout << "connecting ..." << endl;

        if (!bc.connect(args, err))
        {
            cerr << "CANT CONNECT ! Details : " << err << endl;
            EXPECT_TRUE(false);
        }

        timed_data_raw rd; 
        rd.object.resize(sizeof(int_2));
        int i = 0;
        int_2* i2 = (int_2*)rd.object.data();

        i2->key = key;
        while (i++ < nb_expected)
        {
            cout << "pushing " << i << endl;
            i2->ix = i-1;
            rd.ts = now();
            EXPECT_EQ(bc.push(rd), producer_status::ok);

        }

        cout << "end prod" << endl;
        EXPECT_EQ(i, nb_expected);
    }

    // force bc dtor before leaving
    EXPECT_TRUE(true);

}


TEST(prod_cons, basic) 
{

    int key = rand();
    cout << "key=" << key << endl;

    int nbexpected = 10;
    basic_consumer bc;

    cout << "starting thread cons" << endl;
    thread co(start_basic_cons_thread, key, nbexpected, ref(bc));

    cout << "starting thread prod" << endl;
    thread pr(start_basic_prod_thread, key, nbexpected);


    pr.join();
    cout << "thread prod joined" << endl;
    bc.stop();
    cout << "consumer stopped" << endl;

    co.join();
    cout << "thread cons joined" << endl;

    
}