#include <iostream>
#include <string>
#include <vector>
#include <stdio.h>
#include <sys/stat.h>

#include "ScriptableObject.h"
#include "plugin.h"


using namespace std;

ScriptableObject::ScriptableObject (NPP instance) 
{
	this->m_getVersion_id = NPN_GetStringIdentifier("getVersion");
}



void ScriptableObject::Deallocate()
{
}

void ScriptableObject::Invalidate()
{

}

bool ScriptableObject::HasMethod(NPIdentifier name)
{

	if (name == this->m_getVersion_id)
	{
		return true;
	}

	return false;
}


bool ScriptableObject::Invoke(NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	if (name == this->m_getVersion_id)
	{
		string utfRes = "Version as String";
		
		char *pstrRet = (char *) NPN_MemAlloc (utfRes.length () + 1);
		strcpy (pstrRet, utfRes.c_str ());

		STRINGZ_TO_NPVARIANT (pstrRet, *result);
		return PR_TRUE;
	}
	
  return false;
}

bool ScriptableObject::InvokeDefault(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return false;

}

bool ScriptableObject::HasProperty(NPIdentifier name)
{
	return false;
}

bool ScriptableObject::GetProperty(NPIdentifier name, NPVariant *result)
{
	return false;
}

bool ScriptableObject::SetProperty(NPIdentifier name, const NPVariant *value)
{
	return false;
}

bool ScriptableObject::RemoveProperty(NPIdentifier name)
{
	return false;
}

bool ScriptableObject::Enumerate(NPIdentifier **identifier, uint32_t *count)
{
	return false;
}

bool ScriptableObject::Construct(const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return true;
}

NPObject *ScriptableObject::Allocate(NPP npp, NPClass *aClass)
{
	NPObject *pObj = (NPObject *)new ScriptableObject(npp);
	return pObj;
}

void ScriptableObject::Detatch (void)
{
	m_Instance = NULL;
}



void ScriptableObject::_Deallocate(NPObject *npobj)
{
	ScriptableObject *pObj = ((ScriptableObject *) npobj);

  // Call the virtual destructor.
	pObj->Deallocate ();
	delete pObj;
}

void ScriptableObject::_Invalidate(NPObject *npobj)
{
	((ScriptableObject*)npobj)->Invalidate();
}

bool ScriptableObject::_HasMethod(NPObject *npobj, NPIdentifier name)
{
	return ((ScriptableObject*)npobj)->HasMethod (name);
}

bool ScriptableObject::_Invoke(NPObject *npobj, NPIdentifier name, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((ScriptableObject*)npobj)->Invoke (name, args, argCount, result);
}

bool ScriptableObject::_InvokeDefault(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((ScriptableObject*)npobj)->InvokeDefault (args, argCount, result);
}

bool ScriptableObject::_HasProperty(NPObject * npobj, NPIdentifier name)
{
	return ((ScriptableObject*)npobj)->HasProperty (name);
}

bool ScriptableObject::_GetProperty(NPObject *npobj, NPIdentifier name, NPVariant *result)
{
	return ((ScriptableObject*)npobj)->GetProperty (name, result);
}

bool ScriptableObject::_SetProperty(NPObject *npobj, NPIdentifier name, const NPVariant *value)
{
	return ((ScriptableObject*)npobj)->SetProperty (name, value);
}

bool ScriptableObject::_RemoveProperty(NPObject *npobj, NPIdentifier name)
{
	return ((ScriptableObject*)npobj)->RemoveProperty (name);
}

bool ScriptableObject::_Enumerate(NPObject *npobj, NPIdentifier **identifier, uint32_t *count)
{
	return ((ScriptableObject*)npobj)->Enumerate (identifier, count);
}

bool ScriptableObject::_Construct(NPObject *npobj, const NPVariant *args, uint32_t argCount, NPVariant *result)
{
	return ((ScriptableObject*)npobj)->Construct (args, argCount, result);
}

 
NPClass ScriptableObject::_npclass = {                              
  NP_CLASS_STRUCT_VERSION,								                          
  ScriptableObject::Allocate,                                       
  ScriptableObject::_Deallocate,                                    
  ScriptableObject::_Invalidate,                                    
  ScriptableObject::_HasMethod,                                     
  ScriptableObject::_Invoke,                                        
  ScriptableObject::_InvokeDefault,                                 
  ScriptableObject::_HasProperty,                                   
  ScriptableObject::_GetProperty,                                   
  ScriptableObject::_SetProperty,                                   
  ScriptableObject::_RemoveProperty,                                
  ScriptableObject::_Enumerate,                                     
  ScriptableObject::_Construct                                      
};

