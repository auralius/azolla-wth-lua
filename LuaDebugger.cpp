// ---------------------------------------------------------------------------
// FILE NAME            : LuaDebugger.cpp
// ---------------------------------------------------------------------------
// DESCRIPTION :
//
// Simple debugging routines
// 
// ---------------------------------------------------------------------------
// VERSION              : 1.00
// DATE                 : 1-Sep-2005
// AUTHOR               : Richard Shephard
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#include "stdafx.h"
#include "luadebugger.h"
#include <string>
// ---------------------------------------------------------------------------

bool CLuaDebugger::m_bAbortScript = false;

// typedef void (*lua_Hook) (lua_State *L, lua_Debug *ar);
void CLuaDebugger::LuaHookCall (lua_State *lua)
{
   
   printf ("---- Call Stack ----\n");
   //printf ("[Level] [Function] [# args] [@line] [src]\n");

   lua_Debug ar;
   
   // Look at call stack
   for (int iLevel = 0; lua_getstack (lua, iLevel, &ar) != 0; ++iLevel)
   {
      if (lua_getinfo (lua, "Snlu", &ar) != 0)
      {
         printf ("%d %s %s %d @%d %s\n", iLevel, ar.namewhat, ar.name, ar.nups, ar.linedefined, ar.short_src);
      }
   }
}

void CLuaDebugger::LuaHookRet (lua_State *lua)
{
   
}

void CLuaDebugger::LuaHookLine (lua_State *lua)
{
	// +- Modified by Auralius based : http://www.codegurus.be/codegurus/Programming/luahooks_en.htm
	// Check the global flag to know if we should abort
    if(m_bAbortScript)
    {
      // Ok, let's abort the script
      lua_pushstring(lua, "HookRoutine: Abort requested!");
      lua_error(lua);
    }
}

void CLuaDebugger::LuaHookCount (lua_State *lua)
{
   LuaHookLine (lua);
}

void CLuaDebugger::LuaHook (lua_State *lua, lua_Debug *ar)
{
   // Handover to the correct hook
   switch (ar->event)
   {
   case LUA_HOOKCALL:
      LuaHookCall (lua);
      break;
   case LUA_HOOKRET:
   case LUA_HOOKTAILRET:
      LuaHookRet (lua);
      break;
   case LUA_HOOKLINE:
      LuaHookLine (lua);
      break;
   case LUA_HOOKCOUNT:
      LuaHookCount (lua);
      break;
   }
}

//============================================================================

CLuaDebugger::CLuaDebugger (CLuaVirtualMachine& vm) : m_iCountMask (10), m_vm (vm)
{
   // Clear all current hooks
   if (vm.Ok ())
   {
      vm.AttachDebugger (this);
      lua_sethook ((lua_State *) vm, LuaHook, 0, m_iCountMask);
   }
   m_bAbortScript = false;
}

CLuaDebugger::~CLuaDebugger (void)
{
   // Clear all current hooks
   if (m_vm.Ok ())
   {
      lua_sethook ((lua_State *) m_vm, LuaHook, 0, m_iCountMask);
   }
}

void CLuaDebugger::SetHooksFlag (int iMask)
{
   // Set hooks
   lua_sethook ((lua_State *) m_vm, LuaHook, iMask, m_iCountMask);
}

// +- Modified by Auralius July 1, 2010
void CLuaDebugger::ErrorRun (int iErrorCode)
{
   sprintf(m_errorMsg, "%s\r\n", lua_tostring ((lua_State *) m_vm, -1));

   std::string find = m_errorMsg;
   size_t found1 = 0;
   size_t found2 = 0;

   // Parsing based on colon (:)
   found1 = find.find(':');
   found2 = find.find(':', found1 + 1);
  
   size_t error_line = atoi(find.substr(found1 + 1, found2 - found1).c_str());

   if (error_line != 0)
	   sprintf(m_errorMsg , "Error at line %i => %s", error_line, find.substr(found2 + 1, strlen(m_errorMsg) - found2 - 2).c_str());      
}

// ++ Added by Auralius Jan 10, 2009
void CLuaDebugger::GetErrorMsg(char *msg)
{
	strcpy(msg, m_errorMsg);
}

void CLuaDebugger::Abort()
{ 
	m_bAbortScript = true; 
}