#include <iostream>
#include <gtest/gtest.h>
#include <thread>
using namespace std;

#include "demo/demo_api_declare.h"

#if 0
    #define ___methods_of_interface___IBuzzMath(_, ...)            \
        _(1, ResponseBar, negative_fields, RequestFoo, __VA_ARGS__)\
        _(2, ResponseBar, plus1_to_fields, RequestFoo, __VA_ARGS__)

    ___as_interface(IBuzzMath)
#endif

using namespace demo;

////////////////////////////////////////////////////////////////////////////////
struct IBuzzMath {
    virtual ResponseBar negative_fields(const RequestFoo&) = 0;
    virtual ResponseBar plus1_to_fields(const RequestFoo&) = 0;
};

////////////////////////////////////////////////////////////////////////////////

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::udp;

std::string make_daytime_string() {
    using namespace std; // For time_t, time and ctime;
    time_t now = time(0);
    return ctime(&now);
}

struct udp_server {
    udp_server() : io_service_(), socket_(io_service_, udp::endpoint(udp::v4(), 2222)) {
        start_receive();
        io_service_.run();
    }

    void start_receive() {
        socket_.async_receive_from(
                boost::asio::buffer(recv_buffer_), remote_endpoint_,
                boost::bind(&udp_server::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, std::size_t /*bytes_transferred*/) {
        cout << "handle receive:--------------->" << endl;
        if (!error || error == boost::asio::error::message_size)
        {
            boost::shared_ptr<std::string> message( new std::string(make_daytime_string()));
            socket_.async_send_to(boost::asio::buffer(*message), remote_endpoint_,
                                  boost::bind(&udp_server::handle_send, this, message, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
            //start_receive(); //do not loop here
        }
    }

    void handle_send(boost::shared_ptr<std::string> /*message*/, const boost::system::error_code& /*error*/, std::size_t bytes_transferred) {
        cout << "msg send---> " << bytes_transferred << endl;
    }

    boost::asio::io_service io_service_;
    udp::socket socket_;
    udp::endpoint remote_endpoint_;
    boost::array<char, 10240> recv_buffer_;
};

void local_rpc_stub() {
    cout << __FUNCTION__ << endl;
    try
    {
        boost::asio::io_service io_service;
        udp::endpoint receiver_endpoint = udp::endpoint(udp::v4(), 2222);
        udp::socket socket(io_service);
        socket.open(udp::v4());

        boost::array<char, 1> send_buf  = { 0 };
        socket.send_to(boost::asio::buffer(send_buf), receiver_endpoint);

        boost::array<char, 10240> recv_buf;

        udp::endpoint sender_endpoint;
        size_t len = socket.receive_from(boost::asio::buffer(recv_buf), sender_endpoint);

        std::cout.write(recv_buf.data(), len);
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

void remote_rpc_implement() {
    cout << __FUNCTION__ << endl;
    try
    {
        udp_server server;
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

TEST(async_rpc, should_able_to__auto__register_rpc_interface__after__application_startup) {
    demo::RequestFoo req;
    req.fooa = 1;
    req.__set_foob(2);

    std::thread local_thread(local_rpc_stub);
    std::thread remote_thread(remote_rpc_implement);

    local_thread.join();
    remote_thread.join();
};

