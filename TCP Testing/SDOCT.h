#ifndef SDOCT_H
#define SDOCT_H

#include "SpectralRadar.h"
#include "string"
#include "iostream"
#include "vector"
#include "iterator"
#include <stdint.h>

using namespace std;


class SDOCT
{
public:

	SDOCT();
	~SDOCT();

	void Init();
	void Close();

	void InitDataHandler();
	void CleanDataHandler();

	//int getXSteps();
//	int getYSteps();
//	int getZSteps();

	int getXSteps();
	int getYSteps();
	int getZSteps();
	void setXSteps(int);
	void setYSteps(int);
	void setZSteps(int);

	double getXRange();
	double getYRange();
	double getZRange();	
	void setXRange(double);
	void setYRange(double);
	void setZRange(double);

	double getXOffset();
	double getYOffset();
	void setXOffset(double);
	void setYOffset(double);

	//void setVolScanProp(double xRange, int xSize, double yRange, int ySize);
//
//	std::vector<unsigned long> captureVolScan();
//
//	void setAScanProperties(double Contrast, double Brightness, double dBRange, double fMaxSigAmplitude);
	
	void captureVolScan(std::vector<uint8_t>&);

	unsigned long* getCameraPicture(int width, int height);

	void setAScanProperties(double Contrast, double Brightness, double dBRange, double fMaxSigAmplitude);

private:

	//SDK Handles
	OCTDeviceHandle dev;
	ProbeHandle probe;
	ScanPatternHandle pattern;
	ProcessingHandle proc;

	//SDK Data Handles
	RawDataHandle rawhandle;
	DataHandle datahandle;
	DataHandle voldata;
	ColoredDataHandle colorhandle;
	ColoredDataHandle volcolorhandle;
	Coloring32BitHandle color32handle;
	ColoredDataHandle camerahandle;

	//Daten Pointer
	float *data;
	unsigned long *colordata;
	unsigned long* cameradata;

	//Settings
	//double xrange, yrange;
//	int xsteps, ysteps;
	double xrange, yrange, zrange;
	uint32_t xsteps, ysteps, zsteps;

	int numAScans;
	int spokesteps;

	double xshift;
	double yshift;
	double angle;

	//Flags
	bool BScanPropertiesFlag;
	bool BScanAttitudeFlag;
	bool BScanSpokesFlag;

	//Methoden
	void UpdateBScanProperties();
	void UpdateBScanAttitude();

};


#endif // SDOCT_H
