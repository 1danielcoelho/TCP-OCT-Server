/*
To-do list:

>> Things to do at the lab
-Testing
+Check to see if there is any code in the demo that renders 4d plots
-Qtcreator and ROS
+Figure out how to run the OCT_Wrapper node and how to call its functions and services

-Dynamic reconfigure
+Add groups to the cfg file
+Lidar com o Z range depender do Z steps
+Complete publishImageDataFromDynamicReconfigure
+Add octType in the interface exactly like the publish function (like a param pre-set)
-Service, requests and responses
+Are all three services needed? We could pass the octType or the other params through the message

>> Improvements
-Find a way of looking at the data produced by a volume scan
-Have the client try and reconnect after a connection has been dropped for some reason
-Find out the networks speed at the lab and consider transferring the camera output
+JPEG compression?

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