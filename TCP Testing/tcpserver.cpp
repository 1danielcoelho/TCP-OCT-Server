#include "tcpserver.h"

//Constructor
TCPserver::TCPserver(boost::asio::io_service &io, SDOCT &oct, unsigned int port) : acceptor(io,tcp::endpoint(tcp::v4(),port)), oct(oct)
{
    start_accept();
}

//Accept request and start new tcp connection session
void TCPserver::start_accept()
{
    TCPconnection::SharedPtr new_conn = TCPconnection::create(this->acceptor.get_io_service(),this->oct);

    this->acceptor.async_accept(new_conn->get_socket(),boost::bind(&TCPserver::handle_accept,this,boost::asio::placeholders::error,new_conn));
}

// Important to use THIS (error, reference) handler callback structure!!!!
void TCPserver::handle_accept(const boost::system::error_code &e, TCPconnection::SharedPtr new_conn)
{
    std::cout << "Connection from " << new_conn->get_socket().remote_endpoint().address().to_string() << " accepted" <<  std::endl;
    std::cout << "Waiting for commands..." << std::endl;
    if(!e)
    {
        new_conn->start();
        start_accept();
    }
}
