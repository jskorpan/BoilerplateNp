////////////////////////////////////////////////////////////
//
// Implementation of plugin entry points (NPP_*)
// most are just empty stubs for this particular plugin 
//
#include "plugin.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <string>
using namespace std;

#ifndef XP_WIN
#define stricmp strcasecmp
#endif

char* NPP_GetMIMEDescription(void)
{
  return "application/boilerplate-plugin";
}

NPError NPP_Initialize(void)
{
  return NPERR_NO_ERROR;
}

void NPP_Shutdown(void)
{
}

string GetCurrentUrl (NPP _npp)
{
	NPObject *pWindowObj;

	// Get the window object.
	if (NPN_GetValue (_npp, NPNVWindowNPObject, &pWindowObj) != NPERR_NO_ERROR)
	{
		return "ERR_WINDOWOBJECT";
	}

	// Create a "location" identifier.
	NPIdentifier identifier = NPN_GetStringIdentifier( "location" );

	// Declare a local variant value.
	NPVariant variantValue;

	// Get the location property from the window object (which is another object).
	bool b1 = NPN_GetProperty (_npp, pWindowObj, identifier, &variantValue );

	NPN_ReleaseObject(pWindowObj);


	if (!b1)
	{
		return "ERR_LOCATIONPROP";
	}

	if (!NPVARIANT_IS_OBJECT (variantValue))
	{
		return "ERR_VARIANTISNOTOBJECT";
	}

	// Get a pointer to the "location" object.
	NPObject *locationObj = variantValue.value.objectValue;

	// Create a "href" identifier.
	identifier = NPN_GetStringIdentifier( "href" );

	NPVariant locationValue;

	// Get the location property from the location object.
	bool b2 = NPN_GetProperty (_npp, locationObj, identifier, &locationValue );

	NPN_ReleaseVariantValue (&variantValue);

	if (!b2)
	{
		return "ERR_LOCATIONVALUE";
	}
	
	if (!NPVARIANT_IS_STRING (locationValue))
	{
		NPN_ReleaseVariantValue (&locationValue);
		return "ERR_LOCATIONVALUEISNOTSTRING";
	}

	std::string ret = NPVARIANT_TO_STRING(locationValue).utf8characters;
	NPN_ReleaseVariantValue (&locationValue);

	return ret;
}



volatile static long g_plugCount = 0;

// here the plugin creates an instance of our CPlugin object which 
// will be associated with this newly created plugin instance and 
// will do all the neccessary job
NPError NPP_New(NPMIMEType pluginType,
                NPP instance,
                uint16 mode,
                int16 argc,
                char* argn[],
                char* argv[],
                NPSavedData* saved)
{ 

#define NUM_LOCATIONS 3

	static const char *validLocations[NUM_LOCATIONS] = 
	{ 
		"http://www.google.com",
		"http://127.0.0.1",
		"file://",
	};

	if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;


	// Do site locking 
	string ret = GetCurrentUrl (instance);

	bool bAcceptURL = false;

	for (int index = 0; index < NUM_LOCATIONS; index ++)
	{
		string location = validLocations[index];

		size_t pos = ret.find_first_of (location);

		if (pos == 0)
		{
			bAcceptURL = true;
			break;
		}
	}

	if (!bAcceptURL)
	{
		return NPERR_INVALID_URL;			
	}

#ifdef XP_MACOSX
  // select the right drawing model if necessary
  NPBool supportsCoreGraphics = false;
  if (NPN_GetValue(instance, NPNVsupportsCoreGraphicsBool, &supportsCoreGraphics) == NPERR_NO_ERROR && supportsCoreGraphics) 
	{
    NPN_SetValue (instance, NPPVpluginDrawingModel, (void*)NPDrawingModelCoreGraphics);
  } 
	else 
	{
    return NPERR_INCOMPATIBLE_VERSION_ERROR;
  }
#endif

	
  NPError rv = NPERR_NO_ERROR;

  CPlugin * pPlugin = new CPlugin(instance);
	
	if(pPlugin == NULL)
    return NPERR_OUT_OF_MEMORY_ERROR;

  instance->pdata = (void *)pPlugin;

  return rv;
}

// here is the place to clean up and destroy the CPlugin object
NPError NPP_Destroy (NPP instance, NPSavedData** save)
{

  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  CPlugin * pPlugin = (CPlugin *)instance->pdata;
  if(pPlugin != NULL) {
    pPlugin->shut();
    delete pPlugin;

		instance->pdata = NULL;

	}
  return rv;
}

// during this call we know when the plugin window is ready or
// is about to be destroyed so we can do some gui specific
// initialization and shutdown
NPError NPP_SetWindow (NPP instance, NPWindow* pNPWindow)
{ 
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  if(pNPWindow == NULL)
    return NPERR_GENERIC_ERROR;

  CPlugin * pPlugin = (CPlugin *)instance->pdata;

  if(pPlugin == NULL) 
    return NPERR_GENERIC_ERROR;

  // window just created
  if(!pPlugin->isInitialized() && (pNPWindow->window != NULL)) { 
    if(!pPlugin->init(pNPWindow)) {
      delete pPlugin;
      pPlugin = NULL;
      return NPERR_MODULE_LOAD_FAILED_ERROR;
    }
  }

  // window goes away
  if((pNPWindow->window == NULL) && pPlugin->isInitialized())
    return NPERR_NO_ERROR;

  // window resized
  if(pPlugin->isInitialized() && (pNPWindow->window != NULL))
    return NPERR_NO_ERROR;

  // this should not happen, nothing to do
  if((pNPWindow->window == NULL) && !pPlugin->isInitialized())
    return NPERR_NO_ERROR;

  return rv;
}

// ==============================
// ! Scriptability related code !
// ==============================
//
// here the plugin is asked by Mozilla to tell if it is scriptable
// we should return a valid interface id and a pointer to 
// nsScriptablePeer interface which we should have implemented
// and which should be defined in the corressponding *.xpt file
// in the bin/components folder
NPError	NPP_GetValue(NPP instance, NPPVariable variable, void *value)
{
	if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;

  if(instance == NULL)
    return NPERR_GENERIC_ERROR;

  CPlugin * plugin = (CPlugin *)instance->pdata;
  if(plugin == NULL)
    return NPERR_GENERIC_ERROR;

  switch (variable) 
	{
		case NPPVpluginWindowBool:
			*((PRBool *)value) = PR_TRUE;
			break;
	
		case NPPVpluginNameString:
			*((char **)value) = "Boilerplate Plugin";
			break;
  
		case NPPVpluginDescriptionString:
			*((char **)value) = "Boilerplate web plugin";
			break;

		case NPPVpluginScriptableNPObject:
		
			if (!plugin->isInitialized())
			{
				return NPERR_GENERIC_ERROR;
			}
			
			*((NPObject **)value) = plugin->GetScriptableObject();

			break;
  
		default:
			rv = NPERR_GENERIC_ERROR;
			break;
  }

  return rv;
}

NPError NPP_NewStream(NPP instance,
                      NPMIMEType type,
                      NPStream* stream, 
                      NPBool seekable,
                      uint16* stype)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

int32 NPP_WriteReady (NPP instance, NPStream *stream)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  int32 rv = 0x0fffffff;
  return rv;
}

int32 NPP_Write (NPP instance, NPStream *stream, int32 offset, int32 len, void *buffer)
{   
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  int32 rv = len;
  return rv;
}

NPError NPP_DestroyStream (NPP instance, NPStream *stream, NPError reason)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

void NPP_StreamAsFile (NPP instance, NPStream* stream, const char* fname)
{
  if(instance == NULL)
    return;
}

void NPP_Print (NPP instance, NPPrint* printInfo)
{
  if(instance == NULL)
    return;
}

void NPP_URLNotify(NPP instance, const char* url, NPReason reason, void* notifyData)
{
  if(instance == NULL)
    return;
}

NPError NPP_SetValue(NPP instance, NPNVariable variable, void *value)
{
  if(instance == NULL)
    return NPERR_INVALID_INSTANCE_ERROR;

  NPError rv = NPERR_NO_ERROR;
  return rv;
}

int16	NPP_HandleEvent(NPP instance, void* event)
{
	if(instance == NULL)
    return 0;

  int16 rv = 0;
  CPlugin * pPlugin = (CPlugin *)instance->pdata;
  if (pPlugin)
    rv = pPlugin->handleEvent(event);

  return rv;
}
