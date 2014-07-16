#include <TCP_Connection.h>

TCP_Connection::TCP_Connection(boost::asio::io_service& io_service, SDOCT& oct) : m_socket(io_service), m_oct(oct)
{	
}

boost::asio::ip::tcp::socket& TCP_Connection::socket()
{
	return m_socket;
}

void TCP_Connection::start()
{
	//New connection started
	std::cout << "->start\n";

	//Waits until there is something to read
	while (m_socket.available() == 0)
	{
		;
	}

	//Read the first byte
	boost::asio::read(m_socket, m_readBuffer, boost::asio::transfer_exactly(1));

	//Extracts the contents of the streambuf to a simple char array
	const char* message = boost::asio::buffer_cast<const char*>(m_readBuffer.data());
	std::cout << "Received: \"" << message << "\"\n";

	//Parses the char array
	this->parse_data(message);
}

void TCP_Connection::parse_data(const char* message)
{
	std::cout << "->parse data. String: " << message << "." << std::endl;
	
	//Received a 'P' message: Change one of the oct properties variables 
	if (*message == 'P')
	{
		m_oct.Init();
		
		//Waits until there is something to read
		while (m_socket.available() == 0)
		{
			;
		}

		//Reads the 32 bytes of the 8 4-byte variables transferred as params
		boost::asio::read(m_socket, m_readBuffer, boost::asio::transfer_exactly(32));
		
		//Retrieves the data out of the read buffer in a simple c char array format easier to deal with
		const char* readBufferData = boost::asio::buffer_cast<const char*>(m_readBuffer.data());

		//Sets the new params into the oct
		this->set_oct_params(readBufferData);
				
		//Starts up the m_volScanMessage by building the 512 byte header 
		this->prepare_header(m_volScanMessage);

		//Appends the voxel data to the m_volScanMessage
		m_oct.captureVolScan(m_volScanMessage);

		//Finds the current filesize of m_volScanMessage (used to detect when transfer is complete) and begins message transfer
		m_fileSize = m_volScanMessage.size();
		this->send_volScan_message();	
	}
	else if (*message == 'B')
	{
		std::cout << "B mode requested\n";
	}
	else
	{
		//Incorrect request
		throw "Incorrect/invalid request! It should either be a \'P\' for a volume scan or a \'B\' for a B scan";
	}
}

void TCP_Connection::set_oct_params(const char* paramMessage)
{
	//Create some temporary variables to hold the params
	float xrange;
	float yrange;
	float zrange;
	uint32_t xsteps;
	uint32_t ysteps;
	uint32_t zsteps;
	float xoffset;
	float yoffset;

	//Memcpy the variables out of the param message into the temp variables. All offset one byte because of the 'P'
	memcpy(&xrange, &(paramMessage[1]), sizeof(float));
	memcpy(&yrange, &(paramMessage[5]), sizeof(float));
	memcpy(&zrange, &(paramMessage[9]), sizeof(float));
	memcpy(&xsteps, &(paramMessage[13]), sizeof(uint32_t));
	memcpy(&ysteps, &(paramMessage[17]), sizeof(uint32_t));
	memcpy(&zsteps, &(paramMessage[21]), sizeof(uint32_t));
	memcpy(&xoffset, &(paramMessage[25]), sizeof(float));
	memcpy(&yoffset, &(paramMessage[29]), sizeof(float));

	//Set the oct params to the values from the temp variables
	this->m_oct.setXRange(xrange);
	this->m_oct.setYRange(yrange);
	this->m_oct.setZRange(zrange);
	this->m_oct.setXSteps(xsteps);
	this->m_oct.setYSteps(ysteps);
	this->m_oct.setZSteps(zsteps);
	this->m_oct.setXOffset(xoffset);
	this->m_oct.setYOffset(yoffset);

	//Print out the change log for debug
	std::cout << "Params changed to:\n\t\tXRANGE: " << xrange
		<< "\n\t\tYRANGE: " << yrange
		<< "\n\t\tZRANGE: " << zrange
		<< "\n\t\tXSTEPS: " << xsteps
		<< "\n\t\tYSTEPS: " << ysteps
		<< "\n\t\tZSTEPS: " << zsteps
		<< "\n\t\tXOFFSET: " << xoffset
		<< "\n\t\tYOFFSET: " << yoffset
		<< "\n";

	//Clear the read buffer
	m_readBuffer.consume(m_readBuffer.size());
}

void TCP_Connection::prepare_header(std::vector<uint8_t>& header)
{
	header.clear();
	header.reserve(512);

	//Header size is 512 bytes in total. "Allocate" them here by pushing back NULLs into the vector
	for (int i = 0; i < 512; i++)
	{
		header.push_back(NULL);
	}

	//Fetch the current parameters from the oct scanner to build the header. Only these parameters are used by the client application, but the 512 byte size is kept in case other parameters start being used in the future
	uint32_t numOfImagesInFile = this->m_oct.getYSteps();
	uint32_t imageWidth = this->m_oct.getXSteps();
	uint32_t imageDepth = this->m_oct.getZSteps();
	float scanWidth = this->m_oct.getXRange();
	float scanLength = this->m_oct.getYRange();

	//Fetch the other parameters from the oct. These aren't built by the standard .img files, but are also packed for sake of completeness
	float scanDepth = this->m_oct.getZRange();
	float xOffset = this->m_oct.getXOffset();
	float yOffset = this->m_oct.getYOffset();

	//Copy the necessary header variables into the header vector
	memcpy(&header[16], &numOfImagesInFile, sizeof(uint32_t));
	memcpy(&header[20], &imageWidth, sizeof(uint32_t));
	memcpy(&header[24], &imageDepth, sizeof(uint32_t));
	memcpy(&header[72], &scanWidth, sizeof(float));
	memcpy(&header[76], &scanLength, sizeof(float));
	memcpy(&header[80], &scanDepth, sizeof(float));
	memcpy(&header[84], &xOffset, sizeof(float));
	memcpy(&header[88], &yOffset, sizeof(float));
}

void TCP_Connection::send_volScan_message()
{
	m_sendFillBuffer[2048] = 0;

	//std::this_thread::sleep_for(std::chrono::milliseconds(5000));

	SYSTEMTIME time;
	GetSystemTime(&time);
	WORD millisStart = (time.wSecond * 1000) + time.wMilliseconds;

	for (unsigned int i = 0; i < m_fileSize; i++)
	{
		//The ammount to transfer is usually the total capacity of the send buffer, unless the ammount of bytes still left to transfer is smaller than that capacity, in which case just ammount left is sent
		int ammountToSend = min(m_sendFillBuffer.size(), m_fileSize - i);

		//Copy (exclusively) the file data into the send buffer
		memcpy(&m_sendFillBuffer, &m_volScanMessage[i], ammountToSend);

		//Writes the remaining data to the socket
		size_t transferred = boost::asio::write(m_socket, boost::asio::buffer(m_sendFillBuffer, ammountToSend));
		
		std::cout << "Transferred " << transferred << " bytes\n";

		//Keeps track of how many bytes were written in total
		i += transferred;
	}

	GetSystemTime(&time);
	WORD millisEnd = (time.wSecond * 1000) + time.wMilliseconds;
		
	std::cout << "File transfer complete!\n";
	std::cout << m_fileSize << " " << m_volScanMessage.size() << "\n\n";

	float diff = (millisEnd - millisStart)/1000.0;
	std::cout << "Total time: " << diff << " s. Speed: " << (((float)m_fileSize) / diff) * (1 / 1024.0f) << " KBps " << std::endl;

	m_volScanMessage.clear();
}