/*
To-do list:

>> Things to do here
-Setup wrapper class to simulate ROS
-Study ROS service request/response a little bit more
+What is the format of the request/response
+Any files that need to be created
Make sure I know how to add them to CMakeLists.txt
-40 bytes between B scans
-Image streaming?

>> Things to do at the lab
-Update boost
+Find out where all the boost files are (includes and libraries)
+Try and update boost with 'sudo apt-get install libboost-all-dev'
-Qtcreator and ROS
+Figure out if I have all the files I need and how to include them
+Figure out how to run the OCT_Wrapper node and how to call its functions and services
-Client - Server connection
+Transfer a block with all params before capture to prevent spam when changing sliders on dynamic reconfigure
-Dynamic reconfigure
+Find out how to actually open the interface
+Change the 'default' params to the new ones
+Find out how it updates
+Keep a config member file and update it with the newest values on callback. Serialize and transmit this config file before a scan
-Service, requests and responses
+What services am I going to need?
Initially make it receive a voxel data request, respond by fetching a scan
-Remember to update the CMakeLists file with the possible new additions
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

//oct.Init();

//unsigned long* cameraImage = oct.getCameraPicture(512, 512);


TCP_Server server(service, oct);

service.run();


}
catch (...)
{
std::cout << "main exception" << std::endl;
}
return 0;
}