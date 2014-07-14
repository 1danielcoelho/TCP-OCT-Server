#ifndef _SPECTRALRADAR_H
#define _SPECTRALRADAR_H

#include <stddef.h>

/*! \mainpage Spectral Radar SDK

\section intro Introduction

This document gives an introduction into using the ANSI C Spectral Radar SDK and shows how to use the most important functions.

\subsection overview Overview

The ANSI C Spectral Radar SDK follows an object oriented approach.
All objects are represented by pointers where appropriate typedefs are provided for convenience. The defined types are called Handles and are used as return-values when created and are passed as value when used. All functionality has been created with full LabVIEW compatibility in mind and it should be possible to use the SDK with most other programming languages as well.
The most important handles are given in the following sections.

\subsection sec_datahandles Data Handle (DataHandle, ColoredDataHandle, ComplexDataHandle, RawDataHandle)
Data acquired and used by the SDK is provided via data objects. A data object can contain
- floating point data (via #DataHandle)
- complex floating point data (via #ComplexDataHandle)
- ARGB32 colored data (via #ColoredDataHandle)
- unprocessed RAW data (via #RawDataHandle)
The data objects store all information belonging to them, such as pxiel data, spacing between pixels, comments attached to their data, etc.
Data objects are automatically resized if necessary and can contain 1-, 2- or 3-dimension data. The dimensionality can be read by #getDataPropertyInt(), etc.
Direct access to their memory is possible via #getDataPtr(), etc.
Data properties can be read out via #getDataPropertyInt(), #getDataPropertyFloat(), etc. These include sizes along their first, second and thirs axis, physical spacing between pixels, their total range, etc.

\subsection sec_OCTDeviceHandle OCTDeviceHandle
A handle specifying the OCT device that is used. In most cases the #OCTDeviceHandle ist obtained using the #initDevice() function and needs to be closed after using by #closeDevice(). The complete device will be initialized, the SLD will be switched on and all start-up dependent calibration will be performed.
All hardware and hardware dependend actions require the #OCTDeviceHandle to be passed. These include for example
- starting and stopping a measurement (#startMeasurement() and #stopMeasurement())
- getting properties of the device (#getDevicePropertyInt() and #getDevicePropertyFloat())

\subsection sec_processinghandle ProcessingHandle
The numerics and processing routines that are needed in order to create A-scans, B-scans and volumes out of directly measured spectra is accessed via the #ProcessingHandle. When the #ProcessingHandle is created all needed temporary memory and routines are initialized and prepared and several threads are started.
In most cases the ideal way to create a processing handle is to use #createProcessingForDevice() which creates optimized processing algorithms for the #OCTDeviceHandle specified. If no device is available or the processing routines are to be tweaked manually #createProcessing() is to be used. When all required processing is done #closeProcessing should be used to stop all threads and free all temporary memory.
All functions whose output is dependent on the processing routines used have a #ProcessingHandle parameter. These include for example
- set parameters that are used for processing (#setProcessingParameterInt() and #setProcessingFlag())
- execute the processing of data (#executeProcessing())

\subsection sec_probehandle ProbeHandle
The probe is the hardware that is used for scanning the sample, usually with help of galvanometric scanners. The object referenced by #ProbeHandle is responsible for creating scan patterns and holds all information and settings of the probe attached to the device. It needs to be calibrated to map suitable output voltage (for analog galvo drivers) or digital values (for digital galvo drivers) to scanning angles, inches or milimeters. In most cases this configuration is provided by *.ini files and the probe is initialized by #initProbe() where the probe needs be specified as string (either "Probe", "Handheld" or "Microscope" in most cases, though custom strings and probes are possible, if appropriate configuration ini-files exist).
If the probe is to be hardcoded into the software one can also provide an empty string as parameter and provide the configuration manually using #setProbeParameterInt() and #setProbeParameterFloat(). When done using the probe one should close it by calling #closeProbe() to free temporary memory.
All actions that depend on the probe configuration need a #ProbeHandle to be specified, such as:
- move galvo scanner to a specific position (#moveScanner()).
- create a scan pattern (#createBScanPattern()), see also \ref sec_scanpatternhandle.
- set calibration for a specific probe (#setProbeParameterFloat() and #setProbeParameterInt())

\subsection sec_scanpatternhandle ScanPatternHandle
A scan pattern is used to specifiy the points on the probe to scan during data acquisition, and its information is accessible via the #ScanPatternHandle. A dedicated function can be used to create a specific scan pattern, such as #createBScanPattern() for a simple B-scan or #createVolumePattern() for a simple volume scan. When the scan pattern is no longer needed its ressources can be freed using #clearScanPattern().
The #ScanPatternHandle needs to be specified to all functions that need information on the resulting scan. For example:
- creating a pattern (#createBScanPattern(), #createVolumePattern(), etc.)
- starting a measurement (#startMeasurement())

\subsection sec_otherhandles Other Handles
Other Handles that are used in the Spectral Radar SDK are
- #DopplerProcessingHandle: Handle to Doppler processing routines that can be used to transform complex data to Doppler phase and amplitude signals.
- #SettingsHandle: Handle to an INI file that can be read and written to without explicitly taking care of parsing the file.
- #Coloring32BitHandle: Handle to processing routines that can map floating point data to 32 bit color data.

\section first_steps First Steps
The following section describes first steps that are needed to acquire data with the Spectral Radar SDK.
\subsection init Initializing The Device

The easiest way to initialize the device is to use the #initDevice() function. It returns an approprate #OCTDeviceHandle that can be used to identifiy the device:
\code
OCTDeviceHandle Dev = initDevice();
// Acquire data, processing, direct hardware access...
closeDevice(Dev);
\endcode

\subsection processing Creating Processing Routines
In most cases raw data acquired by the OCT device needs to be transformed using FastFourier transform and other pre- and postprocessing algorithms.
To get a #ProcessingHandle on these algorithms the most convenient way is to use the #createProcessingForDevice() functionality which requires a valid #OCTDeviceHandle:
\code
// ...
ProcessingHandle Proc = createProcessingForDevice(Dev);
// acquire data and perform processing
closeProcessing(Proc);
// ...
\endcode

\subsection scanpattern Creating A Scan Pattern
In order to scan a sample and acquire B-scan OCT data one needs to specifiy a scan pattern that describes at which point to acquire data. To get the data of a simple B-Scan on can simply use #createBScanPattern():
\code
// ...
ProbeHandle Probe = initProbe(Dev, "Microscope"); // or initProbe(Dev, "Handheld");
ScanPatternHandle Pattern = createBScanPattern(Probe, 2.0, 512); // get B-scans with 2.0mm scanning angle and 512 A-scans per B-scan
// acquire data, ...
clearScanPattern(Pattern);
closeProbe(Probe);
// ...
\endcode

\subsection Asynchronous Acquisition
The most convenient and fast way to acquire data is to acquire data asynchronously. For this one starts a measurement using #startMeasurement() and retrieves the latest available #getRawData(). The memory needed to store the data needs to be allocated first:
\code
int i;


RawDataHandle Raw = createRawData();
DataHandle BScan = createData();
startMeasurement(Dev, Pattern, ASyncContinuous);

for(i=0; i<1000; ++i); // get 1000 B-scans
{
getRawData(Raw);
setProcessedDataOutput(Proc, BScan);
executeProcessing(Proc, Raw);
// data is now in BScan...
// do something with the data...
}
clearData(BScan);
stopMeasurement(Dev);
\endcode

\section ErrorHandling Error Handling
Error handling is done by calling the function #getError(). The function will return an #ErrorCode and if the result is not NO_ERROR an error string will be provided giving details about the problem.
\code
#define ERROR_STRLEN 1024;
//...
char error[ERROR_STRLEN];
OCTDeviceHandle Dev = initDevice();
if(!getError(error, ERROR_STRLEN)) // check whether the previous calls to SDK functions caused an error
{
printf("An error occured: %s", error);
}
// ...
\endcode
*/

/// \file SpectralRadar.h
/// \brief Header containing all functions of the Spectral Radar SDK. This SDK can be used for Callisto, Ganymede and Hyperion devices.
///
/// \def SPECTRALRADAR_API
/// \brief Export/Import of define of DLL members.

#ifdef SPECTRALRADAR_EXPORTS
#define SPECTRALRADAR_API __declspec(dllexport)
#else
#define SPECTRALRADAR_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
	/// \typedef BOOL
	/// \brief A standard boolean data type used in the API.
	typedef int BOOL;

	/// \def TRUE
	/// \brief TRUE for use with data type #BOOL.
#define TRUE 1

	/// \def FALSE
	/// \brief FALSE for use with data type #BOOL.
#define FALSE 0

	/// \struct ComplexFloat
	/// \ingroup Data
	/// \brief A standard complex data type that is used to access complex data.
	// needed for compatibility reasons if compiled with x byte aligned struct data.
	typedef struct C_ComplexFloat {
		/// data[0] is the real part and data[1] is the imaginary part. 
		float data[2];
	} ComplexFloat;

	/// \typedef RawDataHandle
	/// \ingroup Data
	/// \brief Handle to an object holding the unprocessed raw data. 
	///
	/// \typedef DataHandle
	/// \ingroup Data
	/// \brief Handle to an object holding 1-, 2- or 3-dimensional floating point data.
	///
	/// \typedef ColoredDataHandle
	/// \ingroup Data
	/// \brief Handle to an object holding 1-, 2- or 3-dimensional colored data.
	/// 
	/// \typedef ComplexDataHandle
	/// \ingroup Data
	/// \brief Handle to an object holding complex 1-, 2- or 3-dimensional complex floating point data.
	///
	/// \struct ComplexFloat
	/// \ingroup Data
	/// \brief a simple single precision floating point complex data type

	struct C_RawData;
	typedef struct C_RawData* RawDataHandle;

	struct C_Data;
	typedef struct C_Data* DataHandle;

	struct C_ColoredData;
	typedef struct C_ColoredData* ColoredDataHandle;

	struct C_ComplexData;
	typedef struct C_ComplexData* ComplexDataHandle;

	/// \typedef BufferHandle
	/// \ingroup Buffer
	/// \brief The BufferHandle identifies a data buffer.
	struct C_Buffer;
	typedef struct C_Buffer* BufferHandle;

	/// \typedef OCTDeviceHandle
	/// \ingroup Hardware
	/// \brief The OCTDeviceHandle type is used as Handle for using the SpectralRadar.
	struct C_OCTDevice;
	typedef struct C_OCTDevice* OCTDeviceHandle;

	/// \typedef ProbeHandle
	/// \ingroup Probe
	/// \brief Handle for controling the galvo scanner.
	struct C_Probe;
	typedef struct C_Probe* ProbeHandle;

	/// \typedef ScanPatternHandle
	/// \ingroup ScanPattern
	/// \brief Handle for controling the scan pattern.
	struct C_ScanPattern;
	typedef C_ScanPattern* ScanPatternHandle;

	/// \typedef ProcessingHandle
	/// \ingroup Processing
	/// \brief Handle for a processing routine.
	struct C_Processing;
	typedef struct C_Processing* ProcessingHandle;

	/// \typedef DopplerProcessingHandle
	/// \ingroup Doppler
	/// \brief Handle used for Doppler processing.
	struct C_DopplerProcessing;
	typedef struct C_DopplerProcessing* DopplerProcessingHandle;

	/// \typedef VolumeHandle
	/// \ingroup Volume
	/// \brief Handle for a buffer that contains the data of a volume measurement.
	struct C_Volume;
	typedef struct C_Volume* VolumeHandle;

	/// \typedef SurfaceHandle
	/// \ingroup Surface
	/// \brief Handle for an object that contains surface data.
	struct C_Surface;
	typedef struct C_Surface* SurfaceHandle;

	/// \typedef Coloring32BitHandle
	/// \ingroup Coloring
	/// \brief Handle for routines that color avaible scans for displaying.
	struct C_Coloring32Bit;
	typedef struct C_Coloring32Bit* Coloring32BitHandle;

	/// \typedef ImageFieldHandle
	/// \ingroup Data
	/// \brief Handle to the image field description
	struct C_ImageFieldCorrection;
	typedef struct C_ImageFieldCorrection* ImageFieldHandle;

	/// \fn unsigned long InterpretReferenceIntensity(float);
	/// \ingroup Helper
	/// \brief interprets the reference intensity and gives a color code that reflects its state. 
	/// 
	/// Possible colors include: 
	/// - red = 0x00FF0000 (bad intensity);
	/// - orange = 0x00FF7700 (okay intensity);
	/// - green = 0x0000FF00 (good intensity);
	/// \param intensity the current reference intensity as a value between 0.0 and 1.0
	/// \return the color code reflecting the state of the refernce intensity	
	SPECTRALRADAR_API unsigned long InterpretReferenceIntensity(float intensity);

	// ERROR HANDLING

	/// \defgroup ErrorHandling Error Handling
	/// \brief Error handling
	///
	/// \enum ErrorCode
	/// \ingroup ErrorHandling
	/// \brief This enum is used to describe errors that occur when operating an OCT device.
	///
	/// \warning Error codes and error description texts are subject to change in future releases.
	typedef enum C_ErrorCode {
		/// No error occured. This entry can be cast to FALSE.
		NoError = 0x00000000,
		/// Error occured. This entry can be cast to TRUE.
		Error = 0xE0000000
	} ErrorCode;

	/// \fn ErrorCode getError(char* Message, int StringSize);
	/// \ingroup ErrorHandling
	/// \brief Returns an error code and a message if an error occured. The error flag will be cleared.
	/// \param Message Error message describing the error.
	/// \param StringSize Size of the string that was given to Message. 

	SPECTRALRADAR_API ErrorCode getError(char* Message, int StringSize);

	// DATA ACCESS

	/// \defgroup Data Data Access
	/// \brief Functions for accessing the information stored in data objects. 
	///
	/// \enum RawDataPropertyInt
	/// \ingroup Data
	/// \brief Specifies properties of RawData
	typedef enum C_RawDataPropertyInt {
		/// Size of the first dimension. This will be the spectral dimension, i. e. z-dimension prior to Fourier transformation. 
		RawData_Size1,
		/// Size of the second dimension. This is a transversal axis (x). 
		RawData_Size2,
		/// Size of the third dimension. This is a transversal axis (y). 
		RawData_Size3,
		/// The number of elements in the raw data object.
		RawData_NumberOfElements,
		/// The size of the data object in bytes.
		RawData_SizeInBytes,
		/// The number of bytes of a single element, i. e. the data type of the raw data
		RawData_BytesPerElement
	} RawDataPropertyInt;

	/// \enum DataPropertyInt
	/// \ingroup Data
	/// \brief Selects integer point data property.
	typedef enum C_DataPropertyInt {
		/// Dimension of the data object. Usually 1, 2 or 3. 0 indicates empty data.
		Data_Dimensions,
		/// Size of the first dimension. For OCT data this is usually the longitudinal axis (z)
		Data_Size1,
		/// Size of the first dimension. For OCT data this is usually a transversal axis (x)
		Data_Size2,
		/// Size of the first dimension. For OCT data this is usually a transversal axis (y)
		Data_Size3,
		/// The number of elements in the data object.
		Data_NumberOfElements,
		/// The size of the data object in bytes.
		Data_SizeInBytes,
		/// The number of bytes of a single element.
		Data_BytesPerElement
	} DataPropertyInt;

	/// \enum DataPropertyFloat
	/// \ingroup Data
	/// \brief Selects floating point data property.
	typedef enum C_DataPropertyFloat {
		/// Spacing between two subsequent data elements in direction of the first axis in physical units.
		Data_Spacing1,
		/// Spacing between two subsequent data elements in direction of the second axis in physical units.
		Data_Spacing2,
		/// Spacing between two subsequent data elements in direction of the third axis in physical units.
		Data_Spacing3,
		/// Total range of the data in direction of the first axis in physical units.
		Data_Range1,
		/// Total range of the data in direction of the second axis in physical units.
		Data_Range2,
		/// Total range of the data in direction of the third axis in physical units.
		Data_Range3
	} DataPropertyFloat;

	/// \enum DataAnalyzation
	/// \ingroup Data
	/// \brief Selects data property to analyze.
	typedef enum C_DataAnalyzation {
		/// Minimum of the values in the data.
		Data_Min,
		/// Arithmetic mean of all values in the data.
		Data_Mean,
		/// Maximum of the values in the data.
		Data_Max
	} DataAnalyzation;

	/// \enum AScanAnalyzation
	/// \ingroup Data
	/// \brief Selects an appropriate A-San analyzation.
	typedef enum C_AScanAnalyzation {
		/// Noise of the A-scan in dB. This assumes that no signal is present in the A-scan.
		/// The noise is computed by averaging all fourier channels larger than 50.
		Data_Noise_dB,
		/// Noise of the A-scan in electrons. This assumes that no signal is present in the A-scan.
		/// The noise is computed by averaging all fourier channels larger than 50.
		Data_Noise_electrons,
		/// Peak position of the highest peak in pixels. 
		/// The peak position is determined by computing a parable going through the maximum value point and its surrounding pixels. The position of the maximum is used.
		Data_PeakPos_Pixel,
		/// Peak position of the highest peak in physical units. 
		/// The peak position is determined by computing a parable going through the maximum value point and its surrounding pixels. The position of the maximum is used. Physical coordinates are computed by using the calibrated zSpacing property of the device. The concrete physical units of the return value depends on the calibration.
		Data_PeakPos_PhysUnits,
		/// Peak height of the highest peak in dB.
		/// The peak hieght is determined by computing a parable going through the maximum value point and its surrounding pixels. The height of the resulting parable is returned. 
		Data_PeakHeight_dB,
		/// Signal width at -6dB. This is the FWHM.
		Data_PeakWidth_6dB,
		/// Signal width at -20dB.
		Data_PeakWidth_20dB,
		/// Signal width at -40dB.
		Data_PeakWidth_40dB
	} AScanAnalyzation;

	/// \fn void getRawDataSize(RawDataHandle, int*, int*, int* );
	/// \ingroup Data
	/// \brief Returns the size of the specified raw data (#RawDataHandle).
	///
	/// \fn int getDataPropertyInt(DataHandle Data, DataPropertyInt Selection)
	/// \ingroup Data
	/// \brief Returns the selected integer property of the specified data.
	///
	/// \fn double getDataPropertyFloat(DataHandle Data, DataPropertyFloat Selection)
	/// \ingroup Data
	/// \brief Returns the selected floating point property of the specified data.
	///
	/// \fn void copyData(DataHandle DataSource, DataHandle DataDestination);
	/// \ingroup Data
	/// \brief Copies the content of the specified source to the specified destination.
	///
	/// \fn void copyDataContent(DataHandle, float* )
	/// \ingroup Data
	/// \brief Copies the data in the specified data object (#DataHandle) into the specified pointer.
	///
	/// \fn float* getDataPtr(DataHandle Data)
	/// \ingroup Data
	/// \brief Returns a pointer to the content of the specified data.
	///
	/// \fn void resizeData(DataHandle Data, int Size1, int Size2, int Size3)
	/// \ingroup Data
	/// \brief Resizes the respective data object. In general the data will be 1-dimensional if Size2 and Size3 are equal to 1, 2-dimensional if Size3 is equal to 1 dn 3-dimensional if all, Size1, Size2, Size3, are unequal to 1.
	///
	/// \fn void setDataContent(DataHandle Data, float* NewContent)
	/// \ingroup Data
	/// \brief Sets the data content of the data object. The data chung pointed to by NewContent needs to be of the size expected by the data object, i. e. Size1*Size2*Size*sizeof(float).
	///
	/// \fn int getColoredDataPropertyInt(ColoredDataHandle ImageData, DataPropertyInt Selection);
	/// \ingroup Data
	/// \brief Returns the selected integer property of the specified colored data.
	///
	/// \fn int getColoredDataPropertyFloat(ColoredDataHandle ImageData, DataPropertyFloat Selection);
	/// \ingroup Data
	/// \brief Returns the selected integer property of the specified colored data.
	///
	/// \fn void copyColoredDataContent(ColoredDataHandle ImageSource, unsigned long* Destination);
	/// \ingroup Data
	/// \brief Copies the data in the specified colored data object (#ColoredDataHandle) into the specified pointer.
	///
	/// \fn unsigned long* getColoredDataPtr(ColoredDataHandle ColData)
	/// \ingroup Data
	/// \brief Returns a pointer to the content of the specified #ColoredDataHandle.
	///
	/// \fn void resizeColoredData(ColoredDataHandle ColData, int Size1, int Size2, int Size3)
	/// \ingroup Data
	/// \brief Resizes the respective colored data object. In general the data will be 1-dimensional if Size2 and Size3 are equal to 1, 2-dimensional if Size3 is equal to 1 dn 3-dimensional if all, Size1, Size2, Size3, are unequal to 1.
	///
	/// \fn  void setColoredDataContent(ColoredDataHandle ColData, unsigned long* NewContent)
	/// \ingroup Data
	/// \brief Sets the data content of the colored data object. The data chung pointed to by NewContent needs to be of the size expected by the data object, i. e. Size1*Size2*Size*sizeof(unsigned long).
	///
	/// \fn int getComplexDataPropertyInt(ComplexDataHandle Data, DataPropertyInt Selection)
	/// \ingroup Data
	/// \brief Returns the selected integer property of the specified data.
	///
	/// \fn double analyzeData(DataHandle Data, DataAnalyzation Selection);
	/// \ingroup Data
	/// \brief Performs the selected analyzation of the specified data and returns the resulting value.
	///
	/// \fn double analyzeAScan(DataHandle Data, AScanAnalyzation Selection);
	/// \ingroup Data
	/// \brief Performs the selected analyzation of the specified A-scan and returns the resulting value.
	///
	/// This functions assumes that the data contains an A-scan and performs A-scan specific analysis on it.
	///
	/// \fn void determineDynamicRange(DataHandle Data, float* MinRange_dB, float* MaxRange_dB);
	/// \ingroup Data
	/// 
	/// \brief Gives a rough estimation of the dynamic range of the specified data object.

	// RAW DATA
	SPECTRALRADAR_API void getRawDataSize(RawDataHandle Raw, int* SizeX, int* SizeY, int* SizeZ);

	// FLOATING POINT DATA
	SPECTRALRADAR_API int getDataPropertyInt(DataHandle Data, DataPropertyInt Selection);
	SPECTRALRADAR_API double getDataPropertyFloat(DataHandle Data, DataPropertyFloat Selection);
	SPECTRALRADAR_API void copyData(DataHandle DataSource, DataHandle DataDestination);
	SPECTRALRADAR_API void copyDataContent(DataHandle DataSource, float* Destination);
	SPECTRALRADAR_API float* getDataPtr(DataHandle Data);
	SPECTRALRADAR_API void setDataContent(DataHandle Data, float* NewContent);
	SPECTRALRADAR_API void reserveData(DataHandle Data, int Size1, int Size2, int Size3);
	SPECTRALRADAR_API void resizeData(DataHandle Data, int Size1, int Size2, int Size3);

	// COMPLEX FLOATING POINT DATA
	SPECTRALRADAR_API int getComplexDataPropertyInt(ComplexDataHandle Data, DataPropertyInt Selection);
	SPECTRALRADAR_API void copyComplexDataContent(ComplexDataHandle DataSource, ComplexFloat* Destination);
	SPECTRALRADAR_API ComplexFloat* getComplexDataPtr(ComplexDataHandle Data);
	SPECTRALRADAR_API void setComplexDataContent(ComplexDataHandle Data, ComplexFloat* NewContent);
	SPECTRALRADAR_API void resizeData(DataHandle Data, int Size1, int Size2, int Size3);

	// COLORED (UNSIGNED 32BIT) DATA
	SPECTRALRADAR_API int getColoredDataPropertyInt(ColoredDataHandle ColData, DataPropertyInt Selection);
	SPECTRALRADAR_API double getColoredDataPropertyFloat(ColoredDataHandle ColData, DataPropertyFloat Selection);
	SPECTRALRADAR_API void copyColoredData(ColoredDataHandle ImageSource, ColoredDataHandle ImageDestionation);
	SPECTRALRADAR_API void copyColoredDataContent(ColoredDataHandle Source, unsigned long* Destination);
	SPECTRALRADAR_API unsigned long* getColoredDataPtr(ColoredDataHandle ColData);
	SPECTRALRADAR_API void resizeColoredData(ColoredDataHandle ColData, int Size1, int Size2, int Size3);
	SPECTRALRADAR_API void setColoredDataContent(ColoredDataHandle ColData, unsigned long* NewContent);

	// FLOATING POINT DATA ANALYSIS
	SPECTRALRADAR_API double analyzeData(DataHandle Data, DataAnalyzation Selection);
	SPECTRALRADAR_API double analyzeAScan(DataHandle Data, AScanAnalyzation Selection);
	SPECTRALRADAR_API void determineDynamicRange(DataHandle Data, float* MinRange_dB, float* MaxRange_dB);

	/// \fn void copyRawDataContent(RawDataHandle RawDataSource, void* DataContent);
	/// \ingroup Data
	/// \brief Copies the content of the raw data into the specified buffer. The suer needs to assure that enough memory is allocated. 
	///
	/// \fn void* getRawDataPtr(RawDataHandle RawDataSource);
	/// \ingroup Data
	/// \brief Returns the pointer to the raw data content. The pointer might no longer after additional actions using the RawDataHandle. 
	///
	/// \fn int getRawDataPropertyInt(RawDataHandle RawData, RawDataPropertyInt Property);
	/// \ingroup Data
	/// \brief Returns a raw data property
	///
	/// \fn void setRawDataBytesPerPixel(RawDataHandle Raw, int BytesPerPixel);
	/// \ingroup Data
	/// \brief Sets the bytes per pixel for raw data.
	///
	/// \fn void resizeRawData(RawDataHandle Raw, int Size1, int Size2, int Size3);
	/// \ingroup Data
	/// \brief Resizes the specified raw data buffer accordingly.
	///
	/// \fn void setRawDataContent(RawDataHandle RawDataSource, void* NewContent);
	/// \ingroup Data
	/// \brief Sets the content of the raw data buffer. The size of the RawDataHandle needs to be adjusted first, as otherwise not all data might be copied. 
	///
	/// \fn void setScanSpectra(RawDataHandle RawData, int NumberOfScanRegions, int* ScanRegions);
	/// \ingroup Data
	/// \brief Sets the number of the spectra in the raw data that are used for creating A-scan/B-scan data. 
	///
	/// \fn void setApodizationSpectra(RawDataHandle RawData, int NumberOfScanRegions, int* ApodizationRegions)
	/// \ingroup Data
	/// \brief Sets the number of the spectra in the raw data that contain data useful as apodization spectra. 
	///
	/// \fn  int getNumberOfScanRegions(RawDataHandle Raw)
	/// \ingroup Data
	/// \brief Returns the number of regions that have been acquired that contain scan data, i. e. spectra that are used to compute A-scans.
	///
	/// \fn  int getNumberOfApodizationRegions(RawDataHandle Raw)
	/// \ingroup Data
	/// \brief Returns the number of regions in the raw data containing spectra that are supposed to be used for apodization.
	///
	/// \fn  void getScanSpectra(RawDataHandle Raw, int* SpectraIndex)
	/// \ingroup Data
	/// \brief Returns the indices of spectra that contain scan data, i. e. spectra that are supposed to be used to compute A-scans. 
	/// 
	/// An array needs to be provided that has twice the sice of the number of scan regions which can be obtained by getNumberOfScanRegions()
	///
	/// \fn  void getApodizationSpectra(RawDataHandle Raw, int* SpectraIndex)
	/// \ingroup Data
	/// \brief Returns the indices of spectra that contain apodization data, i. e. spectra that are supposed to be used as input for apodization. 
	/// 
	/// An array needs to be provided that has twice the sice of the number of apodization regions which can be obtained by getNumberOfApodizationRegions()

	SPECTRALRADAR_API void copyRawDataContent(RawDataHandle RawDataSource, void* DataContent);
	SPECTRALRADAR_API void* getRawDataPtr(RawDataHandle RawDataSource);
	SPECTRALRADAR_API int getRawDataPropertyInt(RawDataHandle RawData, RawDataPropertyInt Property);
	SPECTRALRADAR_API void setRawDataBytesPerPixel(RawDataHandle Raw, int BytesPerPixel);
	SPECTRALRADAR_API void resizeRawData(RawDataHandle Raw, int Size1, int Size2, int Size3);
	SPECTRALRADAR_API void setRawDataContent(RawDataHandle RawDataSource, void* NewContent);
	SPECTRALRADAR_API void setScanSpectra(RawDataHandle RawData, int NumberOfScanRegions, int* ScanRegions);
	SPECTRALRADAR_API void setApodizationSpectra(RawDataHandle RawData, int NumberOfScanRegions, int* ApodizationRegions);

	SPECTRALRADAR_API int getNumberOfScanRegions(RawDataHandle Raw);
	SPECTRALRADAR_API int getNumberOfApodizationRegions(RawDataHandle Raw);
	SPECTRALRADAR_API void getScanSpectra(RawDataHandle Raw, int* SpectraIndex);
	SPECTRALRADAR_API void getApodizationSpectra(RawDataHandle Raw, int* SpectraIndex);

	// DATA CREATION/CLEARING

	/// \defgroup DataCreation Data Creation and Clearing
	/// \brief Functions to create and clear object containing data.
	///
	/// \fn RawDataHandle createRawData();
	/// \ingroup DataCreation
	/// \brief Creates a raw data object (#RawDataHandle).
	///
	/// \fn void clearRawData(RawDataHandle);
	/// \ingroup DataCreation
	/// \brief Clears a raw data object (#RawDataHandle)
	///
	/// \fn DataHandle createData(void);
	/// \ingroup DataCreation
	/// \brief Creates a 1-dimensional data object, containing floating point data.
	///
	/// \fn void clearData(DataHandle);
	/// \ingroup DataCreation
	/// \brief Clears the specified #DataHandle, #DataHandle, #DataHandle or #DataHandle objects.
	///
	/// \fn  ColoredDataHandle createColoredData(void);
	/// \ingroup DataCreation
	/// \brief Creates a colored data object (#ColoredDataHandle).
	///
	/// \fn void clearColoredData(ColoredDataHandle)
	/// \ingroup DataCreation
	/// \brief Clears a colored volume object. 
	///
	/// \fn ComplexDataHandle createComplexData(void);
	/// \ingroup DataCreation
	/// \brief Creates a data object holding complex data.
	///
	/// \fn void clearComplexData(ComplexDataHandle Data);
	/// \ingroup DataCreation
	/// \brief Clears a data object holding complex data.

	SPECTRALRADAR_API RawDataHandle createRawData(void);
	SPECTRALRADAR_API void clearRawData(RawDataHandle Raw);

	SPECTRALRADAR_API DataHandle createData(void);
	SPECTRALRADAR_API void clearData(DataHandle Data);

	SPECTRALRADAR_API ColoredDataHandle createColoredData(void);
	SPECTRALRADAR_API void clearColoredData(ColoredDataHandle Volume);

	SPECTRALRADAR_API ComplexDataHandle createComplexData(void);
	SPECTRALRADAR_API void clearComplexData(ComplexDataHandle Data);

	// GROUP HARDWARE

	/// \defgroup Hardware Hardware 
	/// \brief Functions providing direct access to OCT Hardware functionality.
	///
	/// \enum DevicePropertyFloat
	/// \ingroup Hardware
	/// \brief Properties of the device that can be read or measured.
	typedef enum C_DevicePropertyFloat {
		/// The full well capacity of the device.
		Device_FullWellCapacity,
		/// The spacing between two pixels in an A-scan.
		Device_zSpacing,
		/// The maximum measurement range for an A-scan.
		Device_zRange,
		/// The minimum expected dB value for final data.
		Device_SignalAmplitudeMin_dB,
		/// The typical low dB value for final data.
		Device_SignalAmplitudeLow_dB,
		/// The typical high dB value for final data.
		Device_SignalAmplitudeHigh_dB,
		/// The maximum expected dB value for final data.
		Device_SignalAmplitudeMax_dB,
		/// Scaling factor between binary raw data and electrons/photons
		Device_BinToElectronScaling,
		/// Internal device temperature in degrees C (only on devices incorporating a temperature sensor)
		Device_Temperature,
		/// Absolute power-on time of the SLD since first start in seconds (only on devices incorporating an RTC)
		Device_SLD_OnTime_sec
	} DevicePropertyFloat;

	/// \enum DevicePropertyInt
	/// \ingroup Hardware
	/// \brief Properties of the device that can be read or measured.
	typedef enum C_DevicePropertyInt {
		/// The number of pixels provided by the spectrometer.
		Device_SpectrumElements,
		Device_BytesPerElement
	} DevicePropertyInt;

	/// \enum ScanAxis
	/// \ingroup Hardware
	/// \brief used to select the axis for manual galvo operations.
	typedef enum C_ScanAxis {
		/// X-Axis of the scanner
		ScanAxis_X = 0,
		/// Y-Axis of the scanner
		ScanAxis_Y = 1
	} ScanAxis;

	/// \fn OCTDeviceHandle initDevice(void);
	/// \ingroup Hardware
	/// \brief Initializes the installed device.
	/// \return Handle to the initialized OCT device.
	///
	/// \fn int getDevicePropertyInt(OCTDeviceHandle Dev, DevicePropertyInt Selection);
	/// \ingroup Hardware
	/// \brief Returns properties of the device belonging to the specfied #OCTDeviceHandle.
	///
	/// \fn double getDevicePropertyFloat(OCTDeviceHandle, DevicePropertyFloat)
	/// \ingroup Hardware
	/// \brief Returns properties of the device belonging to the specfied #OCTDeviceHandle.
	///
	/// \fn void closeDevice(OCTDeviceHandle);
	/// \ingroup Hardware
	/// \brief Closes the device opened previously with initDevice.
	/// \param Dev The #OCTDeviceHandle that was initially provided by initDevice.
	///
	/// \fn void setSLD(OCTDeviceHandle, BOOL);
	/// \ingroup Hardware
	/// \brief switches the SLD of the SpectralRadar device on and off.
	/// \param Dev handle to the #OCTDeviceHandle that was initially provided by initDevice.
	/// \param OnOff TRUE switches the SLD on, FALSE swichted the SLD off.
	///
	/// \fn void moveScanner(OCTDeviceHandle, ProbeHandle, ScanAxis, double);
	/// \ingroup Hardware
	/// \brief manually moves the scanner to a given position
	/// \param Dev the #OCTDeviceHandle that was initially provided by initDevice.
	/// \param Probe A handle to the probe (#ProbeHandle); whose galvo position is to be set.
	/// \param Axis the axis in which you want to set the position manually
	/// \param Position the actual position you want to move the galvo to.
	///
	/// \fn void setLaserDiode(OCTDeviceHandle, BOOL);
	/// \ingroup Hardware
	/// \brief switches the LaserDiode of the SpectralRadar device on and off.
	///
	/// \warning Not all devices are equiped 
	/// \param Dev handle to the #OCTDeviceHandle that was initially provided by initDevice.
	/// \param OnOff TRUE switches the VisLD on, FALSE swichted the VisLD off.

	SPECTRALRADAR_API OCTDeviceHandle initDevice(void);
	SPECTRALRADAR_API void getDeviceType(OCTDeviceHandle Dev, char DevName[], int BufferSize);
	SPECTRALRADAR_API int getDeviceRevision(OCTDeviceHandle Dev);
	SPECTRALRADAR_API int getDevicePropertyInt(OCTDeviceHandle Dev, DevicePropertyInt Selection);
	SPECTRALRADAR_API double getDevicePropertyFloat(OCTDeviceHandle Dev, DevicePropertyFloat Selection);
	SPECTRALRADAR_API void closeDevice(OCTDeviceHandle Dev);

	SPECTRALRADAR_API void setSLD(OCTDeviceHandle Dev, BOOL OnOff);
	SPECTRALRADAR_API void moveScanner(OCTDeviceHandle Dev, ProbeHandle Probe, ScanAxis Axis, double Position);
	SPECTRALRADAR_API void setLaserDiode(OCTDeviceHandle Dev, BOOL OnOff);

	// GROUP Internal values

	/// \defgroup Internal Internal Values
	/// \brief Functions for access to all kinds of Digital-to-Analog and Analog-to-Digital on the device.
	///
	/// \fn int getNumberOfInternalValues(OCTDeviceHandle);
	/// \ingroup Internal
	/// \brief Returns the number of Analog-to-Digital Converter present in the device.
	///
	/// \fn void getInternalValueName(OCTDeviceHandle, int, char*, int NameStringSize, char*, int UnitStringSize);
	/// \ingroup Internal
	/// \brief Returns names and unit for the specified Analog-to-Digital Converter. 
	/// 
	/// The index is running number, starting with 0, smaller than the number specified by #getNumberOfInternalValues.
	///
	/// \fn double getInternalValueByName(OCTDeviceHandle, const char*);
	/// \ingroup Internal 
	/// \brief Returns the value of the specified Analog-to-Digital Converter (ADC);. 
	///
	/// The ADC is specified by the name returned by #getInternalValueName.
	///
	/// \fn double getInternalValueByIndex(OCTDeviceHandle, int);
	/// \ingroup Internal
	/// \brief Returns the value of the selected ADC. 
	/// 
	/// The index is running number, starting with 0, smaller than the number specified by #getNumberOfInternalValues.

	SPECTRALRADAR_API int getNumberOfInternalValues(OCTDeviceHandle Dev);
	SPECTRALRADAR_API void getInternalValueName(OCTDeviceHandle Dev, int Index, char* Name, int NameStringSize, char* Unit, int UnitStringSize);
	SPECTRALRADAR_API double getInternalValueByName(OCTDeviceHandle Dev, const char* Name);
	SPECTRALRADAR_API double getInternalValueByIndex(OCTDeviceHandle Dev, int Index);

	// PROBE

	/// \defgroup Probe Pattern Factory/Probe
	/// \brief Functions setting up a probe that can be used to create scan patterns.
	///
	/// \enum ProbeParameterFloat
	/// \ingroup Probe
	/// \brief Parameters describing the behaviour of the Probe, such as calibration factors and scan parameters.
	///
	/// Computation of physical position and raw values for the scanner is done by
	/// PhyscialPosition = Factor * RawValue + Offset
	typedef enum C_ProbeParameterFloat {
		/// Factor for the x axis.
		Probe_FactorX,
		/// Offset for the x axis.
		Probe_OffsetX,
		/// Factor for the y axis.
		Probe_FactorY,
		/// Offset for the y axis.
		Probe_OffsetY,
		/// Flyback time of the system. This time is usually needed to get from an apodization position to scan position and vice versa.
		Probe_FlybackTime_Sec,
		/// The scanning range is extended by a number of A-scans equivalent to the expansion time.
		Probe_ExpansionTime_Sec,
		/// The scan pattern is usually shifted by a number of A-scans equivalent to the rotation time.
		Probe_RotationTime_Sec,
		/// The expected scan rate. \warning In general the expected scan rate is set during initialization of the probe with respect to the attached device. In most cases it should not be altered manually.
		Probe_ExpectedScanRate_Hz,
		// The ratio OffsetX(px)/Image Width(px) for the BScan overlay on the camera image. This must be a value between 0.0 and 1.0.
		Probe_CameraScalingX,
		Probe_CameraOffsetX,
		// The ratio OffsetY(px)/Image Height(px) for the BScan overlay on the camera image. This must be a value between 0.0 and 1.0.
		Probe_CameraScalingY,
		Probe_CameraOffsetY,
		// Corrective rotation angle for the BScan overlay.
		Probe_CameraAngle
		// Scaling factor in px/mm for the BScan overlay.
	} ProbeParameterFloat;

	/// \enum ProbeParameterInt
	/// \ingroup Probe
	/// \brief Parameters describing the behaviour of the Probe, such as calibration factors and scan parameters.
	typedef enum C_ProbeParameterInt {
		/// The number of cycles used for apodization.
		Probe_ApodizationCycles,
		/// A factor used as oversampling.
		Probe_Oversampling
	} ProbeParameterInt;

	/// \fn ProbeHandle initProbe(OCTDeviceHandle, const char*);
	/// \ingroup Probe
	/// \brief Initializes a probe specified by ProbeFile.
	///
	/// In older systems up until a manufacturing date of May 2011 either "Handheld" or "Microscope" are used. An according ini-file (i. e. "Handheld.ini" or "Microscope.ini); will be loaded from the config path of the SpectralRadar installation containing all necessary information.
	/// With systems manufactured after May 2011 "Probe" should be used.
	///
	///	It is recommended to use #initStandardProbe for systems manufactured in or after May 2011.
	///
	/// \fn ProbeHandle initStandardProbe(OCTDeviceHandle Dev);
	/// \ingroup Probe
	/// \brief Creates a standard probe using the Probe.ini file. If this configuration file is not found, standard parameters without valid calibration will be used.
	///
	/// \fn void setProbeParameterInt(ProbeHandle, ProbeParameterInt, int)
	/// \ingroup Probe
	/// \brief Sets 
	///
	/// \fn void setProbeParameterFloat(ProbeHandle, ProbeParameterFloat, double)
	/// \ingroup Probe
	/// \brief Sets floating point parameters of the specified probe.
	///
	/// \fn int getProbeParameterInt(ProbeHandle, ProbeParameterInt)
	/// \ingroup Probe
	/// \brief Gets integer parameters of the specified probe.
	///
	/// \fn double getProbeParameterFloat(ProbeHandle, ProbeParameterFloat)
	/// \ingroup Probe
	/// \brief Gets floating point parameters of the specified probe.
	///
	/// \fn void closeProbe(ProbeHandle Probe);
	/// \ingroup Probe
	/// \brief Closes the probe and frees all memory associated with it.

	SPECTRALRADAR_API ProbeHandle initProbe(OCTDeviceHandle Dev, const char* ProbeFile);
	SPECTRALRADAR_API ProbeHandle initStandardProbe(OCTDeviceHandle Dev);
	SPECTRALRADAR_API void saveProbe(ProbeHandle Probe, const char* ProbeFile);
	SPECTRALRADAR_API void setProbeParameterInt(ProbeHandle Probe, ProbeParameterInt Selection, int Value);
	SPECTRALRADAR_API void setProbeParameterFloat(ProbeHandle Probe, ProbeParameterFloat Selection, double Value);
	SPECTRALRADAR_API int getProbeParameterInt(ProbeHandle Probe, ProbeParameterInt Selection);
	SPECTRALRADAR_API double getProbeParameterFloat(ProbeHandle Probe, ProbeParameterFloat Selection);
	SPECTRALRADAR_API void closeProbe(ProbeHandle Probe);

	SPECTRALRADAR_API void blendEnFaceInCamera(ProbeHandle Probe, ScanPatternHandle Pattern, ColoredDataHandle EnFace2D, ColoredDataHandle Image, float Ratio, BOOL DenseView);
	SPECTRALRADAR_API void drawScanPatternOverlay(ProbeHandle Probe, ScanPatternHandle Pattern, ColoredDataHandle Image, float Opacity);

	SPECTRALRADAR_API void CameraPixelToPosition(ProbeHandle Probe, ColoredDataHandle Image, int PixelX, int PixelY, double* PosX, double* PosY);
	SPECTRALRADAR_API void PositionToCameraPixel(ProbeHandle Probe, ColoredDataHandle Image, double PosX, double PosY, int* PixelX, int* PixelY);

	// SCAN PATTERN

	/// \defgroup ScanPattern Scan Pattern
	/// \brief Functions that describe the movement of the Scanner during measurement.
	///
	/// \fn ScanPatternHandle createNoScanPattern(ProbeHandle Probe, int Scans, int NumberOfScans);
	/// \ingroup ScanPattern
	/// \brief Creates a simple scan pattern that does not move the galvo. Use this pattern for point scans and/or non-scanning probes.
	///
	/// \fn ScanPatternHandle createBScanPattern(ProbeHandle Probe, double Range, int AScans);
	/// \ingroup ScanPattern
	/// \brief Creates a simple B-scan pattern that moves the galvo over a specified range.
	///
	/// \fn ScanPatternHandle createVolumePattern(ProbeHandle Probe, double RangeX, int SizeX, double RangeY, int SizeY);
	/// \ingroup ScanPattern
	/// \brief Creates a simple volume pattern.
	///
	/// The volume pattern consists of a single uninterrupted scan and all data is acquired in a single measurement. In contrast to a B-scan stack pattern (see #createBScanStackPattern()) only one apodization is performed for the complete volume. 
	/// The complete volume will be returned in one raw data (#RawDataHandle) by calling #getRawData() if a continuous asynchroneous measurement is performed (#Acquisition_AsyncContinuous).
	/// The volume will be returned slice-by-slice by calling #getRawData() if a single/finite asynchroneous measurement is performed (#Acquisition_AsyncFinite).
	///
	/// \fn ScanPatternHandle createBScanStackPattern(ProbeHandle Probe, double RangeX, int SizeX, double RangeY, int SizeY);
	/// \ingroup ScanPattern
	/// \brief Creates a simple stack pattern.
	///
	/// The BScan stack pattern is a volume measurement which consists of several shifted B-Scan measurements. The resulting data will be identical to a volume (see #createVolumePattern()) but an apodization is performed for each slice (B-scan).
	/// The complete volume will be returned in one raw data (#RawDataHandle) if a single asynchroneous measurement is performed (#Acquisition_AsyncContinuous).
	/// The volume will be returned slice-by-slice by calling #getRawData() if a single/finite asynchroneous measurement is performed (#Acquisition_AsyncFinite).
	///
	/// \fn void rotateScanPattern(ScanPatternHandle, double);
	/// \ingroup ScanPattern
	/// \brief Rotates the specfied pattern (#ScanPatternHandle);.
	///
	/// \fn void shiftScanPattern(ScanPatternHandle, double, double);
	/// \ingroup ScanPattern
	/// \brief Shifts the specified pattern (#ScanPatternHandle). 
	///
	/// \fn void clearScanPattern(ScanPatternHandle Pattern);
	/// \ingroup ScanPattern
	/// \brief Clears the specified scan pattern (#ScanPatternHandle).

	SPECTRALRADAR_API ScanPatternHandle createNoScanPattern(ProbeHandle Probe, int Scans, int NumberOfScans);
	SPECTRALRADAR_API ScanPatternHandle createTriggerPattern(ProbeHandle Probe, int Scans);
	SPECTRALRADAR_API ScanPatternHandle createBScanPattern(ProbeHandle Probe, double Range, int AScans);
	SPECTRALRADAR_API ScanPatternHandle createBScanPatternManual(ProbeHandle Probe, double StartX, double StartY, double StopX, double StopY, int AScans);
	SPECTRALRADAR_API ScanPatternHandle createIdealBScanPattern(ProbeHandle Probe, double Range, int AScans);
	SPECTRALRADAR_API ScanPatternHandle createCirclePattern(ProbeHandle Probe, double Radius, int AScans);
	SPECTRALRADAR_API ScanPatternHandle createVolumePattern(ProbeHandle Probe, double RangeX, int SizeX, double RangeY, int SizeY);
	SPECTRALRADAR_API ScanPatternHandle createBScanStackPattern(ProbeHandle Probe, double RangeX, int SizeX, double RangeY, int SizeY);
	SPECTRALRADAR_API void rotateScanPattern(ScanPatternHandle Pattern, double Angle);
	SPECTRALRADAR_API void shiftScanPattern(ScanPatternHandle Pattern, double ShiftX, double ShiftY);
	SPECTRALRADAR_API void clearScanPattern(ScanPatternHandle Pattern);

	//  ACQUISITION

	/// \defgroup Acquisition Acquisition
	/// \brief Functions for acquisition.
	///
	/// \enum AcquisitionType 
	/// \ingroup Acquisition
	/// \brief Determines the kind of acquisition

	typedef enum C_AcquisitionType {
		/// Specifies a asynchroneous, infinite/continuous measurement.
		Acquisition_AsyncContinuous,
		/// Specifies a asynchroneous, finite measurement.
		Acquisition_AsyncFinite,
		/// Specfies a synchroneous measurement.
		Acquisition_Sync
	} AcquisitionType;

	/// \fn void startMeasurement(OCTDeviceHandle, ScanPatternHandle, AcquisitionType);
	/// \ingroup Acquisition
	/// \brief starts a continuous measurement BScans. 
	///
	/// Scanning takes place according to the specified scan pattern handle. 
	/// Data can be recorded using the #getRawData() function.  
	/// If you are done, call #stopMeasurement().
	/// \param Dev The #OCTDeviceHandle that was initially provided by initDevice.
	/// \param Pattern the #ScanPatternHandle
	///
	/// \fn void getRawData(OCTDeviceHandle, RawDataHandle);
	/// \ingroup Acquisition
	/// \brief Acquires data and stores the data unprocessed.
	///
	/// \fn void stopMeasurement(OCTDeviceHandle);
	/// \ingroup Acquisition
	/// \brief stops the current measurement.
	/// \param Dev The #OCTDeviceHandle that was initially provided by initDevice.
	///
	/// \fn void measureSpectra(OCTDeviceHandle Dev, int NumberOfSpectra, RawDataHandle Raw)
	/// \ingroup Acquisition
	/// \brief Acquires N spectra of raw data without moving galvo scanners.

	SPECTRALRADAR_API void startMeasurement(OCTDeviceHandle Dev, ScanPatternHandle Pattern, AcquisitionType);
	SPECTRALRADAR_API void getRawData(OCTDeviceHandle Dev, RawDataHandle RawData);
	SPECTRALRADAR_API void stopMeasurement(OCTDeviceHandle Dev);
	SPECTRALRADAR_API void measureSpectra(OCTDeviceHandle Dev, int NumberOfSpectra, RawDataHandle Raw);

	// PROCESSING

	/// \defgroup Processing Processing
	/// \brief Standard Processing Routines.
	///
	/// \enum ProcessingType
	/// \ingroup Processing
	/// \brief defindes the algorithm used for dechirping the input signal and Fourier transformation
	typedef enum C_ProcessingType {
		/// FFT with no dehchirp algorithm applied.
		Processing_StandardFFT,
		/// Full matrix multiplication ("filter bank"). Mathematical precise dechirp, but rather slow.
		Processing_StandardNDFT,
		/// Linear interpolation prior to FFT.
		Processing_iFFT1,
		/// Linear interpolation with 2x oversampling prior to FFT.
		Processing_iFFT2,
		/// Linear interpolation with 3x oversampling prior to FFT.
		Processing_iFFT3,
		/// Linear interpolation with 4x oversampling prior to FFT.
		Processing_iFFT4,
		/// NFFT algorithm with parameters alpha=1 and m=1.
		Processing_NFFT11,
		/// NFFT algorithm with parameters alpha=1 and m=2.
		Processing_NFFT12,
		/// NFFT algorithm with parameters alpha=1 and m=3.
		Processing_NFFT13,
		/// NFFT algorithm with parameters alpha=2 and m=1.
		Processing_NFFT21,
		/// NFFT algorithm with parameters alpha=2 and m=2.
		Processing_NFFT22,
		/// NFFT algorithm with parameters alpha=2 and m=3.
		Processing_NFFT23,
		/// NFFT algorithm with parameters alpha=3 and m=1.
		Processing_NFFT31,
		/// NFFT algorithm with parameters alpha=3 and m=2.
		Processing_NFFT32,
		/// NFFT algorithm with parameters alpha=3 and m=3.
		Processing_NFFT33,
		/// NFFT algorithm with parameters alpha=3 and m=4.
		Processing_NFFT34
	} ProcessingType;

	/// \enum ApodizationWindow
	/// \ingroup Processing
	/// \brief To select the apodization window function.
	typedef enum {
		/// Hann window function
		Apodization_Hann = 0,
		/// Hamming window function
		Apodization_Hamming = 1,
		/// Gaussian window function
		Apodization_Gauss = 2,
		/// Tapered cosine window function
		Apodization_TaperedCosine = 3,
		/// Blackman window function
		Apodization_Blackman = 4,
		/// 4-Term Blackman-Harris window function
		Apodization_BlackmanHarris = 5
	} ApodizationWindow;

	/// \enum ProcessingParameterInt
	/// \ingroup Processing
	/// \brief Parameters that set the behavious of the processing algorithms.
	typedef enum C_ProcessingParameterInt {
		/// Identifyer for binning of several subsequent spectra.
		Processing_Binning,
		/// Identifyer for averaging of several subsequent spectra.
		Processing_Averaging,
		/// Identifier for zero padding prior to Fourier transformation.
		Processing_ZeroPadding
	} ProcessingParameterInt;

	/// \enum ProcessingParameterFloat
	/// \ingroup Processing
	/// \brief Parameters that set the behavious of the processing algorithms.
	typedef enum C_ProcessingParameterFloat {
		/// Sets how much influence newly acquired apodizations have compared to older ones.
		Processing_ApodizationDamping
	} ProcessingParameterFloat;

	/// \enum CalibrationData
	/// \ingroup Processing
	/// \brief Data describing the calibration of the processing routines. 
	typedef enum C_CalibrationData {
		/// Calibration vector used as offset.
		Calibration_OffsetErrors,
		/// Calibration data used as reference spectrum.
		Calibration_ApodizationSpectrum,
		/// Calibration data used as apodization multiplicators.
		Calibration_ApodizationVector,
		/// Calibration data used to compensate for dispersion.
		Calibration_Dispersion,
		/// Calibration data used for dechirping spectral data.
		Calibration_Chirp,
		/// Calibration data used as extended adjust.
		Calibration_ExtendedAdjust
	} CalibrationData;

	/// \enum ProcessingFlag
	/// \ingroup Processing
	/// \brief Flags that set the behaviour of the processing algorithms.
	typedef enum C_ProcessingFlag {
		/// Flag identifying whether to apply offset error removal. This flag is activated by default.
		Processing_UseOffsetErrors,
		/// Flag sets whether the complete DC spectrum is to be removed from the spectral data.
		Processing_RemoveDCSpectrum,
		/// Flag identifying whether to apply apodization. This flag is activated by default.
		Processing_UseApodization,
		Processing_UseDispersionCompensation,
		/// Flag identifying whether to apply dechirp. This flag is activated by default.
		Processing_UseDechirp,
		/// Flag identifying whether to use extended adjust. This flag is activated by default.
		Processing_UseExtendedAdjust
	} ProcessingFlag;

	typedef enum C_ProcessingAveragingAlgorithm {
		Processing_Averaging_Min,
		Processing_Averaging_Mean,
		Processing_Averaging_GeometricalMean,
		Processing_Averaging_Median,
		Processing_Averaging_Norm2,
		Processing_Averaging_Norm4,
		Processing_Averaging_MaxNorm,
		Processing_Averaging_Fourier_Min,
		Processing_Averaging_Fourier_Mean,
		Processing_Averaging_Fourier_GeometricalMean,
		Processing_Averaging_Fourier_Median,
		Processing_Averaging_Fourier_Norm2,
		Processing_Averaging_Fourier_Norm4,
		Processing_Averaging_Fourier_MaxNorm
	} ProcessingAveragingAlgorithm;

	typedef enum ApodizationWindowParameter_ {
		ApodizationWindowParameter_Sigma,
		ApodizationWindowParameter_Ratio
	} ApodizationWindowParameter;

	/// \fn ProcessingHandle createProcessing(int SpectrumSize, int BytesPerRawPixel, float ScalingFactor, float MinElectrons, ProcessingType Type);
	/// \ingroup Processing
	/// \brief Creates processing routines with the specified properties. 
	///
	/// \fn ProcessingHandle createProcessingForDevice(OCTDeviceHandle Dev);
	/// \ingroup Processing
	/// \brief Creates suitable standard processing routines for the specified device (#OCTDeviceHandle);.
	///
	/// \fn int getInputSize(ProcessingHandle);
	/// \ingroup Processing
	/// \brief Returns the expected input size (pixels per spectrum); of the processing algorithms. 
	/// 
	/// This function is provided for convenience as processing routines can be used independently of the device.
	///
	/// \fn int getAScanSize(ProcessingHandle Proc);
	/// \ingroup Processing
	/// \brief gives the number of pixels in an A-Scan of the SpectralRadar device. This number is identical to the number of rows in a finished B-Scan.
	/// \param Proc Processing that is used to get the A-Scan.
	/// \return The number of 
	///
	/// \fn void setApodizationWindow(ProcessingHandle, ApodizationWindow);
	/// \ingroup Processing
	/// \brief set the window function that is to be used for apodization. The selected function will be used in all subsequent processings.
	/// 
	/// If this function is not explicitly called a Hann window will be used.
	/// \param Proc handle to the #OCTDeviceHandle that was initially provided by initDevice.
	/// \param Window the apodization window that is used for data processing. 
	///
	/// \fn void setDechirpAlgorithm(ProcessingHandle, ProcessingType)
	/// \ingroup Processing
	/// \brief Sets the algorithm that is to be sued for dechirping the input spectra.
	///
	/// \fn setProcessingParameterInt(ProcessingHandle Proc, ProcessingParameterInt Selection, int Value);
	/// \ingroup Processing
	/// \brief Sets the specified integer value processing parameter.
	///
	/// \fn int getProcessingParameterInt(ProcessingHandle Proc, ProcessingParameterInt Selection);
	/// \ingroup Processing
	/// \brief Returns the specified integer value processing parameter.
	///
	/// \fn void setProcessingFlag(ProcessingHandle Proc, ProcessingFlag Flag, BOOL Value);
	/// \ingroup Processing
	/// \brief Sets the specified processing flag.
	///
	/// \fn BOOL getProcessingFlag(ProcessingHandle Proc, ProcessingFlag Flag);
	/// \ingroup Processing
	/// \brief Returns TRUE if the specified processing flag is set, FALSE otherwise.
	///
	/// \fn void setCalibration(ProcessingHandle Proc, CalibrationData Selection, DataHandle Data);
	/// \ingroup Processing
	/// \brief Sets the current active calibration data.
	///
	/// \fn void getCalibration(ProcessingHandle Proc, CalibrationData Selection, DataHandle Data);
	/// \ingroup Processing
	/// \brief Returns the currently active calibration parameter.
	///
	/// \fn void measureCalibration(OCTDeviceHandle Dev, ProcessingHandle Proc, CalibrationData Selection);
	/// \ingroup Processing
	/// \brief Measures the specified calibration parameters and uses them in subsequent processing.
	///
	/// \fn void saveCalibrationAuto(ProcessingHandle Proc, CalibrationData Selection);
	/// \ingroup Processing
	/// \brief Saves the selected calibration in its default path.
	/// \warning This will overwride your default calibration of the device.
	///
	/// \fn void saveCalibration(ProcessingHandle Proc, CalibrationData Selection, const char Path[]);
	/// \ingroup Processing
	/// \brief Saves the selected calibration in the specified path.
	/// \warning This will overwride your default calibration of the device if you specifiy the default path. 
	///
	/// \fn void setSpectrumOutput(ProcessingHandle Proc, DataHandle Spectrum);
	/// \ingroup Processing
	/// \brief Sets the location for the resulting spectral data.
	///
	/// \fn void setOffsetCorrectedSpectrumOutput(ProcessingHandle Proc, DataHandle OffsetCorrectedSpectrum);
	/// \ingroup Processing
	/// \brief Sets the location for the resulting offset corrected spectral data.
	///
	/// \fn void setApodizedSpectrumOutput(ProcessingHandle Proc, DataHandle ApodizedSpectrum);
	/// \ingroup Processing
	/// \brief Sets the location for the resulting apodized spectral data.
	///
	/// \fn void setComplexDataOutput(ProcessingHandle, ComplexDataHandle);
	/// \ingroup Processing
	/// \brief Sets the pointer the resulting complex B-Scan of the next processing is written to. 
	///
	/// If set to 0 no complex data result will be created in the next processing.
	///
	/// \fn void setProcessedDataOutput(ProcessingHandle, DataHandle)
	/// \ingroup Processing
	/// \brief Sets the pointer the resulting B-Scan of the next processing is written to. 
	///
	/// If set to 0 no processed floating point data in dB will be created in the next processing.
	///
	/// \fn void setHorMirroredDataOutput(ProcessingHandle, DataHandle)
	/// \ingroup Processing
	/// \brief Sets the pointer the resulting B-Scan of the next processing is written to. The result will be written mirrored at the horizontal axis.
	///
	/// If set to 0 no floating point processed data in dB will be created in the next processing.
	///
	/// \fn void setColoredDataOutput(ProcessingHandle, ColoredDataHandle, Coloring32BitHandle)
	/// \ingroup Processing
	/// \brief Sets the pointer the resulting colored B-Scan of the next processing is written to. 
	///
	/// \fn void executeProcessing(ProcessingHandle, RawDataHandle)
	/// \ingroup Processing
	/// \brief Execute the processing.
	///
	/// The specified raw data will be transformed. Results will be written to data objects specified by #setProcessedDataOutput(), #setComplexDataOutput(), #setColoredDataOutput(), etc. 
	///
	/// \fn void closeProcessing(ProcessingHandle Proc);
	/// \ingroup Processing
	/// \brief Closes the processing and frees all temporary memory that was associated with it. Processing threads will be stopped.
	/// 
	/// \fn void computeDispersion(DataHandle D1, DataHandle D2, DataHandle Chirp, DataHandle Disp);
	/// \ingroup Processing
	/// \brief Calculates a dispersion compensation vector from two AScans
	/// 
	/// D1 and D2 must be two AScans with their peaks at different depths in order to obtain a good dispersion compensation vector.
	///
	/// \fn void computeDispersionByCoeff(double Quadratic, DataHandle Chirp, DataHandle Disp);
	/// \ingroup Processing
	/// \brief Calculates a simpler form of dispersion compensation by using a quadratic coeffient.



	SPECTRALRADAR_API ProcessingHandle createProcessing(int SpectrumSize, int BytesPerRawPixel, float ScalingFactor, float MinElectrons, ProcessingType Type);
	SPECTRALRADAR_API ProcessingHandle createProcessingForDevice(OCTDeviceHandle Dev);
	SPECTRALRADAR_API int getInputSize(ProcessingHandle Proc);
	SPECTRALRADAR_API int getAScanSize(ProcessingHandle Handle);
	SPECTRALRADAR_API void setApodizationWindow(ProcessingHandle Proc, ApodizationWindow Window);
	SPECTRALRADAR_API void setApodizationWindowParameter(ProcessingHandle Proc, ApodizationWindowParameter Selection, double Value);
	SPECTRALRADAR_API void setDechirpAlgorithm(ProcessingHandle Proc, ProcessingType Type);
	SPECTRALRADAR_API void setProcessingParameterInt(ProcessingHandle Proc, ProcessingParameterInt Selection, int Value);
	SPECTRALRADAR_API int getProcessingParameterInt(ProcessingHandle Proc, ProcessingParameterInt Selection);
	SPECTRALRADAR_API void setProcessingParameterFloat(ProcessingHandle Proc, ProcessingParameterInt Selection, double Value);
	SPECTRALRADAR_API void setProcessingFlag(ProcessingHandle Proc, ProcessingFlag Flag, BOOL Value);
	SPECTRALRADAR_API BOOL getProcessingFlag(ProcessingHandle Proc, ProcessingFlag Flag);
	SPECTRALRADAR_API void setProcessingAveragingAlgorithm(ProcessingHandle Proc, ProcessingAveragingAlgorithm Algorithm);
	SPECTRALRADAR_API void setCalibration(ProcessingHandle Proc, CalibrationData Selection, DataHandle Data);
	SPECTRALRADAR_API void getCalibration(ProcessingHandle Proc, CalibrationData Selection, DataHandle Data);
	SPECTRALRADAR_API void measureCalibration(OCTDeviceHandle Dev, ProcessingHandle Proc, CalibrationData Selection);
	SPECTRALRADAR_API void saveCalibrationAuto(ProcessingHandle Proc, CalibrationData Selection);
	SPECTRALRADAR_API void saveCalibration(ProcessingHandle Proc, CalibrationData Selection, const char Path[]);
	SPECTRALRADAR_API void loadCalibration(ProcessingHandle Proc, CalibrationData Selection, const char Path[]);
	SPECTRALRADAR_API void setSpectrumOutput(ProcessingHandle Proc, DataHandle Spectrum);
	SPECTRALRADAR_API void setOffsetCorrectedSpectrumOutput(ProcessingHandle Proc, DataHandle OffsetCorrectedSpectrum);
	SPECTRALRADAR_API void setApodizedSpectrumOutput(ProcessingHandle Proc, DataHandle ApodizedSpectrum);
	SPECTRALRADAR_API void setProcessedDataOutput(ProcessingHandle Proc, DataHandle Scan);
	SPECTRALRADAR_API void setHorMirroredDataOutput(ProcessingHandle Proc, DataHandle Scan);
	SPECTRALRADAR_API void setColoredDataOutput(ProcessingHandle Proc, ColoredDataHandle BScan, Coloring32BitHandle Color);
	SPECTRALRADAR_API void setComplexDataOutput(ProcessingHandle Proc, ComplexDataHandle ComplexBScan);
	SPECTRALRADAR_API void executeProcessing(ProcessingHandle Proc, RawDataHandle RawData);
	SPECTRALRADAR_API void closeProcessing(ProcessingHandle Proc);

	SPECTRALRADAR_API void computeDispersion(DataHandle D1, DataHandle D2, DataHandle Chirp, DataHandle Disp);
	SPECTRALRADAR_API void computeDispersionByCoeff(double Quadratic, DataHandle Chirp, DataHandle Disp);

	// EXPORT 

	/// \defgroup Export Export and Import
	/// \brief Export functionality to store data to disk and load it from there. 
	///
	/// \enum Data2DExportFormat
	/// \ingroup Export
	/// \brief Export format for 2-dimensional data (#DataHandle). 
	typedef enum C_Data1DExportFormat {
		/// RAW data format containing the data of the object as binary, single precision floating point values, little endian.
		Data1DExport_RAW,
		/// TXT is a text file having all values stored space seperated and human readable.
		Data1DExport_TXT,
		/// CSV (Comma Seperated Values) is a text file having all values stored, comma seperated and human readable.
		Data1DExport_CSV,
		/// TableTXT is a human readable text-file in a table like format, having the physical 1- and 2-axis as first two columns and the data value as third.
		Data1DExport_TableTXT,
		/// FITS Data format
		Data1DExport_Fits
	} Data1DExportFormat;

	/// \enum Data2DExportFormat
	/// \ingroup Export
	/// \brief Export format for 2-dimensional data (#DataHandle). 
	typedef enum C_Data2DExportFormat {
		/// Spectral Radar Metaformat, containing no data but all additinal parameters, such as spacing, size, etc.
		Data2DExport_SRM,
		/// RAW data format containing the data of the object as binary, single precision floating point values, little endian.
		Data2DExport_RAW,
		/// TXT is a text file having all values stored space seperated and human readable.
		Data2DExport_TXT,
		/// CSV (Comma Seperated Values) is a text file having all values stored, comma seperated and human readable.
		Data2DExport_CSV,
		/// TableTXT is a human readable text-file in a table like format, having the physical 1- and 2-axis as first two columns and the data value as third.
		Data2DExport_TableTXT,
		/// FITS Data format
		Data2DExport_Fits
	} Data2DExportFormat;

	/// \enum Data3DExportFormat
	/// \ingroup Export
	/// \brief Export format for 3-dimensional data (#DataHandle).
	typedef enum C_Data3DExportFormat {
		/// Spectral Radar Metaformat, containing no data but all additinal parameters, such as spacing, size, etc.
		Data3DExport_SRM,
		/// RAW data format containing the data of the object as binary, single precision floating point values, little endian.
		Data3DExport_RAW,
		/// VFF data format.
		// TODO: More text.
		Data3DExport_VFF,
		/// VTK data format.
		// TODO: More text.
		Data3DExport_VTK,
		/// FITS Data format
		Data3DExport_Fits
	} Data3DExportFormat;


	/// \enum ComplexDataExportFormat
	/// \ingroup Export
	/// \brief Export format for complex data
	typedef enum C_ComplexDataExportFormat {
		ComplexDataExport_RAW
	} ComplexDataExportFormat;

	/// \enum ColoredDataExportFormat
	/// \ingroup Export
	/// \brief Export format for images (#ColoredDataHandle).
	typedef enum C_ColoredDataExportFormat {
		/// Spectral Radar Metaformat, containing no data but all additinal parameters, such as spacing, size, etc.
		ColoredDataExport_SRM,
		/// RAW data format containing the data of the object as binary, 32-bit unsigned integer values, little endian. The concrete format of the data depends on the colored data object (#ColoredDataHandle). In most cases it will be RGB32 or RGBA32.
		ColoredDataExport_RAW,
		/// BMP - Bitmap image format.
		ColoredDataExport_BMP,
		/// PNG image format.
		ColoredDataExport_PNG,
		/// JPG/JPEG image format.
		ColoredDataExport_JPG
	} ColoredDataExportFormat;

	/// \enum DataImportFormat
	/// \ingroup Export
	/// \brief Supported import format to load data from disk.
	typedef enum C_DataImportFormat {
		/// Spectral Radar Metaformat, containing no data but all additinal parameters, such as spacing, size, etc. It is searched for an appropriate file with same name but different extension containg the according data.
		DataImport_SRM
	} DataImportFormat;

	typedef enum C_RawDataExportFormat {
		RawDataExport_RAW,
		/// Spectral Radar raw data
		RawDataExport_SRR
	} RawDataExportFormat;

	typedef enum C_RawDataImportFormat {
		/// Spectral Radar raw data
		RawDataImport_SRR
	} RawDataImportFormat;

	/// \fn void exportComplexData(ComplexDataHandle, ComplexDataExportFormat, const char*)
	/// \ingroup Export
	/// \brief Exports 1-, 2- and 3-dimensional complex data (#ComplexDataHandle)
	///
	/// \fn void exportData2D(DataHandle, Data2DExportFormat, const char*);
	/// \ingroup Export
	/// \brief Exports 2-dimensional data (#DataHandle). 
	///
	/// \fn void exportData3D(DataHandle, Data3DExportFormat, const char*);
	/// \ingroup Export
	/// \brief Exports 3-dimensional data (#DataHandle). 
	///
	/// \fn void exportData2DAsImage(DataHandle, Coloring32BitHandle, ColoredDataExportFormat format, const char*);
	/// \ingroup Export
	/// \brief Exports 2-dimensional data (#DataHandle) as image data (such as BMP, PNG, JPEG, ...). 
	///
	/// \fn void exportColoredData(ColoredDataHandle, ColoredDataExportFormat, const char*);
	/// \ingroup Export
	/// \brief Exports colored data (#ColoredDataHandle)
	///
	/// \fn void importData(DataHandle Data, DataImportFormat Format, const char* Path);
	/// \ingroup Export
	/// \brief Imports data with the specified format and copies it into a data object (#DataHandle).
	///
	/// \fn void exportRawData(RawDataHandle Raw, RawDataExportFormat Format, const char* Path);
	/// \ingroup Export
	/// \brief Exports unprocessed raw data with the specified format and copies it into a data object (#DataHandle).
	///
	/// \fn void importRawData(RawDataHandle Raw, RawDataImportFormat Format, const char* Path);
	/// \ingroup Export
	/// \brief Exports unprocessed raw data with the specified format from the data object (#RawDataHandle).

	SPECTRALRADAR_API void exportData1D(DataHandle Data, Data1DExportFormat Format, const char* Path);
	SPECTRALRADAR_API void exportData2D(DataHandle Data, Data2DExportFormat Format, const char* Path);
	SPECTRALRADAR_API void exportData3D(DataHandle Volume, Data3DExportFormat Format, const char* Path);
	SPECTRALRADAR_API void exportData2DAsImage(DataHandle Data, Coloring32BitHandle Color, ColoredDataExportFormat format, const char* fileName);
	SPECTRALRADAR_API void exportComplexData(ComplexDataHandle, ComplexDataExportFormat, const char*);
	SPECTRALRADAR_API void exportColoredData(ColoredDataHandle Image, ColoredDataExportFormat Format, const char* fileName);
	SPECTRALRADAR_API void importData(DataHandle Data, DataImportFormat Format, const char* Path);
	SPECTRALRADAR_API void exportRawData(RawDataHandle Raw, RawDataExportFormat, const char* Path);
	SPECTRALRADAR_API void importRawData(RawDataHandle Raw, RawDataImportFormat Format, const char* Path);

	// STACK PATTERN VOLUME

	/// \defgroup Volume Volume
	/// \brief Functionality to store and access volume data.
	///
	/// \enum Plane2D
	/// \ingroup Volume
	/// \brief Planes for slices of the volume data.
	typedef enum C_Plane2D {
		/// The 12 (XZ) plane, orthogonal to the 3 (Y) axis
		Plane2D_12,
		/// The 23 (XY) plane, orthogonal to the 3 (Z) axis
		Plane2D_23,
		/// The 13 (ZY) plane, orthogonal to the 2 (X) axis
		Plane2D_13
	} Plane2D;

	/// \enum Direction
	/// \ingroup Volume 
	/// \brief Specifies a direction
	typedef enum C_Direction {
		/// The 1-axis direction.
		Direction_1,
		/// The 2-axis direction.
		Direction_2,
		/// The 3-axis direction.
		Direction_3
	} Direction;

	/// \fn void getDataSlicePos(DataHandle Data, DataHandle Slice, Direction SliceNormalDirection, double Pos)
	/// \ingroup Volume
	/// \brief Returns a slice of data in the specified direction at the specified position.
	///
	/// \fn void getDataSliceIndex(DataHandle Data, DataHandle Slice, Direction SliceNormalDirection, int Index)
	/// \ingroup Volume
	/// \brief Returns a slice of data in the specified direction at the specified index.
	///
	/// \fn void appendData(DataHandle Data, DataHandle NewData, Direction direction)
	/// \ingroup Volume
	/// \brief Appends the new data to the old data in the specified direction.


	SPECTRALRADAR_API void getDataSlicePos(DataHandle Data, DataHandle Slice, Direction SliceNormalDirection, double Pos);
	SPECTRALRADAR_API void getDataSliceIndex(DataHandle Data, DataHandle Slice, Direction SliceNormalDirection, int Index);
	SPECTRALRADAR_API void getDataSliceAnalyzed(DataHandle Data, DataHandle Slice, Direction SliceNormalDirection, DataAnalyzation Selection, int Threshold_lower, int Threshold_upper);
	SPECTRALRADAR_API void appendData(DataHandle Data, DataHandle NewData, Direction direction);

	/// \fn ImageFieldHandle createImageField(void);
	/// \ingroup Volume
	/// \brief Creates an object holding image field data.
	///
	/// \fn void clearImageField(ImageFieldHandle ImageField);
	/// \ingroup Volume
	/// \brief Frees an object holding image field data.
	///
	/// \fn void saveImageField(ImageFieldHandle ImageField, const char* Path);
	/// \ingroup Volume
	/// \brief Saves data containing image field data.
	///
	/// \fn void loadImageField(ImageFieldHandle ImageField, const char* Path);
	/// \ingroup Volume
	/// \brief Loads data containing image field data.
	///
	/// \fn void determineImageField(ImageFieldHandle ImageField, DataHandle Surface);
	/// \ingroup Volume
	/// \brief Determines the image field correction of the surface.
	///
	/// \fn void correctSurface(ImageFieldHandle ImageField, DataHandle Data);
	/// \ingroup Volume
	/// \brief Applies the image field correction to the surface (i.e. a slice in the y-z plane).
	///
	/// \fn void correctImageField(ImageFieldHandle ImageField, ScanPatternHandle Pattern, DataHandle BScan);
	/// \ingroup Volume
	/// \brief Applies the image field correction to the given 2-dimensional or 3-dimensional dataset.

	SPECTRALRADAR_API ImageFieldHandle createImageField(void);
	SPECTRALRADAR_API void clearImageField(ImageFieldHandle ImageField);
	SPECTRALRADAR_API void saveImageField(ImageFieldHandle ImageField, const char* Path);
	SPECTRALRADAR_API void loadImageField(ImageFieldHandle ImageField, const char* Path);
	SPECTRALRADAR_API void determineImageField(ImageFieldHandle ImageField, DataHandle Surface);
	SPECTRALRADAR_API void correctSurface(ImageFieldHandle ImageField, DataHandle Surface);
	SPECTRALRADAR_API void correctImageField(ImageFieldHandle ImageField, ScanPatternHandle Pattern, DataHandle Data);

	// DOPPLER

	/// \defgroup Doppler Doppler 
	/// \brief Doppler Processing Routines.
	///
	/// \enum DopplerPropertyInt
	/// \ingroup Doppler
	/// \brief Values that determine the behaviour of the Doppler processing routines.
	typedef enum C_DopplerPropertyInt {
		/// Averaging along the first axis, usually the longitudinal axis (z)
		DopplerAveraging_1,
		/// Averaging along the first axis, usually the first transversal axis (x)
		DopplerAveraging_2
	} DopplerPropertyInt;

	/// \fn DopplerProcessingHandle createDopplerProcessing(void);
	/// \ingroup Doppler
	/// \return #DopplerProcessingHandle to the created Doppler routines.
	///
	/// \fn void createDopplerProcessingForProcessing(DopplerProcessingHandle*, ProcessingHandle);
	/// \ingroup Doppler
	/// \param Doppler Handle to the Doppler processing.
	/// \param Proc Handle to the Processing routines whose results are used as input for Doppler routines.
	///
	/// \fn void setDopplerPropertyInt(DopplerProcessingHandle, DopplerPropertyInt, int Value)
	/// \ingroup Doppler
	/// \brief Sets Doppler processing properties.
	///
	/// \fn void setDopplerAmplitudeOutput(DopplerProcessingHandle Doppler, DataHandle AmpOut)
	/// \ingroup Doppler
	/// \brief Sets the location of the resulting doppler amplitude output.
	///
	/// \fn void setDopplerPhaseOutput(DopplerProcessingHandle Doppler, DataHandle PhasesOut)
	/// \ingroup Doppler
	/// \brief Sets the location of the resulting doppler phase output.
	///
	/// \fn void executeDopplerProcessing(DopplerProcessingHandle, ComplexDataHandle)
	/// \ingroup Doppler
	/// \brief Executes the Doppler processing of the input data and returns phases and amplitudes.
	///
	/// \fn void closeDopplerProcessing(DopplerProcessingHandle)
	/// \ingroup Doppler
	/// \brief Closes the Doppler processing routines and frees the memory that has been allocated for these to work properly.


	SPECTRALRADAR_API DopplerProcessingHandle createDopplerProcessing(void);
	SPECTRALRADAR_API void createDopplerProcessingForProcessing(DopplerProcessingHandle* Doppler, ProcessingHandle Proc);
	SPECTRALRADAR_API void closeDopplerProcessing(DopplerProcessingHandle Doppler);
	SPECTRALRADAR_API void setDopplerPropertyInt(DopplerProcessingHandle Doppler, DopplerPropertyInt Property, int Value);
	SPECTRALRADAR_API void setDopplerAmplitudeOutput(DopplerProcessingHandle Doppler, DataHandle AmpOut);
	SPECTRALRADAR_API void setDopplerPhaseOutput(DopplerProcessingHandle Doppler, DataHandle PhasesOut);
	SPECTRALRADAR_API void executeDopplerProcessing(DopplerProcessingHandle Doppler, ComplexDataHandle Input);

	// SERVICE

	/// \defgroup Service Service
	/// \brief Service functions for additional analyzing of OCT functionality.
	///
	/// \fn void calcContrast(DataHandle ApodizedSpectrum, DataHandle Contrast);
	/// \ingroup Service
	/// \brief Computes the contrast for the specified (apodized); spectrum.
	///
	/// \fn double computeWellness(DataHandle AScan, double, double, double*, double*, BOOL*, BOOL*)
	/// \ingroup Service
	/// \brief measures the welleness of the
	/// \deprecated

	SPECTRALRADAR_API void calcContrast(DataHandle ApodizedSpectrum, DataHandle Contrast);
	SPECTRALRADAR_API double computeWellness(
		DataHandle AScan,
		double SpacingZ,
		double NoiseStart_mm,
		double* xTriangle,
		double* yTriangle,
		BOOL* SurfaceFound,
		BOOL* FitOkay);

	// DIRECT INI FILE ACCESS

	/// \defgroup Settings Settings
	/// \brief Direct access to INI files and settings.
	///
	/// \typedef SettingsHandle
	/// \ingroup Settings
	/// \brief Handle for saving settings on disk.
	struct C_Settings;
	typedef struct C_Settings* SettingsHandle;

	/// \fn SettingsHandle loadSettingsFile(const char*);
	/// \ingroup Settings
	/// \brief Loads a settings file (usually *.ini); and prepares its properties to be read.
	///
	/// \fn int getSettingsEntryInt(SettingsHandle, const char*, int);
	/// \ingroup Settings
	/// \brief Gets an integer number from the specified ini file (see #SettingsHandle and #loadSettingsFile);.
	///
	/// \fn double getSettingsEntryFloat(SettingsHandle, const char*, double);
	/// \ingroup Settings
	/// \brief Gets an floating point number from the specified ini file (see #SettingsHandle and #loadSettingsFile);.
	///
	/// \fn void getSettingsEntryString(SettingsHandle SettingsFile, const char* Node, const char* Default, char* Data, int MaxDataSize);
	/// \ingroup Settings
	/// \brief Gets a string from the specified ini file (see #SettingsHandle and #loadSettingsFile);.
	///
	/// \fn void setSettingsEntryInt(SettingsHandle SettingsFile, const char* Node, int Value);
	/// \ingroup Settings
	/// \brief Sets an integer entry in the specified ini file (see #SettingsHandle and #loadSettingsFile);.
	///
	/// \fn void setSettingsEntryFloat(SettingsHandle SettingsFile, const char* Node, double Value);
	/// \ingroup Settings
	/// \brief Sets a floating point entry in the specified ini file (see #SettingsHandle and #loadSettingsFile);.
	///
	/// \fn void setSettingsEntryString(SettingsHandle SettingsFile, const char* Node, const char* Value);
	/// \ingroup Settings
	/// \brief Sets a string in the specified ini file (see #SettingsHandle and #loadSettingsFile);.
	///
	/// \fn void saveSettings(SettingsHandle)
	/// \ingroup Settings
	/// \brief Saves the changes to the specified Settings file.
	///
	/// \fn void closeSettingsFile(SettingsHandle SettingsFile);
	/// \ingroup Settings
	/// \brief Closes the specified ini file and stores the set entries (see #SettingsHandle and #loadSettingsFile);.

	SPECTRALRADAR_API SettingsHandle loadSettingsFile(const char* Path);
	SPECTRALRADAR_API int getSettingsEntryInt(SettingsHandle SettingsFile, const char* Node, int DefaultValue);
	SPECTRALRADAR_API double getSettingsEntryFloat(SettingsHandle SettingsFile, const char* Node, double DefaultValue);
	SPECTRALRADAR_API void getSettingsEntryString(SettingsHandle SettingsFile, const char* Node, const char* Default, char* Data, int MaxDataSize);
	SPECTRALRADAR_API void setSettingsEntryInt(SettingsHandle SettingsFile, const char* Node, int Value);
	SPECTRALRADAR_API void setSettingsEntryFloat(SettingsHandle SettingsFile, const char* Node, double Value);
	SPECTRALRADAR_API void saveSettings(SettingsHandle SettingsFile);
	SPECTRALRADAR_API void closeSettingsFile(SettingsHandle SettingsFile);
	SPECTRALRADAR_API void setSettingsEntryString(SettingsHandle SettingsFile, const char* Node, const char* Value);

	// POSTPROCESSING

	/// \defgroup PostProcessing Post Processing
	/// \brief Algorithms and functions used for post processing of floating point data.
	///
	/// \enum PepperFilterType
	/// \ingroup PostProcessing
	/// \brief Specifies the type of pepper filter to be applied.
	typedef enum C_PepperFilterType {
		/// Values along the horizontal axis are taken into account for the pepper filter.
		PepperFilter_Horizontal,
		/// Values along the vertical axis are taken into account for the pepper filter.
		PepperFilter_Vertical,
		/// Values along the vertical and horizontal axis (star shape) are taken into account for the pepper filter.
		PepperFilter_Star,
		/// Values in a block surrounding the destination pixel are taken into account.
		PepperFilter_Block
	} PepperFilterType;

	/// \fn void medianFilter(DataHandle, int);
	/// \ingroup PostProcessing
	/// \brief Computes a median filter on the specified 2D data. 
	///
	/// \fn void levelData(DataHandle Data)
	/// \ingroup PostProcessing
	/// \brief Levels the specified data and removes tilt.
	///
	/// \fn void pepperFilter(DataHandle Data, PepperFilterType Type, float Threshold)
	/// \ingroup PostProcessing
	/// \brief Removes pepper-noise (very low values, i. e. dark spots in the data). This enhances the visual (colored) representation of the data.
	///
	/// The pepper filter compares all pixels to a mean of surrounding pixels. The surrouding pixels taking into account are specified by #PepperFilterType. If the pixels is lower than specified by the Threshold the pixel will be replaced by the mean.

	SPECTRALRADAR_API void medianFilter(DataHandle Data, int Rank);
	SPECTRALRADAR_API void levelData(DataHandle Data);
	SPECTRALRADAR_API void pepperFilter(DataHandle Data, PepperFilterType Type, float Threshold);

	// COLORING

	/// \defgroup Coloring Coloring
	/// \brief Functions used for coloring of floating point data.
	///
	/// \enum ColorScheme
	/// \ingroup Coloring
	/// \brief selects the ColorScheme of the BScans for file-export functions
	typedef enum {
		/// Black and white (monochrome) coloring
		ColorScheme_RGBA32_BlackAndWhite = 0,
		/// Black and white inverted (monochrome inverted) coloring
		ColorScheme_RGBA32_Inverted = 1,
		/// colored
		ColorScheme_RGBA32_Color = 2,
		/// orange and black coloring
		ColorScheme_RGBA32_BlackAndOrange = 3,
		/// red and black coloring
		ColorScheme_RGBA32_BlackAndRed = 4
	} ColorScheme;

	/// \fn Coloring32BitHandle createColoring32Bit(ColorScheme)
	/// \ingroup Coloring
	/// \brief Creates processing that can be used to color given floating point B-scans to 32 bit colored images.
	///
	/// \param Color The color-table to be used
	/// \return The handle (#Coloring32BitHandle) to the coloring algorithm. 
	///
	/// \fn void setColoringBoundaries(Coloring32BitHandle, float, float)
	/// \ingroup Coloring
	/// \brief Sets the boundaries in dB which are used by the coloring algorithm to map colors to floating point values in dB.
	///
	/// \fn void colorizeData(Coloring32BitHandle Coloring, DataHandle Data, ColoredDataHandle ColoredData)
	/// \ingroup Coloring
	/// \brief Colors a given data object (#DataHandle) into a given colored object (#ColoredDataHandle).
	///
	/// \fn void clearColoring32Bit(Coloring32BitHandle Color)
	/// \ingroup Coloring
	/// \brief Clears the coloring previously created by #createColoring32Bit().

	SPECTRALRADAR_API Coloring32BitHandle createColoring32Bit(ColorScheme Color);
	SPECTRALRADAR_API Coloring32BitHandle createCustomColoring32Bit(int LUTSize, unsigned long LUT[]);
	SPECTRALRADAR_API void setColoringBoundaries(Coloring32BitHandle Colorng, float Min_dB, float Max_dB);
	SPECTRALRADAR_API void colorizeData(Coloring32BitHandle Coloring, DataHandle Data, ColoredDataHandle ColoredData);
	SPECTRALRADAR_API void clearColoring32Bit(Coloring32BitHandle Coloring);

	// CAMERA IMAGE

	/// \defgroup Camera Camera
	/// \brief Functions for acquiring camera video images.
	///
	/// \enum CameraPropertyFloat
	/// \ingroup Camera
	/// \brief Enum identifying properties of the camera.
	typedef enum C_CameraPropertyFloat{
		/// Saturation of the video camera.
		Camera_Saturation,
		/// Brightness of the video camera.
		Camera_Brightness,
		/// Contrast of the video camera.
		Camera_Contrast
	} CameraPropertyFloat;

	/// \fn void getMaxCameraImageSize(OCTDeviceHandle, int*, int*)
	/// \ingroup Camera
	/// \brief Returns the maximum possible camera image size for the current device.
	///
	/// \fn void getCameraImage(OCTDeviceHandle, int SizeX, int SizeY, ColoredDataHandle)
	/// \ingroup Camera
	/// \brief Gets a camera image.
	///
	/// \fn void getFlippedCameraImage(OCTDeviceHandle, int SizeX, int SizeY, ColoredDataHandle)
	/// \ingroup Camera
	/// \brief Gets a camera image.
	///
	/// The returned camera image is mirrored.
	///
	/// \fn void setCameraPropertyFloat(OCTDeviceHandle, CameraPropertyFloat, double);
	/// \ingroup Camera
	/// \brief Sets saturation, brightness and contrast for the camera images if this option is available for the current device.

	SPECTRALRADAR_API void getMaxCameraImageSize(OCTDeviceHandle Dev, int* SizeX, int* SizeY);
	SPECTRALRADAR_API void getCameraImage(OCTDeviceHandle Dev, int SizeX, int SizeY, ColoredDataHandle Image);
	SPECTRALRADAR_API void getFlippedCameraImage(OCTDeviceHandle Dev, int SizeX, int SizeY, ColoredDataHandle Image);
	SPECTRALRADAR_API void setCameraPropertyFloat(OCTDeviceHandle Dev, CameraPropertyFloat Selection, double Value);

	// HELPER

	/// \fn void getConfigPath(char* Path, int StrSize);
	/// \ingroup Helper
	/// \brief Returns the path that hold the config files.
	///
	/// \fn double getReferenceIntensity(ProcessingHandle Proc)
	/// \ingroup Helper
	/// \brief Returns an absolute value that indicates the refernce intensity that was present when the currently used apodization was determined.
	///
	/// \fn double double getRelativeReferenceIntensity(OCTDeviceHandle Device, ProcessingHandle Proc)
	/// \ingroup Helper
	/// \brief Returns a value larger than 0.0 and smaller than 1.0 that indicates the refernce intensity that was present when the currently used apodization was determined.

	SPECTRALRADAR_API double getReferenceIntensity(ProcessingHandle Proc);
	SPECTRALRADAR_API double getRelativeReferenceIntensity(OCTDeviceHandle Device, ProcessingHandle Proc);
	SPECTRALRADAR_API void getConfigPath(char* Path, int StrSize);

	// BUFFER

	/// \defgroup Buffer Buffer
	/// \brief Functions for acquiring camera video images.
	///
	/// \fn BufferHandle createMemoryBuffer()
	/// \ingroup Buffer
	/// \brief Creates a buffer holding data and colored data.
	///
	/// \fn void appendToBuffer(BufferHandle, DataHandle, ColoredDataHandle)
	/// \ingroup Buffer
	/// \brief Appends specified data and colored data to the requested buffer. 
	/// 
	/// If insufficient memory is availalbe the oldest items in the buffer will be freed automatically. 
	///
	/// \fn int getBufferSize(BufferHandle)
	/// \ingroup Buffer
	/// \brief Returns the currently avaiable data sets in the buffer.
	///
	/// \fn DataHandle getBufferData(BufferHandle, int Index)
	/// \ingroup Buffer
	/// \brief Returns the data in the buffer.
	///
	/// \fn ColoredDataHandle getColoredBufferData(BufferHandle, int Index)
	/// \ingroup Buffer
	/// \brief Returns the colored data in the buffer.
	///
	/// \fn void clearBuffer(BufferHandle)
	/// \ingroup Buffer
	/// \brief Clears the buffer and frees all data and colored data objects in it. 

	SPECTRALRADAR_API BufferHandle createMemoryBuffer(void);
	SPECTRALRADAR_API void appendToBuffer(BufferHandle, DataHandle, ColoredDataHandle);
	SPECTRALRADAR_API int getBufferSize(BufferHandle);
	SPECTRALRADAR_API DataHandle getBufferData(BufferHandle, int Index);
	SPECTRALRADAR_API ColoredDataHandle getColoredBufferData(BufferHandle, int Index);
	SPECTRALRADAR_API void clearBuffer(BufferHandle);

	/// \fn int getNumberOfOutputValues(OCTDeviceHandle Dev)
	/// \ingroup Output Values
	/// \brief Returns the number of output values.
	///
	/// \fn void getOutputValueName(OCTDeviceHandle Handle, int Index, char* Name, int NameStringSize, char* Unit, int UnitStringSize);
	/// \ingroup Output Values
	/// \brief Returns names and units of the requested output values.
	///
	/// \fn void setOutputValueByName(OCTDeviceHandle Handle, const char* Name, double value);
	/// \ingroup Output Values
	/// \brief Sets the specified output value.
	///
	/// \fn void setOutputValueByIndex(OCTDeviceHandle Handle, int Index, double Value);
	/// \ingroup Output Values
	/// \brief Sets the specified output value.

	SPECTRALRADAR_API int getNumberOfOutputValues(OCTDeviceHandle Dev);
	SPECTRALRADAR_API void getOutputValueName(OCTDeviceHandle Handle, int Index, char* Name, int NameStringSize, char* Unit, int UnitStringSize);
	SPECTRALRADAR_API void getOutputValueRangeByName(OCTDeviceHandle Handle, const char* Name, double* Min, double* Max);
	SPECTRALRADAR_API void getOutputValueRangeByIndex(OCTDeviceHandle Handle, int Index, double* Min, double* Max);
	SPECTRALRADAR_API void setOutputValueByName(OCTDeviceHandle Handle, const char* Name, double value);
	SPECTRALRADAR_API void setOutputValueByIndex(OCTDeviceHandle Handle, int Index, double Value);

	// OTHER
	typedef enum Device_CameraPreset_ {
		Device_CameraPreset_1,
		Device_CameraPreset_2,
		Device_CameraPreset_3,
		Device_CameraPreset_Default
	} Device_CameraPreset;

	SPECTRALRADAR_API void setCameraPreset(OCTDeviceHandle Dev, Device_CameraPreset Preset);
	SPECTRALRADAR_API void getCameraPresetDescription(OCTDeviceHandle Dev, Device_CameraPreset Preset, char* Description, int* BufferSize);

	// BETA API
	// DO NOT USE
	SPECTRALRADAR_API int getScanPatternLUTSize(ScanPatternHandle Pattern);
	SPECTRALRADAR_API void getScanPatternLUT(ScanPatternHandle Pattern, double* PosX, double* PosY);

	SPECTRALRADAR_API void startScanBenchmark(OCTDeviceHandle Dev, double* RawX, double* RawY, int Size, double RateHz);
	SPECTRALRADAR_API void stopScanBenchmark(OCTDeviceHandle Dev);
	SPECTRALRADAR_API void getScanFeedback(OCTDeviceHandle Dev, double* RawX, double* RawY);
	SPECTRALRADAR_API int getScanFeedbackSize(OCTDeviceHandle Dev);
	SPECTRALRADAR_API double QuantumEfficiency(OCTDeviceHandle Dev, double CenterWavelength_nm, double PowerIntoSpectrometer_W, DataHandle Spectrum_e);
	SPECTRALRADAR_API void computeLinearKRawData(ComplexDataHandle ComplexDataAfterFFT, DataHandle LinearKData);

	// FULL RANGE BETA API
	// DO NOT USE
	struct C_FullRange;
	typedef struct C_FullRange* FullRangeHandle;
	SPECTRALRADAR_API FullRangeHandle initFullRange();
	SPECTRALRADAR_API void executeFullRange(FullRangeHandle FullRange, DataHandle ApodizedDataIn, ComplexDataHandle ApodizedDataOut);
	SPECTRALRADAR_API void closeFullRange(FullRangeHandle FullRange);
	SPECTRALRADAR_API void setFullRangeSensitivity(FullRangeHandle FullRangeObject, float CutOff, float Smoothness);

	SPECTRALRADAR_API void executeComplexProcessing(ProcessingHandle Proc, ComplexDataHandle ApodizedSpectralData);

	// END

	SPECTRALRADAR_API void flattenImage(DataHandle ImageData);
	SPECTRALRADAR_API void determineThickness(DataHandle Data, float* front, float* back);

	SPECTRALRADAR_API ScanPatternHandle createFragmentedScanPattern(int ChunkSize, int NumberOfChunks);

	// ALPHA APIs
	SPECTRALRADAR_API unsigned long long getFreeMemory();

	/// \fn BOOL checkAvailableMemoryForScanPattern(OCTDeviceHandle Dev, ScanPatternHandle Pattern, ptrdiff_t AdditionalMemory)
	/// \brief Checks whether sufficient memory is available for acquiring the specified scan pattern.
	/// 
	/// \par AdditionalMemory The parameter specifies additional memory that will be required during the measurement (from startMeasurement() to stopMeasruement()) unknown to the SDK and/or memory that will be freed/available prior to the call of startMeasurement(). 
	SPECTRALRADAR_API BOOL checkAvailableMemoryForScanPattern(OCTDeviceHandle Dev, ScanPatternHandle Pattern, ptrdiff_t AdditionalMemory);

#ifdef __cplusplus 
}
#endif

#endif // _SPECTRALRADAR_H
