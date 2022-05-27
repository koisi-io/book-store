// Copyright (c) 2020 by Chrono

#include "cpplang.hpp"
#include "SalesData.hpp"
#include "izmp.hpp"
#include "json.hpp"

USING_NAMESPACE(std);
USING_NAMESPACE(cpp_study);

static
auto debug_print = [](auto& b){
    using json_t = nlohmann::json;
    json_t j;
    j["id"]      = b.id();
    j["sold"]    = b.sold();
    j["revenue"] = b.revenue();
    //j["average"] = b.average();

    std::cout << j.dump(2) << std::endl;
};

// sales data
static
auto make_sales = [](const auto& id, auto sold, auto revenue){
    return SalesData(id, sold, revenue).pack();
};

// zmq send
static
auto send_sales = [](const auto& addr, const auto& buf){
    using zmq_ctx = ZmqContext<1>;
    auto sock = zmq_ctx::send_sock();
    sock.connect(addr);
    assert(sock);

    zmq::const_buffer zmq_buf((void*)buf.data(),buf.size());
    auto len = sock.send(zmq_buf);
    assert(len == buf.size());
    if(len == buf.size()){ cout << "yes" << endl;  }
};

int main()
try {
    cout << "hello cpp_study client" << endl;

    send_sales("tcp://127.0.0.1:5555",
             make_sales("001", 10, 100));

    std::this_thread::sleep_for(100ms);

    send_sales("tcp://127.0.0.1:5555",
               make_sales("002", 20, 200));
}
catch(std::exception& e){
    std::cerr << e.what() << std::endl;
}
