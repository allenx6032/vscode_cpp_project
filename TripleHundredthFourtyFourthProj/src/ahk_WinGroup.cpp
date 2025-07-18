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
#include "ahk_WinGroup.h"
#include "ahk_window.h" // for several lower level window functions
#include "ahk_globaldata.h" // for DoWinDelay
#include "ahk_application.h" // for DoWinDelay's MsgSleep()

// Define static members data:
WinGroup *WinGroup::sGroupLastUsed = NULL;
HWND *WinGroup::sAlreadyVisited = NULL;
int WinGroup::sAlreadyVisitedCount = 0;


ResultType WinGroup::AddWindow(LPTSTR aTitle, LPTSTR aText, LPTSTR aExcludeTitle, LPTSTR aExcludeText)
// Caller should ensure that at least one param isn't NULL/blank.
// GroupActivate will tell its caller to jump to aJumpToLabel if a WindowSpec isn't found.
// This function is not thread-safe because it adds an entry to the list of window specs.
// In addition, if this function is being called by one thread while another thread is calling IsMember(),
// the thread-safety notes in IsMember() apply.
{
	// v1.0.41: If a window group can ever be deleted (or its window specs), that might defeat the
	// thread-safety of WinExist/WinActive.
	// v1.0.36.05: If all four window parameters are blank, allow it to be added but provide
	// a non-blank ExcludeTitle so that the window-finding routines won't see it as the
	// "last found window".  99.99% of the time, it is undesirable to have Program Manager
	// in a window group of any kind, so that is used as the placeholder:
	if (!(*aTitle || *aText || *aExcludeTitle || *aExcludeText))
		aExcludeTitle = _T("Program Manager");

	// Though the documentation is clear on this, some users will still probably execute
	// each GroupAdd statement more than once.  Thus, to prevent more and more memory
	// from being allocated for duplicates, do not add the window specification if it
	// already exists in the group:
	if (mFirstWindow) // Traverse the circular linked-list to look for a match.
		for (WindowSpec *win = mFirstWindow
			; win != NULL; win = (win->mNextWindow == mFirstWindow) ? NULL : win->mNextWindow)
			if (!_tcscmp(win->mTitle, aTitle) && !_tcscmp(win->mText, aText) // All are case sensitive.
				&& !_tcscmp(win->mExcludeTitle, aExcludeTitle) && !_tcscmp(win->mExcludeText, aExcludeText))
				return OK;

	// SimpleHeap::Malloc() will set these new vars to the constant empty string if their
	// corresponding params are blank:
	LPTSTR new_title, new_text, new_exclude_title, new_exclude_text;
	if (!(new_title = SimpleHeap::Malloc(aTitle))) return FAIL; // It already displayed the error for us.
	if (!(new_text = SimpleHeap::Malloc(aText)))return FAIL;
	if (!(new_exclude_title = SimpleHeap::Malloc(aExcludeTitle))) return FAIL;
	if (!(new_exclude_text = SimpleHeap::Malloc(aExcludeText)))   return FAIL;

	// The precise method by which the follows steps are done should be thread-safe even if
	// some other thread calls IsMember() in the middle of the operation.  But any changes
	// must be carefully reviewed:
	WindowSpec *the_new_win = new WindowSpec(new_title, new_text, new_exclude_title, new_exclude_text);
	if (the_new_win == NULL)
		return g_script.ScriptError(ERR_OUTOFMEM);
	if (mFirstWindow == NULL)
		mFirstWindow = the_new_win;
	else
		mLastWindow->mNextWindow = the_new_win; // Formerly it pointed to First, so nothing is lost here.
	// This must be done after the above:
	mLastWindow = the_new_win;
	// Make it circular: Last always points to First.  It's okay if it points to itself:
	mLastWindow->mNextWindow = mFirstWindow;
	++mWindowCount;
	return OK;
}



ResultType WinGroup::ActUponAll(ActionTypeType aActionType, int aTimeToWaitForClose)
{
	if (IsEmpty())
		return OK;  // OK since this is the expected behavior in this case.
	// Don't need to call Update() in this case.
	WindowSearch ws;
	ws.mFirstWinSpec = mFirstWindow; // Act upon all windows that match any WindowSpec in the group.
	ws.mActionType = aActionType;    // Set the type of action to be performed on each window.
	ws.mTimeToWaitForClose = aTimeToWaitForClose;  // Only relevant for WinClose and WinKill.
	EnumWindows(EnumParentActUponAll, (LPARAM)&ws);
	if (ws.mFoundParent) // It acted upon least one window.
		DoWinDelay;
	return OK;
}



ResultType WinGroup::CloseAndGoToNext(bool aStartWithMostRecent)
// If the foreground window is a member of this group, close it and activate
// the next member.
{
	if (IsEmpty())
		return OK;  // OK since this is the expected behavior in this case.
	// Otherwise:
	// Don't call Update(), let (De)Activate() do that.

	HWND fore_win = GetForegroundWindow();
	// Even if it's NULL, don't return since the legacy behavior is to continue on to the final part below.

	WindowSpec *win_spec = IsMember(fore_win, *g);
	if (   (mIsModeActivate && win_spec) || (!mIsModeActivate && !win_spec)   )
	{
		// If the user is using a GroupActivate hotkey, we don't want to close
		// the foreground window if it's not a member of the group.  Conversely,
		// if the user is using GroupDeactivate, we don't want to close a
		// member of the group.  This precaution helps prevent accidental closing
		// of windows that suddenly pop up to the foreground just as you've
		// realized (too late) that you pressed the "close" hotkey.
		// MS Visual Studio/C++ gets messed up when it is directly sent a WM_CLOSE,
		// probably because the wrong window (it has two mains) is being sent the close.
		// But since that's the only app I've ever found that doesn't work right,
		// it seems best not to change our close method just for it because sending
		// keys is a fairly high overhead operation, and not without some risk due to
		// not knowing exactly what keys the user may have physically held down.
		// Also, we'd have to make this module dependent on the keyboard module,
		// which would be another drawback.
		// Try to wait for it to close, otherwise the same window may be activated
		// again before it has been destroyed, defeating the purpose of the
		// "ActivateNext" part of this function's job:
		// SendKeys("!{F4}");
		if (fore_win)
		{
			WinClose(fore_win, 500);
			DoWinDelay;
		}
		if (mIsModeActivate)
		{
			HWND new_fore_win = GetForegroundWindow();
			if (new_fore_win != fore_win && IsMember(new_fore_win, *g))
			{
				// new_fore_win appears to have been activated as a result of closing fore_win.
				// Since new_fore_win belongs to the group, avoid activating some other window
				// even if !aStartWithMostRecent, since it would be a bit jarring.  Historical
				// behaviour was to leave it active only if it belonged to the same *spec*,
				// which doesn't make a lot of sense.
				return OK;
			}
		}
	}
	//else do the activation below anyway, even though no close was done.
	return mIsModeActivate ? Activate(aStartWithMostRecent) : Deactivate(aStartWithMostRecent);
}



ResultType WinGroup::Activate(bool aStartWithMostRecent, Label **aJumpToLabel)
{
	if (aJumpToLabel) // Initialize early in case of early return.
		*aJumpToLabel = NULL;
	if (IsEmpty())
		return OK;  // OK since this is the expected behavior in this case.
	// Otherwise:
	if (!Update(true)) // Update our private member vars.
		return FAIL;  // It already displayed the error for us.
	HWND active_window = GetForegroundWindow();
	bool group_is_active = IsMember(active_window, *g);
	if (!group_is_active) // Foreground window is not a member of this group.
	{
		// Reset the list of visited windows:
		sAlreadyVisitedCount = 0;
	}

	// Find the first/last unvisited window that matches any spec in the group.
	WindowSearch ws;
	// This next line is whether to find last or first match.  We always find the oldest
	// (bottommost) match except when the user has specifically asked to start with the
	// most recent AND the group isn't currently active (i.e. we're starting fresh).
	// This allows the order to be the same each time the sequence repeats, instead of
	// reversing due to the nature of the window z-order.
	ws.mFindLastMatch = !aStartWithMostRecent || group_is_active;
	ws.mAlreadyVisited = sAlreadyVisited;
	ws.mAlreadyVisitedCount = sAlreadyVisitedCount;
	ws.SetCriteria(*g, *this);

	for (bool retry_needed = sAlreadyVisitedCount > 0; ; retry_needed = false)
	{
		if (group_is_active) // Foreground window is a member of this group.
		{
			// Ensure the active window is excluded from both attempts to find a match.
			MarkAsVisited(active_window);
		}
		ws.mAlreadyVisitedCount = sAlreadyVisitedCount;
		EnumWindows(EnumParentFind, (LPARAM)&ws);
		if (ws.mFoundParent)
		{
			// We found a window to activate, so we're done.
			SetForegroundWindowEx(ws.mFoundParent);
			MarkAsVisited(ws.mFoundParent);
			DoWinDelay;
			return OK;
		}
		if (!retry_needed)
			break;
		// Try again without excluding previously visited windows.
		sAlreadyVisitedCount = 0;
	}
	// No window was found to activate.
	if (aJumpToLabel && mJumpToLabel)
	{
		// Caller asked us to return in this case, so that it can
		// use this value to execute a user-specified Gosub:
		*aJumpToLabel = mJumpToLabel;  // Set output param for the caller.
	}
	return FAIL; // Let GroupActivate set ErrorLevel to indicate what happened.
}



ResultType WinGroup::Deactivate(bool aStartWithMostRecent)
{
	if (IsEmpty())
		return OK;  // OK since this is the expected behavior in this case.
	// Otherwise:
	if (!Update(false)) // Update our private member vars.
		return FAIL;  // It already displayed the error for us.

	HWND active_window = GetForegroundWindow();
	if (IsMember(active_window, *g))
		sAlreadyVisitedCount = 0;

	// Activate the next unvisited non-member:
	WindowSearch ws;
	ws.mFindLastMatch = !aStartWithMostRecent || sAlreadyVisitedCount;
	ws.mAlreadyVisited = sAlreadyVisited;
	ws.mAlreadyVisitedCount = sAlreadyVisitedCount;
	ws.mFirstWinSpec = mFirstWindow;

	EnumWindows(EnumParentFindAnyExcept, (LPARAM)&ws);

	if (ws.mFoundParent)
	{
		// If the window we're about to activate owns other visible parent windows, it can
		// never truly be activated because it must always be below them in the z-order.
		// Thus, instead of activating it, activate the first (and usually the only?)
		// visible window that it owns.  Doing this makes things nicer for some apps that
		// have a pair of main windows, such as MS Visual Studio (and probably many more),
		// because it avoids activating such apps twice in a row as the user progresses
		// through the sequence:
		HWND first_visible_owned = WindowOwnsOthers(ws.mFoundParent);
		if (first_visible_owned)
		{
			MarkAsVisited(ws.mFoundParent);  // Must mark owner as well as the owned window.
			// Activate the owned window instead of the owner because it usually
			// (probably always, given the comments above) is the real main window:
			ws.mFoundParent = first_visible_owned;
		}
		SetForegroundWindowEx(ws.mFoundParent);
		// Probably best to do this before WinDelay in case another hotkey fires during the delay:
		MarkAsVisited(ws.mFoundParent);
		DoWinDelay;
	}
	else // No window was found to activate (they have all been visited).
	{
		if (sAlreadyVisitedCount)
		{
			bool wrap_around = (sAlreadyVisitedCount > 1);
			sAlreadyVisitedCount = 0;
			if (wrap_around)
			{
				// The user pressed a hotkey to do something, yet nothing has happened yet.
				// We want something to happen every time if there's a qualifying
				// "something" that we can do.  And in this case there is: we can start
				// over again through the list, excluding the foreground window (which
				// the user has already had a chance to review):
				MarkAsVisited(active_window);
				// Make a recursive call to self.  This can't result in an infinite
				// recursion (stack fault) because the called layer will only
				// recurse a second time if sAlreadyVisitedCount > 1, which is
				// impossible with the current logic:
				Deactivate(false); // Seems best to ignore aStartWithMostRecent in this case?
			}
		}
	}
	// Even if a window wasn't found, we've done our job so return OK:
	return OK;
}



inline ResultType WinGroup::Update(bool aIsModeActivate)
{
	mIsModeActivate = aIsModeActivate;
	if (sGroupLastUsed != this)
	{
		sGroupLastUsed = this;
		sAlreadyVisitedCount = 0; // Since it's a new group, reset the array to start fresh.
	}
	if (!sAlreadyVisited) // Allocate the array on first use.
		// Getting it from SimpleHeap reduces overhead for the avg. case (i.e. the first
		// block of SimpleHeap is usually never fully used, and this array won't even
		// be allocated for short scripts that don't even using window groups.
		if (   !(sAlreadyVisited = (HWND *)SimpleHeap::Malloc(MAX_ALREADY_VISITED * sizeof(HWND)))   )
			return FAIL;  // It already displayed the error for us.
	return OK;
}



WindowSpec *WinGroup::IsMember(HWND aWnd, global_struct &aSettings)
// Thread-safety: This function is thread-safe even when the main thread happens to be calling AddWindow()
// and changing the linked list while it's being traversed here by the hook thread.  However, any subsequent
// changes to this function or AddWindow() must be carefully reviewed.
// Although our caller may be a WindowSearch method, and thus we might make
// a recursive call back to that same method, things have been reviewed to ensure that
// thread-safety is maintained, even if the calling thread is the hook.
{
	if (!aWnd)
		return NULL;  // Some callers on this.
	WindowSearch ws;
	ws.SetCandidate(aWnd);
	for (WindowSpec *win = mFirstWindow; win != NULL;)  // v1.0.41: "win != NULL" was added for thread-safety.
	{
		if (ws.SetCriteria(aSettings, win->mTitle, win->mText, win->mExcludeTitle, win->mExcludeText) && ws.IsMatch())
			return win;
		// Otherwise, no match, so go onto the next one:
		win = win->mNextWindow;
		if (!win || win == mFirstWindow) // v1.0.41: The check of !win was added for thread-safety.
			// We've made one full circuit of the circular linked list,
			// discovering that the foreground window isn't a member
			// of the group:
			break;
	}
	return NULL;  // Because it would have returned already if a match was found.
}


/////////////////////////////////////////////////////////////////////////


BOOL CALLBACK EnumParentFindAnyExcept(HWND aWnd, LPARAM lParam)
// Find the first parent window that doesn't match any of the WindowSpecs in
// the linked list, and that hasn't already been visited.
{
	// Since the following two sections apply only to GroupDeactivate (since that's our only caller),
	// they both seem okay even in light of the ahk_group method.

	if (!IsWindowVisible(aWnd) || IsWindowCloaked(aWnd))
		// Skip these because we always want them to stay invisible, regardless
		// of the setting for g->DetectHiddenWindows:
		return TRUE;

	// UPDATE: Because the window of class Shell_TrayWnd (the taskbar) is also always-on-top,
	// the below prevents it from ever being activated too, which is almost always desirable.
	// However, this prevents the addition of WS_DISABLED as an extra criteria for skipping
	// a window.  Maybe that's best for backward compatibility anyway.
	// Skip always-on-top windows, such as SplashText, because probably shouldn't
	// be activated (especially in this mode, which is often used to visit the user's
	// "non-favorite" windows).  In addition, they're already visible so the user already
	// knows about them, so there's no need to have them presented for review.
	DWORD ex_style = GetWindowLong(aWnd, GWL_EXSTYLE);
	if (ex_style & WS_EX_TOPMOST)
		return TRUE;

	// Skip "Program Manager" (the Desktop) because the user probably doesn't want it
	// activated (although that would serve the purpose of deactivating any other window),
	// and for backward-compatibility.  For consistency, also skip the "ahk_class WorkerW"
	// window which is the Desktop on Windows 7 and later (but is hidden on Windows 7).
	// Since the class name is ambiguous, also check for WS_EX_TOOLWINDOW.
	TCHAR class_name[9];
	if (   GetClassName(aWnd, class_name, _countof(class_name))
		&& (!_tcsicmp(class_name, _T("Progman"))
			|| (ex_style & WS_EX_TOOLWINDOW) && !_tcsicmp(class_name, _T("WorkerW")))   )
		return TRUE;

	WindowSearch &ws = *(WindowSearch *)lParam;  // For performance and convenience.
	ws.SetCandidate(aWnd);

	// Check this window's attributes against each set of criteria present in the group.  If
	// it's a match for any set of criteria, it's a member of the group and thus should be
	// excluded since we want only NON-members:
	for (WindowSpec *win = ws.mFirstWinSpec;;)
	{
		// For each window in the linked list, check if aWnd is a match for it:
		if (ws.SetCriteria(*g, win->mTitle, win->mText, win->mExcludeTitle, win->mExcludeText) && ws.IsMatch(true))
			// Match found, so aWnd is a member of the group. But we want to find non-members only,
			// so keep searching:
			return TRUE;
		// Otherwise, no match, but keep checking until aWnd has been compared against
		// all the WindowSpecs in the group:
		win = win->mNextWindow;
		if (win == ws.mFirstWinSpec)
		{
			// We've made one full circuit of the circular linked list without
			// finding a match.  So aWnd is the one we're looking for unless
			// it's in the list of exceptions:
			for (int i = 0; i < ws.mAlreadyVisitedCount; ++i)
				if (aWnd == ws.mAlreadyVisited[i])
					return TRUE; // It's an exception, so keep searching.
			// Otherwise, this window meets the criteria, so return it to the caller and
			// stop the enumeration.  UPDATE: Rather than stopping the enumeration,
			// continue on through all windows so that the last match is found.
			// That makes things nicer because when the sequence wraps around to the
			// beginning, the windows will occur in the same order that they did
			// the first time, rather than going backwards through the sequence
			// (which is counterintuitive for the user):
			ws.mFoundParent = aWnd; // No need to increment ws.mFoundCount in this case.
			return ws.mFindLastMatch;
		}
	} // The loop above is infinite unless a "return" is encountered inside.
}



BOOL CALLBACK EnumParentActUponAll(HWND aWnd, LPARAM lParam)
// Caller must have ensured that lParam isn't NULL and that it contains a non-NULL mFirstWinSpec.
{
	WindowSearch &ws = *(WindowSearch *)lParam;  // For performance and convenience.

	// Skip windows the command isn't supposed to detect.  ACT_WINSHOW is exempt because
	// hidden windows are always detected by the WinShow command:
	if (!(ws.mActionType == ACT_WINSHOW || g->DetectWindow(aWnd)))
		return TRUE;

	int nCmdShow;
	ws.SetCandidate(aWnd);

	for (WindowSpec *win = ws.mFirstWinSpec;;)
	{
		// For each window in the linked list, check if aWnd is a match for it:
		if (ws.SetCriteria(*g, win->mTitle, win->mText, win->mExcludeTitle, win->mExcludeText) && ws.IsMatch())
		{
			// Match found, so aWnd is a member of the group.  In addition, IsMatch() has set
			// the value of ws.mFoundParent to tell our caller that at least one window was acted upon.
			// See Line::PerformShowWindow() for comments about the following section.
			nCmdShow = SW_NONE; // Set default each time.
			switch (ws.mActionType)
			{
			case ACT_WINCLOSE:
			case ACT_WINKILL:
				// mTimeToWaitForClose is not done in a very efficient way here: to keep code size
				// in check, it closes each window individually rather than sending WM_CLOSE to all
				// the windows simultaneously and then waiting until all have vanished:
				WinClose(aWnd, ws.mTimeToWaitForClose, ws.mActionType == ACT_WINKILL); // DoWinDelay is done by our caller.
				return TRUE; // All done with the current window, so fetch the next one.

			case ACT_WINMINIMIZE:
				if (IsWindowHung(aWnd))
					nCmdShow = SW_FORCEMINIMIZE;
				else
					nCmdShow = SW_MINIMIZE;
				break;

			case ACT_WINMAXIMIZE: if (!IsWindowHung(aWnd)) nCmdShow = SW_MAXIMIZE; break;
			case ACT_WINRESTORE:  if (!IsWindowHung(aWnd)) nCmdShow = SW_RESTORE;  break;
			case ACT_WINHIDE: nCmdShow = SW_HIDE; break;
			case ACT_WINSHOW: nCmdShow = SW_SHOW; break;
			}

			if (nCmdShow != SW_NONE)
				ShowWindow(aWnd, nCmdShow);
				// DoWinDelay is not done here because our caller will do it once only, which
				// seems best when there are a lot of windows being acted upon here.

			// Now that this matching window has been acted upon (or avoided due to being hung),
			// continue the enumeration to get the next candidate window:
			return TRUE;
		}
		// Otherwise, no match, keep checking until aWnd has been compared against all the WindowSpecs in the group:
		win = win->mNextWindow;
		if (win == ws.mFirstWinSpec)
			// We've made one full circuit of the circular linked list without
			// finding a match, so aWnd is not a member of the group and
			// should not be closed.
			return TRUE; // Continue the enumeration.
	}
}
