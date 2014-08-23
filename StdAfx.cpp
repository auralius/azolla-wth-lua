// stdafx.cpp : source file that includes just the standard includes
// Azolla.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"


// Input method manager? see ImmGetContext()
#pragma comment( lib, "Imm32.lib" )

#ifdef _DEBUG
#	pragma comment( lib, "scintilla/bin/sd_scintilla.lib" )
#else
#	pragma comment( lib, "scintilla/bin/s_scintilla.lib" )
#endif