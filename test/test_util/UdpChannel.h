#ifndef MSGRPC_UDPCHANNEL_H
#define MSGRPC_UDPCHANNEL_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
using boost::asio::ip::udp;

typedef std::function<void(const char* msg, size_t len)> OnMsgFunc;
typedef std::function<void(void)> OnChannelInitFunc;

struct UdpChannel {
    UdpChannel(unsigned short udp_port, OnChannelInitFunc on_channel_init, OnMsgFunc on_msg_func)
            : io_service_(), socket_(io_service_, udp::endpoint(udp::v4(), udp_port)), on_msg_func_(on_msg_func) {
        start_receive();
        on_channel_init();
        io_service_.run();
    }

    void start_receive() {
        socket_.async_receive_from(
                boost::asio::buffer(recv_buffer_), remote_endpoint_,
                boost::bind(&UdpChannel::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error || error == boost::asio::error::message_size) {
            on_msg_func_(recv_buffer_.data(), bytes_transferred);

            if (!close_channel_) {
                start_receive();
            }
        }
    }

    void send_msg_to_sender(const std::string& msg) {
        socket_.async_send_to(boost::asio::buffer(msg), remote_endpoint_,
                              boost::bind( &UdpChannel::handle_send, this, msg
                                      , boost::asio::placeholders::error
                                      , boost::asio::placeholders::bytes_transferred));
    }

    void send_msg_to_remote(const std::string& msg, const udp::endpoint& endpoint) {
        socket_.async_send_to(boost::asio::buffer(msg), endpoint,
                              boost::bind( &UdpChannel::handle_send, this, msg
                                      , boost::asio::placeholders::error
                                      , boost::asio::placeholders::bytes_transferred));
    }

    void handle_send(const std::string& /*message*/, const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (error) { cout << "msg send failed." << endl; }
    }

    void close() {
        close_channel_ = true;
    }

    boost::asio::io_service io_service_;
    udp::socket socket_;
    OnMsgFunc on_msg_func_;
    bool close_channel_ = { false };

    udp::endpoint remote_endpoint_;
    boost::array<char, 10240> recv_buffer_;
};


#endif //MSGRPC_UDPCHANNEL_H
