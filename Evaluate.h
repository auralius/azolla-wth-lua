#pragma once

// Lua C++ Wrapper
#include "luavirtualmachine.h"
#include "luascript.h"
#include "luadebugger.h"
// Lua library
#include "lualib/luainc.h"


///////////////////////////////////////////////////////////////////////////////
class CMyScript : public CLuaScript
{
public:
	CMyScript (CLuaVirtualMachine& vm);
	~CMyScript();
	int ScriptCalling (CLuaVirtualMachine& vm, int iFunctionNumber);
	void HandleReturns (CLuaVirtualMachine& vm, const char *strFunc);

	// Custom function
	int setspeed(CLuaVirtualMachine& vm);
	int readsensor(CLuaVirtualMachine& vm);
	int getangle(CLuaVirtualMachine& vm);
	int textline(CLuaVirtualMachine& vm);
	int setactiverobot(CLuaVirtualMachine& vm);
	int getnumberofrobots(CLuaVirtualMachine& vm);
	int stepforward(CLuaVirtualMachine& vm);
	int gettargetposition(CLuaVirtualMachine& vm);
	int getposition(CLuaVirtualMachine& vm);
	
	// TO ADD:
	/*
	int getangle(CLuaVirtualMachine& vm);
	*/

	CRobot	*m_robot;
	char	*m_msg;

protected:
	int m_iMethodBase;
};
///////////////////////////////////////////////////////////////////////////////

class CEvaluate
{
public:
	CEvaluate(void);
	~CEvaluate(void);

    int run();
	static DWORD WINAPI workerThread(LPVOID param);
	int init(CString codes, CRobot *robot);
	void setQuitFlag();
	char *getEvalMsg();
	char *getTextLineMsg();
	bool isItDone(){return m_bDone;};

private:
	CLuaVirtualMachine m_luaVM;
	CMyScript *m_ms;
	CLuaDebugger *m_dbg;
	char m_errorMsg[128];
	char m_textLineMsg[128];
	bool m_bDone;
};
