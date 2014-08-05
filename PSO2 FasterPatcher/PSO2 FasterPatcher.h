
// PSO2 FasterPatcher.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPSO2FasterPatcherApp:
// See PSO2 FasterPatcher.cpp for the implementation of this class
//

class CPSO2FasterPatcherApp : public CWinApp
{
public:
	CPSO2FasterPatcherApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPSO2FasterPatcherApp theApp;