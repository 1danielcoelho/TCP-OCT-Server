/*
To-do list:

>> Things to do at the lab
	-Visualization
		+Fix the output from the SDOCT class
			Use the demo code for example
			Is it a fourier transform of the data?
	-TCP-IP
		+Finish implementing 100 byte edge tests for headers and voxel data
			Stopped responding for no reason
			Debug afc_test_client to see header pointer
	-Dynamic reconfigure
		+Add groups to the cfg file
		+Lidar com o Z range depender do Z steps
	
	
>> Improvements
	-Have the client try and reconnect after a connection has been dropped for some reason
	
*/

//#define BOOST_ASIO_ENABLE_HANDLER_TRACKING


#include "boost/asio.hpp"
#include <boost/asio.hpp>

#include <SDOCT.h>
#include <TCP_Server.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main()
{
	try
	{
		boost::asio::io_service service;
		SDOCT oct;

		TCP_Server server(service, oct);

		service.run();


	}
	catch (...)
	{
		std::cout << "main exception" << std::endl;
	}

	return 0;
}
//
////OCT_Wrapper: msg, srv, cfg, OCT_wrapper_qtros.hpp and .cpp
////OCT_Segmentation: msg, srv, OCT_segmentation_qtros.cpp
//
//

//int main()
//{
//    SDOCT oct;
//    oct.Init();
// 
//    std::vector<uint8_t> voxeldata;
//    voxeldata.reserve(64*64*1024);
//    oct.captureVolScan(voxeldata);
// 
//    std::ofstream m_fileStream;
// 
//    std::string path = "C:\\Users\\OCT\\Desktop\\voxeldata.txt";
// 
//    m_fileStream.open(path.c_str(), std::ios::binary | std::ios::out);
//    if (!m_fileStream)
//    {
//        std::cout << "Couldn't create the file!" << std::endl;
//    }
// 
//    std::string number;
// 
//    for (int i = 0; i < 64*64*1024; i++)
//    {
//        number = boost::lexical_cast<std::string>((int)((unsigned char)voxeldata[i])) + " ";
//        
////
//		if((i+1)%1024 == 0) number += "line\n ";
//		if((i+1)%(1024*64) == 0) number += "bscan\n\n\n ";
////
//		if (m_fileStream.is_open()) m_fileStream.write(number.c_str(), number.size());
//		
//    }
// 
//}
