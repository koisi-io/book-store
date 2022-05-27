// Copyright (c) 2020 by Chrono
//
// g++ srv.cpp -std=c++14 -I../common -I../common/include -I/usr/local/include/luajit-2.1 -lluajit-5.1 -ldl -lzmq -lpthread -lcpr -lcurl -o a.out;./a.out
// g++ srv.cpp -std=c++14 -I../common -I../common/include -I/usr/local/include/luajit-2.1 -lluajit-5.1 -ldl -lzmq -lpthread -lcpr -lcurl -g -O0 -o a.out
// g++ srv.cpp -std=c++14 -I../common -I../common/include -I/usr/local/include/luajit-2.1 -lluajit-5.1 -ldl -lzmq -lpthread -lcpr -lcurl -g -O0 -o a.out;./a.out

#include "cpplang.hpp"
#include "Summary.hpp"
#include "izmp.hpp"
#include "Config.hpp"
#include "json.hpp"
#include <cpr/cpr.h>

USING_NAMESPACE(std);
USING_NAMESPACE(cpp_study);

// c++14 泛型 lambda
static auto debug_print = [](auto& b) {
    using json_t = nlohmann::json;
    json_t j;
    j["id"]      = b.id();
    j["sold"]    = b.sold();
    j["revenue"] = b.revenue();
    //j["average"] = b.average();

    std::cout << j.dump(2) << std::endl;
};

int main() try {
    cout << "hello cpp_study server" << endl;

    Config conf;
    conf.load("./config/conf.lua");

    Summary sum;
    std::atomic_int count {0};

    // todo: try-catch
    // [&]: 捕获上面的变量
    auto recv_cycle = [&](){
        using zmq_ctx = ZmqContext<1>;
        auto sock = zmq_ctx::recv_sock();
        sock.bind(conf.get<string>("config.zmq_ipc_addr"));
        assert(sock.handle() != nullptr);

        for(;;) {
            auto msg_ptr = std::make_shared<zmq_message_type>();

            if (sock.recv(*(msg_ptr) ) ) { cout << "recv message" << endl ; }
            //cout << msg_ptr->size() << endl;

            ++count;
            cout << count << endl;

            std::thread( [&sum, msg_ptr](){
                SalesData book;
                auto obj = msgpack::unpack(msg_ptr->data<char>(), msg_ptr->size()).get();
                obj.convert(book);
                debug_print(book);
                sum.add_sales(book);
            }).detach();
        }
    };  // recv_cycle lambda

    auto log_cycle = [&](){
        auto http_addr = conf.get<string>("config.http_addr");
        auto time_interval = conf.get<int>("config.time_interval");

        for(;;) {
            std::this_thread::sleep_for(time_interval * 1s);

            using json_t = nlohmann::json;
            json_t j;
            j["count"]  = static_cast<int>(count);
            j["minmax"] = sum.minmax_sales();//{info.first, info.second};

            auto res = cpr::Post(
                       cpr::Url{http_addr},
                       cpr::Body{j.dump()},
                       cpr::Timeout{200ms}
            );

            if (res.status_code != 200) {
                cerr << "http post failed" << endl;
            }
        }
    };

    // launch log_cycle
    auto fu1 = std::async(std::launch::async, log_cycle);
    // launch recv_cycle then wait
    auto fu2 = std::async(std::launch::async, recv_cycle);

    fu2.wait();
}
catch(std::exception& e){
    std::cerr << e.what() << std::endl;
}
