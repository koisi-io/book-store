// Copyright (c) 2020 by Chrono

#ifndef _IZMQ_HPP
#define _IZMQ_HPP

#include <zmq.hpp>
#include "cpplang.hpp"

BEGIN_NAMESPACE(cpp_study)

using zmq_context_type  = zmq::context_t;
using zmq_socket_type   = zmq::socket_t;
using zmq_message_type  = zmq::message_t;

template<int thread_num = 1>
class ZmqContext final {
#if 0
  public:
    using zmq_context_type  = zmq::context_t;
    using zmq_socket_type   = zmq::socket_t;
    using zmq_message_type  = zmq::message_t;
#endif
  public:
    ZmqContext() = default;
   ~ZmqContext() = default;
  public:
    static zmq_context_type& context() {
        static zmq_context_type ctx(thread_num);
        return ctx;
    }
  public:
    // 工厂方法: 创建收发用的 sock 对象
    static zmq_socket_type recv_sock(int hwm = 1000, int linger = 10){
      zmq_socket_type sock(context(), ZMQ_PULL);
      sock.set(zmq::sockopt::rcvhwm, hwm);
      sock.set(zmq::sockopt::linger, linger);    // wait for 10ms
      return sock;
    }

    static zmq_socket_type send_sock(int hwm = 1000, int linger = 10){
      zmq_socket_type sock(context(), ZMQ_PUSH);
      sock.set(zmq::sockopt::rcvhwm, hwm);
      sock.set(zmq::sockopt::linger, linger);    // wait for 10ms
      return sock;
    }
};

END_NAMESPACE(cpp_study)

#endif  //_ZMQ_HPP
