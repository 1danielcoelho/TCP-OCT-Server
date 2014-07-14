#include "tcpconnection.h"

//Constructor
TCPconnection::TCPconnection(boost::asio::io_service & io_service, SDOCT &oct) : sock(io_service), oct(oct) {}

//Destructor
TCPconnection::~TCPconnection(){}

//Create individual TCPconnection
TCPconnection::SharedPtr TCPconnection::create(boost::asio::io_service &io_service, SDOCT& oct)
{
    return SharedPtr(new TCPconnection(io_service,oct));
}

//Return socket reference
tcp::socket & TCPconnection::get_socket()
{
    return this->sock;
}

void TCPconnection::close()
{
    this->sock.close();
}


/// Functions for sending/reading/evaluate simple MSG via TCP socket connection

//Start tcp session, listening
void TCPconnection::start()
{
    //Wait for first client msg
    boost::asio::async_read_until(this->get_socket(),this->msg_receive,'\n', boost::bind(&TCPconnection::handle_read_msg,shared_from_this(),boost::asio::placeholders::error));
}

//Read messages from tcp port
void TCPconnection::handle_read_msg(const boost::system::error_code &e)
{
    try {
    if(!e)
    {
        //Pass received MSG to Message Handler
        std::istream is(&this->msg_receive);
        std::string msgreceive;
        std::getline(is,msgreceive);

        evaluate(msgreceive);

        this->msg_send = "successfull received";

        write_msg();

        //receive next msg
        boost::asio::async_read_until(this->get_socket(),this->msg_receive,'\n', boost::bind(&TCPconnection::handle_read_msg,shared_from_this(),boost::asio::placeholders::error));
    }
    } catch(std::exception &e) {
                std::cerr << "Unexpected error while reading MSG" << e.what() << std::endl;
            }
    }

void TCPconnection::write_msg()
{
 //   boost::asio::async_write(this->get_socket(),boost::asio::buffer(this->msg_send),boost::bind(&TCPconnection::handle_write,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred()));

}

void TCPconnection::handle_write(const boost::system::error_code &e, size_t)
{
//NOTHING TO do
}

//Evaluate received MSG TODO: external handling
void TCPconnection::evaluate(std::string msgreceived)
{
	if(msgreceived.find("OCTINIT") != std::string::npos)
    {
		this->oct.Init();
	}

    if(msgreceived.find("SETCONFIG") != std::string::npos)
    {
        // Usage: SETCONFIG x_range(double) y_range(double) x_steps(short) y_steps(short)
        // Parse received message to extract parameters

        // TODO: Totally validate the received string to eliminate errors!
        std::istringstream buf(msgreceived);
        std::istream_iterator<std::string> beg(buf), end;
        std::vector<std::string> tokens(beg,end);

		
		this->oct.setVolScanProp(10,100,10,100);


        this->msg_send = "1";
        write_msg();


    } else if(msgreceived.find("GETVOLUME") != std::string::npos)
    {
        std::cout << "Get OCT C-Scan" << std::endl;

        //prepare data
        oct_volume oct_vol;

		std::vector<unsigned long> test = this->oct.captureVolScan();

		std::cout << "Elements:" << test.size() << std::endl;

        //Write current config to struct
       // this->oct.getConfig(oct_vol.x_range,oct_vol.y_range,oct_vol.x_steps,oct_vol.y_steps);
        //short *temp;
        //this->oct.getVolume(temp);

        //memcpy(temp,oct_vol.data,2*sizeof(short));

        //send serialized data to client
       // async_write_serialized(oct_vol,boost::bind(&TCPconnection::handle_write,shared_from_this(),boost::asio::placeholders::error,boost::asio::placeholders::bytes_transferred()));

		//clean up
		test.clear();

        //async_write_serialized()
    } else if (msgreceived.find("GETBSCAN") != std::string::npos)
    {
        //TODO!
    }

    else
        std::cout << "Unknown MSG! Please use SETCONFIG, GETVOLUME etc." << std::endl;

}

// Convert string to double
double TCPconnection::s2d(std::string str)
{
    std::istringstream buffer(str);
    double temp;
    buffer >> temp;
    return temp;
}

//Convert string to short
short TCPconnection::s2s(std::string str)
{
    std::istringstream buffer(str);
    int temp;
    buffer >> temp;
    return temp;
}
//
/////Functions for sending serialized data via TCP socket connection
///// Asynchronously write a data structure to the socket.
//template <typename T, typename Handler>
//void TCPconnection::async_write_serialized(const T& t, Handler handler)
//{
//  // Serialize the data first so we know how large it is.
//  std::ostringstream archive_stream;
//  boost::archive::text_oarchive archive(archive_stream);
//  archive << t;
//  outbound_data_ = archive_stream.str();
//
//  // Format the header.
//  std::ostringstream header_stream;
//  header_stream << std::setw(header_length)
//    << std::hex << outbound_data_.size();
//  if (!header_stream || header_stream.str().size() != header_length)
//  {
//    // Something went wrong, inform the caller.
//    boost::system::error_code error(boost::asio::error::invalid_argument);
//
//    sock.get_io_service().post(boost::bind(handler, error));
//    return;
//  }
//  outbound_header_ = header_stream.str();
//
//  // Write the serialized data to the socket. We use "gather-write" to send
//  // both the header and the data in a single write operation.
//  std::vector<boost::asio::const_buffer> buffers;
//  buffers.push_back(boost::asio::buffer(outbound_header_));
//  buffers.push_back(boost::asio::buffer(outbound_data_));
//  boost::asio::async_write(sock, buffers, handler);
//}
//
///// Asynchronously read a data structure from the socket.
//template <typename T, typename Handler>
//void TCPconnection::async_read_serialized(T& t, Handler handler)
//{
//  // Issue a read operation to read exactly the number of bytes in a header.
//  void (TCPconnection::*f)(
//      const boost::system::error_code&,
//      T&, boost::tuple<Handler>)
//    = &TCPconnection::handle_read_header_serialized<T, Handler>;
//  boost::asio::async_read(sock, boost::asio::buffer(inbound_header_),
//      boost::bind(f,
//        this, boost::asio::placeholders::error, boost::ref(t),
//        boost::make_tuple(handler)));
//}
//
///// Handle a completed read of a message header. The handler is passed using
///// a tuple since boost::bind seems to have trouble binding a function object
///// created using boost::bind as a parameter.
//template <typename T, typename Handler>
//void TCPconnection::handle_read_header_serialized(const boost::system::error_code& e,
//    T& t, boost::tuple<Handler> handler)
//{
//  if (e)
//  {
//    boost::get<0>(handler)(e);
//  }
//  else
//  {
//    // Determine the length of the serialized data.
//    std::istringstream is(std::string(inbound_header_, header_length));
//    std::size_t inbound_data_size = 0;
//    if (!(is >> std::hex >> inbound_data_size))
//    {
//      // Header doesn't seem to be valid. Inform the caller.
//      boost::system::error_code error(boost::asio::error::invalid_argument);
//      boost::get<0>(handler)(error);
//      return;
//    }
//
//    // Start an asynchronous call to receive the data.
//    inbound_data_.resize(inbound_data_size);
//    void (TCPconnection::*f)(
//        const boost::system::error_code&,
//        T&, boost::tuple<Handler>)
//      = &TCPconnection::handle_read_data_serialized<T, Handler>;
//    boost::asio::async_read(sock, boost::asio::buffer(inbound_data_),
//      boost::bind(f, this,
//        boost::asio::placeholders::error, boost::ref(t), handler));
//  }
//}
//
///// Handle a completed read of message data.
//template <typename T, typename Handler>
//void TCPconnection::handle_read_data_serialized(const boost::system::error_code& e,
//    T& t, boost::tuple<Handler> handler)
//{
//  if (e)
//  {
//    boost::get<0>(handler)(e);
//  }
//  else
//  {
//    // Extract the data structure from the data just received.
//    try
//    {
//      std::string archive_data(&inbound_data_[0], inbound_data_.size());
//      std::istringstream archive_stream(archive_data);
//      boost::archive::text_iarchive archive(archive_stream);
//      archive >> t;
//    }
//    catch (std::exception& e)
//    {
//      // Unable to decode data.
//      boost::system::error_code error(boost::asio::error::invalid_argument);
//      boost::get<0>(handler)(error);
//      return;
//    }
//
//    // Inform caller that data has been received ok.
//    boost::get<0>(handler)(e);
//  }
//}
