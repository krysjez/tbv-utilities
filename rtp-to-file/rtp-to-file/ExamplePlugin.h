// ExamplePlugin.h : main header file for the plugin
//

#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include "TBVPluginInterface.h"

/*
 * This class implements a plugin by deriving it from the generic PluginInterface
 */
class ExamplePlugin : public TBVPluginInterface
{
public:
	
	PLUGIN_ACCESS ExamplePlugin();
	PLUGIN_ACCESS ~ExamplePlugin();

	PLUGIN_ACCESS bool initPlugin();
	PLUGIN_ACCESS bool executePreRun();
	PLUGIN_ACCESS bool executePreStep();
	PLUGIN_ACCESS bool executePostStep();
	PLUGIN_ACCESS bool executePostRun();
	
protected:
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


#endif // EXAMPLEPLUGIN_H
