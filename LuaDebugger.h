// ---------------------------------------------------------------------------
// FILE NAME            : LuaDebugger.h
// ---------------------------------------------------------------------------
// DESCRIPTION :
//
// Debugging interface
// 
// ---------------------------------------------------------------------------
// VERSION              : 1.00
// DATE                 : 1-Sep-2005
// AUTHOR               : Richard Shephard
// ---------------------------------------------------------------------------
// LIBRARY INCLUDE FILES
#ifndef __LUA_DEBUGGER_H__
#define __LUA_DEBUGGER_H__

#include "stdafx.h"
#include "lualib/luainc.h"
#include "luavirtualmachine.h"

enum
{
   DBG_MASK_CALL = LUA_MASKCALL,
   DBG_MASK_RET = LUA_MASKRET,
   DBG_MASK_LINE = LUA_MASKLINE,
   DBG_MASK_COUNT = LUA_MASKCOUNT
};

class CLuaVirtualMachine;

class CLuaDebugger
{
public:
   CLuaDebugger (CLuaVirtualMachine& vm);
   virtual ~CLuaDebugger (void);

   static void LuaHook (lua_State *lua, lua_Debug *ar);
   static void LuaHookCount (lua_State *lua);
   static void LuaHookLine (lua_State *lua);
   static void LuaHookRet (lua_State *lua);
   static void LuaHookCall (lua_State *lua);


   void SetHooksFlag (int iMask);
   void SetCount (int iCount) { m_iCountMask = iCount; }

   void ErrorRun (int iErrorCode);
   void GetErrorMsg(char *msg);
   void Abort();

protected:
   int m_iCountMask;
   CLuaVirtualMachine& m_vm;
   char m_errorMsg[128];
   static bool m_bAbortScript;
};


#endif // __LUA_DEBUGGER_H__