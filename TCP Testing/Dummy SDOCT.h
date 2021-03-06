#ifndef DUMMY_SDOCT_H
#define DUMMY_SDOCT_H

//#include "SpectralRadar.h"
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

	void captureVolScan(std::vector<uint8_t>&);

	void setAScanProperties(double Contrast, double Brightness, double dBRange, double fMaxSigAmplitude);


private:
	//Daten Pointer
	float *data;
	unsigned long *colordata;

	//Settings
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

#endif