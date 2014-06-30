// rtpToFile.h : main header file for plugin
// Jessica Yang.org at ZI Mannheim

#ifndef RTPTOFILE_H
#define RTPTOFILE_H

#include "TBVPluginInterface.h"
#include <iostream>
#include <fstream>
using namespace std;

/*
 * This class implements a plugin by deriving it from the generic PluginInterface
 */
class rtpToFile : public TBVPluginInterface
{
public:
	
	PLUGIN_ACCESS rtpToFile();
	PLUGIN_ACCESS ~rtpToFile();

	PLUGIN_ACCESS bool initPlugin();
	PLUGIN_ACCESS bool executePreRun();
	PLUGIN_ACCESS bool executePreStep();
	PLUGIN_ACCESS bool executePostStep();
	PLUGIN_ACCESS bool executePostRun();
	
protected:
	char* logfileName;
	ofstream outf;
	int stepCounter;
};

// To avoid C++ name mangling problems, we need to bootstrap access of plugin via C functions
//
extern "C" {
	PLUGIN_ACCESS TBVPluginInterface *createPlugin();
	PLUGIN_ACCESS const char *getPluginName();
	PLUGIN_ACCESS const char *getPluginDescription();
	PLUGIN_ACCESS const char *getAuthor();
	PLUGIN_ACCESS const char *getHelpFile();
	PLUGIN_ACCESS const char *getPluginVersion();
}


#endif // RTPTOFILE_H
