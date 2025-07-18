﻿/*
AutoHotkey

Copyright 2003-2009 Chris Mallett (support@autohotkey.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
*/

#include "ahk_stdafx.h" // pre-compiled headers
#include "ahk_globaldata.h" // for access to many global vars
#include "ahk_application.h" // for MsgSleep()
#include "ahk_window.h" // For MsgBox() & SetForegroundLockTimeout()
#include "ahk_TextIO.h"

// General note:
// The use of Sleep() should be avoided *anywhere* in the code.  Instead, call MsgSleep().
// The reason for this is that if the keyboard or mouse hook is installed, a straight call
// to Sleep() will cause user keystrokes & mouse events to lag because the message pump
// (GetMessage() or PeekMessage()) is the only means by which events are ever sent to the
// hook functions.


ResultType InitForExecution();
ResultType ParseCmdLineArgs(LPTSTR &script_filespec);
ResultType CheckPriorInstance();
int MainExecuteScript(bool aMsgSleep = true);
ResultType SetLegacyArgVars(LPTSTR *aArg, int aArgCount);


// Performs any initialization that should be done before LoadFromFile().
void EarlyAppInit()
{
	// v1.1.22+: This is done unconditionally, on startup, so that any attempts to read a drive
	// that has no media (and possibly other errors) won't cause the system to display an error
	// dialog that the script can't suppress.  This is known to affect floppy drives and some
	// but not all CD/DVD drives.  MSDN says: "Best practice is that all applications call the
	// process-wide SetErrorMode function with a parameter of SEM_FAILCRITICALERRORS at startup."
	// Note that in previous versions, this was done by the Drive/DriveGet commands and not
	// reverted afterward, so it affected all subsequent commands.
	SetErrorMode(SEM_FAILCRITICALERRORS);

	// g_WorkingDir is used by various functions but might currently only be used at runtime.
	// g_WorkingDirOrig needs to be initialized before LoadFromFile() is called.
	UpdateWorkingDir();
	g_WorkingDirOrig = SimpleHeap::Malloc(const_cast<LPTSTR>(g_WorkingDir.GetString()));

	// Initialize early since g is used in many places, including some at load-time.
	global_init(*g);

	// Initialize g_default for any threads that start before AutoExecSection() returns, or in
	// case it never returns.  This is done early for maintainability, though it currently might
	// not be needed until the script is about to execute.
	CopyMemory(&g_default, g, sizeof(global_struct));
}


int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
{
	g_hInstance = hInstance;

	EarlyAppInit();

	LPTSTR script_filespec; // Script path as originally specified, or NULL if omitted/defaulted.
	if (!ParseCmdLineArgs(script_filespec))
		return CRITICAL_ERROR;

	UINT load_result = g_script.LoadFromFile(script_filespec);
	if (load_result == LOADING_FAILED) // Error during load (was already displayed by the function call).
		return CRITICAL_ERROR;  // Should return this value because PostQuitMessage() also uses it.
	if (!load_result) // LoadFromFile() relies upon us to do this check.  No script was loaded or we're in /iLib mode, so nothing more to do.
		return 0;

	switch (CheckPriorInstance())
	{
	case EARLY_EXIT: return 0;
	case FAIL: return CRITICAL_ERROR;
	}

	if (!InitForExecution())
		return CRITICAL_ERROR;

	return MainExecuteScript();
}


ResultType ParseCmdLineArgs(LPTSTR &script_filespec)
{
	script_filespec = NULL; // Set default as "unspecified/omitted".
#ifndef AUTOHOTKEYSC
	// Is this a compiled script?
	if (FindResource(NULL, SCRIPT_RESOURCE_NAME, RT_RCDATA))
		script_filespec = SCRIPT_RESOURCE_SPEC;
#endif

	// The problem of some command line parameters such as /r being "reserved" is a design flaw (one that
	// can't be fixed without breaking existing scripts).  Fortunately, I think it affects only compiled
	// scripts because running a script via AutoHotkey.exe should avoid treating anything after the
	// filename as switches. This flaw probably occurred because when this part of the program was designed,
	// there was no plan to have compiled scripts.
	// 
	// Examine command line args.  Rules:
	// Any special flags (e.g. /force and /restart) must appear prior to the script filespec.
	// The script filespec (if present) must be the first non-backslash arg.
	// All args that appear after the filespec are considered to be parameters for the script
	// and will be added as variables %1% %2% etc.
	// The above rules effectively make it impossible to autostart AutoHotkey.ini with parameters
	// unless the filename is explicitly given (shouldn't be an issue for 99.9% of people).
	int i;
	for (i = 1; i < __argc; ++i) // Start at 1 because 0 contains the program name.
	{
		LPTSTR param = __targv[i]; // For performance and convenience.
		// Insist that switches be an exact match for the allowed values to cut down on ambiguity.
		// For example, if the user runs "CompiledScript.exe /find", we want /find to be considered
		// an input parameter for the script rather than a switch:
		if (!_tcsicmp(param, _T("/R")) || !_tcsicmp(param, _T("/restart")))
			g_script.mIsRestart = true;
		else if (!_tcsicmp(param, _T("/F")) || !_tcsicmp(param, _T("/force")))
			g_ForceLaunch = true;
		else if (!_tcsnicmp(param, _T("/ErrorStdOut"), 12))
			g_script.SetErrorStdOut(param[12] == '=' ? param + 13 : NULL);
#ifndef AUTOHOTKEYSC // i.e. the following switch is recognized only by AutoHotkey.exe (especially since recognizing new switches in compiled scripts can break them, unlike AutoHotkey.exe).
		else if (!_tcsicmp(param, _T("/script")))
			script_filespec = NULL; // Override compiled script mode, otherwise no effect.
		else if (script_filespec) // Compiled script mode.
			break;
		else if (!_tcsicmp(param, _T("/include")))
		{
			++i; // Consume the next parameter too, because it's associated with this one.
			if (i >= __argc // Missing the expected filename parameter.
				|| g_script.mCmdLineInclude) // Only one is supported, so abort if there's more.
				return FAIL;
			g_script.mCmdLineInclude = __targv[i];
		}
		else if (!_tcsicmp(param, _T("/iLib"))) // v1.0.47: Build an include-file so that ahk2exe can include library functions called by the script.
		{
			++i; // Consume the next parameter too, because it's associated with this one.
			if (i >= __argc) // Missing the expected filename parameter.
				return FAIL;
			// For performance and simplicity, open/create the file unconditionally and keep it open until exit.
			g_script.mIncludeLibraryFunctionsThenExit = new TextFile;
			if (!g_script.mIncludeLibraryFunctionsThenExit->Open(__targv[i], TextStream::WRITE | TextStream::EOL_CRLF | TextStream::BOM_UTF8, CP_UTF8)) // Can't open the temp file.
				return FAIL;
		}
		else if (!_tcsnicmp(param, _T("/CP"), 3)) // /CPnnn
		{
			// Default codepage for the script file, NOT the default for commands used by it.
			g_DefaultScriptCodepage = ATOU(param + 3);
		}
#endif
#ifdef CONFIG_DEBUGGER
		// Allow a debug session to be initiated by command-line.
		else if (!_tcsnicmp(param, _T("/Debug"), 6) && (param[6] == '\0' || param[6] == '='))
		{
			if (param[6] == '=')
			{
				param += 7;

				LPTSTR c = _tcsrchr(param, ':');

				if (c)
				{
					StringTCharToChar(param, g_DebuggerHost, (int)(c-param));
					StringTCharToChar(c + 1, g_DebuggerPort);
				}
				else
				{
					StringTCharToChar(param, g_DebuggerHost);
					g_DebuggerPort = "9000";
				}
			}
			else
			{
				g_DebuggerHost = "localhost";
				g_DebuggerPort = "9000";
			}
			// The actual debug session is initiated after the script is successfully parsed.
		}
#endif
		else // since this is not a recognized switch, the end of the [Switches] section has been reached (by design).
		{
#ifndef AUTOHOTKEYSC
			script_filespec = param;  // The first unrecognized switch must be the script filespec, by design.
			++i; // Omit this from the "args" array.
#endif
			break; // No more switches allowed after this point.
		}
	}
	
	// All remaining args are considered to be input parameters for the script.
	if (!SetLegacyArgVars(__targv + i, __argc - i))
		return FAIL;

	if (Var *var = g_script.FindOrAddVar(_T("A_Args"), 6, VAR_DECLARE_SUPER_GLOBAL))
	{
		// Store the remaining args in an array and assign it to "Args".
		// If there are no args, assign an empty array.
		Object *args = Object::CreateFromArgV(__targv + i, __argc - i);
		if (!args)
			return FAIL;  // Realistically should never happen.
		var->AssignSkipAddRef(args);
	}
	else
		return FAIL;

	// Set up the basics of the script:
	return g_script.Init(script_filespec);
}


ResultType CheckPriorInstance()
{
	// Unless explicitly set to be non-SingleInstance via SINGLE_INSTANCE_OFF or a special kind of
	// SingleInstance such as SINGLE_INSTANCE_REPLACE and SINGLE_INSTANCE_IGNORE, persistent scripts
	// and those that contain hotkeys/hotstrings are automatically SINGLE_INSTANCE_PROMPT as of v1.0.16:
	if (g_AllowOnlyOneInstance == ALLOW_MULTI_INSTANCE && IS_PERSISTENT)
		g_AllowOnlyOneInstance = SINGLE_INSTANCE_PROMPT;

	HWND w_existing = NULL;
	UserMessages reason_to_close_prior = (UserMessages)0;
	if (g_AllowOnlyOneInstance && g_AllowOnlyOneInstance != SINGLE_INSTANCE_OFF && !g_script.mIsRestart && !g_ForceLaunch)
	{
		if (w_existing = FindWindow(WINDOW_CLASS_MAIN, g_script.mMainWindowTitle))
		{
			if (g_AllowOnlyOneInstance == SINGLE_INSTANCE_IGNORE)
				return EARLY_EXIT;
			if (g_AllowOnlyOneInstance != SINGLE_INSTANCE_REPLACE)
				if (MsgBox(_T("An older instance of this script is already running.  Replace it with this")
					_T(" instance?\nNote: To avoid this message, see #SingleInstance in the help file.")
					, MB_YESNO, g_script.mFileName) == IDNO)
					return EARLY_EXIT;
			// Otherwise:
			reason_to_close_prior = AHK_EXIT_BY_SINGLEINSTANCE;
		}
	}
	if (!reason_to_close_prior && g_script.mIsRestart)
		if (w_existing = FindWindow(WINDOW_CLASS_MAIN, g_script.mMainWindowTitle))
			reason_to_close_prior = AHK_EXIT_BY_RELOAD;
	if (reason_to_close_prior)
	{
		// Now that the script has been validated and is ready to run, close the prior instance.
		// We wait until now to do this so that the prior instance's "restart" hotkey will still
		// be available to use again after the user has fixed the script.  UPDATE: We now inform
		// the prior instance of why it is being asked to close so that it can make that reason
		// available to the OnExit subroutine via a built-in variable:
		ASK_INSTANCE_TO_CLOSE(w_existing, reason_to_close_prior);
		//PostMessage(w_existing, WM_CLOSE, 0, 0);

		// Wait for it to close before we continue, so that it will deinstall any
		// hooks and unregister any hotkeys it has:
		int interval_count;
		for (interval_count = 0; ; ++interval_count)
		{
			Sleep(20);  // No need to use MsgSleep() in this case.
			if (!IsWindow(w_existing))
				break;  // done waiting.
			if (interval_count == 100)
			{
				// This can happen if the previous instance has an OnExit subroutine that takes a long
				// time to finish, or if it's waiting for a network drive to timeout or some other
				// operation in which it's thread is occupied.
				if (MsgBox(_T("Could not close the previous instance of this script.  Keep waiting?"), 4) == IDNO)
					return FAIL;
				interval_count = 0;
			}
		}
		// Give it a small amount of additional time to completely terminate, even though
		// its main window has already been destroyed:
		Sleep(100);
	}
	return OK;
}


ResultType InitForExecution()
{
	// Create all our windows and the tray icon.  This is done after all other chances
	// to return early due to an error have passed, above.
	if (!g_script.CreateWindows())
		return FAIL;

	SetForegroundLockTimeout();

	if (g_MaxHistoryKeys && (g_KeyHistory = (KeyHistoryItem *)malloc(g_MaxHistoryKeys * sizeof(KeyHistoryItem))))
		ZeroMemory(g_KeyHistory, g_MaxHistoryKeys * sizeof(KeyHistoryItem)); // Must be zeroed.
	//else leave it NULL as it was initialized in globaldata.

	// MSDN: "Windows XP: If a manifest is used, InitCommonControlsEx is not required."
	// Therefore, in case it's a high overhead call, it's not done on XP or later:
	if (!g_os.IsWinXPorLater())
	{
		// Since InitCommonControls() is apparently incapable of initializing DateTime and MonthCal
		// controls, InitCommonControlsEx() must be called.
		INITCOMMONCONTROLSEX icce;
		icce.dwSize = sizeof(INITCOMMONCONTROLSEX);
		icce.dwICC = ICC_WIN95_CLASSES | ICC_DATE_CLASSES; // ICC_WIN95_CLASSES is equivalent to calling InitCommonControls().
		InitCommonControlsEx(&icce);
	}

	// From this point on, any errors that are reported should not indicate that they will exit the program.
	// Various functions also use this to detect that they are being called by the script at runtime.
	g_script.mIsReadyToExecute = true;

	return OK;
}


int MainExecuteScript(bool aMsgSleep)
{
#ifdef CONFIG_DEBUGGER
	// Initiate debug session now if applicable.
	if (!g_DebuggerHost.IsEmpty() && g_Debugger.Connect(g_DebuggerHost, g_DebuggerPort) == DEBUGGER_E_OK)
	{
		g_Debugger.Break();
	}
#endif

	// Activate the hotkeys, hotstrings, and any hooks that are required prior to executing the
	// top part (the auto-execute part) of the script so that they will be in effect even if the
	// top part is something that's very involved and requires user interaction:
	Hotkey::ManifestAllHotkeysHotstringsHooks(); // We want these active now in case auto-execute never returns (e.g. loop)

#ifndef _DEBUG
	__try
#endif
	{
		// Run the auto-execute part at the top of the script:
		auto exec_result = g_script.AutoExecSection();
		// REMEMBER: The call above will never return if one of the following happens:
		// 1) The AutoExec section never finishes (e.g. infinite loop).
		// 2) The AutoExec function uses the Exit or ExitApp command to terminate the script.
		// 3) The script isn't persistent and its last line is reached (in which case an Exit is implicit).
		// However, #ifdef CONFIG_DLL, the call will return after Exit is used (but not explicit ExitApp).

#ifdef CONFIG_DLL
		if (!aMsgSleep)
			return exec_result ? 0 : CRITICAL_ERROR;
#endif
		if (IS_PERSISTENT)
		{
			// Call it in this special mode to kick off the main event loop.
			// Be sure to pass something >0 for the first param or it will
			// return (and we never want this to return):
			MsgSleep(SLEEP_INTERVAL, WAIT_FOR_MESSAGES);
		}
		else
		{
			// The script isn't persistent, so call OnExit handlers and terminate.
			g_script.ExitApp(exec_result == FAIL ? EXIT_ERROR : EXIT_EXIT);
		}
	}
#ifndef _DEBUG
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		LPCTSTR msg;
		auto ecode = GetExceptionCode();
		switch (ecode)
		{
		// Having specific messages for the most common exceptions seems worth the added code size.
		// The term "stack overflow" is not used because it is probably less easily understood by
		// the average user, and is not useful as a search term due to stackoverflow.com.
		case EXCEPTION_STACK_OVERFLOW: msg = _T("Function recursion limit exceeded."); break;
		case EXCEPTION_ACCESS_VIOLATION: msg = _T("Invalid memory read/write."); break;
		default: msg = _T("System exception 0x%X."); break;
		}
		TCHAR buf[127];
		sntprintf(buf, _countof(buf), msg, ecode);
		g_script.CriticalError(buf);
		return ecode;
	}
#endif 
	return 0;
}


ResultType SetLegacyArgVars(LPTSTR *aArg, int aArgCount)
{
	TCHAR var_name[32]; // Small size since only numbers will be used (e.g. %1%, %2%).
	Var *var;

	// All remaining args are considered to be input parameters for the script.
	for (int i = 0; i < aArgCount; ++i)
	{
		if (   !(var = g_script.FindOrAddVar(var_name, _stprintf(var_name, _T("%d"), i + 1)))   )
			return FAIL;  // Realistically should never happen.
		var->Assign(aArg[i]);
	}

	// Like AutoIt2, store the number of script parameters in the script variable %0%, even if it's zero:
	if (   !(var = g_script.FindOrAddVar(_T("0")))   )
		return FAIL;  // Realistically should never happen.
	return var->Assign(aArgCount);
}
