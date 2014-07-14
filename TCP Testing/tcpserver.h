#ifndef TCPSERVER_H
#define TCPSERVER_H

#include "tcpconnection.h"
#include "SDOCT.h"

#include "boost/asio.hpp"
#include "boost/lexical_cast.hpp"
#include "boost/thread.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/enable_shared_from_this.hpp"

using boost::asio::ip::tcp;

class TCPserver
{
public:
    TCPserver(boost::asio::io_service & io, SDOCT &oct, unsigned int port);


    void start_accept();

    //void handle_accept(const boost::system::error_code & e);

 void handle_accept(const boost::system::error_code &e, TCPconnection::SharedPtr new_conn);
private:
    tcp::acceptor acceptor;
   SDOCT &oct;

};

#endif // TCPSERVER_H
