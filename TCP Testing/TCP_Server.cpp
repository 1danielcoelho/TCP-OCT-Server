#include <TCP_Server.h>

TCP_Server::TCP_Server(boost::asio::io_service& service, SDOCT& oct) : m_acceptor(service, tcp::endpoint(tcp::v4(), 12345)), m_oct(oct)
{
	std::cout << "Constructor called\n";
	boost::asio::ip::tcp::no_delay opt_nodelay(true);
	m_acceptor.set_option(opt_nodelay);

	std::cout << "Constructor ended\n";
	do_accept();
}

void TCP_Server::do_accept()
{
	std::cout << "do_accept called\n";
	boost::shared_ptr<TCP_Connection> new_connection = TCP_Connection::create(m_acceptor.get_io_service(), this->m_oct);

	std::cout << "Waiting for connections" << std::endl;
	m_acceptor.accept(new_connection->socket());

	std::cout << "New client connected" << std::endl;
	new_connection->start();
	
	m_acceptor.close();
	std::cout << "do_accept ended\n";
	do_accept();
}
