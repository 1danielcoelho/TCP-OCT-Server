//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#ifndef TCP_SERVER
#define TCP_SERVER

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>

#include <SDOCT.h>
#include <TCP_Connection.h>

//This class handles accepting and creating TCP_Connections between the server and potential clients
class TCP_Server
{
private:
	typedef boost::asio::ip::tcp tcp;
	tcp::acceptor m_acceptor;
	SDOCT &m_oct;

public:
	//Constructs the acceptor and sockets with the proper input from the class constructor. Should only deal with IPv4 at the specific port
	TCP_Server(boost::asio::io_service& service, SDOCT& oct);

private:
	//This function runs recursively. They wait and accept an incomming connection, create a shared pointer to that created socket, then create and start a TCP_Connection instance. When and if the client drops, do_accept() is called again and the servers just waits for the next connection
	void do_accept();
};

#endif