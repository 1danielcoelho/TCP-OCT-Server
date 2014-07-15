#include "Dummy SDOCT.h"

SDOCT::SDOCT() : xrange(NULL), xsteps(NULL), yrange(NULL), ysteps(NULL)
{
	//Init OCT device
	//Init();
}

SDOCT::~SDOCT()
{


}

void SDOCT::Init()
{
	// Init device & probe
	//this->dev = initDevice();
	//this->probe = initProbe(this->dev, "Microscope");
	//Setup internal data processing
	//this->proc = createProcessingForDevice(this->dev);
	std::cout << "Initializing probe\n";
	setXRange(1.0f);
	setYRange(1.0f);
	setZRange(1.0f);
	setXSteps(1);
	setYSteps(1000);
	setZSteps(1);
}

void SDOCT::Close()
{
	//closeProbe(this->probe);
	//closeDevice(this->dev);
	std::cout << "Closing probe\n";
}

//Setting up SDK data handlers
void SDOCT::InitDataHandler()
{
	/*this->rawhandle = createRawData();
	this->datahandle = createData();
	this->voldata = createData();
	this->colorhandle = createColoredData();
	this->color32handle = createColoring32Bit(ColorScheme_RGBA32_BlackAndWhite);*/
	std::cout << "Initializing data handlers\n";
}

//clean up SDK data handler
void SDOCT::CleanDataHandler()
{
	/*clearRawData(this->rawhandle);
	clearData(this->datahandle);
	clearData(this->voldata);
	clearColoredData(this->colorhandle);
	clearColoring32Bit(this->color32handle);*/
	std::cout << "Cleaning data handlers\n";
}

//Setters
void SDOCT::setXRange(double xrange)
{
	this->xrange = xrange;
	std::cout << "xrange set to " << xrange << std::endl;
}

void SDOCT::setYRange(double yrange)
{
	this->yrange = yrange;
	std::cout << "yrange set to " << yrange << std::endl;
}

void SDOCT::setZRange(double zrange)
{
	this->zrange = zrange;
	std::cout << "zrange set to " << zrange << std::endl;
}

void SDOCT::setXSteps(int xsteps)
{
	this->xsteps = xsteps;
	std::cout << "xsteps set to " << xsteps << std::endl;
}

void SDOCT::setYSteps(int ysteps)
{
	this->ysteps = ysteps;
	std::cout << "ysteps set to " << ysteps << std::endl;
}

void SDOCT::setZSteps(int ysteps)
{
	this->zsteps = zsteps;
	std::cout << "zsteps set to " << zsteps << std::endl;
}

//Getters
int SDOCT::getXSteps()
{
	return this->xsteps;
}

int SDOCT::getYSteps()
{
	return this->ysteps;
}

int SDOCT::getZSteps()
{
	//USE SDK command
	return 1;
}

double SDOCT::getXRange()
{
	return this->xrange;
}

double SDOCT::getYRange()
{
	return this->yrange;
}

double SDOCT::getZRange()
{
	return this->zrange;
}

void SDOCT::captureVolScan(std::vector<uint8_t>& result)
{
	InitDataHandler();

	//this->pattern = createBScanStackPattern(this->probe, this->xrange, this->xsteps, this->yrange, this->ysteps);

	//setColoringBoundaries(this->color32handle, 0.0f, 70.0f);

	//rotateScanPattern(this->pattern, 0.0);
	//shiftScanPattern(this->pattern, 0.0, 0.0);

	//startMeasurement(this->dev, this->pattern, Acquisition_AsyncFinite);
	//for (int i = 0; i < this->ysteps; i++)
	//{
	//	//get data from oct
	//	getRawData(this->dev, this->rawhandle);
	//	//set output object
	//	setProcessedDataOutput(this->proc, this->datahandle);
	//	setColoredDataOutput(this->proc, this->colorhandle, this->color32handle);
	//	//apply fourier trafo
	//	executeProcessing(this->proc, this->rawhandle);
	//	//append data to volumedata
	//	appendData(this->voldata, this->datahandle, Direction_3);
	//	///appendData(this->voldata,this->datahandle,Direction_3);
	//	cout << "Current volume size: " << getDataPropertyInt(this->voldata, Data_Size1) << " x " << getDataPropertyInt(this->voldata, Data_Size2) << " x " << getDataPropertyInt(this->voldata, Data_Size3) << endl;
	//}
	//stopMeasurement(this->dev);

	////Get pointer to volume data
	//this->data = getDataPtr(this->voldata);

	//Copy data from pointer to std::vector
	//int size = this->xsteps*this->ysteps;	
	int value = this->ysteps;

	for (int i = 0; i < value; i++)
	{	
		result.push_back((int)(rand()*255));			
	}
	
	return;
}
