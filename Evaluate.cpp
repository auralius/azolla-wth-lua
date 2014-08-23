#include "StdAfx.h"
#include "robot.h"
#include "Evaluate.h"
///////////////////////////////////////////////////////////////////////////////

CMyScript::CMyScript(CLuaVirtualMachine &vm)
:CLuaScript (vm)
{
	m_iMethodBase = RegisterFunction ("setspeed");
	RegisterFunction ("readsensor");
	RegisterFunction ("getangle");
	RegisterFunction ("textline");
	RegisterFunction ("setactiverobot");
	RegisterFunction ("getnumofrobots");
	RegisterFunction ("stepforward");
	RegisterFunction ("gettarget");
	RegisterFunction ("getposition");
}

CMyScript::~CMyScript()
{
}

int CMyScript::ScriptCalling (CLuaVirtualMachine& vm, int iFunctionNumber)
{
	switch (iFunctionNumber - m_iMethodBase)
	{
	case 0:
		return setspeed(vm);
	case 1:
		return readsensor(vm);
	case 2:
		return getangle(vm);
	case 3:
		return textline(vm);
	case 4:
		return setactiverobot(vm);
	case 5:
		return getnumberofrobots(vm);
	case 6:
		return stepforward(vm);
	case 7:
		return gettargetposition(vm);
	case 8:
		return getposition(vm);
	}
	return -1;
}

void CMyScript::HandleReturns (CLuaVirtualMachine& vm, const char *strFunc)
{
}

int CMyScript::setspeed(CLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;

	float wRight = (float) lua_tonumber (state, -1);
	float wLeft = (float) lua_tonumber (state, -2);

	m_robot->SetSpeed(m_robot->CmToPixel(wLeft), m_robot->CmToPixel(wRight)); // in cm/s

	//Sleep(1);
	return 0;
}

int CMyScript::readsensor(CLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	int index = (int) lua_tonumber (state, -1);
	m_robot->GetSensorValue(index);
	float retVal = m_robot->PixelToCm(m_robot->GetSensorValue(index)); // in cm
	lua_pushnumber (state, retVal);

	//Sleep(1);	
	return 1;	// We are returning 1 stack
}

int CMyScript::getangle(CLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	float retVal = m_robot->GetAngle();
	lua_pushnumber (state, retVal);

	//Sleep(1);	
	return 1;	// We are returning 1 stack
}

int CMyScript::textline(CLuaVirtualMachine &vm)
{
	lua_State *state = (lua_State *) vm;
	size_t len;
	const char *msg = lua_tolstring (state, -1, &len);
	
	sprintf(m_msg, "DEBUG: %s", msg);

	//Sleep(1);
	return 0;
}

int CMyScript::setactiverobot(CLuaVirtualMachine &vm)
{
	lua_State *state = (lua_State *) vm;
	int index = (int) lua_tonumber (state, -1);

	m_robot->SetActiveRobot(index);

	//Sleep(1);
	return 0;
}

int CMyScript::getnumberofrobots(CLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	int retVal = m_robot->GetNumberOfRobot();
	lua_pushnumber (state, retVal);

	//Sleep(1);	
	return 1;	// We are returning 1 stack
}

int CMyScript::stepforward(CLuaVirtualMachine& vm)
{
	m_robot->GoRobotGo();

	Sleep(2);
	return 0;
}

int CMyScript::gettargetposition(CLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	int index = (int) lua_tonumber (state, -1);
	CPoint retVal = m_robot->GetLocationMarkPosition(index);
	lua_pushnumber (state, m_robot->PixelToCm(retVal.x));
	lua_pushnumber (state, m_robot->PixelToCm(retVal.y));

	return 2;
}

int CMyScript::getposition(CLuaVirtualMachine& vm)
{
	lua_State *state = (lua_State *) vm;
	CPoint retVal = m_robot->GetPosition();
	lua_pushnumber (state, m_robot->PixelToCm(retVal.x));
	lua_pushnumber (state, m_robot->PixelToCm(retVal.y));

	return 2;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

CEvaluate::CEvaluate(void)
{
	// Init virtual machine
	m_luaVM.InitialiseVM ();
	strcpy(m_errorMsg, "\r\n");
	strcpy(m_textLineMsg, "\r\n");
}

CEvaluate::~CEvaluate(void)
{
	// Premature stop
	if (!m_bDone)
		setQuitFlag();
}

int CEvaluate::run()
{
	// Get "main" function
	int iTopS = lua_gettop ((lua_State *) m_luaVM);
	m_ms->SelectScriptFunction ("main");
	m_ms->Go();

	// Get error message before clean up
	m_dbg->GetErrorMsg(m_errorMsg);

	// Clean up
	delete m_dbg;
	delete m_ms;

	m_bDone = true;

	return 0;
}

DWORD WINAPI CEvaluate::workerThread(LPVOID param)
{
	CEvaluate *newobj = (CEvaluate *)param;
	newobj->run();

	return 0;
}

int CEvaluate::init(CString codes, CRobot *robot)
{
	//Init scripter and debugger
	m_bDone = false;
	m_ms = new CMyScript(m_luaVM);
	m_dbg = new CLuaDebugger(m_luaVM);

	m_dbg->SetCount (10);
	m_dbg->SetHooksFlag (DBG_MASK_LINE); // Line Hook

	m_ms->m_robot = robot;
	m_ms->m_msg = getTextLineMsg();

	// Error happens
	if (m_ms->CompileBuffer((unsigned char *)codes.GetBuffer(0), codes.GetLength()) == false){
		m_dbg->GetErrorMsg(m_errorMsg);

		// Clean up
		delete m_dbg;
		delete m_ms;

		return -1; // FAIL
	}

	DWORD thid;
	HANDLE hThread = CreateThread(NULL, 0, workerThread, this, 0, &thid);
	if (!hThread)
		return -1;

	return 0; // SUCCESS
}

char *CEvaluate::getEvalMsg()
{	
	return m_errorMsg;
}

char *CEvaluate::getTextLineMsg()
{
	return m_textLineMsg;
}

void CEvaluate::setQuitFlag()
{
	m_dbg->Abort();
}