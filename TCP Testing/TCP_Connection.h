#define BOOST_ASIO_ENABLE_HANDLER_TRACKING

#ifndef TCP_CONNECTION
#define TCP_CONNECTION

#include <iostream>
//#include <fstream>
#include <sstream>
#include <string>

#include <chrono>
#include <thread>

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/make_shared.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>

#include <Dummy SDOCT.h>

//This class moderates the transfer of information between the server and the client. It is created by a TCP_Server instance and gets deleted when the connection is droppedy
class TCP_Connection
{
private:
	boost::asio::ip::tcp::socket m_socket;
	SDOCT m_oct;

	boost::asio::streambuf m_readBuffer;
	std::string m_sendBuffer;
	boost::array<char, 4096> m_sendFillBuffer;
	std::vector<uint8_t> m_volScanMessage;

	uint32_t m_fileSize;

	int m_startTime;
	int m_endTime;

	//int m_rollingSum;

public:

	//Constructor receives an io_service instance and an SDOCT instance
	TCP_Connection(boost::asio::io_service& io_service, SDOCT& m_oct);
	
	//Returns it's own socket object. Inside the class we just access the member variable directly for shorter syntax
	boost::asio::ip::tcp::socket& socket();

	//Server begins listening for messages on the socket
	void start();
	
	//Interprets the read data and calls the intended functions
	void parse_data(const char*);

	//Parses the message containing the new oct parameters sent from the client and sets them in the oct
	void set_oct_params(const char*);

	//Clears and prepares a vector to hold 512 bytes of header according to the specifications of the .img files produced by the GUI software, with the intent on using the same pipelines. Only the necessary parameters are filled, the rest is populated with NULLs
	void prepare_header(std::vector<uint8_t>&);

	//Sends voxel data + header to the client. Gets recursively called writing several packets
	void TCP_Connection::send_volScan_message();		
};

#endif