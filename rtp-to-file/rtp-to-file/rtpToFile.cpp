// rtpToFile.cpp : Defines the initialization routines for the DLL. (I think.)
// Jessica Yang.org at ZI Mannheim

#include "rtpToFile.h"

#if defined(OS_WIN32)
#include <windows.h>
#include <malloc.h>
#endif

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <iostream>
#include <fstream>
using namespace std;

// constructor of your plugin class
//
rtpToFile::rtpToFile()
{
}

// destructor of your plugin class
//
rtpToFile::~rtpToFile()
{
}

bool rtpToFile::initPlugin() // this function is called once after class has been created
{
	// This function call and check of return value is necessary to check whether access to TBV works!

	int ret = InitTBVAccess();

	// Never continue, if the "InitQXAccess" function fails
	//
	if(!ret)
		return false;

	// Test whether the used QX version is new enough to support the commands used in your plugin
	//
	int Major, Minor, SubMinor;
	tGetVersionOfTBV(&Major, &Minor, &SubMinor);

	// We require v3.0.0 or higher
	if( !checkVersionGreaterEqualVersion(Major, Minor, SubMinor, 3, 0, 0) )
	{
		//tMessageBox("Can not execute this plugin - Turbo-BrainVoyager version 3.0 or higher required.");
		return false;
	}

	return true;
}

// this is once called before real-time processing is started
// (Plugin log window is available when this fn is called)
//
bool rtpToFile::executePreRun()
{	
	char cTargetFolder[512];
	tGetFeedbackFolder(cTargetFolder);

	// Prompt for file to save to.
	logfileName = (char*)malloc(sizeof(char)*256);
	logfileName[255] = '\0';
	if (tGetSaveFileName("Enter filename to save to...", cTargetFolder, "File (*.*)", logfileName) == 0) {
		// TODO: some code for failure handling	
		return false;
	};
		
	// Create a file in the current directory to log to.
	outf.open(logfileName, ios::out);
	if (!outf) {
		tMessageBox("Could not create log file.");
		return false; // TODO: check?
	}

	// If all is good -- 
	// "tLogText()" sends a line of text to the "Log" text box in the plugin window
	tLogText("RTP to File Plugin 0.0.1 (feedback: jessica@jessicayang.org)");
	tLogText("Log ROI time points to a single plaintext file.");
	tLogText("Plugin ready to log to: ");
	tLogText(logfileName);
	tLogText(" ");

	stepCounter = 0;
		
	return true;
}

// This fn is called just before analyzing the newly read in data for current volume
// We don't want to preprocess anything so leave this blank.
//
bool rtpToFile::executePreStep()
{
	return true;
}

// This is the most important plugin function and is called repeatedly 
// once every time step during real-time processing
bool rtpToFile::executePostStep()
{
	stepCounter++; // a counter of received calls; used here to compare with value obtained via plugin access API 

	char cInfo[601]; // to format log strings

	// Folders are accessible that might be relevant for storing (and reading) data
	char cWatchFolder[512];
	tGetFeedbackFolder(cWatchFolder);
	char cTargetFolder[512];
	tGetFeedbackFolder(cTargetFolder);
	char cFeedbackFolder[512];
	tGetFeedbackFolder(cFeedbackFolder);
	if(stepCounter == 1)
	{
		sprintf(cInfo, "Feedback folder: %s", cFeedbackFolder);
		tLogText(cInfo);
		tLogText(" ");
	}
	
	// this is a dynamic var that can change from call to call! User can open and close ROI dialogs...
	int n_rois = tGetNrOfROIs();
	
	int n_preds = tGetFullNrOfPredictors(); // we use this fixed version since for ROIs dynamic nature of GLM is handled for us (not for whole-brain GLM, see other example plugin)
	int n_confound_preds = tGetNrOfConfoundPredictors(); // can also change over time!
	
	// access time point just finished processing in TBV
	//
	int tbvTime = tGetCurrentTimePoint(); // 1-based, for access of 0-based data, subtract 1 (see, e.g. below fn call "tGetValueOfVoxelAtTime()")
	
	// check whether current TBV time matches our own counter:
	if(tbvTime != stepCounter)
		sprintf(cInfo, "Processing unexpected TBV data point \"%i\", expected time point \"%i\"", tbvTime, stepCounter);
	else
		sprintf(cInfo, "Processing data point <b>%i</b> (No. of ROIs: %i) ...", tbvTime, n_rois);
	tLogText(cInfo);
	
	if(n_rois > 0)
	{
		int n_roi_voxels;
		float roi_val, b_val;
		char cVal[201];
		
		for(int i=0; i<n_rois; i++)
		{
			n_roi_voxels = tGetNrOfVoxelsOfROI(i);
			roi_val = tGetMeanOfROI(i);
			sprintf(cInfo, "ROI %i - n voxels: %4i, mean value: %6.3f", i+1, n_roi_voxels, roi_val);
			if(n_roi_voxels > 0)
			{
				sprintf(cVal, "  %i betas: ", n_preds);
				strcat(cInfo, cVal);
				for(int b=0; b<n_preds; b++) // 
				{
					b_val = tGetBetaOfROI(i, b);
					sprintf(cVal, "%6.3f ", b_val);
					strcat(cInfo, cVal);
				}

				// these commands allow to inspect individual ROI voxels
				int vx_x=0, vx_y=0, vx_z=0;
				float roi_mean_x=0.0f, roi_mean_y=0.0f, roi_mean_z=0.0f, signal_value=0.0f, mean_signal=0.0f;
				for(int vx=0; vx<n_roi_voxels; vx++)
				{
					bool ret = tGetCoordsOfVoxelOfROI(i, vx, vx_x, vx_y, vx_z); // get coordinates of enumerated voxels
					if(ret)
					{
						roi_mean_x += (float)vx_x;
						roi_mean_y += (float)vx_y;
						roi_mean_z += (float)vx_z;
						
						// with coordinates, the signal value of any voxel at any (existing) time point can be retrieved
						// while this access is already quite fast, a direct pointer to the whole data is also available (see "Save Volumes" plugin)
						signal_value = tGetValueOfVoxelAtTime(vx_x, vx_y, vx_z, tbvTime - 1); // "- 1" since data access is (always) 0-based
						mean_signal += signal_value;
					}
				}
				roi_mean_x /= (float)n_roi_voxels;
				roi_mean_y /= (float)n_roi_voxels;
				roi_mean_z /= (float)n_roi_voxels;
				
				mean_signal /= (float)n_roi_voxels;
				
				sprintf(cVal, "   mean coordinates: %6.3f %6.3f %6.3f", roi_mean_x, roi_mean_y, roi_mean_z);
				strcat(cInfo, cVal);
				sprintf(cVal, "   calculated mean value: %6.3f", mean_signal); // this should be same value as retrieved from TBV from ROI 
				strcat(cInfo, cVal);
				tLogText(cInfo);
			}
		}
	}
	
	return true;
}

// This is once called after real-time processing has been completed (end of scan)
//
bool rtpToFile::executePostRun()
{
	tLogText(" ");
	tLogText("Plugin completed processing.");
	
	outf.close();
	free(logfileName);

	return true;
}


//
// C access functions (extern "C" in header)
//

// THIS FUNCTION CREATES YOUR PLUGIN CLASS
//
PLUGIN_ACCESS TBVPluginInterface *createPlugin() 
{
	return new rtpToFile();
}

// return a name for your plugin
//
PLUGIN_ACCESS const char *getPluginName() 
{
	return "Log real-time point info to file";
}

// returns a descriptive string for your plugin
//   it is recommended to use a static C string to describe your plugin
//   (the potentially lengthy string needs then not be created repeatedly in successive calls)
//   Note that you may use simple HTML tags to format the string, such as <b></b>, <i></i> and <br>
//
static const char *PluginDescription = "This simple plugin shows how to access TBV data for custom processing at each time step during real-time processing. Use its source code to get started with writing your own plugins.<br>";

PLUGIN_ACCESS const char *getPluginDescription() 
{
	return PluginDescription;
}

// get credit for your work!
//
PLUGIN_ACCESS const char *getAuthor()
{
	return "<i>Jessica Yang</i>, for ZI Mannheim";
}

// you may provide the name for a HTML file containing online help
//
PLUGIN_ACCESS const char *getHelpFile()
{
	return ""; // "rtpToFileHelp/index.html";
}

// you may provide a version string ("Major.Minor") for (updated) releases of your plugins
//
PLUGIN_ACCESS const char *getPluginVersion()
{
	return "1.0";
}


