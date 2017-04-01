#ifndef MSGRPC_UDPCHANNEL_H
#define MSGRPC_UDPCHANNEL_H

#include <ctime>
#include <iostream>
#include <string>
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>
#include <mutex>
using boost::asio::ip::udp;

typedef std::function<void(msgrpc::msg_id_t msg_id, const char* msg, size_t len)> OnMsgFunc;

struct UdpChannel;
thread_local UdpChannel* g_msg_channel;

struct UdpChannel {
    UdpChannel(unsigned short udp_port, OnMsgFunc on_msg_func)
            : io_service_(), socket_(io_service_, udp::endpoint(udp::v4(), udp_port)), on_msg_func_(on_msg_func) {

        start_receive();
        this->send_msg_to_remote("00init", udp::endpoint(udp::v4(), udp_port)); //00 means leading msgrpc::msg_id_t
        g_msg_channel = this;

        mutex_.lock();
        sockets_.push_back(&socket_);
        io_services_.push_back(&io_service_);
        mutex_.unlock();

        io_service_.run();
    }

    void start_receive() {
        socket_.async_receive_from(
                boost::asio::buffer(recv_buffer_), remote_endpoint_,
                boost::bind(&UdpChannel::handle_receive, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    void handle_receive(const boost::system::error_code& error, std::size_t bytes_transferred) {
        if (!error || error == boost::asio::error::message_size) {
            msgrpc::msg_id_t* msg_id = (msgrpc::msg_id_t*)recv_buffer_.data();
            on_msg_func_(*msg_id, (const char*)(msg_id + 1), bytes_transferred - sizeof(msgrpc::msg_id_t));

            start_receive();
        }
    }

    void send_msg_to_sender(const std::string& msg) {
        return send_msg_to_remote(msg, remote_endpoint_);
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

    static void close_all_channels() {
        for (auto* s: io_services_) {
            s->stop();
        }
    }

    static std::list<boost::asio::io_service*> io_services_;
    static std::list<udp::socket*> sockets_;
    static std::mutex mutex_;
    boost::asio::io_service io_service_;
    udp::socket socket_;
    OnMsgFunc on_msg_func_;

    udp::endpoint remote_endpoint_;
    boost::array<char, 10240> recv_buffer_;
};

std::list<boost::asio::io_service*> UdpChannel::io_services_;
std::list<udp::socket*> UdpChannel::sockets_;
std::mutex UdpChannel::mutex_;

#endif //MSGRPC_UDPCHANNEL_H
