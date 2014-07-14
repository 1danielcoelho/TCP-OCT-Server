#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "iostream"

#include "SDOCT.h"
#include "oct_volume.h"

#include "boost/archive/text_iarchive.hpp"
#include "boost/archive/text_oarchive.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"
#include "boost/tuple/tuple.hpp"
#include "boost/enable_shared_from_this.hpp"


using boost::asio::ip::tcp;

class TCPconnection : public boost::enable_shared_from_this<TCPconnection>
{
public:
    typedef boost::shared_ptr<TCPconnection> SharedPtr;

    static SharedPtr create(boost::asio::io_service & io_service, SDOCT &oct);

    TCPconnection(boost::asio::io_service &io_service,SDOCT &oct);
    ~TCPconnection();

    tcp::socket& get_socket();

    void start();
    void close();

    /// Functions for sending/reading/evaluating simple MSG via tcp connection
    void handle_read_msg(const boost::system::error_code & e);
    void write_msg();


    void handle_write(const boost::system::error_code &e,size_t);
    void evaluate(std::string msgreceived);

    /// Functions for sending and receiving serialized data via tcp connection
 /*   template <typename T, typename Handler>
    void async_write_serialized(const T& t, Handler handler);
    template <typename T, typename Handler>
    void async_read_serialized(T& t, Handler handler);
    template <typename T, typename Handler>
    void handle_read_header_serialized(const boost::system::error_code& e,
        T& t, boost::tuple<Handler> handler);
    template <typename T, typename Handler>
    void handle_read_data_serialized(const boost::system::error_code& e,
        T& t, boost::tuple<Handler> handler);*/


    double s2d(std::string str);
    short s2s(std::string str);
private:
    tcp::socket sock;

    ///Member for msg handling
    std::string msg_send;
    boost::asio::streambuf msg_receive;
    SDOCT &oct;



    /// Member for data serialization
    enum { header_length = 8 };
    // Holds an outbound header.
    std::string outbound_header_;
    // Holds the outbound data.
    std::string outbound_data_;
    // Holds an inbound header.
    char inbound_header_[header_length];
   //Holds inbound data
    std::vector<char> inbound_data_;

};

#endif // TCPCONNECTION_H
