#include "SDOCT.h"

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
	std::cout << "		Initializing probe\n";
	// Init device & probe
	this->dev = initDevice();

	this->probe = initStandardProbe(this->dev);
	
	//Setup internal data processing
	this->proc = createProcessingForDevice(this->dev);

	//Start up the probe with some valid default values
	setXRange(2.0);
	setYRange(2.0);
	setZRange(2.762);
	setXSteps(64);
	setYSteps(64);
	setZSteps(1024);

}

void SDOCT::Close()
{
	closeProbe(this->probe);
	closeDevice(this->dev);	
	std::cout << "		Closing probe\n";
}

//Setting up SDK data handlers
void SDOCT::InitDataHandler()
{	
	std::cout << "		Initializing data handlers\n";
	this->rawhandle = createRawData();
	this->datahandle = createData();
	this->voldata = createData();
	this->colorhandle = createColoredData();
	this->color32handle = createColoring32Bit(ColorScheme_RGBA32_BlackAndWhite);
	this->camerahandle = createColoredData();
}

//clean up SDK data handler
void SDOCT::CleanDataHandler()
{	
	std::cout << "		Cleaning data handlers\n";
	clearRawData(this->rawhandle);
	clearData(this->datahandle);
	clearData(this->voldata);
	clearColoredData(this->colorhandle);
	clearColoring32Bit(this->color32handle);
	clearColoredData(this->camerahandle);
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

void SDOCT::setXOffset(double xoffset)
{
	setProbeParameterFloat(this->probe, Probe_OffsetX, 0.1);
	std::cout << "xoffset set to " << xoffset << std::endl;
}

void SDOCT::setYOffset(double yoffset)
{
	setProbeParameterFloat(this->probe, Probe_OffsetY, 0.1);
	std::cout << "yoffset set to " << yoffset << std::endl;
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

void SDOCT::setZSteps(int zsteps)
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
	return this->zsteps;
}

double SDOCT::getXOffset()
{
	return getProbeParameterFloat(this->probe, Probe_OffsetX);
}

double SDOCT::getYOffset()
{
	return getProbeParameterFloat(this->probe, Probe_OffsetY);
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
	std::cout << "		Capturing volume scan\n";
	InitDataHandler();	

	this->pattern = createBScanStackPattern(this->probe, this->xrange, this->xsteps, this->yrange, this->ysteps);

	setColoringBoundaries(this->color32handle, 0.0f, 70.0f);

	rotateScanPattern(this->pattern, 0.0);
	shiftScanPattern(this->pattern, 0.0, 0.0);

	std::cout << "		Measurement starting\n";
	startMeasurement(this->dev, this->pattern, Acquisition_AsyncFinite);
	for (int i = 0; i < this->ysteps; i++)
	{
		//get data from oct
		getRawData(this->dev, this->rawhandle);
		//set output object
		setProcessedDataOutput(this->proc, this->datahandle);
		setColoredDataOutput(this->proc, this->colorhandle, this->color32handle);
		//apply fourier trafo
		executeProcessing(this->proc, this->rawhandle);
		//append data to volumedata
		appendData(this->voldata, this->datahandle, Direction_3);
	}
	stopMeasurement(this->dev);

	//Get pointer to volume data
	this->data = getDataPtr(this->voldata);

	//Copy data from pointer to std::vector
	int size = this->xsteps*this->ysteps*this->zsteps;
	//static std::vector<uint8_t> float_data;
	
	//Clear and reallocate the output vector to a spot in memory with enough free size
	result.clear();
	result.reserve(size);

	std::copy(this->data, this->data + size, std::back_inserter(result));
	std::cout << "Total number of elements: " << result.size() << std::endl;

	//clean up to prepare next scan MAYBE CLEARING DATA OBJECTs
	clearScanPattern(this->pattern);
	result.clear();

	CleanDataHandler();
}

unsigned long* SDOCT::getCameraPicture(int width, int height)
{
	getCameraImage(this->dev, width, height, this->camerahandle);
    exportColoredData(this->camerahandle, ColoredDataExport_JPG, "C:\\Users\\OCT\\Desktop\\OCTci\\Test.jpg");
	return cameradata;
}







