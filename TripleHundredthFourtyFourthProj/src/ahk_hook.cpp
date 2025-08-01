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
#include "ahk_hook.h"
#include "ahk_globaldata.h"  // for access to several global vars
#include "ahk_util.h" // for snprintfcat()
#include "ahk_window.h" // for MsgBox()
#include "ahk_application.h" // For MsgSleep().

// Declare static variables (global to only this file/module, i.e. no external linkage):
static HANDLE sKeybdMutex = NULL;
static HANDLE sMouseMutex = NULL;
#define KEYBD_MUTEX_NAME _T("AHK Keybd")
#define MOUSE_MUTEX_NAME _T("AHK Mouse")

// It's done the following way because:
// It's unclear that zero is always an invalid thread ID (not even GetWindowThreadProcessId's
// documentation gives any hint), so its safer to assume that a thread ID can be zero and yet still valid.
static HANDLE sThreadHandle = NULL;

// Whether to disguise the next up-event for lwin/rwin to suppress Start Menu.
// There is only one variable because even if multiple modifiers are pressed
// simultaneously and they do not cancel each other out, disguising one will
// have the effect of disguising all (with the exception that key-repeat can
// reset LWin/RWin after the other modifier is released, so this variable
// should not be reset until all Win keys are released).
// These are made global, rather than static inside the hook function, so that
// we can ensure they are initialized by the keyboard init function every
// time it's called (currently it can be only called once):
static bool sDisguiseNextMenu;          // Initialized by ResetHook().
static bool sUndisguisedMenuInEffect;	//

// Whether the alt-tab menu was shown by an AltTab hotkey or alt-tab was detected
// by the hook.  This might be inaccurate if the menu was displayed before the hook
// was installed or the keys weren't detected because of UIPI.  If this turns out to
// be a problem, the accuracy could be improved by additional checks with FindWindow(),
// keeping in mind that there are at least 3 different window classes to check,
// depending on OS and the "AltTabSettings" registry value.
static bool sAltTabMenuIsVisible;       // Initialized by ResetHook().

// The prefix key that's currently down (i.e. in effect).
// It's tracked this way, rather than as a count of the number of prefixes currently down, out of
// concern that such a count might accidentally wind up above zero (due to a key-up being missed somehow)
// and never come back down, thus penalizing performance until the program is restarted:
key_type *pPrefixKey;  // Initialized by ResetHook().

// Less memory overhead (space and performance) to allocate a solid block for multidimensional arrays:
// These store all the valid modifier+suffix combinations (those that result in hotkey actions) except
// those with a ModifierVK/SC.  Doing it this way should cut the CPU overhead caused by having many
// hotkeys handled by the hook, down to a fraction of what it would be otherwise.  Indeed, doing it
// this way makes the performance impact of adding many additional hotkeys of this type exactly zero
// once the program has started up and initialized.  The main alternative is a binary search on an
// array of keyboard-hook hotkeys (similar to how the mouse is done):
static HotkeyIDType *kvkm = NULL;
static HotkeyIDType *kscm = NULL;
static HotkeyIDType *hotkey_up = NULL;
static key_type *kvk = NULL;
static key_type *ksc = NULL;
// Macros for convenience in accessing the above arrays as multidimensional objects.
// When using them, be sure to consistently access the first index as ModLR (i.e. the rows)
// and the second as VK or SC (i.e. the columns):
#define Kvkm(i,j) kvkm[(i)*(VK_ARRAY_COUNT) + (j)]
#define Kscm(i,j) kscm[(i)*(SC_ARRAY_COUNT) + (j)]
#define KVKM_SIZE ((MODLR_MAX + 1)*(VK_ARRAY_COUNT))
#define KSCM_SIZE ((MODLR_MAX + 1)*(SC_ARRAY_COUNT))


// Notes about fake shift-key events (there used to be some related variables defined here,
// but they were superseded by SC_FAKE_LSHIFT and SC_FAKE_RSHIFT):
// Used to help make a workaround for the way the keyboard driver generates physical
// shift-key events to release the shift key whenever it is physically down during
// the press or release of a dual-state Numpad key. These keyboard driver generated
// shift-key events only seem to happen when Numlock is ON, the shift key is logically
// or physically down, and a dual-state numpad key is pressed or released (i.e. the shift
// key might not have been down for the press, but if it's down for the release, the driver
// will suddenly start generating shift events).  I think the purpose of these events is to
// allow the shift key to temporarily alter the state of the Numlock key for the purpose of
// sending that one key, without the shift key actually being "seen" as down while the key
// itself is sent (since some apps may have special behavior when they detect the shift key
// is down).

// Note: numlock, numpaddiv/mult/sub/add/enter are not affected by this because they have only
// a single state (i.e. they are unaffected by the state of the Numlock key).  Also, these
// driver-generated events occur at a level lower than the hook, so it doesn't matter whether
// the hook suppresses the keys involved (i.e. the shift events still happen anyway).

// So which keys are not physical even though they're non-injected?:
// 1) The shift-up that precedes a down of a dual-state numpad key (only happens when shift key is logically down).
// 2) The shift-down that precedes a pressing down (or releasing in certain very rare cases caused by the
//    exact sequence of keys received) of a key WHILE the numpad key in question is still down.
//    Although this case may seem rare, it's happened to both Robert Yaklin and myself when doing various
//    sorts of hotkeys.
// 3) The shift-up that precedes an up of a dual-state numpad key.  This only happens if the shift key is
//    logically down for any reason at this exact moment, which can be achieved via the send command.
// 4) The shift-down that follows the up of a dual-state numpad key (i.e. the driver is restoring the shift state
//    to what it was before).  This can be either immediate or "lazy".  It's lazy whenever the user had pressed
//    another key while a numpad key was being held down (i.e. case #2 above), in which case the driver waits
//    indefinitely for the user to press any other key and then immediately sneaks in the shift key-down event
//    right before it in the input stream (insertion).
// 5) Similar to #4, but if the driver needs to generate a shift-up for an unexpected Numpad-up event,
//    the restoration of the shift key will be "lazy".  This case was added in response to the below
//    example, wherein the shift key got stuck physically down (incorrectly) by the hook:
// 68  048	 	d	0.00	Num 8          	
// 6B  04E	 	d	0.09	Num +          	
// 68  048	i	d	0.00	Num 8          	
// 68  048	i	u	0.00	Num 8          	
// A0  02A	i	d	0.02	Shift          	part of the macro
// 01  000	i	d	0.03	LButton        	
// A0  02A	 	u	0.00	Shift          	driver, for the next key
// 26  048	 	u	0.00	Num 8          	
// A0  02A	 	d	0.49	Shift          	driver lazy down (but not detected as non-physical)
// 6B  04E	 	d	0.00	Num +          	


static bool sHookSyncd; // Only valid while in WaitHookIdle().

/////////////////////////////////////////////////////////////////////////////////////////////

/*
One of the main objectives of a the hooks is to minimize the amount of CPU overhead caused by every
input event being handled by the procedure.  One way this is done is to return immediately on
simple conditions that are relatively frequent (such as receiving a key that's not involved in any
hotkey combination).

Another way is to avoid API or system calls that might have a high overhead.  That's why the state of
every prefix key is tracked independently, rather than calling the WinAPI to determine if the
key is actually down at the moment of consideration.
*/

inline bool IsIgnored(ULONG_PTR aExtraInfo)
// KEY_PHYS_IGNORE events must be mostly ignored because currently there is no way for a given
// hook instance to detect if it sent the event or some other instance.  Therefore, to treat
// such events as true physical events might cause infinite loops or other side-effects in
// the instance that generated the event.  More review of this is needed if KEY_PHYS_IGNORE
// events ever need to be treated as true physical events by the instances of the hook that
// didn't originate them. UPDATE: The foregoing can now be accomplished using SendLevel.
{
	return aExtraInfo == KEY_IGNORE || aExtraInfo == KEY_PHYS_IGNORE || aExtraInfo == KEY_IGNORE_ALL_EXCEPT_MODIFIER;
}



LRESULT CALLBACK LowLevelKeybdProc(int aCode, WPARAM wParam, LPARAM lParam)
{
	if (aCode != HC_ACTION)  // MSDN docs specify that both LL keybd & mouse hook should return in this case.
		return CallNextHookEx(g_KeybdHook, aCode, wParam, lParam);

	KBDLLHOOKSTRUCT &event = *(PKBDLLHOOKSTRUCT)lParam;  // For convenience, maintainability, and possibly performance.

	// Change the event to be physical if that is indicated in its dwExtraInfo attribute.
	// This is done for cases when the hook is installed multiple times and one instance of
	// it wants to inform the others that this event should be considered physical for the
	// purpose of updating modifier and key states:
	if (event.dwExtraInfo == KEY_PHYS_IGNORE)
		event.flags &= ~LLKHF_INJECTED;
	else if (event.dwExtraInfo == KEY_BLOCK_THIS)
		return TRUE;

	// Make all keybd events physical to try to fool the system into accepting CTRL-ALT-DELETE.
	// This didn't work, which implies that Ctrl-Alt-Delete is trapped at a lower level than
	// this hook (folks have said that it's trapped in the keyboard driver itself):
	//event.flags &= ~LLKHF_INJECTED;

	// Note: Some scan codes are shared by more than one key (e.g. Numpad7 and NumpadHome).  This is why
	// the keyboard hook must be able to handle hotkeys by either their virtual key or their scan code.
	// i.e. if sc were always used in preference to vk, we wouldn't be able to distinguish between such keys.

	bool key_up = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);
	vk_type vk = (vk_type)event.vkCode;
	sc_type sc = (sc_type)event.scanCode;
	if (vk && !sc) // Might happen if another app calls keybd_event with a zero scan code.
		sc = vk_to_sc(vk);
	// MapVirtualKey() does *not* include 0xE0 in HIBYTE if key is extended.  In case it ever
	// does in the future (or if event.scanCode ever does), force sc to be an 8-bit value
	// so that it's guaranteed consistent and to ensure it won't exceed SC_MAX (which might cause
	// array indexes to be out-of-bounds).  The 9th bit is later set to 1 if the key is extended:
	sc &= 0xFF;
	// Change sc to be extended if indicated.  But avoid doing so for VK_RSHIFT, which is
	// apparently considered extended by the API when it shouldn't be.  Update: Well, it looks like
	// VK_RSHIFT really is an extended key, at least on WinXP (and probably be extension on the other
	// NT based OSes as well).  What little info I could find on the 'net about this is contradictory,
	// but it's clear that some things just don't work right if the non-extended scan code is sent.  For
	// example, the shift key will appear to get stuck down in the foreground app if the non-extended
	// scan code is sent with VK_RSHIFT key-up event:
	if ((event.flags & LLKHF_EXTENDED)) // && vk != VK_RSHIFT)
		sc |= 0x100;

	// The below must be done prior to any returns that indirectly call UpdateKeybdState() to update
	// modifier state.
	// Update: It seems best to do the below unconditionally, even if the OS is Win2k or WinXP,
	// since it seems like this translation will add value even in those cases:
	// To help ensure consistency with Windows XP and 2k, for which this hook has been primarily
	// designed and tested, translate neutral modifier keys into their left/right specific VKs,
	// since beardboy's testing shows that NT4 receives the neutral keys like Win9x does:
	switch (vk)
	{
	case VK_SHIFT:   vk = (sc == SC_RSHIFT)   ? VK_RSHIFT   : VK_LSHIFT; break;
	case VK_CONTROL: vk = (sc == SC_RCONTROL) ? VK_RCONTROL : VK_LCONTROL; break;
	case VK_MENU:    vk = (sc == SC_RALT)     ? VK_RMENU    : VK_LMENU; break;
	}

	// Now that the above has translated VK_CONTROL to VK_LCONTROL (if necessary):
	if (vk == VK_LCONTROL)
	{
		// The following helps hasten AltGr detection after script startup.  It's kept to supplement
		// LayoutHasAltGr() because that function isn't 100% reliable for the reasons described there.
		// It shouldn't be necessary to check what type of LControl event (up or down) is received, since
		// it should be impossible for any unrelated keystrokes to be received while g_HookReceiptOfLControlMeansAltGr
		// is true.  This is because all unrelated keystrokes stay buffered until the main thread calls GetMessage().
		// UPDATE for v1.0.39: Now that the hook has a dedicated thread, the above is no longer 100% certain.
		// However, I think confidence is very high that this AltGr detection method is okay as it is because:
		// 1) Hook thread has high priority, which means it generally shouldn't get backlogged with buffered keystrokes.
		// 2) When the main thread calls keybd_event(), there is strong evidence that the OS immediately preempts
		//    the main thread (or executes a SendMessage(), which is the same as preemption) and gives the next
		//    timeslice to the hook thread, which then immediately processes the incoming keystroke as long
		//    as there are no keystrokes in the queue ahead of it (see #1).
		// 3) Even if #1 and #2 fail to hold, the probability of misclassifying an LControl event seems very low.
		//    If there is ever concern, adding a call to IsIgnored() below would help weed out physical keystrokes
		//    (or those of other programs) that arrive during a vulnerable time.
		if (g_HookReceiptOfLControlMeansAltGr)
		{
			// But don't reset g_HookReceiptOfLControlMeansAltGr here to avoid timing problems where the hook
			// is installed at a time when g_HookReceiptOfLControlMeansAltGr is wrongly true because the
			// inactive hook never made it false.  Let KeyEvent() do that.
			Get_active_window_keybd_layout // Defines the variable active_window_keybd_layout for use below.
			LayoutHasAltGr(active_window_keybd_layout, CONDITION_TRUE);
			// The following must be done; otherwise, if LayoutHasAltGr hasn't yet been autodetected by the
			// time the first AltGr keystroke comes through, that keystroke would cause LControl to get stuck down
			// as seen in g_modifiersLR_physical.
			event.flags |= LLKHF_INJECTED; // Flag it as artificial for any other instances of the hook that may be running.
			event.dwExtraInfo = g_HookReceiptOfLControlMeansAltGr; // The one who set this variable put the desired ExtraInfo in it.
		}
		else // Since AltGr wasn't detected above, see if any other means is ready to detect it.
		{
			// v1.0.42.04: This section was moved out of IsIgnored() to here because:
			// 1) Immediately correcting the incoming event simplifies other parts of the hook.
			// 2) It allows this instance of the hook to communicate with other instances of the hook by
			//    correcting the bogus values directly inside the event structure.  This is something those
			//    other hooks can't do easily if the keystrokes were generated/simulated inside our instance
			//    (due to our instance's KeyEvent() function communicating corrections via
			//    g_HookReceiptOfLControlMeansAltGr and g_IgnoreNextLControlDown/Up).
			//
			// This new approach solves an AltGr keystroke's disruption of A_TimeIdlePhysical and related
			// problems that were caused by AltGr's incoming keystroke being marked by the driver or OS as a
			// physical event (even when the AltGr keystroke that caused it was artificial).  It might not
			// be a perfect solution, but it's pretty complete. For example, with the exception of artificial
			// AltGr keystrokes from non-AHK sources, it completely solves the A_TimeIdlePhysical issue because
			// by definition, any script that *uses* A_TimeIdlePhysical in a way that the fix applies to also
			// has the keyboard hook installed (if it only has the mouse hook installed, the fix isn't in effect,
			// but it also doesn't matter because that script detects only mouse events as true physical events,
			// as described in the documentation for A_TimeIdlePhysical).
			if (key_up)
			{
				if (g_IgnoreNextLControlUp)
				{
					event.flags |= LLKHF_INJECTED; // Flag it as artificial for any other instances of the hook that may be running.
					event.dwExtraInfo = g_IgnoreNextLControlUp; // The one who set this variable put the desired ExtraInfo in here.
				}
			}
			else // key-down event
			{
				if (g_IgnoreNextLControlDown)
				{
					event.flags |= LLKHF_INJECTED; // Flag it as artificial for any other instances of the hook that may be running.
					event.dwExtraInfo = g_IgnoreNextLControlDown; // The one who set this variable put the desired ExtraInfo in here.
				}
			}
		}
	} // if (vk == VK_LCONTROL)

	return LowLevelCommon(g_KeybdHook, aCode, wParam, lParam, vk, sc, key_up, event.dwExtraInfo, event.flags);
}



LRESULT CALLBACK LowLevelMouseProc(int aCode, WPARAM wParam, LPARAM lParam)
{
	// code != HC_ACTION should be evaluated PRIOR to considering the values
	// of wParam and lParam, because those values may be invalid or untrustworthy
	// whenever code < 0.
	if (aCode != HC_ACTION)
		return CallNextHookEx(g_MouseHook, aCode, wParam, lParam);

	MSLLHOOKSTRUCT &event = *(PMSLLHOOKSTRUCT)lParam;  // For convenience, maintainability, and possibly performance.

	// Make all mouse events physical to try to simulate mouse clicks in games that normally ignore
	// artificial input.
	//event.flags &= ~LLMHF_INJECTED;

	if (!(event.flags & LLMHF_INJECTED)) // Physical mouse movement or button action (uses LLMHF vs. LLKHF).
		g_TimeLastInputPhysical = g_TimeLastInputMouse = GetTickCount();
		// Above: Don't use event.time, mostly because SendInput can produce invalid timestamps on such events
		// (though in truth, that concern isn't valid because SendInput's input isn't marked as physical).
		// Another concern is the comments at the other update of "g_TimeLastInputPhysical" elsewhere in this file.
		// A final concern is that some drivers might be faulty and might not generate an accurate timestamp.

	if (wParam == WM_MOUSEMOVE) // Only after updating for physical input, above, is this checked.
		return (g_BlockMouseMove && !(event.flags & LLMHF_INJECTED)) ? 1 : CallNextHookEx(g_MouseHook, aCode, wParam, lParam);
		// Above: In v1.0.43.11, a new mode was added to block mouse movement only since it's more flexible than
		// BlockInput (which keybd too, and blocks all mouse buttons too).  However, this mode blocks only
		// physical mouse movement because it seems most flexible (and simplest) to allow all artificial
		// movement, even if that movement came from a source other than an AHK script (such as some other
		// macro program).

	// MSDN: WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MOUSEMOVE, WM_MOUSEWHEEL [, WM_MOUSEHWHEEL], WM_RBUTTONDOWN, or WM_RBUTTONUP.
	// But what about the middle button?  It's undocumented, but it is received.
	// What about doubleclicks (e.g. WM_LBUTTONDBLCLK): I checked: They are NOT received.
	// This is expected because each click in a doubleclick could be separately suppressed by
	// the hook, which would make it become a non-doubleclick.
	vk_type vk = 0;
	sc_type sc = 0; // To be overridden if this even is a wheel turn.
	short wheel_delta;
	bool key_up = true;  // Set default to safest value.

	switch (wParam)
	{
		case WM_MOUSEWHEEL:
		case WM_MOUSEHWHEEL: // v1.0.48: Lexikos: Support horizontal scrolling in Windows Vista and later.
			// MSDN: "A positive value indicates that the wheel was rotated forward, away from the user;
			// a negative value indicates that the wheel was rotated backward, toward the user. One wheel
			// click is defined as WHEEL_DELTA, which is 120."  Testing shows that on XP at least, the
			// abs(delta) is greater than 120 when the user turns the wheel quickly (also depends on
			// granularity of wheel hardware); i.e. the system combines multiple turns into a single event.
			wheel_delta = GET_WHEEL_DELTA_WPARAM(event.mouseData); // Must typecast to short (not int) via macro, otherwise the conversion to negative/positive number won't be correct.
			if (wParam == WM_MOUSEWHEEL)
				vk = wheel_delta < 0 ? VK_WHEEL_DOWN : VK_WHEEL_UP;
			else
				vk = wheel_delta < 0 ? VK_WHEEL_LEFT : VK_WHEEL_RIGHT;
			// Dividing by WHEEL_DELTA was a mistake because some mice can yield deltas less than 120.
			// However, this behavior is kept for backward compatibility because some scripts may rely
			// on A_EventInfo==0 meaning "delta is between 1 and 119".  WheelLeft/Right were also done
			// that way because consistency may be more important than correctness.  In the future, perhaps
			// an A_EventInfo2 can be added, or some hotkey aliases like "FineWheelXXX".
			sc = (wheel_delta > 0 ? wheel_delta : -wheel_delta) / WHEEL_DELTA; // See above. Note that sc is unsigned.
			key_up = false; // Always consider wheel movements to be "key down" events.
			break;
		case WM_LBUTTONUP: vk = VK_LBUTTON;	break;
		case WM_RBUTTONUP: vk = VK_RBUTTON; break;
		case WM_MBUTTONUP: vk = VK_MBUTTON; break;
		case WM_NCXBUTTONUP:  // NC means non-client.
		case WM_XBUTTONUP: vk = (HIWORD(event.mouseData) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2; break;
		case WM_LBUTTONDOWN: vk = VK_LBUTTON; key_up = false; break;
		case WM_RBUTTONDOWN: vk = VK_RBUTTON; key_up = false; break;
		case WM_MBUTTONDOWN: vk = VK_MBUTTON; key_up = false; break;
		case WM_NCXBUTTONDOWN:
		case WM_XBUTTONDOWN: vk = (HIWORD(event.mouseData) == XBUTTON1) ? VK_XBUTTON1 : VK_XBUTTON2; key_up = false; break;
	}

	return LowLevelCommon(g_MouseHook, aCode, wParam, lParam, vk, sc, key_up, event.dwExtraInfo, event.flags);
}



LRESULT LowLevelCommon(const HHOOK aHook, int aCode, WPARAM wParam, LPARAM lParam, const vk_type aVK
	, sc_type aSC, bool aKeyUp, ULONG_PTR aExtraInfo, DWORD aEventFlags)
// v1.0.38.06: The keyboard and mouse hooks now call this common function to reduce code size and improve
// maintainability.  The code size savings as of v1.0.38.06 is 3.5 KB of uncompressed code, but that
// savings will grow larger if more complexity is ever added to the hooks.
{
	WPARAM hotkey_id_to_post = HOTKEY_ID_INVALID; // Set default.
	bool is_ignored = IsIgnored(aExtraInfo);

	// The following is done for more than just convenience.  It solves problems that would otherwise arise
	// due to the value of a global var such as KeyHistoryNext changing due to the reentrancy of
	// this procedure.  For example, a call to KeyEvent() in here would alter the value of
	// KeyHistoryNext, in most cases before we had a chance to finish using the old value.  In other
	// words, we use an automatic variable so that every instance of this function will get its
	// own copy of the variable whose value will stays constant until that instance returns:
	KeyHistoryItem *pKeyHistoryCurr, khi_temp; // Must not be static (see above).  Serves as a storage spot for a single keystroke in case key history is disabled.
	if (!g_KeyHistory)
		pKeyHistoryCurr = &khi_temp;  // Having a non-NULL pKeyHistoryCurr simplifies the code in other places.
	else
	{
		pKeyHistoryCurr = g_KeyHistory + g_KeyHistoryNext;
		if (++g_KeyHistoryNext >= g_MaxHistoryKeys)
			g_KeyHistoryNext = 0;
		pKeyHistoryCurr->vk = aVK; // aSC is done later below.
		pKeyHistoryCurr->key_up = aKeyUp;
		g_HistoryTickNow = GetTickCount();
		pKeyHistoryCurr->elapsed_time = (g_HistoryTickNow - g_HistoryTickPrev) / (float)1000;
		g_HistoryTickPrev = g_HistoryTickNow;
		HWND fore_win = GetForegroundWindow();
		if (fore_win)
		{
			if (fore_win != g_HistoryHwndPrev)
			{
				// The following line is commented out in favor of the one beneath it (seem below comment):
				//GetWindowText(fore_win, pKeyHistoryCurr->target_window, sizeof(pKeyHistoryCurr->target_window));
				PostMessage(g_hWnd, AHK_GETWINDOWTEXT, (WPARAM)pKeyHistoryCurr->target_window, (LPARAM)fore_win);
				// v1.0.44.12: The reason for the above is that clicking a window's close or minimize button
				// (and possibly other types of title bar clicks) causes a delay for the following window, at least
				// when XP Theme (but not classic theme) is in effect:
				//#InstallMouseHook
				//Gui, +AlwaysOnTop
				//Gui, Show, w200 h100
				//return
				// The problem came about from the following sequence of events:
				// 1) User clicks the one of the script's window's title bar's close, minimize, or maximize button.
				// 2) WM_NCLBUTTONDOWN is sent to the window's window proc, which then passes it on to
				//    DefWindowProc or DefDlgProc, which then apparently enters a loop in which no messages
				//    (or a very limited subset) are pumped.
				// 3) If anyone sends a message to that window (such as GetWindowText(), which sends a message
				//    in cases where it doesn't have the title pre-cached), the message will not receive a reply
				//    until after the mouse button is released.
				// 4) But the hook is the very thing that's supposed to release the mouse button, and it can't
				//    until a reply is received.
				// 5) Thus, a deadlock occurs.  So after a short but noticeable delay, the OS sees the hook as
				//    unresponsive and bypasses it, sending the click through normally, which breaks the deadlock.
				// 6) A similar situation might arise when a right-click-down is sent to the title bar or
				//    sys-menu-icon.
				//
				// SOLUTION:
				// Post the message to our main thread to have it do the GetWindowText call.  That way, if
				// the target window is one of the main thread's own window's, there's no chance it can be
				// in an unresponsive state like the deadlock described above.  In addition, do this for ALL
				// windows because its simpler, more maintainable, and especially might solve other hook
				// performance problems if GetWindowText() has other situations where it is slow to return
				// (which seems likely).
				// Although the above solution could create rare situations where there's a lag before window text
				// is updated, that seems unlikely to be common or have significant consequences.  Furthermore,
				// it has the advantage of improving hook performance by avoiding the call to GetWindowText (which
				// incidentally might solve hotkey lag problems that have been observed while the active window
				// is momentarily busy/unresponsive -- but maybe not because the main thread would then be lagged
				// instead of the hook thread, which is effectively the same result from user's POV).
				// Note: It seems best not to post the message to the hook thread because if LButton is down,
				// the hook's main event loop would be sending a message to an unresponsive thread (our main thread),
				// which would create the same deadlock.
				// ALTERNATE SOLUTIONS:
				// - #1: Avoid calling GetWindowText at all when LButton or RButton is in a logically-down state.
				// - Same as #1, but do so only if one of the main thread's target windows is known to be in a tight loop (might be too unreliable to detect all such cases).
				// - Same as #1 but less rigorous and more catch-all, such as by checking if the active window belongs to our thread.
				// - Avoid calling GetWindowText at all upon release of LButton.
				// - Same, but only if the window to have text retrieved belongs to our process.
				// - Same, but only if the mouse is inside the close/minimize/etc. buttons of the active window.
			}
			else // i.e. where possible, avoid the overhead of the call to GetWindowText().
				*pKeyHistoryCurr->target_window = '\0';
		}
		else
			_tcscpy(pKeyHistoryCurr->target_window, _T("N/A")); // Due to AHK_GETWINDOWTEXT, this could collide with main thread's writing to same string; but in addition to being extremely rare, it would likely be inconsequential.
		g_HistoryHwndPrev = fore_win;  // Updated unconditionally in case fore_win is NULL.
	}
	// Keep the following flush with the above to indicate that they're related.
	// The following is done even if key history is disabled because firing a wheel hotkey via PostMessage gets
	// the notch count from pKeyHistoryCurr->sc.
	if (aVK == VK_PACKET) // Win2k/XP: VK_PACKET is used to send Unicode characters as if they were keystrokes.  sc is a 16-bit character code in that case.
	{
		aSC = 0; // This held a truncated character code, not to be mistaken for a real scan code.
		pKeyHistoryCurr->sc = (sc_type)((PKBDLLHOOKSTRUCT)lParam)->scanCode; // Get the full character code.
		pKeyHistoryCurr->event_type = 'U'; // Give it a unique identifier even though it can be distinguished by the 4-digit "SC".  'U' vs 'u' to avoid confusion with 'u'=up.
		// Artificial character input via VK_PACKET isn't supported by hotkeys, since they always work via
		// keycode, but hotstrings and Input are supported via the macro below when #InputLevel is non-zero.
		// Must return now to avoid misinterpreting aSC as an actual scancode in the code below.
		return AllowKeyToGoToSystem;
	}
	//else: Use usual modified value.
	pKeyHistoryCurr->sc = aSC; // Will be zero if our caller is the mouse hook (except for wheel notch count).
	// After logging the wheel notch count (above), purify aSC for readability and maintainability.
	if (IS_WHEEL_VK(aVK))
		aSC = 0; // Also relied upon by sc_takes_precedence below.

	bool is_artificial;
	if (aHook == g_MouseHook)
	{
		if (   !(is_artificial = (aEventFlags & LLMHF_INJECTED))   ) // It's a physical mouse event.
			g_PhysicalKeyState[aVK] = aKeyUp ? 0 : STATE_DOWN;
	}
	else // Keybd hook.
	{
		// Even if the below is set to false, the event might be reclassified as artificial later (though it
		// won't be logged as such).  See comments in KeybdEventIsPhysical() for details.
		is_artificial = aEventFlags & LLKHF_INJECTED; // LLKHF vs. LLMHF

		// Track physical state of keyboard & mouse buttons. Also, if it's a modifier, let another section
		// handle it because it's not as simple as just setting the value to true or false (e.g. if LShift
		// goes up, the state of VK_SHIFT should stay down if VK_RSHIFT is down, or up otherwise).
		// Also, even if this input event will wind up being suppressed (usually because of being
		// a hotkey), still update the physical state anyway, because we want the physical state to
		// be entirely independent of the logical state (i.e. we want the key to be reported as
		// physically down even if it isn't logically down):
		if (!kvk[aVK].as_modifiersLR && KeybdEventIsPhysical(aEventFlags, aVK, aKeyUp))
			g_PhysicalKeyState[aVK] = aKeyUp ? 0 : STATE_DOWN;

		// Pointer to the key record for the current key event.  Establishes this_key as an alias
		// for the array element in kvk or ksc that corresponds to the vk or sc, respectively.
		// I think the compiler can optimize the performance of reference variables better than
		// pointers because the pointer indirection step is avoided.  In any case, this must be
		// a true alias to the object, not a copy of it, because it's address (&this_key) is compared
		// to other addresses for equality further below.
	}

	// The following is done even if key history is disabled because sAltTabMenuIsVisible relies on it:
	pKeyHistoryCurr->event_type = is_ignored ? 'i' : (is_artificial ? 'a' : ' '); // v1.0.42.04: 'a' was added, but 'i' takes precedence over 'a'.

	// v1.0.43: Block the Win keys during journal playback to prevent keystrokes hitting the Start Menu
	// if the user accidentally presses one of those keys during playback.  Note: Keys other than Win
	// don't need to be blocked because the playback hook defers them until after playback.
	// Only block the down-events in case the user is physically holding down the key at the start
	// of playback but releases it during the Send (avoids Win key becoming logically stuck down).
	// This also serves to block Win shortcuts such as Win+R and Win+E during playback.
	// Also, it seems best to block artificial LWIN keystrokes too, in case some other script or
	// program tries to display the Start Menu during playback.
	if (g_BlockWinKeys && (aVK == VK_LWIN || aVK == VK_RWIN) && !aKeyUp)
		return SuppressThisKey;

	// v1.0.37.07: Cancel the alt-tab menu upon receipt of Escape so that it behaves like the OS's native Alt-Tab.
	// Even if is_ignored==true, it seems more flexible/useful to cancel the Alt-Tab menu upon receiving
	// an Escape keystroke of any kind.
	// Update: Must not handle Alt-Up here in a way similar to Esc-down in case the hook sent Alt-up to
	// dismiss its own menu. Otherwise, the shift key might get stuck down if Shift-Alt-Tab was in effect.
	// Instead, the release-of-prefix-key section should handle it via its checks of this_key.it_put_shift_down, etc.
	if (sAltTabMenuIsVisible && aVK == VK_ESCAPE && !aKeyUp)
	{
		// When the alt-tab window is owned by the script (it is owned by csrss.exe unless the script
		// is the process that invoked the alt-tab window), testing shows that the script must be the
		// originator of the Escape keystroke.  Therefore, substitute a simulated keystroke for the
		// user's physical keystroke. It might be necessary to do this even if is_ignored==true because
		// a keystroke from some other script/process might not qualify as a valid means to cancel it.
		// UPDATE for v1.0.39: The escape handler below works only if the hook's thread invoked the
		// alt-tab window, not if the script's thread did via something like "Send {Alt down}{tab down}".
		// This is true even if the process ID is checked instead of the thread ID below.  I think this
		// behavior is due to the window obeying escape only when its own thread sends it.  This
		// is probably done to allow a program to automate the alt-tab menu without interference
		// from Escape keystrokes typed by the user.  Although this could probably be fixed by
		// sending a message to the main thread and having it send the Escape keystroke, it seems
		// best not to do this because:
		// 1) The ability to dismiss a script-invoked alt-tab menu with escape would vary depending on
		//    whether the keyboard hook is installed (i.e. it's inconsistent).
		// 2) It's more flexible to preserve the ability to protect the alt-tab menu from physical
		//    escape keystrokes typed by the user.  The script can simulate an escape key to explicitly
		//    close an alt-tab window it invoked (a simulated escape keystroke can apparently also close
		//    any alt-tab menu, even one invoked by physical keystrokes; but the converse isn't true).
		// 3) Lesser reason: Reduces code size and complexity.
		// UPDATE in 2019: Testing on Windows 7 and 10 indicate this does not apply to the more modern
		// versions of Alt-Tab, but it still applies if the classic Alt-Tab is restored via the registry.
		// However, on these OSes, the user is able to press Esc to dismiss our Alt-Tab.  Other scripts
		// (and presumably other processes) are *NOT* able to dismiss it by simulating Esc.
		HWND alt_tab_window;
		if ((alt_tab_window = FindWindow(_T("#32771"), NULL)) // There is an alt-tab window...
			&& GetWindowThreadProcessId(alt_tab_window, NULL) == GetCurrentThreadId()) // ...and it's owned by the hook thread (not the main thread).
		{
			KeyEvent(KEYDOWN, VK_ESCAPE);
			// By definition, an Alt key should be logically down if the alt-tab menu is visible (even if it
			// isn't, sending an extra up-event seems harmless).  Releasing that Alt key seems best because:
			// 1) If the prefix key that pushed down the alt key is still physically held down and the user
			//    presses a new (non-alt-tab) suffix key to form a hotkey, it avoids any alt-key disruption
			//    of things such as MouseClick that that subroutine might due.
			// 2) If the user holds down the prefix, presses Escape to dismiss the menu, then presses an
			//    alt-tab suffix, testing shows that the existing alt-tab logic here in the hook will put
			//    alt or shift-alt back down if it needs to.
			KeyEvent(KEYUP, (g_modifiersLR_logical & MOD_RALT) ? VK_RMENU : VK_LMENU);
			return SuppressThisKey; // Testing shows that by contrast, the upcoming key-up on Escape doesn't require this logic.
		}
		// Otherwise, the alt-tab window doesn't exist or (more likely) it's owned by some other process
		// such as crss.exe.  Do nothing extra to avoid interfering with the native function of Escape or
		// any remappings or hotkeys assigned to Escape.  Also, do not set sAltTabMenuIsVisible to false
		// in any of the cases here because there is logic elsewhere in the hook that does that more
		// reliably; it takes into account things such as whether the Escape keystroke will be suppressed
		// due to being a hotkey).
	}

	bool sc_takes_precedence = ksc[aSC].sc_takes_precedence;
	// Check hook type too in case a script every explicitly specifies scan code zero as a hotkey:
	key_type &this_key = *((aHook == g_KeybdHook && sc_takes_precedence) ? (ksc + aSC) : (kvk + aVK));

	// Do this after above since AllowKeyToGoToSystem requires that sc be properly determined.
	// Another reason to do it after the above is due to the fact that KEY_PHYS_IGNORE permits
	// an ignored key to be considered physical input, which is handled above:
	if (is_ignored)
	{
		// This is a key sent by our own app that we want to ignore.
		// It's important never to change this to call the SuppressKey function because
		// that function would cause an infinite loop when the Numlock key is pressed,
		// which would likely hang the entire system:
		// UPDATE: This next part is for cases where more than one script is using the hook
		// simultaneously.  In such cases, it desirable for the KEYEVENT_PHYS() of one
		// instance to affect the down-state of the current prefix-key in the other
		// instances.  This check is done here -- even though there may be a better way to
		// implement it -- to minimize the chance of side-effects that a more fundamental
		// change might cause (i.e. a more fundamental change would require a lot more
		// testing, though it might also fix more things):
		if (aExtraInfo == KEY_PHYS_IGNORE && aKeyUp && pPrefixKey == &this_key)
		{
			this_key.is_down = false;
			this_key.down_performed_action = false;  // Seems best, but only for PHYS_IGNORE.
			pPrefixKey = NULL;
		}
		return AllowKeyToGoToSystem;
	}

	if (!aKeyUp) // Set defaults for this down event.
	{
		this_key.hotkey_down_was_suppressed = false;
		this_key.hotkey_to_fire_upon_release = HOTKEY_ID_INVALID;
		// Don't do the following because of the keyboard key-repeat feature.  In other words,
		// the NO_SUPPRESS_NEXT_UP_EVENT should stay pending even in the face of consecutive
		// down-events.  Even if it's possible for the flag to never be cleared due to never
		// reaching any of the parts that clear it (which currently seems impossible), it seems
		// inconsequential since by its very nature, this_key never consults the flag.
		// this_key.no_suppress &= ~NO_SUPPRESS_NEXT_UP_EVENT;
	}

	if (aHook == g_MouseHook)
	{
		// If no vk, there's no mapping for this key, so currently there's no way to process it.
		if (!aVK)
			return AllowKeyToGoToSystem;
		// Also, if the script is displaying a menu (tray, main, or custom popup menu), always
		// pass left-button events through -- even if LButton is defined as a hotkey -- so
		// that menu items can be properly selected.  This is necessary because if LButton is
		// a hotkey, it can't launch now anyway due to the script being uninterruptible while
		// a menu is visible.  And since it can't launch, it can't do its typical "MouseClick
		// left" to send a true mouse-click through as a replacement for the suppressed
		// button-down and button-up events caused by the hotkey.  Also, for simplicity this
		// is done regardless of which modifier keys the user is holding down since the desire
		// to fire mouse hotkeys while a context or popup menu is displayed seems too rare.
		//
		// Update for 1.0.37.05: The below has been extended to look for menus beyond those
		// supported by g_MenuIsVisible, namely the context menus of a MonthCal or Edit control
		// (even the script's main window's edit control's context menu).  It has also been
		// extended to include RButton because:
		// 1) Right and left buttons may have been swapped via control panel to take on each others' functions.
		// 2) Right-click is a valid way to select a context menu items (but apparently not popup or menu bar items).
		// 3) Right-click should invoke another instance of the context menu (or dismiss existing menu, depending
		//    on where the click occurs) if user clicks outside of our thread's existing context menu.
		HWND menu_hwnd;
		if (   (aVK == VK_LBUTTON || aVK == VK_RBUTTON) && (g_MenuIsVisible // Ordered for short-circuit performance.
				|| ((menu_hwnd = FindWindow(_T("#32768"), NULL))
					&& GetWindowThreadProcessId(menu_hwnd, NULL) == g_MainThreadID))   ) // Don't call GetCurrentThreadId() because our thread is different than main's.
		{
			// Bug-fix for v1.0.22: If "LControl & LButton::" (and perhaps similar combinations)
			// is a hotkey, the foreground window would think that the mouse is stuck down, at least
			// if the user clicked outside the menu to dismiss it.  Specifically, this comes about
			// as follows:
			// The wrong up-event is suppressed:
			// ...because down_performed_action was true when it should have been false
			// ...because the while-menu-was-displayed up-event never set it to false
			// ...because it returned too early here before it could get to that part further below.
			this_key.down_performed_action = false; // Seems ok in this case to do this for both aKeyUp and !aKeyUp.
			this_key.is_down = !aKeyUp;
			return AllowKeyToGoToSystem;
		}
	} // Mouse hook.

	// Any key-down event (other than those already ignored and returned from,
	// above) should probably be considered an attempt by the user to use the
	// prefix key that's currently being held down as a "modifier".  That way,
	// if pPrefixKey happens to also be a suffix, its suffix action won't fire
	// when the key is released, which is probably the correct thing to do 90%
	// or more of the time.  But don't consider the modifiers themselves to have
	// been modified by a prefix key, since that is almost never desirable:
	if (   pPrefixKey && pPrefixKey != &this_key && !aKeyUp // There is a prefix key being held down and the user has now pressed some other key.
		&& pPrefixKey->was_just_used != AS_PASSTHROUGH_PREFIX // v1.1.34.02: Retain this value for prefix key-up.
		&& ((aHook == g_KeybdHook) ? !this_key.as_modifiersLR : pPrefixKey->as_modifiersLR)  )
			pPrefixKey->was_just_used = AS_PREFIX; // Indicate that currently-down prefix key has been "used".
	// Formerly, the above was done only for keyboard hook, not the mouse.  This was because
	// most people probably would not want a prefix key's suffix-action to be stopped
	// from firing just because a non-hotkey mouse button was pressed while the key
	// was held down (i.e. for games).  But now a small exception to this has been made:
	// Prefix keys that are also modifiers (ALT/SHIFT/CTRL/WIN) will now not fire their
	// suffix action on key-up if they modified a mouse button event (since Ctrl-LeftClick,
	// for example, is a valid native action and we don't want to give up that flexibility).

	// WinAPI docs state that for both virtual keys and scan codes:
	// "If there is no translation, the return value is zero."
	// Therefore, zero is never a key that can be validly configured (and likely it's never received here anyway).
	// UPDATE: For performance reasons, this check isn't even done.  Even if sc and vk are both zero, both kvk[0]
	// and ksc[0] should have all their attributes initialized to FALSE so nothing should happen for that key
	// anyway.
	//if (!vk && !sc)
	//	return AllowKeyToGoToSystem;

	

	if (!this_key.used_as_prefix && !this_key.used_as_suffix)
	{
		// Fix for v1.1.31.02: This is done regardless of used_as to ensure it doesn't get "stuck down"
		// when a custom combination hotkey Suspends itself, thereby causing used_as to be reset to false.
		// Fix for v1.1.31.03: Done conditionally because its previous value is used below.  This affects
		// modifier keys as hotkeys, such as Shift::MsgBox.
		this_key.is_down = !aKeyUp;
		return AllowKeyToGoToSystem;
	}

	HotkeyIDType hotkey_id_with_flags = HOTKEY_ID_INVALID; // Set default.
	HotkeyVariant *firing_is_certain = NULL;               //
	HotkeyIDType hotkey_id_temp; // For informal/temp storage of the ID-without-flags.

	bool fire_with_no_suppress = false; // Set default.
	bool down_performed_action, was_down_before_up;
	if (aKeyUp)
	{
		// Save prior to reset.  These var's should only be used further below in conjunction with aKeyUp
		// being TRUE.  Otherwise, their values will be unreliable (refer to some other key, probably).
		was_down_before_up = this_key.is_down;
		down_performed_action = this_key.down_performed_action;  // Save prior to reset below.
		// Reset these values in preparation for the next call to this procedure that involves this key:
		this_key.down_performed_action = false;
		if (this_key.hotkey_to_fire_upon_release != HOTKEY_ID_INVALID)
		{
			hotkey_id_with_flags = this_key.hotkey_to_fire_upon_release;
			// The line below is done even though the down-event also resets it in case it is ever
			// possible for keys to generate multiple consecutive key-up events (faulty or unusual keyboards?)
			this_key.hotkey_to_fire_upon_release = HOTKEY_ID_INVALID;
		}
		// v1.1.34.01: Use up the no-suppress ticket early for simplicity and maintainability.  Its value
		// might not be used further below, but in any case the ticket shouldn't be applied to any event
		// after this one.
		if (this_key.no_suppress & NO_SUPPRESS_NEXT_UP_EVENT)
		{
			fire_with_no_suppress = true;
			this_key.no_suppress &= ~NO_SUPPRESS_NEXT_UP_EVENT; // This ticket has been used up, so remove it.
		}
	}
	this_key.is_down = !aKeyUp;
	bool modifiers_were_corrected = false;

	if (aHook == g_KeybdHook)
	{
		// The below was added to fix hotkeys that have a neutral suffix such as "Control & LShift".
		// It may also fix other things and help future enhancements:
		if (this_key.as_modifiersLR)
		{
			// The neutral modifier "Win" is not currently supported.
			kvk[VK_CONTROL].is_down = kvk[VK_LCONTROL].is_down || kvk[VK_RCONTROL].is_down;
			kvk[VK_MENU].is_down = kvk[VK_LMENU].is_down || kvk[VK_RMENU].is_down;
			kvk[VK_SHIFT].is_down = kvk[VK_LSHIFT].is_down || kvk[VK_RSHIFT].is_down;
			// No longer possible because vk is translated early on from neutral to left-right specific:
			// I don't think these ever happen with physical keyboard input, but it might with artificial input:
			//case VK_CONTROL: kvk[sc == SC_RCONTROL ? VK_RCONTROL : VK_LCONTROL].is_down = !aKeyUp; break;
			//case VK_MENU: kvk[sc == SC_RALT ? VK_RMENU : VK_LMENU].is_down = !aKeyUp; break;
			//case VK_SHIFT: kvk[sc == SC_RSHIFT ? VK_RSHIFT : VK_LSHIFT].is_down = !aKeyUp; break;
		}
	}
	else // Mouse hook
	{
		// If the mouse hook is installed without the keyboard hook, update g_modifiersLR_logical
		// manually so that it can be referred to by the mouse hook after this point:
		if (!g_KeybdHook)
		{
			g_modifiersLR_logical = g_modifiersLR_logical_non_ignored = GetModifierLRState(true);
			modifiers_were_corrected = true;
		}
	}

	modLR_type modifiersLRnew;

	bool this_toggle_key_can_be_toggled = this_key.pForceToggle && *this_key.pForceToggle == NEUTRAL; // Relies on short-circuit boolean order.

	// Prior to considering whether to fire a hotkey, correct the hook's modifier state.
	// Although this is rarely needed, there are times when the OS disables the hook, thus
	// it is possible for it to miss keystrokes.  This should be done before pPrefixKey is
	// consulted below as pPrefixKey itself might be corrected if it is a standard modifier.
	// See comments in GetModifierLRState() for more info:
	if (!modifiers_were_corrected)
	{
		modifiers_were_corrected = true;
		GetModifierLRState(true);
	}


	///////////////////////////////////////////////////////////////////////////////////////
	// CASE #1 of 4: PREFIX key has been pressed down.  But use it in this capacity only if
	// no other prefix is already in effect or if this key isn't a suffix.  Update: Or if
	// this key-down is the same as the prefix already down, since we want to be able to
	// a prefix when it's being used in its role as a modified suffix (see below comments).
	///////////////////////////////////////////////////////////////////////////////////////
	if (this_key.used_as_prefix && !aKeyUp && (!pPrefixKey || !this_key.used_as_suffix || &this_key == pPrefixKey))
	{
		// v1.0.41: Even if this prefix key is non-suppressed (passes through to active window),
		// still call PrefixHasNoEnabledSuffixes() because don't want to overwrite the old value of
		// pPrefixKey (see comments in "else" later below).
		// v1.0.44: Added check for PREFIX_ACTUAL so that a PREFIX_FORCED prefix will be considered
		// a prefix even if it has no suffixes.  This fixes an unintentional change in v1.0.41 where
		// naked, neutral modifier hotkeys Control::, Alt::, and Shift:: started firing on press-down
		// rather than release as intended.  The PREFIX_FORCED facility may also provide the means to
		// introduce a new hotkey modifier such as an "up2" keyword that makes any key into a prefix
		// key even if it never acts as a prefix for other keys, which in turn has the benefit of firing
		// on key-up, but only if the no other key was pressed while the user was holding it down.
		bool suppress_this_prefix = !(this_key.no_suppress & AT_LEAST_ONE_COMBO_HAS_TILDE); // Set default.
		bool has_no_enabled_suffixes;
		if (   !(has_no_enabled_suffixes = (this_key.used_as_prefix == PREFIX_ACTUAL)
			&& Hotkey::PrefixHasNoEnabledSuffixes(sc_takes_precedence ? aSC : aVK, sc_takes_precedence, suppress_this_prefix))   )
		{
			// This check is necessary in cases such as the following, in which the "A" key continues
			// to repeat because pressing a mouse button (unlike pressing a keyboard key) does not
			// stop the prefix key from repeating:
			// $a::send, a
			// a & lbutton::
			if (&this_key != pPrefixKey)
			{
				// Override any other prefix key that might be in effect with this one, in case the
				// prior one, due to be old for example, was invalid somehow.  UPDATE: It seems better
				// to leave the old one in effect to support the case where one prefix key is modifying
				// a second one in its role as a suffix.  In other words, if key1 is a prefix and
				// key2 is both a prefix and a suffix, we want to leave key1 in effect as a prefix,
				// rather than key2.  Hence, a null-check was added in the above if-stmt:
				pPrefixKey = &this_key;
				// It should be safe to init this because even if the current key is repeating,
				// it should be impossible to receive here the key-downs that occurred after
				// the first, because there's a return-on-repeat check farther above (update: that check
				// is gone now).  Even if that check weren't done, it's safe to reinitialize this to zero
				// because on most (all?) keyboards & OSs, the moment the user presses another key while
				// this one is held down, the key-repeating ceases and does not resume for
				// this key (though the second key will begin to repeat if it too is held down).
				// In other words, the fear that this would be wrongly initialized and thus cause
				// this prefix's suffix-action to fire upon key-release seems unfounded.
				// It seems easier (and may perform better than alternative ways) to init this
				// here rather than say, upon the release of the prefix key:
				this_key.was_just_used = 0; // Init to indicate it hasn't yet been used in its role as a prefix.
			}
		}
		//else this prefix has no enabled suffixes, so its role as prefix is also disabled.
		// Therefore, don't set pPrefixKey to this_key because don't want the following line
		// (in another section) to execute when a suffix comes in (there may be other reasons too,
		// such as not wanting to lose track of the previous prefix key in cases where the user is
		// holding down more than one prefix):
		// pPrefixKey->was_just_used = AS_PREFIX

		if (this_key.used_as_suffix) // v1.0.41: Added this check to avoid doing all of the below when unnecessary.
		{
			// This new section was added May 30, 2004, to fix scenarios such as the following example:
			// a & b::Msgbox a & b
			// $^a::MsgBox a
			// Previously, the ^a hotkey would only fire on key-up (unless it was registered, in which
			// case it worked as intended on the down-event).  When the user presses A, it's okay (and
			// probably desirable) to have recorded that event as a prefix-key-down event (above).
			// But in addition to that, we now check if this is a normal, modified hotkey that should
			// fire now rather than waiting for the key-up event.  This is done because it makes sense,
			// it's more correct, and also it makes the behavior of a hooked ^a hotkey consistent with
			// that of a registered ^a.

			// non_ignored is always used when considering whether a key combination is in place to
			// trigger a hotkey:
			modifiersLRnew = g_modifiersLR_logical_non_ignored;
			if (this_key.as_modifiersLR) // This will always be false if our caller is the mouse hook.
				// Hotkeys are not defined to modify themselves, so look for a match accordingly.
				modifiersLRnew &= ~this_key.as_modifiersLR;
			
			// This prefix key's hotkey needs to be checked even if it will ultimately fire only on release.
			// If suppress_this_prefix == false, this prefix key's key-down hotkey should fire immediately.
			// If suppress_this_prefix == true, its final value can only be confirmed by verifying whether
			// this prefix key's hotkey has the no-suppress prefix (which should cause the hotkey to fire
			// immediately and not be suppressed).
			// This prefix key's hotkey should also be fired immediately if there are any modifiers down.
			// Check hook type too in case a script ever explicitly specifies scan code zero as a hotkey:
			hotkey_id_with_flags = (aHook == g_KeybdHook && sc_takes_precedence)
				? Kscm(modifiersLRnew, aSC) : Kvkm(modifiersLRnew, aVK);
			hotkey_id_temp = hotkey_id_with_flags & HOTKEY_ID_MASK;
			if (IS_ALT_TAB(hotkey_id_temp))
				hotkey_id_with_flags = HOTKEY_ID_INVALID; // Let it be rediscovered when the key is released.
			else if (hotkey_id_with_flags != HOTKEY_ID_INVALID)
			{
				if (!suppress_this_prefix) // v1.1.34.02: Retain this as a flag for key-up.
					this_key.was_just_used = AS_PASSTHROUGH_PREFIX;
				if (suppress_this_prefix && !modifiersLRnew) // So far, it looks like the prefix should be suppressed.
				{
					firing_is_certain = Hotkey::CriterionFiringIsCertain(hotkey_id_with_flags, aKeyUp, aExtraInfo, fire_with_no_suppress, NULL);
					if (!firing_is_certain || !fire_with_no_suppress) // Hotkey is ineligible to fire or lacks the no-suppress prefix.
					{
						// Resetting the ID is necessary to avoid the following cases:
						//  1) A key-down hotkey which isn't eligible to fire prevents the prefix key from being suppressed.
						//  2) A key-down hotkey which isn't eligible to fire causes its key-up counterpart to fire even if
						//     the prefix key was used to activate a custom combo.
						//  3) A key-down hotkey without ~ fires immediately instead of on release.
						//  4) A key-up hotkey without ~ fires even if the prefix key was used to activate a custom combo.
						if (hotkey_id_with_flags < Hotkey::sHotkeyCount && hotkey_up[hotkey_id_with_flags] != HOTKEY_ID_INVALID)
						{
							// This key-down hotkey has a key-up counterpart.
							fire_with_no_suppress = false; // Reset for the call below.
							auto firing_up = Hotkey::CriterionFiringIsCertain(hotkey_up[hotkey_id_with_flags], aKeyUp, aExtraInfo, fire_with_no_suppress, NULL);
							if (  !(firing_up && fire_with_no_suppress)  ) // Both key-down and key-up are either ineligible or lack the no-suppress prefix.
								hotkey_id_with_flags = HOTKEY_ID_INVALID; // See comments above about resetting the ID.
							else if (firing_is_certain) // Both key-down and key-up are eligible, but key-down should be suppressed.
								fire_with_no_suppress = false; // For backward-compatibility, suppress the key-down but leave hotkey_id_with_flags set so it fires immediately.
							else // Key-down is not eligible, but key-up is.
							{
								firing_is_certain = firing_up;
								hotkey_id_with_flags = hotkey_up[hotkey_id_with_flags];
							}
						}
						else
							hotkey_id_with_flags = HOTKEY_ID_INVALID; // See comments above about resetting the ID.
					}
				}
				if (hotkey_id_with_flags & HOTKEY_KEY_UP)
				{
					// Queue it for later, which is done here rather than upon release of the key so that
					// the user can release the key's modifiers before releasing the key itself, which
					// is likely to happen pretty often. v1.0.41: This is done even if the hotkey is subject
					// to #IfWin because it seems more correct to check those criteria at the actual time
					// the key is released rather than now:
					this_key.hotkey_to_fire_upon_release = hotkey_id_with_flags;
					hotkey_id_with_flags = HOTKEY_ID_INVALID;
				}
				else if (hotkey_id_with_flags < Hotkey::sHotkeyCount) // Valid key-down hotkey.
				{
					this_key.hotkey_to_fire_upon_release = hotkey_up[hotkey_id_with_flags]; // Might assign HOTKEY_ID_INVALID.
					// Since this prefix key is being used in its capacity as a suffix instead,
					// hotkey_id_with_flags now contains a hotkey ready for firing later below.
					// v1.0.41: Above is done even if the hotkey is subject to #IfWin because:
					// 1) The down-hotkey's #IfWin criteria might be different from that of the up's.
					// 2) It seems more correct to check those criteria at the actual time the key is
					// released rather than now (and also probably reduces code size).
				}
			}
		} // if (this_key.used_as_suffix)

		if (hotkey_id_with_flags == HOTKEY_ID_INVALID)
		{
			if (has_no_enabled_suffixes)
				pKeyHistoryCurr->event_type = _T('#'); // '#' to indicate this prefix key is disabled due to #IfWin criterion.
			// In this case, a key-down event can't trigger a suffix, so return immediately.
			// If our caller is the mouse hook, both of the following will always be false:
			// this_key.as_modifiersLR
			// this_toggle_key_can_be_toggled
			if (!suppress_this_prefix) // Only for this condition. Not needed for toggle keys and not wanted for modifiers as it would prevent menu suppression.
				this_key.no_suppress |= NO_SUPPRESS_NEXT_UP_EVENT;
			if (this_key.as_modifiersLR || !suppress_this_prefix || this_toggle_key_can_be_toggled)
				return AllowKeyToGoToSystem;
			// Mark this key as having been suppressed.  This currently doesn't have any known effect
			// since the change to tilde (~) handling in v1.0.95 (commit 161162b8), but may in future.
			this_key.hotkey_down_was_suppressed = true;
			return SuppressThisKey;
		}
		//else valid suffix hotkey has been found; this will now fall through to Case #4 by virtue of aKeyUp==false.
	}

	//////////////////////////////////////////////////////////////////////////////////
	// CASE #2 of 4: SUFFIX key (that's not a prefix, or is one but has just been used
	// in its capacity as a suffix instead) has been released.
	// This is done before Case #3 for performance reasons.
	//////////////////////////////////////////////////////////////////////////////////
	// v1.0.37.05: Added "|| down_performed_action" to the final check below because otherwise a
	// script such as the following would send two M's for +b, one upon down and one upon up:
	// +b::Send, M
	// b & z::return
	// I don't remember exactly what the "pPrefixKey != &this_key" check is for below, but it is kept
	// to minimize the chance of breaking other things:
	bool fell_through_from_case2 = false; // Set default.
	if (this_key.used_as_suffix && aKeyUp && (pPrefixKey != &this_key || down_performed_action)) // Note: hotkey_id_with_flags might be already valid due to this_key.hotkey_to_fire_upon_release.
	{
		if (pPrefixKey == &this_key) // v1.0.37.05: Added so that scripts such as the example above don't leave pPrefixKey wrongly non-NULL.
			pPrefixKey = NULL;       // Also, it seems unnecessary to check this_key.it_put_alt_down and such like is done in Case #3.
		// If it did perform an action, suppress this key-up event.  Do this even
		// if this key is a modifier because it's previous key-down would have
		// already been suppressed (since this case is for suffixes that aren't
		// also prefixes), thus the key-up can be safely suppressed as well.
		// It's especially important to do this for keys whose up-events are
		// special actions within the OS, such as AppsKey, Lwin, and Rwin.
		// Toggleable keys are also suppressed here on key-up because their
		// previous key-down event would have been suppressed in order for
		// down_performed_action to be true.  UPDATE: Added handling for
		// NO_SUPPRESS_NEXT_UP_EVENT and also applied this next part to both
		// mouse and keyboard.
		// v1.0.40.01: It was observed that a hotkey that consists of a mouse button as a prefix and
		// a keyboard key as a suffix can cause sticking keys in rare cases.  For example, when
		// "MButton & LShift" is a hotkey, if you hold down LShift long enough for it to begin
		// auto-repeating then press MButton, the hotkey fires the next time LShift auto-repeats (since
		// pressing a mouse button doesn't stop a keyboard key from auto-repeating).  Fixing that type
		// of firing seems likely to break more things than it fixes.  But since it isn't fixed, when
		// the user releases LShift, the up-event is suppressed here, which causes the key to get
		// stuck down.  That could be fixed in the following ways, but all of them seem likely to break
		// more things than they fix, especially given the rarity that both a hotkey of this type would
		// exist and its mirror image does something useful that isn't a hotkey (for example, Shift+MButton
		// is a meaningful action in few if any applications):
		// 1) Don't suppress the physical release of a suffix key if that key is logically down (as reported
		//    by GetKeyState/GetAsyncKeyState): Seems too broad in scope because there might be cases where
		//    the script or user wants the key to stay logically down (e.g. Send {Shift down}{a down}).
		// 2) Same as #1 but limit the non-suppression to only times when the suffix key was logically down
		//    when its first qualified physical down-event came in.  This is definitely better but like
		//    #1, the uncertainty of breaking existing scripts and/or causing more harm than good seems too
		//    high.
		// 3) Same as #2 but limit it only to cases where the suffix key is a keyboard key and its prefix
		//    is a mouse key.  Although very selective, it doesn't mitigate the fact it might still do more
		//    harm than good and/or break existing scripts.
		// In light of the above, it seems best to keep this documented here as a known limitation for now.
		//
		// v1.0.28: The following check is done to support certain keyboards whose keys or scroll wheels
		// generate up events without first having generated any down-event for the key.  UPDATE: I think
		// this check is now also needed to allow fall-through in cases like "b" and "b up" both existing.
		if (!this_key.used_as_key_up)
			return (down_performed_action && !fire_with_no_suppress) ? SuppressThisKey : AllowKeyToGoToSystem;
		//else continue checking to see if the right modifiers are down to trigger one of this
		// suffix key's key-up hotkeys.
		fell_through_from_case2 = true;
	}

	//////////////////////////////////////////////
	// CASE #3 of 4: PREFIX key has been released.
	//////////////////////////////////////////////
	if (this_key.used_as_prefix && aKeyUp) // If these are true, hotkey_id_with_flags should be valid only by means of this_key.hotkey_to_fire_upon_release.
	{
		if (pPrefixKey == &this_key)
			pPrefixKey = NULL;
		// Else it seems best to keep the old one in effect.  This could happen, for example,
		// if the user holds down prefix1, holds down prefix2, then releases prefix1.
		// In that case, we would want to keep the most recent prefix (prefix2) in effect.
		// This logic would fail to work properly in a case like this if the user releases
		// prefix2 but still has prefix1 held down.  The user would then have to release
		// prefix1 and press it down again to get the hook to realize that it's in effect.
		// This seems very unlikely to be something commonly done by anyone, so for now
		// it's just documented here as a limitation.

		if (this_key.it_put_alt_down) // key pushed ALT down, or relied upon it already being down, so go up:
		{
			this_key.it_put_alt_down = false;
			KeyEvent(KEYUP, VK_MENU);
		}
		if (this_key.it_put_shift_down) // similar to above
		{
			this_key.it_put_shift_down = false;
			KeyEvent(KEYUP, VK_SHIFT);
		}

		if (this_toggle_key_can_be_toggled) // Always false if our caller is the mouse hook.
		{
			// It's done this way because CapsLock, for example, is a key users often
			// press quickly while typing.  I suspect many users are like me in that
			// they're in the habit of not having released the CapsLock key quite yet
			// before they resume typing, expecting its new mode to be in effect.
			// This resolves that problem by always toggling the state of a toggleable
			// key upon key-down.  If this key has just acted in its role of a prefix
			// to trigger a suffix action, toggle its state back to what it was before
			// because the firing of a hotkey should not have the side-effect of also
			// toggling the key:
			// Toggle the key by replacing this key-up event with a new sequence
			// of our own.  This entire-replacement is done so that the system
			// will see all three events in the right order:
			if (this_key.was_just_used == AS_PREFIX_FOR_HOTKEY) // If this is true, it's probably impossible for hotkey_id_with_flags to be valid by means of this_key.hotkey_to_fire_upon_release.
			{
				KEYEVENT_PHYS(KEYUP, aVK, aSC); // Mark it as physical for any other hook instances.
				KeyEvent(KEYDOWNANDUP, aVK, aSC);
				return SuppressThisKey;
			}

			// Otherwise, if it was used to modify a non-suffix key, or it was just
			// pressed and released without any keys in between, don't suppress its up-event
			// at all.  UPDATE: Don't return here if it didn't modify anything because
			// this prefix might also be a suffix. Let later sections handle it then.
			if (this_key.was_just_used == AS_PREFIX)
				return AllowKeyToGoToSystem;
		}
		else // It's not a toggleable key, or it is but it's being kept forcibly on or off.
			// If the user pressed any non-modifier key while this prefix key was held down
			// (and it wasn't already determined that a hotkey should fire, such as because
			// other modifiers are being held down), return early to avoid using this prefix
			// in its role as a suffix.
			if (this_key.was_just_used > 0  // AS_PREFIX or AS_PREFIX_FOR_HOTKEY.  v1.1.34.02: Excludes AS_PASSTHROUGH_PREFIX, which would indicate the prefix key's suffix hotkey should always fire.
				&& hotkey_id_with_flags == HOTKEY_ID_INVALID) // v1.0.44.04: Must check this because this prefix might be being used in its role as a suffix instead.  At this point id is only set if modifiers are held down.
			{
				if (this_key.as_modifiersLR // Always false if our caller is the mouse hook.
					|| fire_with_no_suppress) // Can be true due to NO_SUPPRESS_NEXT_UP_EVENT.
					return AllowKeyToGoToSystem; // Win/Alt will be disguised if needed.
				// Otherwise:
				return SuppressThisKey;
			}

		// v1.0.41: This spot cannot be reached when a disabled prefix key's up-action fires on
		// key-down instead (via Case #1).  This is because upon release, that prefix key would be
		// returned from in Case #2 (by virtue of its check of down_performed_action).

		// Since above didn't return, this key-up for this prefix key wasn't used in it's role
		// as a prefix.  If it's not a suffix, we're done, so just return.  Don't do
		// "DisguiseWinAlt" because we want the key's native key-up function to take effect.
		// Also, allow key-ups for toggleable keys that the user wants to be toggleable to
		// go through to the system, because the prior key-down for this prefix key
		// wouldn't have been suppressed and thus this up-event goes with it (and this
		// up-event is also needed by the OS, at least WinXP, to properly set the indicator
		// light and toggle state):
		if (!this_key.used_as_suffix)
			// If our caller is the mouse hook, both of the following will always be false:
			// this_key.as_modifiersLR
			// this_toggle_key_can_be_toggled
			return (this_key.as_modifiersLR
				|| fire_with_no_suppress
				|| this_toggle_key_can_be_toggled) ? AllowKeyToGoToSystem : SuppressThisKey;

		// Since the above didn't return, this key is both a prefix and a suffix, but
		// is currently operating in its capacity as a suffix.
		// If this key wasn't thought to be down prior to this up-event, it's probably because
		// it is registered with another prefix by RegisterHotkey().  In this case, the keyup
		// should be passed back to the system rather than performing it's key-up suffix
		// action.  UPDATE: This can't happen with a low-level hook.  But if there's another
		// low-level hook installed that receives events before us, and it's not
		// well-implemented (i.e. it sometimes sends ups without downs), this check
		// may help prevent unexpected behavior.  UPDATE: The check "!this_key.used_as_key_up"
		// is now done too so that an explicit key-up hotkey can operate even if the key wasn't
		// thought to be down before. One thing this helps with is certain keyboards (e.g. some
		// Dells) that generate only up events for some of their special keys but no down events,
		// even when *no* keyboard management software is installed). Some keyboards also have
		// scroll wheels that generate a stream of up events in one direction and down in the other.
		if (!(was_down_before_up || this_key.used_as_key_up)) // Verified correct.
			return AllowKeyToGoToSystem;
		//else v1.0.37.05: Since no suffix action was triggered while it was held down, fall through
		// rather than returning so that the key's own unmodified/naked suffix action will be considered.
		// For example:
		// a & b::
		// a::   // This fires upon release of "a".
	}

	////////////////////////////////////////////////////////////////////////////////////////////////////
	// CASE #4 of 4: SUFFIX key has been pressed down (or released if it's a key-up event, in which case
	// it fell through from CASE #3 or #2 above).  This case can also happen if it fell through from
	// case #1 (i.e. it already determined the value of hotkey_id_with_flags).
	////////////////////////////////////////////////////////////////////////////////////////////////////

	Hotkey *found_hk = NULL; // Custom combo hotkey found by case #4.
	if (pPrefixKey && (!aKeyUp || this_key.used_as_key_up) && hotkey_id_with_flags == HOTKEY_ID_INVALID) // Helps performance by avoiding all the below checking.
	{
		// Action here is considered first, and takes precedence since a suffix's ModifierVK/SC should
		// take effect regardless of whether any win/ctrl/alt/shift modifiers are currently down, even if
		// those modifiers themselves form another valid hotkey with this suffix.  In other words,
		// ModifierVK/SC combos take precedence over normally-modified combos:
		for (hotkey_id_temp = this_key.first_hotkey; hotkey_id_temp != HOTKEY_ID_INVALID; )
		{
			Hotkey &this_hk = *Hotkey::shk[hotkey_id_temp]; // hotkey_id_temp does not include flags in this case.
			if (!(this_hk.mModifierVK || this_hk.mModifierSC))
				break; // Not a custom combo.
			hotkey_id_temp = this_hk.mNextHotkey;
			key_type &this_modifier_key = this_hk.mModifierVK ? kvk[this_hk.mModifierVK] : ksc[this_hk.mModifierSC];
			// The following check supports the prefix+suffix pairs that have both an up hotkey and a down,
			// such as:
			//a & b::     ; Down.
			//a & b up::  ; Up.
			//MsgBox %A_ThisHotkey%
			//return
			if (this_modifier_key.is_down) // A prefix key qualified to trigger this suffix is down.
			{
				if (this_hk.mKeyUp)
				{
					if (!aKeyUp) // Key-up hotkey but the event is a down-event.
					{
						// Queue the up-hotkey for later so that the user is free to release the
						// prefix key prior to releasing the suffix (which seems quite common and
						// thus desirable).  v1.0.41: This is done even if the hotkey is subject
						// to #IfWin because it seems more correct to check those criteria at the actual time
						// the key is released rather than now:
						this_key.hotkey_to_fire_upon_release = this_hk.mID;
						if (found_hk) // i.e. a previous iteration already found the down-event to fire.
							break;
						//else continue searching for the down hotkey that goes with this up (if any).
					}
					else // this hotkey is qualified to fire.
					{
						found_hk = &this_hk;
						break;
					}
				}
				else // This is a normal hotkey that fires on suffix-key-down.
				{
					if (!aKeyUp)
					{
						if (!found_hk) // Use the first one found (especially important when both "a & Control" and "a & LControl" are present).
							found_hk = &this_hk;
						// and continue searching for the up hotkey (if any) to queue up for firing upon the key's release).
					}
					//else this key-down hotkey can't fire because the current event is a up-event.
					// But continue searching for an up-hotkey in case this key is of the type that never
					// generates down-events (e.g. certain Dell keyboards).
				}
			} // qualified prefix is down
		} // for each prefix of this suffix
		if (found_hk)
		{
			// Update pPrefixKey, even though it was probably already done close to the top of the
			// function, just in case this hotkey uses a different prefix key (perhaps because there
			// is currently more than one prefix being held down).
			// Since the hook is now designed to receive only left/right specific modifier keys
			// -- never the neutral keys -- never indicate that a neutral prefix key is down because
			// then it would never be released properly by the other main prefix/suffix handling
			// cases of the hook.  Instead, always identify which prefix key (left or right) is
			// in effect:
			switch (found_hk->mModifierVK)
			{
			case VK_SHIFT: pPrefixKey = kvk + (kvk[VK_RSHIFT].is_down ? VK_RSHIFT : VK_LSHIFT); break;
			case VK_CONTROL: pPrefixKey = kvk + (kvk[VK_RCONTROL].is_down ? VK_RCONTROL : VK_LCONTROL); break;
			case VK_MENU: pPrefixKey = kvk + (kvk[VK_RMENU].is_down ? VK_RMENU : VK_LMENU); break;
			case 0: pPrefixKey = ksc + found_hk->mModifierSC; break;
			default: pPrefixKey = kvk + found_hk->mModifierVK;
			}
			if (found_hk->mHookAction)
				hotkey_id_with_flags = found_hk->mHookAction;
			else
				hotkey_id_with_flags = found_hk->mID; // Flags not needed.
			hotkey_id_temp = hotkey_id_with_flags;
			// Let the section further below handle evaluating the hotkey's criterion, since it takes
			// care of determining suppression based on a key-down hotkey's key-up counterpart, etc.
		}

		// Alt-tab: Alt-tab actions that require a prefix key are handled directly here rather than via
		// posting a message back to the main window.  In part, this is because it would be difficult
		// to design a way to tell the main window when to release the alt-key.
		if (hotkey_id_temp == HOTKEY_ID_ALT_TAB || hotkey_id_temp == HOTKEY_ID_ALT_TAB_SHIFT)
		{
			if (pPrefixKey->was_just_used != AS_PASSTHROUGH_PREFIX)
				pPrefixKey->was_just_used = AS_PREFIX_FOR_HOTKEY;

			// Not sure if it's necessary to set this in this case.  Review.
			if (!aKeyUp)
				this_key.down_performed_action = true; // aKeyUp is known to be false due to an earlier check.
		
			if (   !(g_modifiersLR_logical & (MOD_LALT | MOD_RALT))   )  // Neither ALT key is down.
				// Note: Don't set the ignore-flag in this case because we want the hook to notice it.
				// UPDATE: It might be best, after all, to have the hook ignore these keys.  That's because
				// we want to avoid any possibility that other hotkeys will fire off while the user is
				// alt-tabbing (though we can't stop that from happening if they were registered with
				// RegisterHotkey).  In other words, since the
				// alt-tab window is in the foreground until the user released the substitute-alt key,
				// don't allow other hotkeys to be activated.  One good example that this helps is the case
				// where <key1> & rshift is defined as alt-tab but <key1> & <key2> is defined as shift-alt-tab.
				// In that case, if we didn't ignore these events, one hotkey might unintentionally trigger
				// the other.
				KeyEvent(KEYDOWN, VK_MENU);
				// And leave it down until a key-up event on the prefix key occurs.

			if ((aVK == VK_LCONTROL || aVK == VK_RCONTROL) && !aKeyUp)
				// Even though this suffix key would have been suppressed, it seems that the
				// OS's alt-tab functionality sees that it's down somehow and thus this is necessary
				// to allow the alt-tab menu to appear.  This doesn't need to be done for any other
				// modifier than Control, nor any normal key since I don't think normal keys
				// being in a down-state causes any problems with alt-tab:
				KeyEvent(KEYUP, aVK, aSC);

			// Update the prefix key's
			// flag to indicate that it was this key that originally caused the alt-key to go down,
			// so that we know to set it back up again when the key is released.  UPDATE: Actually,
			// it's probably better if this flag is set regardless of whether ALT is already down.
			// That way, in case it's state go stuck down somehow, it will be reset by an Alt-TAB
			// (i.e. alt-tab will always behave as expected even if ALT was down before starting).
			// Note: pPrefixKey must already be non-NULL or this couldn't be an alt-tab event:
			pPrefixKey->it_put_alt_down = true;
			if (hotkey_id_temp == HOTKEY_ID_ALT_TAB_SHIFT)
			{
				if (   !(g_modifiersLR_logical & (MOD_LSHIFT | MOD_RSHIFT))   ) // Neither SHIFT key is down.
					KeyEvent(KEYDOWN, VK_SHIFT);  // Same notes apply to this key.
				pPrefixKey->it_put_shift_down = true;
			}
			// And this may do weird things if VK_TAB itself is already assigned as a naked hotkey, since
			// it will recursively call the hook, resulting in the launch of some other action.  But it's hard
			// to imagine someone ever reassigning the naked VK_TAB key (i.e. with no modifiers).
			// UPDATE: The new "ignore" method should prevent that.  Or in the case of low-level hook:
			// keystrokes sent by our own app by default will not fire hotkeys.  UPDATE: Even though
			// the LL hook will have suppressed this key, it seems that the OS's alt-tab menu uses
			// some weird method (apparently not GetAsyncState(), because then our attempt to put
			// it up would fail) to determine whether the shift-key is down, so we need to still do this:
			else if (hotkey_id_temp == HOTKEY_ID_ALT_TAB) // i.e. it's not shift-alt-tab
			{
				// Force it to be alt-tab as the user intended.
				if ((aVK == VK_LSHIFT || aVK == VK_RSHIFT) && !aKeyUp)  // Needed.  See above comments. vk == VK_SHIFT not needed.
					// If a shift key is the suffix key, this must be done every time,
					// not just the first:
					KeyEvent(KEYUP, aVK, aSC);
				// UPDATE: Don't do "else" because sometimes the opposite key may be down, so the
				// below needs to be unconditional:
				//else

				// In the below cases, it's not necessary to put the shift key back down because
				// the alt-tab menu only disappears after the prefix key has been released (and
				// it's not realistic that a user would try to trigger another hotkey while the
				// alt-tab menu is visible).  In other words, the user will be releasing the
				// shift key anyway as part of the alt-tab process, so it's not necessary to put
				// it back down for the user here (the shift stays in effect as a prefix for us
				// here because it's sent as an ignore event -- but the prefix will be correctly
				// canceled when the user releases the shift key).
				if (g_modifiersLR_logical & MOD_LSHIFT)
					KeyEvent(KEYUP, VK_LSHIFT);
				if (g_modifiersLR_logical & MOD_RSHIFT)
					KeyEvent(KEYUP, VK_RSHIFT);
			}
			// Any down control key prevents alt-tab from working.  This is similar to
			// what's done for the shift-key above, so see those comments for details.
			if (g_modifiersLR_logical & MOD_LCONTROL)
				KeyEvent(KEYUP, VK_LCONTROL);
			if (g_modifiersLR_logical & MOD_RCONTROL)
				KeyEvent(KEYUP, VK_RCONTROL);

			KeyEvent(KEYDOWNANDUP, VK_TAB);

			if (hotkey_id_temp == HOTKEY_ID_ALT_TAB_SHIFT && pPrefixKey->it_put_shift_down
				&& ((aVK >= VK_NUMPAD0 && aVK <= VK_NUMPAD9) || aVK == VK_DECIMAL)) // dual-state numpad key.
			{
				// In this case, if there is a numpad key involved, it's best to put the shift key
				// back up in between every alt-tab to avoid problems caused due to the fact that
				// the shift key being down would CHANGE the VK being received when the key is
				// released (due to the fact that SHIFT temporarily disables numlock).
				KeyEvent(KEYUP, VK_SHIFT);
				pPrefixKey->it_put_shift_down = false;  // Reset for next time since we put it back up already.
			}
			pKeyHistoryCurr->event_type = 'h'; // h = hook hotkey (not one registered with RegisterHotkey)
			if (!aKeyUp)
				this_key.hotkey_down_was_suppressed = true;
			return SuppressThisKey;
		} // end of alt-tab section.
		// Since above didn't return, this isn't a prefix-triggered alt-tab action (though it might be
		// a non-prefix alt-tab action, which is handled later below).
	} // end of section that searches for a suffix modified by the prefix that's currently held down.

	if (hotkey_id_with_flags == HOTKEY_ID_INVALID)  // Since above didn't find a hotkey, check if modifiers+this_key qualifies a firing.
	{
		modifiersLRnew = g_modifiersLR_logical_non_ignored;
		if (this_key.as_modifiersLR)
			// Hotkeys are not defined to modify themselves, so look for a match accordingly.
			modifiersLRnew &= ~this_key.as_modifiersLR;
		// Check hook type too in case a script every explicitly specifies scan code zero as a hotkey:
		hotkey_id_with_flags = (aHook == g_KeybdHook && sc_takes_precedence)
			? Kscm(modifiersLRnew, aSC) : Kvkm(modifiersLRnew, aVK);
		// Bug fix for v1.0.20: The below second attempt is no longer made if the current keystroke
		// is a tab-down/up  This is because doing so causes any naked TAB that has been defined as
		// a hook hotkey to incorrectly fire when the user holds down ALT and presses tab two or more
		// times to advance through the alt-tab menu.  Here is the sequence:
		// $TAB is defined as a hotkey in the script.
		// User holds down ALT and presses TAB two or more times.
		// The Alt-tab menu becomes visible on the first TAB keystroke.
		// The $TAB hotkey fires on the second keystroke because of the below (now-fixed) logic.
		// By the way, the overall idea behind the below might be considered faulty because
		// you could argue that non-modified hotkeys should never be allowed to fire while ALT is
		// down just because the alt-tab menu is visible.  However, it seems justified because
		// the benefit (which I believe was originally and particularly that an unmodified mouse button
		// or wheel hotkey could be used to advance through the menu even though ALT is artificially
		// down due to support displaying the menu) outweighs the cost, which seems low since
		// it would be rare that anyone would press another hotkey while they are navigating through
		// the Alt-Tab menu.
		if (hotkey_id_with_flags == HOTKEY_ID_INVALID && sAltTabMenuIsVisible && aVK != VK_TAB)
		{
			// Try again, this time without the ALT key in case the user is trying to
			// activate an alt-tab related key (i.e. a special hotkey action such as AltTab
			// that relies on the Alt key being logically but not physically down).
			modifiersLRnew &= ~(MOD_LALT | MOD_RALT);
			hotkey_id_with_flags = (aHook == g_KeybdHook && sc_takes_precedence)
				? Kscm(modifiersLRnew, aSC) : Kvkm(modifiersLRnew, aVK);
			// Fix for v1.0.28: If the ID isn't an alt-tab type, don't consider it to be valid.
			// Someone pointed out that pressing Alt-Tab and then pressing ESC while still holding
			// down ALT fired the ~Esc hotkey even when it should just dismiss the alt-tab menu.
			hotkey_id_temp = hotkey_id_with_flags & HOTKEY_ID_MASK;
			if (!IS_ALT_TAB(hotkey_id_temp))
				hotkey_id_with_flags = HOTKEY_ID_INVALID; // Since it's not an Alt-tab action, don't fire this hotkey.
		}

		if (hotkey_id_with_flags & HOTKEY_KEY_UP)
		{
			if (!aKeyUp) // Key-up hotkey but the event is a down-event.
			{
				// Fixed for v1.1.33.01: Any key-up hotkey already found by the custom combo section
				// should take precedence over this hotkey.  This fixes "a up::" erroneously taking
				// precedence over "b & a up::" when "a::" is not defined, which resulted in either
				// firing the wrong hotkey or firing the right hotkey but not suppressing the key.
				if (this_key.hotkey_to_fire_upon_release == HOTKEY_ID_INVALID)
					this_key.hotkey_to_fire_upon_release = hotkey_id_with_flags; // See comments above in other occurrences of this line.
				// v1.1.33.03: ChangeHookState now avoids pairing an up hotkey with a more permissive
				// down hotkey; e.g. "<^a up" and "^a" won't be paired, since that would cause "<^a up"
				// to fire when RCtrl+A is pressed.  To support them both firing on LCtrl+A, this looks
				// for any key-down hotkey which might be elegible to fire.  It's okay if this hotkey
				// has no eligible variants, because Hotkey::CriterionFiringIsCertain will handle that.
				hotkey_id_with_flags = Hotkey::FindPairedHotkey(this_key.first_hotkey, g_modifiersLR_logical_non_ignored, false);
			}
			//else hotkey_id_with_flags contains the up-hotkey that is now eligible for firing.
		}
		else if (hotkey_id_with_flags != HOTKEY_ID_INVALID) // hotkey_id_with_flags is a valid key-down hotkey.
		{
			hotkey_id_temp = hotkey_id_with_flags & HOTKEY_ID_MASK;
			if (aKeyUp)
			{
				// Even though the key is being released, a hotkey should fire unconditionally because
				// the only way we can reach this exact point for a non-key-up hotkey is when it fell
				// through from Case #3, in which case this hotkey_id_with_flags is implicitly a key-up
				// hotkey if there is no actual explicit key-up hotkey for it.  UPDATE: It is now possible
				// to fall through from Case #2, so that is checked below.
				if (hotkey_id_temp < Hotkey::sHotkeyCount && hotkey_up[hotkey_id_temp] != HOTKEY_ID_INVALID) // Relies on short-circuit boolean order.
				{
					if (  fell_through_from_case2
						|| !(firing_is_certain = Hotkey::CriterionFiringIsCertain(hotkey_id_with_flags, aKeyUp, aExtraInfo, fire_with_no_suppress, &pKeyHistoryCurr->event_type))  )
					{
						// The key-down hotkey isn't eligible for firing, so fall back to the key-up hotkey:
						hotkey_id_with_flags = hotkey_up[hotkey_id_temp];
					}
					//else: the key-down hotkey is eligible for firing, so leave hotkey_id_with_flags as-is
					// and SuppressThisKeyFunc() or AllowIt() will post both hotkey-down and hotkey-up,
					// allowing remappings and other hotkey down-up pairs to work.
				}
				else // Leave it at its former value unless case#2.  See comments above and below.
					// Fix for v1.0.44.09: Since no key-up counterpart was found above (either in hotkey_up[]
					// or via the HOTKEY_KEY_UP flag), don't fire this hotkey when it fell through from Case #2.
					// This prevents a hotkey like $^b from firing TWICE (once on down and again on up) when a
					// key-up hotkey with different modifiers also exists, such as "#b" and "#b up" existing with $^b.
					if (fell_through_from_case2)
						hotkey_id_with_flags = HOTKEY_ID_INVALID;
			}
			else // hotkey_id_with_flags contains the down-hotkey that is now eligible for firing. But check if there's an up-event to queue up for later.
				if (hotkey_id_temp < Hotkey::sHotkeyCount)
				{
					// Fixed for v1.1.33.01: Any key-up hotkey already found by the custom combo section
					// should take precedence over this hotkey.  This fixes "b & a up::" not suppressing
					// "a" when "a::" is defined but disabled by #If and "b & a::" is not defined.
					if (this_key.hotkey_to_fire_upon_release == HOTKEY_ID_INVALID)
						this_key.hotkey_to_fire_upon_release = hotkey_up[hotkey_id_temp];
				}
		}

		// Check hotkey_id_with_flags again now that the above possibly changed it:
		if (hotkey_id_with_flags == HOTKEY_ID_INVALID)
		{
			// Even though at this point this_key is a valid suffix, no actionable ModifierVK/SC
			// or modifiers were pressed down, so just let the system process this normally
			// (except if it's a toggleable key).  This case occurs whenever a suffix key (which
			// is also a prefix) is released but the key isn't configured to perform any action
			// upon key-release.  Currently, I think the only way a key-up event will result
			// in a hotkey action is for the release of a naked/modifierless prefix key.
			// Example of a configuration that would result in this case whenever Rshift alone
			// is pressed then released:
			// RControl & RShift = Alt-Tab
			// RShift & RControl = Shift-Alt-Tab
			if (aKeyUp)
				// These sequence is basically the same as the one used in Case #3
				// when a prefix key that isn't a suffix failed to modify anything
				// and was then released, so consider any modifications made here
				// or there for inclusion in the other one.  UPDATE: Since
				// the previous sentence is a bit obsolete, describe this better:
				// If it's a toggleable key that the user wants to allow to be
				// toggled, just allow this up-event to go through because the
				// previous down-event for it (in its role as a prefix) would not
				// have been suppressed:
				return (this_key.as_modifiersLR
					// The following line was added for v1.0.37.02 to take into account key-up hotkeys,
					// the release of which should never be suppressed if it didn't actually fire the
					// up-hotkey (due to the wrong modifiers being down):
					|| !this_key.used_as_prefix
					|| fire_with_no_suppress
					// The order on this line important; it relies on short-circuit boolean:
					|| this_toggle_key_can_be_toggled) ? AllowKeyToGoToSystem : SuppressThisKey;
				// v1.0.37.02: Added !this_key.used_as_prefix for mouse hook too (see comment above).

			// For execution to have reached this point, the following must be true:
			// 1) aKeyUp==false
			// 2) this_key is not a prefix, or it is also a suffix but some other custom prefix key
			//    is being held down (otherwise, Case #1 would have returned).
			// 3) No hotkey is eligible to fire.
			// If this_key is a prefix under these conditions, there are some combinations that are
			// inconsistent with Case #1.  Case #1 would pass it through if it has no enabled suffixes,
			// or it's a modifier/toggleable key, but otherwise would suppress it.  By contrast, this
			// section would unconditionally pass through a prefix key if the user was already holding
			// another prefix key.  Just suppressing it doesn't seem useful since it still wouldn't
			// function as a prefix key (since case #1 didn't set pPrefixKey to this_key), and fixing
			// that would change the behaviour in ways that might be undesired, so it's left as is.
			if (this_key.hotkey_to_fire_upon_release == HOTKEY_ID_INVALID)
				return AllowKeyToGoToSystem;
			// Otherwise (v1.0.44): Since there is a hotkey to fire upon release (somewhat rare under these conditions),
			// check if any of its criteria will allow it to fire, and if so whether that variant is non-suppressed.
			// If it is, this down-event should be non-suppressed too (for symmetry).  This check isn't 100% reliable
			// because the active/existing windows checked by the criteria might change before the user actually
			// releases the key, but there doesn't seem any way around that.
			if (!Hotkey::CriterionFiringIsCertain(this_key.hotkey_to_fire_upon_release
				, true  // Always a key-up since it will fire upon release.
				, aExtraInfo // May affect the result due to #InputLevel.  Assume the key-up's SendLevel will be the same as the key-down.
				, fire_with_no_suppress, NULL)) // fire_with_no_suppress is the value we really need to get back from it.
				fire_with_no_suppress = true; // Although it's not "firing" in this case; just for use below.
			this_key.hotkey_down_was_suppressed = !fire_with_no_suppress; // Fixed for v1.1.33.01: If this isn't set, the key-up won't be suppressed even after the key-down is.
			return fire_with_no_suppress ? AllowKeyToGoToSystem : SuppressThisKey;
		}
		//else an eligible hotkey was found.
	} // Final attempt to find hotkey based on suffix have the right combo of modifiers.

	// Since above didn't return, hotkey_id_with_flags is now a valid hotkey.  The only thing that can
	// stop it from firing now is CriterionFiringIsCertain().

	// v1.0.41: This must be done prior to the setting of sDisguiseNextMenu below.
	hotkey_id_temp = hotkey_id_with_flags & HOTKEY_ID_MASK;
	if (hotkey_id_temp < Hotkey::sHotkeyCount // i.e. don't call the below for Alt-tab hotkeys and similar.
		&& !firing_is_certain  // i.e. CriterionFiringIsCertain() wasn't already called earlier.
		&& !(firing_is_certain = Hotkey::CriterionFiringIsCertain(hotkey_id_with_flags, aKeyUp, aExtraInfo, fire_with_no_suppress, &pKeyHistoryCurr->event_type)))
	{
		if (pKeyHistoryCurr->event_type == 'i') // This non-zero SendLevel event is being ignored due to #InputLevel, so unconditionally pass it through, like with is_ignored.
			return AllowKeyToGoToSystem;
		// v1.1.08: Although the hotkey corresponding to this event is disabled, it may need to
		// be suppressed if it has a counterpart (key-down or key-up) hotkey which is enabled.
		// This can be broken down into two cases:
		//  1) This is a key-up event and the key-down event was already suppressed.
		//     Prior to v1.1.08, the key-up was passed through; this caused problems in a
		//     few specific cases, such as XButton1 and XButton2 (which act when released).
		//  2) This is a key-down event, but there is also a key-up hotkey which is enabled.
		//     In that case, the documentation indicates the key-down will be suppressed.
		//     Prior to v1.1.08, neither event was suppressed.
		if (aKeyUp)
			return this_key.hotkey_down_was_suppressed ? SuppressThisKey : AllowKeyToGoToSystem;
		if (this_key.hotkey_to_fire_upon_release == HOTKEY_ID_INVALID)
			return AllowKeyToGoToSystem;
		// Otherwise, this is a key-down event with a corresponding key-up hotkey.
		fire_with_no_suppress = false; // Reset it for the check below.
		// This check should be identical to the section above dealing with hotkey_to_fire_upon_release:
		firing_is_certain = Hotkey::CriterionFiringIsCertain(this_key.hotkey_to_fire_upon_release
			, true  // Always a key-up since it will fire upon release.
			, aExtraInfo // May affect the result due to #InputLevel.  Assume the key-up's SendLevel will be the same as the key-down.
			, fire_with_no_suppress, NULL); // fire_with_no_suppress is the value we really need to get back from it.
		if (!firing_is_certain || fire_with_no_suppress)
		{
			// If the conditions change and allow the key-up hotkey to fire, make sure not to suppress it.
			this_key.no_suppress |= NO_SUPPRESS_NEXT_UP_EVENT;
			return AllowKeyToGoToSystem;
		}
		// Both this down event and the corresponding up event should be suppressed.
		ASSERT(!(this_key.no_suppress & NO_SUPPRESS_NEXT_UP_EVENT));
		this_key.hotkey_down_was_suppressed = true;
		return SuppressThisKey;
	}
	hotkey_id_temp = hotkey_id_with_flags & HOTKEY_ID_MASK; // Update in case CriterionFiringIsCertain() changed the naked/raw ID.
	
	// If pPrefixKey is part of the reason for this hotkey firing, update was_just_used
	// so that when the prefix key is released, it won't perform its key-up action.
	// To match the behaviour prior to v1.1.37, this is done on key-up for custom combos
	// but not standard hotkeys.  Note that if there are multiple key-up hotkeys with
	// different modifier combinations, the one that fires might depend on the modifier
	// state at the time the key was pressed, rather than when it was released.  In other
	// words, pPrefixKey may be unrelated to the key-up hotkey if it is a standard modifier.
	if (pPrefixKey && (found_hk || pPrefixKey->as_modifiersLR && !aKeyUp)
		&& pPrefixKey->was_just_used != AS_PASSTHROUGH_PREFIX)
		pPrefixKey->was_just_used = AS_PREFIX_FOR_HOTKEY;

	// Now above has ensured that everything is in place for an action to be performed.
	// Determine the final ID at this late stage to improve maintainability:
	HotkeyIDType hotkey_id_to_fire = hotkey_id_temp;

	// Check if the WIN or ALT key needs to be masked:
	if (   (g_modifiersLR_logical & (MOD_LALT|MOD_RALT|MOD_LWIN|MOD_RWIN)) // ALT and/or WIN is down.
		&& !fire_with_no_suppress // This hotkey will be suppressed (hotkeys with ~no-suppress should not require masking).
		&& (sUndisguisedMenuInEffect || aHook == g_MouseHook)   ) // Menu has not already been disguised (as tracked by the keyboard hook), or this is the mouse hook, which may require masking anyway.
	{
		// If only a windows key was held down to activate this hotkey, suppress the next win-up
		// event so that the Start Menu won't appear.  The appearance of the Start Menu would be
		// caused by the fact that the hotkey's suffix key was suppressed, therefore the OS doesn't
		// see that the WIN key "modified" anything while it was held down.
		// Although having other modifiers present prevents the Start Menu from appearing, that's
		// handled by later checks since the WIN key can auto-repeat, putting an unmodified WIN
		// back into effect after the other mods are released.  This only happens if the WIN key
		// is the most recently pressed physical key, such as if this hotkey is a mouse button.
		// When the user finally releases the WIN key, that release will be disguised if called
		// for by the logic below and in AllowIt().
		if (   (g_modifiersLR_logical & (MOD_LWIN|MOD_RWIN)) // One or both are down and may require disguising.
			&& ((g_modifiersLR_physical & (MOD_LWIN|MOD_RWIN)) // Always disguise if physically down, for backward compatibility.
				|| Hotkey::HotkeyRequiresModLR(hotkey_id_to_fire, MOD_LWIN|MOD_RWIN))   ) // If not physically down, avoid masking hotkeys which could be intended to send {LWin up}, such as for AppsKey::RWin.
		{
			sDisguiseNextMenu = true;
			// An earlier stage has ensured that the keyboard hook is installed for suppression of LWin/RWin if
			// this is a mouse hotkey, because the sending of CTRL directly (here) would otherwise not suppress
			// the Start Menu (though it does supress menu bar activation for ALT hotkeys, as described below).
		}

		// For maximum reliability on the maximum range of systems, it seems best to do the above
		// for ALT keys also, to prevent them from invoking the icon menu or menu bar of the
		// foreground window (rarer than the Start Menu problem, above, I think).
		// Update for v1.0.25: This is usually only necessary for hotkeys whose only modifier is ALT.
		// For example, Shift-Alt hotkeys do not need it if Shift is pressed after Alt because Alt
		// "modified" the shift so the OS knows it's not a naked ALT press to activate the menu bar.
		// Conversely, if Shift is pressed prior to Alt, but released before Alt, I think the shift-up
		// counts as a "modification" and the same rule applies.  However, if shift is released after Alt,
		// that would activate the menu bar unless the ALT key is disguised below.  This issue does
		// not apply to the WIN key above because apparently it is disguised automatically
		// whenever some other modifier was involved with it in any way and at any time during the
		// keystrokes that comprise the hotkey.
		if (   !sDisguiseNextMenu // It's not already going to be disguised due to the section above or a previous hotkey.
			&& (g_modifiersLR_logical & (MOD_LALT|MOD_RALT)) // If RAlt==AltGr, it should never need disguising, but in that case LCtrl is also down, so ActiveWindowLayoutHasAltGr() isn't checked.
			&& !(g_modifiersLR_logical & (MOD_LCONTROL|MOD_RCONTROL)) // No need to mask if Ctrl is down (the key-repeat issue that affects the WIN key does not affect ALT).
			&& ((g_modifiersLR_physical & (MOD_LALT|MOD_RALT)) // Always disguise if physically down, for backward compatibility.
				|| Hotkey::HotkeyRequiresModLR(hotkey_id_to_fire, MOD_LALT|MOD_RALT))   ) // If not physically down, avoid masking hotkeys which could be intended to send {Alt up}, such as for AppsKey::Alt.
		{
			if (g_KeybdHook)
				sDisguiseNextMenu = true;
			else
				// Since no keyboard hook, no point in setting the variable because it would never be acted upon.
				// Instead, disguise the key now with a CTRL keystroke. Note that this is not done for
				// mouse buttons that use the WIN key as a prefix because it does not work reliably for them
				// (i.e. sometimes the Start Menu appears, even if two CTRL keystrokes are sent rather than one).
				// Therefore, as of v1.0.25.05, mouse button hotkeys that use only the WIN key as a modifier cause
				// the keyboard hook to be installed.  This determination is made during the hotkey loading stage.
				KeyEventMenuMask(KEYDOWNANDUP);
		}
	}

	switch (hotkey_id_to_fire)
	{
		case HOTKEY_ID_ALT_TAB_MENU_DISMISS: // This case must occur before HOTKEY_ID_ALT_TAB_MENU due to non-break.
			if (!sAltTabMenuIsVisible)
				return AllowKeyToGoToSystem;  // Let the key do its native function.
			// else fall through to the next case.
		case HOTKEY_ID_ALT_TAB_MENU:  // These cases must occur before the Alt-tab ones due to conditional break.
		case HOTKEY_ID_ALT_TAB_AND_MENU:
		{
			vk_type which_alt_down = 0;
			if (g_modifiersLR_logical & MOD_LALT)
				which_alt_down = VK_LMENU;
			else if (g_modifiersLR_logical & MOD_RALT)
				which_alt_down = VK_RMENU;

			if (sAltTabMenuIsVisible)  // Can be true even if which_alt_down is zero.
			{
				if (hotkey_id_to_fire != HOTKEY_ID_ALT_TAB_AND_MENU) // then it is MENU or DISMISS.
				{
					// Since it is possible for the menu to be visible when neither ALT
					// key is down, always send an alt-up event if one isn't down
					// so that the menu is dismissed as intended:
					KeyEvent(KEYUP, which_alt_down ? which_alt_down : VK_MENU);
					if (this_key.as_modifiersLR && aVK != VK_LWIN && aVK != VK_RWIN && !aKeyUp)
						// Something strange seems to happen with the foreground app
						// thinking the modifier is still down (even though it was suppressed
						// entirely [confirmed!]).  For example, if the script contains
						// the line "lshift::AltTabMenu", pressing lshift twice would
						// otherwise cause the newly-activated app to think the shift
						// key is down.  Sending an extra UP here seems to fix that,
						// hopefully without breaking anything else.  Note: It's not
						// done for Lwin/Rwin because most (all?) apps don't care whether
						// LWin/RWin is down, and sending an up event might risk triggering
						// the start menu in certain hotkey configurations.  This policy
						// might not be the right one for everyone, however:
						KeyEvent(KEYUP, aVK); // Can't send sc here since it's not defined for the mouse hook.
					sAltTabMenuIsVisible = false;
					break;
				}
				// else HOTKEY_ID_ALT_TAB_AND_MENU, do nothing (don't break) because we want
				// the switch to fall through to the Alt-Tab case.
			}
			else // alt-tab menu is not visible
			{
				// Unlike CONTROL, SHIFT, AND ALT, the LWIN/RWIN keys don't seem to need any
				// special handling to make them work with the alt-tab features.

				bool vk_is_alt = aVK == VK_LMENU || aVK == VK_RMENU;  // Translated & no longer needed: || vk == VK_MENU;
				bool vk_is_shift = aVK == VK_LSHIFT || aVK == VK_RSHIFT;  // || vk == VK_SHIFT;
				bool vk_is_control = aVK == VK_LCONTROL || aVK == VK_RCONTROL;  // || vk == VK_CONTROL;

				vk_type which_shift_down = 0;
				if (g_modifiersLR_logical & MOD_LSHIFT)
					which_shift_down = VK_LSHIFT;
				else if (g_modifiersLR_logical & MOD_RSHIFT)
					which_shift_down = VK_RSHIFT;
				else if (!aKeyUp && vk_is_shift)
					which_shift_down = aVK;

				vk_type which_control_down = 0;
				if (g_modifiersLR_logical & MOD_LCONTROL)
					which_control_down = VK_LCONTROL;
				else if (g_modifiersLR_logical & MOD_RCONTROL)
					which_control_down = VK_RCONTROL;
				else if (!aKeyUp && vk_is_control)
					which_control_down = aVK;

				bool shift_put_up = false;
				if (which_shift_down)
				{
					KeyEvent(KEYUP, which_shift_down);
					shift_put_up = true;
				}

				bool control_put_up = false;
				if (which_control_down)
				{
					// In this case, the control key must be put up because the OS, at least
					// WinXP, knows the control key is down even though the down event was
					// suppressed by the hook.  So put it up and leave it up, because putting
					// it back down would cause it to be down even after the user releases
					// it (since the up-event of a hotkey is also suppressed):
					KeyEvent(KEYUP, which_control_down);
					control_put_up = true;
				}

				// Alt-tab menu is not visible, or was not made visible by us.  In either case,
				// try to make sure it's displayed:
				// Don't put alt down if it's already down, it might mess up cases where the
				// ALT key itself is assigned to be one of the alt-tab actions:
				if (vk_is_alt)
					if (aKeyUp)
						// The system won't see it as down for the purpose of alt-tab, so remove this
						// modifier from consideration.  This is necessary to allow something like this
						// to work:
						// LAlt & WheelDown::AltTab
						// LAlt::AltTabMenu   ; Since LAlt is a prefix key above, it will be a key-up hotkey here.
						which_alt_down = 0;
					else // Because there hasn't been a chance to update g_modifiersLR_logical yet:
						which_alt_down = aVK;
				if (!which_alt_down)
					KeyEvent(KEYDOWN, VK_MENU);

				KeyEvent(KEYDOWNANDUP, VK_TAB); // v1.0.28: KEYDOWNANDUP vs. KEYDOWN.

				// Only put it put it back down if it wasn't the hotkey itself, because
				// the system would never have known it was down because the down-event
				// on the hotkey would have been suppressed.  And since the up-event
				// will also be suppressed, putting it down like this would result in
				// it being permanently down even after the user releases the key!:
				if (shift_put_up && !vk_is_shift) // Must do this regardless of the value of aKeyUp.
					KeyEvent(KEYDOWN, which_shift_down);
				
				// Update: Can't do this one because going down on control will instantly
				// dismiss the alt-tab menu, which we don't want if we're here.
				//if (control_put_up && !vk_is_control) // Must do this regardless of the value of aKeyUp.
				//	KeyEvent(KEYDOWN, which_control_down);

				// At this point, the alt-tab menu has displayed and advanced by one icon
				// (to the next window in the z-order).  Rather than sending a shift-tab to
				// go back to the first icon in the menu, it seems best to leave it where
				// it is because usually the user will want to go forward at least one item.
				// Going backward through the menu is a lot more rare for most people.
				sAltTabMenuIsVisible = true;
				break;
			}
		}
		case HOTKEY_ID_ALT_TAB:
		case HOTKEY_ID_ALT_TAB_SHIFT:
		{
			// Since we're here, this ALT-TAB hotkey didn't have a prefix or it would have
			// already been handled and we would have returned above.  Therefore, this
			// hotkey is defined as taking effect only if the alt-tab menu is currently
			// displayed, otherwise it will just be passed through to perform it's native
			// function.  Example:
			// MButton::AltTabMenu
			// WheelDown::AltTab     ; But if the menu is displayed, the wheel will function normally.
			// WheelUp::ShiftAltTab  ; But if the menu is displayed, the wheel will function normally.
			if (!sAltTabMenuIsVisible)
				return AllowKeyToGoToSystem;

			// Unlike CONTROL, SHIFT, AND ALT, the LWIN/RWIN keys don't seem to need any
			// special handling to make them work with the alt-tab features.

			// Must do this to prevent interference with Alt-tab when these keys
			// are used to do the navigation.  Don't put any of these back down
			// after putting them up since that would probably cause them to become
			// stuck down due to the fact that the user's physical release of the
			// key will be suppressed (since it's a hotkey):
			if (!aKeyUp && (aVK == VK_LCONTROL || aVK == VK_RCONTROL || aVK == VK_LSHIFT || aVK == VK_RSHIFT))
				// Don't do the ALT key because it causes more problems than it solves
				// (in fact, it might not solve any at all).
				KeyEvent(KEYUP, aVK); // Can't send sc here since it's not defined for the mouse hook.

			// Even when the menu is visible, it's possible that neither of the ALT keys
			// is down (such as if Ctrl+Alt+Tab was used, and perhaps other cases):
			if (   !(g_modifiersLR_logical & (MOD_LALT | MOD_RALT))   // Neither ALT key is down 
				|| (aKeyUp && (aVK == VK_LMENU || aVK == VK_RMENU))   ) // Or the suffix key for Alt-tab *is* an ALT key and it's being released: must push ALT down for upcoming TAB to work.
				KeyEvent(KEYDOWN, VK_MENU);
				// And never put it back up because that would dismiss the menu.
			// Otherwise, use keystrokes to navigate through the menu:
			bool shift_put_down = false;
			if (hotkey_id_to_fire == HOTKEY_ID_ALT_TAB_SHIFT && !(g_modifiersLR_logical & (MOD_LSHIFT | MOD_RSHIFT))) // Neither SHIFT key is down.
			{
				KeyEvent(KEYDOWN, VK_SHIFT);
				shift_put_down = true;
			}
			KeyEvent(KEYDOWNANDUP, VK_TAB);
			if (shift_put_down)
				KeyEvent(KEYUP, VK_SHIFT);
			break;
		}
		default:
			// Notify the main thread (via its main window) of which hotkey has been pressed.
			// Post the message rather than sending it, because Send would need
			// SendMessageTimeout(), which is undesirable because the whole point of
			// making this hook thread separate from the main thread is to have it be
			// maximally responsive (especially to prevent mouse cursor lag).
			// v1.0.42: The hotkey variant is not passed via the message below because
			// upon receipt of the message, the variant is recalculated in case conditions
			// have changed between msg-post and arrival.  See comments in the message loop for details.
			// v1.0.42.01: the message is now posted at the latest possible moment to avoid
			// situations in which the message arrives and is processed by the main thread
			// before we finish processing the hotkey's final keystroke here.  This avoids
			// problems with a script calling GetKeyState() and getting an inaccurate value
			// because the hook thread is either pre-empted or is running in parallel
			// (multiprocessor) and hasn't yet returned 1 or 0 to determine whether the final
			// keystroke is suppressed or passed through to the active window.  Similarly, this solves
			// the fact that previously, g_PhysicalKeyState was not updated for modifier keys until after
			// the hotkey message was posted, which on some PCs caused the hotkey subroutine to see
			// the wrong key state via KeyWait (which defaults to detecting the physical key state).
			// For example, the following hotkeys would be a problem on certain PCs, presumably due to
			// split-second timing where the hook thread gets preempted and the main thread gets a
			// timeslice that allows it to launch a script subroutine before the hook can get
			// another timeslice to finish up:
			//$LAlt::
			//if not GetKeyState("LAlt", "P")
			//	ToolTip `nProblem 1`n
			//return
			//
			//~LControl::
			//if not (DllCall("GetAsyncKeyState", int, 0xA2) & 0x8000)
			//    ToolTip `nProblem 2`n
			//return
			hotkey_id_to_post = hotkey_id_to_fire; // Set this only when it is certain that this ID should be sent to the main thread via msg.
			if (firing_is_certain->mHotCriterion && HOT_IF_REQUIRES_EVAL(firing_is_certain->mHotCriterion->Type))
			{
				// To avoid evaluating the expression twice, indicate to the main thread that the appropriate variant
				// has already been determined, by packing the variant's index into the high word of the param:
				hotkey_id_to_post |= firing_is_certain->mIndex << 16;
			}
			// Otherwise CriterionFiringIsCertain() might have returned a global variant (not necessarily the one
			// that will actually fire), so if we ever decide to do the above for other criterion types rather than
			// just re-evaluating the criterion later, must make sure not to send the mIndex of a global variant.
			//if (firing_is_certain->mHotCriterion) // i.e. a specific variant has already been determined.
	}

	pKeyHistoryCurr->event_type = 'h'; // h = hook hotkey (not one registered with RegisterHotkey)

	if (this_toggle_key_can_be_toggled && aKeyUp && this_key.used_as_prefix)
	{
		// In this case, since all the above conditions are true, the key-down
		// event for this key-up (which fired a hotkey) would not have been
		// suppressed.  Thus, we should toggle the state of the key back
		// the what it was before the user pressed it (due to the policy that
		// the natural function of a key should never take effect when that
		// key is used as a hotkey suffix).  You could argue that instead
		// of doing this, we should change *pForceToggle's value to make the
		// key untoggleable whenever it's both a prefix and a naked
		// (key-up triggered) suffix.  However, this isn't too much harder
		// and has the added benefit of allowing the key to be toggled if
		// a modifier is held down before it (e.g. alt-CapsLock would then
		// be able to toggle the CapsLock key):
		KEYEVENT_PHYS(KEYUP, aVK, aSC); // Mark it as physical for any other hook instances.
		KeyEvent(KEYDOWNANDUP, aVK, aSC);
		return SuppressThisKey;
	}

	if (aKeyUp)
	{
		if (this_key.as_modifiersLR)
			// Since this hotkey is fired on a key-up event, and since it's a modifier, must
			// not suppress the key because otherwise the system's state for this modifier
			// key would be stuck down due to the fact that the previous down-event for this
			// key (which is presumably a prefix *and* a suffix) was not suppressed. UPDATE:
			// For v1.0.28, if the new field hotkey_down_was_suppressed is true, also suppress
			// this up event, one purpose of which is to allow a pair of remappings such
			// as the following to display the Start Menu (because otherwise the non-suppressed
			// Alt key events would prevent it):
			// *LAlt up::Send {LWin up}
			// *LAlt::Send {LWin down}
			return this_key.hotkey_down_was_suppressed ? SuppressThisKey : AllowKeyToGoToSystem;

		if (fire_with_no_suppress) // Plus we know it's not a modifier since otherwise it would've returned above.
		{
			// Although it seems more sensible to suppress the key-up if the key-down was suppressed,
			// it probably does no harm to let the key-up pass through, and in this case, it's exactly
			// what the script is asking to happen (by prefixing the key-up hotkey with '~').
			// this_key.pForceToggle isn't checked because AllowIt() handles that.
			return AllowKeyToGoToSystem;
		} // No suppression.
	}
	else // Key Down
	{
		// Do this only for DOWN (not UP) events that triggered an action:
		this_key.down_performed_action = true;

		if (fire_with_no_suppress)
		{
			// Since this hotkey is firing on key-down but the user specified not to suppress its native
			// function, substitute an DOWN+UP pair of events for this event, since we want the
			// DOWN to precede the UP.  It's necessary to send the UP because the user's physical UP
			// will be suppressed automatically when this function is called for that event.
			// UPDATE: The below method causes side-effects due to the fact that it is simulated
			// input vs. physical input, e.g. when used with the Input command, which distinguishes
			// between "ignored" and physical input.  Therefore, let this down event pass through
			// and set things up so that the corresponding up-event is also not suppressed:
			//KeyEvent(KEYDOWNANDUP, aVK, aSC);
			// No longer relevant due to the above change:
			// Now let it just fall through to suppress this down event, because we can't use it
			// since doing so would result in the UP event having preceded the DOWN, which would
			// be the wrong order.
			this_key.no_suppress |= NO_SUPPRESS_NEXT_UP_EVENT;
			return AllowKeyToGoToSystem;
		}
		// Fix for v1.1.37.02 and v2.0.6: The following is also done for LWin/RWin because otherwise,
		// the system does not generate WM_SYSKEYDOWN (or even WM_KEYDOWN) messages for combinations
		// that correspond to some global hotkeys, even though they aren't actually triggering global
		// hotkeys because the logical key state doesn't match.  For example, with LWin::Alt, LWin-T
		// would not activate the Tools menu on a menu bar.
		// Fixes for v1.1.37.02 and v2.0.8:
		//  1) Apply this to Ctrl hotkeys because otherwise, the OS thinks Ctrl is being held down
		//     and therefore translates Alt-key combinations to WM_KEYDOWN instead of WM_SYSKEYDOWN.
		//     (confirmed on Windows 7, but might not be necessary on Windows 11).
		//  2) Apply this to Shift as well for simplicity and consistency.  Although this hasn't been
		//     confirmed, it might be necessary for correct system handling in some cases, such as with
		//     certain language-switching hotkeys, IME or advanced keyboard layouts.
		//  3) Don't apply this if the modifier is logically down, since in that case the system *should*
		//     consider the key to be held down.  For example, pressing Ctrl+Alt should produce WM_KEYDOWN,
		//     but if the system thinks Ctrl has been released, it will instead produce WM_SYSKEYDOWN.
		//     This was confirmed necessary for LCtrl::Alt and LAlt::LCtrl to work correctly on Windows 7.
		else if (this_key.as_modifiersLR & ~g_modifiersLR_logical)
		{
			// Fix for v1.1.26.01: Added KEY_BLOCK_THIS to suppress the Alt key-up, which fixes an issue
			// which could be reproduced as follows:
			//  - Test with an Alt-blocking hotkey such as LAlt::return or LAlt::LCtrl.
			//  - Open Notepad and alt-tab away using the other Alt key or a remapping such as LCtrl::LAlt.
			//  - Reactivate Notepad and note that the keyboard accelerators (underlined letters) are still
			//    visible in the menus (usually).
			//  - Press LAlt and the menus are activated once, even though LAlt is supposed to be blocked.
			// Additionally, a Windows 10 check was added because the original issue this workaround was
			// intended for doesn't appear to occur on Windows 10 (tested on 10.0.15063).  This check was
			// removed for v1.1.27.00 to ensure consistent behaviour of AltGr hotkeys across OS versions.
			// (Sending RAlt up on a layout with AltGr causes the system to send LCtrl up.)
			// Testing on XP, Vista and 8.1 showed that the #LAlt issue below only occurred if the key-up
			// was allowed to pass through to the active window.  It appeared to be a non-issue on Win 10
			// even when the Alt key-up was passed through.
			// Fix for v1.0.34: For some reason, the release of the ALT key here causes the Start Menu
			// to appear instantly for the hotkey #LAlt (and probably #RAlt), even when the hotkey does
			// nothing other than return.  This seems like an OS quirk since it doesn't conform to any
			// known Start Menu activation sequence.  This happens only when neither Shift nor Control is
			// down.  To work around it, send the menu-suppressing Control keystroke here.  Another one
			// will probably be sent later when the WIN key is physically released, but it seems best
			// for simplicity and avoidance of side-effects not to make this one prevent that one.
			//if (   (g_modifiersLR_logical & (MOD_LWIN | MOD_RWIN))   // At least one WIN key is down.
			//	&& !(g_modifiersLR_logical & (MOD_LSHIFT | MOD_RSHIFT | MOD_LCONTROL | MOD_RCONTROL))   ) // But no SHIFT or CONTROL key is down to help us.
			//	KeyEventMenuMask(KEYDOWNANDUP);
			// Since this is a hotkey that fires on ALT-DOWN and it's a normal (suppressed) hotkey,
			// send an up-event to "turn off" the OS's low-level handling for the alt key with
			// respect to having it modify keypresses.  For example, the following hotkeys would
			// fail to work properly without this workaround because the OS apparently sees that
			// the ALT key is physically down even though it is not logically down:
			// RAlt::Send f  ; Actually triggers !f, which activates the FILE menu if the active window has one.
			// RAlt::Send {PgDn}  ; Fails to work because ALT-PgDn usually does nothing.
			KeyEvent(KEYUP, aVK, aSC, NULL, false, KEY_BLOCK_THIS);
		}
	}
	
	// Otherwise:
	if (!aKeyUp)
		this_key.hotkey_down_was_suppressed = true;
	return SuppressThisKey;
}



LRESULT SuppressThisKeyFunc(const HHOOK aHook, LPARAM lParam, const vk_type aVK, const sc_type aSC, bool aKeyUp
	, ULONG_PTR aExtraInfo, KeyHistoryItem *pKeyHistoryCurr, WPARAM aHotkeyIDToPost, WPARAM aHSwParamToPost, LPARAM aHSlParamToPost)
// Always use the parameter vk rather than event.vkCode because the caller or caller's caller
// might have adjusted vk, namely to make it a left/right specific modifier key rather than a
// neutral one.
{
	if (pKeyHistoryCurr->event_type == ' ') // then it hasn't been already set somewhere else
		pKeyHistoryCurr->event_type = 's';
	// This handles the troublesome Numlock key, which on some (most/all?) keyboards
	// will change state independent of the keyboard's indicator light even if its
	// keydown and up events are suppressed.  This is certainly true on the
	// MS Natural Elite keyboard using default drivers on WinXP.  SetKeyboardState()
	// doesn't resolve this, so the only alternative to the below is to use the
	// Win9x method of setting the Numlock state explicitly whenever the key is released.
	// That might be complicated by the fact that the unexpected state change described
	// here can't be detected by GetKeyboardState() and such (it sees the state indicated
	// by the numlock light on the keyboard, which is wrong).  In addition, doing it this
	// way allows Numlock to be a prefix key for something like Numpad7, which would
	// otherwise be impossible because Numpad7 would become NumpadHome the moment
	// Numlock was pressed down.  Note: this problem doesn't appear to affect Capslock
	// or Scrolllock for some reason, possibly hardware or driver related.
	// Note: the check for KEY_IGNORE isn't strictly necessary, but here just for safety
	// in case this is ever called for a key that should be ignored.  If that were
	// to happen and we didn't check for it, and endless loop of keyboard events
	// might be caused due to the keybd events sent below.
	if (aHook == g_KeybdHook)
	{
		KBDLLHOOKSTRUCT &event = *(PKBDLLHOOKSTRUCT)lParam;
		if (aVK == VK_NUMLOCK && !aKeyUp && !IsIgnored(event.dwExtraInfo))
		{
			// This seems to undo the faulty indicator light problem and toggle
			// the key back to the state it was in prior to when the user pressed it.
			// Originally, I had two keydowns and before that some keyups too, but
			// testing reveals that only a single key-down is needed.  UPDATE:
			// It appears that all 4 of these key events are needed to make it work
			// in every situation, especially the case when ForceNumlock is on but
			// numlock isn't used for any hotkeys.
			// Note: The only side-effect I've discovered of this method is that the
			// indicator light can't be toggled after the program is exitted unless the
			// key is pressed twice:
			KeyEvent(KEYUP, VK_NUMLOCK);
			KeyEvent(KEYDOWNANDUP, VK_NUMLOCK);
			KeyEvent(KEYDOWN, VK_NUMLOCK);
		}
		UpdateKeybdState(event, aVK, aSC, aKeyUp, true);
	}

#ifdef ENABLE_KEY_HISTORY_FILE
	if (g_KeyHistoryToFile)
		KeyHistoryToFile(NULL, pKeyHistoryCurr->event_type, pKeyHistoryCurr->key_up
			, pKeyHistoryCurr->vk, pKeyHistoryCurr->sc);  // A fairly low overhead operation.
#endif

	// These should be posted only at the last possible moment before returning in order to
	// minimize the chance that the main thread will receive and process the message before
	// our thread can finish updating key states and other maintenance.  This has been proven
	// to be a problem on single-processor systems when the hook thread gets preempted
	// before it can return.  Apparently, the fact that the hook thread is much higher in priority
	// than the main thread is not enough to prevent the main thread from getting a timeslice
	// before the hook thread gets back another (at least on some systems, perhaps due to their
	// system settings of the same ilk as "favor background processes").
	if (aHotkeyIDToPost != HOTKEY_ID_INVALID)
	{
		int input_level = InputLevelFromInfo(aExtraInfo);
		PostMessage(g_hWnd, AHK_HOOK_HOTKEY, aHotkeyIDToPost, MAKELONG(pKeyHistoryCurr->sc, input_level)); // v1.0.43.03: sc is posted currently only to support the number of wheel turns (to store in A_EventInfo).
		if (aKeyUp && hotkey_up[aHotkeyIDToPost & HOTKEY_ID_MASK] != HOTKEY_ID_INVALID)
		{
			// This is a key-down hotkey being triggered by releasing a prefix key.
			// There's also a corresponding key-up hotkey, so fire it too:
			PostMessage(g_hWnd, AHK_HOOK_HOTKEY, hotkey_up[aHotkeyIDToPost & HOTKEY_ID_MASK], MAKELONG(pKeyHistoryCurr->sc, input_level));
		}
	}
	if (aHSwParamToPost != HOTSTRING_INDEX_INVALID)
		PostMessage(g_hWnd, AHK_HOTSTRING, aHSwParamToPost, aHSlParamToPost);
	return 1;
}



LRESULT AllowIt(const HHOOK aHook, int aCode, WPARAM wParam, LPARAM lParam, const vk_type aVK, const sc_type aSC
	, bool aKeyUp, ULONG_PTR aExtraInfo, KeyHistoryItem *pKeyHistoryCurr, WPARAM aHotkeyIDToPost)
// Always use the parameter vk rather than event.vkCode because the caller or caller's caller
// might have adjusted vk, namely to make it a left/right specific modifier key rather than a
// neutral one.
{
	WPARAM hs_wparam_to_post = HOTSTRING_INDEX_INVALID; // Set default.
	LPARAM hs_lparam_to_post; // Not initialized because the above is the sole indicator of whether its contents should even be examined.

	// Prevent toggleable keys from being toggled (if the user wanted that) by suppressing it.
	// Seems best to suppress key-up events as well as key-down, since a key-up by itself,
	// if seen by the system, doesn't make much sense and might have unwanted side-effects
	// in rare cases (e.g. if the foreground app takes note of these types of key events).
	// Don't do this for ignored keys because that could cause an endless loop of
	// numlock events due to the keybd events that SuppressThisKey sends.
	// It's a little more readable and comfortable not to rely on short-circuit
	// booleans and instead do these conditions as separate IF statements.
	if (aHook == g_MouseHook)
	{
		// Since a mouse button that is physically down is not necessarily logically down -- such as
		// when the mouse button is a suppressed hotkey -- only update the logical state (which is the
		// state the OS believes the key to be in) when this event is non-supressed (i.e. allowed to
		// go to the system):
#ifdef FUTURE_USE_MOUSE_BUTTONS_LOGICAL
		// THIS ENTIRE SECTION might never be necessary if it's true that GetAsyncKeyState() and
		// GetKeyState() can retrieve the logical mouse button state on Windows NT/2000/XP, which are
		// the only OSes that matter for this purpose because the hooks aren't supported on Win9x.
		KBDLLHOOKSTRUCT &event = *(PMSDLLHOOKSTRUCT)lParam;  // For convenience, maintainability, and possibly performance.
		switch (wParam)
		{
			case WM_LBUTTONUP: g_mouse_buttons_logical &= ~MK_LBUTTON; break;
			case WM_RBUTTONUP: g_mouse_buttons_logical &= ~MK_RBUTTON; break;
			case WM_MBUTTONUP: g_mouse_buttons_logical &= ~MK_MBUTTON; break;
			// WM_NCXBUTTONUP is a click in the non-client area of a window.  MSDN implies this message can be
			// received by the mouse hook  but it seems doubtful because its counterparts, such as WM_NCLBUTTONUP,
			// are apparently never received:
			case WM_NCXBUTTONUP:
			case WM_XBUTTONUP:
				g_mouse_buttons_logical &= ~(   (HIWORD(event.mouseData)) == XBUTTON1 ? MK_XBUTTON1 : MK_XBUTTON2   );
				break;
			case WM_LBUTTONDOWN: g_mouse_buttons_logical |= MK_LBUTTON; break;
			case WM_RBUTTONDOWN: g_mouse_buttons_logical |= MK_RBUTTON; break;
			case WM_MBUTTONDOWN: g_mouse_buttons_logical |= MK_MBUTTON; break;
			case WM_NCXBUTTONDOWN:
			case WM_XBUTTONDOWN:
				g_mouse_buttons_logical |= (HIWORD(event.mouseData) == XBUTTON1) ? MK_XBUTTON1 : MK_XBUTTON2;
				break;
		}
#endif
#ifdef ENABLE_KEY_HISTORY_FILE
		if (g_KeyHistoryToFile)
			KeyHistoryToFile(NULL, pKeyHistoryCurr->event_type, pKeyHistoryCurr->key_up
				, pKeyHistoryCurr->vk, pKeyHistoryCurr->sc);  // A fairly low overhead operation.
#endif
	}
	else // Our caller is the keyboard hook.
	{
		KBDLLHOOKSTRUCT &event = *(PKBDLLHOOKSTRUCT)lParam;

		bool is_ignored = IsIgnored(event.dwExtraInfo);
		if (!is_ignored)
		{
			if (kvk[aVK].pForceToggle) // Key is a toggleable key.
			{
				// Dereference to get the global var's value:
				if (*(kvk[aVK].pForceToggle) != NEUTRAL) // Prevent toggle.
					return SuppressThisKeyFunc(aHook, lParam, aVK, aSC, aKeyUp, aExtraInfo, pKeyHistoryCurr, aHotkeyIDToPost);
			}
		}

		if ((Hotstring::sEnabledCount && !is_ignored) || g_input)
			if (!CollectInput(event, aVK, aSC, aKeyUp, is_ignored, pKeyHistoryCurr, hs_wparam_to_post, hs_lparam_to_post)) // Key should be invisible (suppressed).
				return SuppressThisKeyFunc(aHook, lParam, aVK, aSC, aKeyUp, aExtraInfo, pKeyHistoryCurr, aHotkeyIDToPost, hs_wparam_to_post, hs_lparam_to_post);

		// Do these here since the above "return SuppressThisKey" will have already done it in that case.
#ifdef ENABLE_KEY_HISTORY_FILE
		if (g_KeyHistoryToFile)
			KeyHistoryToFile(NULL, pKeyHistoryCurr->event_type, pKeyHistoryCurr->key_up
				, pKeyHistoryCurr->vk, pKeyHistoryCurr->sc);  // A fairly low overhead operation.
#endif

		UpdateKeybdState(event, aVK, aSC, aKeyUp, false);

		// UPDATE: The Win-L and Ctrl-Alt-Del workarounds below are still kept in effect in spite of the
		// anti-stick workaround done via GetModifierLRState().  This is because ResetHook() resets more
		// than just the modifiers and physical key state, which seems appropriate since the user might
		// be away for a long period of time while the computer is locked or the security screen is displayed.
		// Win-L uses logical keys, unlike Ctrl-Alt-Del which uses physical keys (i.e. Win-L can be simulated,
		// but Ctrl-Alt-Del must be physically pressed by the user):
		if (   aVK == 'L' && !aKeyUp && (g_modifiersLR_logical == MOD_LWIN  // i.e. *no* other keys but WIN.
			|| g_modifiersLR_logical == MOD_RWIN || g_modifiersLR_logical == (MOD_LWIN | MOD_RWIN))
			&& g_os.IsWinXPorLater())
		{
			// Since the user has pressed Win-L with *no* other modifier keys held down, and since
			// this key isn't being suppressed (since we're here in this function), the computer
			// is about to be locked.  When that happens, the hook is apparently disabled or
			// deinstalled until the user logs back in.  Because it is disabled, it will not be
			// notified when the user releases the LWIN or RWIN key, so we should assume that
			// it's now not in the down position.  This avoids it being thought to be down when the
			// user logs back in, which might cause hook hotkeys to accidentally fire.
			// Update: I've received an indication from a single Win2k user (unconfirmed from anyone
			// else) that the Win-L hotkey doesn't work on Win2k.  AutoIt3 docs confirm this.
			// Thus, it probably doesn't work on NT either.  So it's been changed to happen only on XP:
			ResetHook(true); // We already know that *only* the WIN key is down.
			// Above will reset g_PhysicalKeyState, especially for the windows keys and the 'L' key
			// (in our case), in preparation for re-logon:
		}

		// Although the delete key itself can be simulated (logical or physical), the user must be physically
		// (not logically) holding down CTRL and ALT for the ctrl-alt-del sequence to take effect,
		// which is why g_modifiersLR_physical is used vs. g_modifiersLR_logical (which is used above since
		// it's different).  Also, this is now done for XP -- in addition to NT4 & Win2k -- in case XP is
		// configured to display the NT/2k style security window instead of the task manager.  This is
		// probably very common because whenever the welcome screen is disabled, that's the default behavior?:
		// Control Panel > User Accounts > Use the welcome screen for fast and easy logon
		if (   (aVK == VK_DELETE || aVK == VK_DECIMAL) && !aKeyUp         // Both of these qualify, see notes.
			// Below: At least one CTRL key is physically down.  physical and ctrlaltdel_mask are combined
			// because ctrlaltdel_mask excludes fake LCtrl (from AltGr) but might not be tracked as reliably.
			&& (g_modifiersLR_physical & g_modifiersLR_ctrlaltdel_mask & (MOD_LCONTROL | MOD_RCONTROL))
			&& (g_modifiersLR_physical & (MOD_LALT | MOD_RALT))         // At least one ALT key is physically down.
			&& !(g_modifiersLR_physical & (MOD_LSHIFT | MOD_RSHIFT))   )// Neither shift key is phys. down (WIN is ok).
		{
			// Similar to the above case except for Windows 2000.  I suspect it also applies to NT,
			// but I'm not sure.  It seems safer to apply it to NT until confirmed otherwise.
			// Note that Ctrl-Alt-Delete works with *either* delete key, and it works regardless
			// of the state of Numlock (at least on XP, so it's probably that way on Win2k/NT also,
			// though it would be nice if this too is someday confirmed).  Here's the key history
			// someone for when the pressed ctrl-alt-del and then pressed esc to dismiss the dialog
			// on Win2k (Win2k invokes a 6-button dialog, with choices such as task manager and lock
			// workstation, if I recall correctly -- unlike XP which invokes task mgr by default):
			// A4  038	 	d	21.24	Alt            	
			// A2  01D	 	d	0.00	Ctrl           	
			// A2  01D	 	d	0.52	Ctrl           	
			// 2E  053	 	d	0.02	Num Del        	<-- notice how there's no following up event
			// 1B  001	 	u	2.80	Esc             <-- notice how there's no preceding down event
			// Other notes: On XP at least, shift key must not be down, otherwise Ctrl-Alt-Delete does
			// not take effect.  Windows key can be down, however.
			// Since the user will be gone for an unknown amount of time, it seems best just to reset
			// all hook tracking of the modifiers to the "up" position.  The user can always press them
			// down again upon return.  It also seems best to reset both logical and physical, just for
			// peace of mind and simplicity:
			ResetHook(true);
			// The above will also reset g_PhysicalKeyState so that especially the following will not
			// be thought to be physically down:CTRL, ALT, and DEL keys.  This is done in preparation
			// for returning from the security screen.  The neutral keys (VK_MENU and VK_CONTROL)
			// must also be reset -- not just because it's correct but because CollectInput() relies on it.
		}

		// Bug-fix for v1.0.20: The below section was moved out of LowLevelKeybdProc() to here because
		// sAltTabMenuIsVisible should not be set to true prior to knowing whether the current tab-down
		// event will be suppressed.  This is because if it is suppressed, the menu will not become visible
		// after all since the system will never see the tab-down event.
		// Having this extra check here, in addition to the other(s) that set sAltTabMenuIsVisible to be
		// true, allows AltTab and ShiftAltTab hotkeys to function even when the AltTab menu was invoked by
		// means other than an AltTabMenu or AltTabAndMenu hotkey.  The alt-tab menu becomes visible only
		// under these exact conditions, at least under WinXP:
		if (aVK == VK_TAB && !aKeyUp && !sAltTabMenuIsVisible
			&& (g_modifiersLR_logical & (MOD_LALT | MOD_RALT)) // At least one ALT key is down.
			&& !(g_modifiersLR_logical & (MOD_LCONTROL | MOD_RCONTROL))) // Neither CTRL key is down.
			sAltTabMenuIsVisible = true;

		if (modLR_type modLR = kvk[aVK].as_modifiersLR) // It's a modifier key.
		{
			// Don't do it this way because then the alt key itself can't be reliable used as "AltTabMenu"
			// (due to ShiftAltTab causing sAltTabMenuIsVisible to become false):
			//if (   sAltTabMenuIsVisible && !((g_modifiersLR_logical & MOD_LALT) || (g_modifiersLR_logical & MOD_RALT))
			//	&& !(aKeyUp && pKeyHistoryCurr->event_type == 'h')   )  // In case the alt key itself is "AltTabMenu"
			if (sAltTabMenuIsVisible && // Release of Alt key (the check above confirmed it is a modifier):
				(aKeyUp && (aVK == VK_LMENU || aVK == VK_RMENU || aVK == VK_MENU))
				// In case the alt key itself is "AltTabMenu":
				&& pKeyHistoryCurr->event_type != 'h' && pKeyHistoryCurr->event_type != 's'   )
				// It's important to reset in this case because if sAltTabMenuIsVisible were to
				// stay true and the user presses ALT in the future for a purpose other than to
				// display the Alt-tab menu, we would incorrectly believe the menu to be displayed:
				sAltTabMenuIsVisible = false;

			if (!aKeyUp) // Key-down.
			{
				// sUndisguisedMenuInEffect can be true or false prior to this.
				//  LAlt (true) + LWin = both disguised (false).
				//  LWin (true) + LAlt = both disguised (false).
				if (modLR & (MOD_LWIN | MOD_RWIN))
					sUndisguisedMenuInEffect = !(g_modifiersLR_logical & ~(MOD_LWIN | MOD_RWIN)); // If any other modifier is down, disguise is already in effect.
				else if (modLR & (MOD_LALT | MOD_RALT))
					sUndisguisedMenuInEffect = !(g_modifiersLR_logical & (MOD_LCONTROL | MOD_RCONTROL)); // If Ctrl is down (including if this Alt is AltGr), disguise is already in effect.
				else // Shift or Ctrl: pressing either serves to disguise any previous Alt or Win.
					sUndisguisedMenuInEffect = false;
			}
			else if (sDisguiseNextMenu)
			{
				// If a menu key is still physically down (or down due to an explicit Send, such as a remapping),
				// keep watching until it is released so that if key-repeat puts it back into effect, it will be
				// disguised again.  _non_ignored is used to ignore temporary modifier changes made during a
				// Send which aren't explicit, such as `Send x` temporarily releasing LWin/RWin.  Without this,
				// something like AppsKey::RWin would not work well with other hotkeys which Send.
				// v1.1.27.01: This section now also handles Ctrl-up and Shift-up events, which not only fail to
				// disguise Win but actually cause the Start menu to immediately appear even though the Win key
				// has not been released.  This only occurs if it was not already disguised by the Ctrl/Shift down
				// event; i.e. when an isolated Ctrl/Shift up event is received without a corresponding down event.
				// "Physical" events of this kind can be sent by the system when switching from a window with UK
				// layout to a window with US layout.  This is likely related to the UK layout having AltGr.
				// v1.1.33.03: This is now applied to LAlt/RAlt, to fix issues with hotkeys like !WheelUp:: in
				// programs with non-standard handling of the Alt key, such as Firefox.
				if (  !(g_modifiersLR_logical_non_ignored & (MOD_LWIN | MOD_RWIN | MOD_LALT | MOD_RALT))  )
				{
					if (modLR & (MOD_LCONTROL | MOD_RCONTROL | MOD_LSHIFT | MOD_RSHIFT))
					{
						// v1.1.27.01: Since this key being released is Ctrl/Shift and Win is not down, this must
						// be in combination with Alt, which can be disguised by this event.  By contrast, if the
						// Win key was down and sUndisguisedMenuInEffect == true (meaning there was no Ctrl/Shift
						// down event prior to this up event), this event needs to be disguised for the reason
						// described above.
						sUndisguisedMenuInEffect = false;
					}
					sDisguiseNextMenu = false;
				}
				// Since the below call to KeyEvent() calls the keybd hook recursively, a quick down-and-up
				// is all that is necessary to disguise the key.  This is because the OS will see that the
				// keystroke occurred while ALT or WIN is still down because we haven't done CallNextHookEx() yet.
				if (sUndisguisedMenuInEffect)
					KeyEventMenuMask(KEYDOWNANDUP); // This should also cause sUndisguisedMenuInEffect to be reset.
			}
			else // A modifier key was released and sDisguiseNextMenu was false.
			{
				// Now either no menu keys are down or they have been disguised by this keyup event.
				// Key-repeat may put the menu key back into effect, but that will be detected above.
				sUndisguisedMenuInEffect = false;
			}
		} // It's a modifier key.
		else // It's not a modifier key.
		{
			// Any key press or release serves to disguise the menu key.
			sUndisguisedMenuInEffect = false;
		}
	} // Keyboard vs. mouse hook.

	// Since above didn't return, this keystroke is being passed through rather than suppressed.
	if (g_HSResetUponMouseClick && (aVK == VK_LBUTTON || aVK == VK_RBUTTON)) // v1.0.42.03
	{
		*g_HSBuf = '\0';
		g_HSBufLength = 0;
	}
	// In case CallNextHookEx() is high overhead or can sometimes take a long time to return,
	// call it before posting the messages.  This solves conditions in which the main thread is
	// able to launch a script subroutine before the hook thread can finish updating its key state.
	// Search on AHK_HOOK_HOTKEY in this file for more comments.
	LRESULT result_to_return = CallNextHookEx(aHook, aCode, wParam, lParam);
	if (aHotkeyIDToPost != HOTKEY_ID_INVALID)
	{
		int input_level = InputLevelFromInfo(aExtraInfo);
		PostMessage(g_hWnd, AHK_HOOK_HOTKEY, aHotkeyIDToPost, MAKELONG(pKeyHistoryCurr->sc, input_level)); // v1.0.43.03: sc is posted currently only to support the number of wheel turns (to store in A_EventInfo).
		if (aKeyUp && hotkey_up[aHotkeyIDToPost & HOTKEY_ID_MASK] != HOTKEY_ID_INVALID)
		{
			// This is a key-down hotkey being triggered by releasing a prefix key.
			// There's also a corresponding key-up hotkey, so fire it too:
    		PostMessage(g_hWnd, AHK_HOOK_HOTKEY, hotkey_up[aHotkeyIDToPost & HOTKEY_ID_MASK], MAKELONG(pKeyHistoryCurr->sc, input_level));
		}
	}
	if (hs_wparam_to_post != HOTSTRING_INDEX_INVALID)
		PostMessage(g_hWnd, AHK_HOTSTRING, hs_wparam_to_post, hs_lparam_to_post);
	return result_to_return;
}



bool CollectInput(KBDLLHOOKSTRUCT &aEvent, const vk_type aVK, const sc_type aSC, bool aKeyUp, bool aIsIgnored
	, KeyHistoryItem *pKeyHistoryCurr, WPARAM &aHotstringWparamToPost, LPARAM &aHotstringLparamToPost)
// Caller is responsible for having initialized aHotstringWparamToPost to HOTSTRING_INDEX_INVALID.
// Returns true if the caller should treat the key as visible (non-suppressed).
// Always use the parameter vk rather than event.vkCode because the caller or caller's caller
// might have adjusted vk, namely to make it a left/right specific modifier key rather than a
// neutral one.
{
	// Transcription is done only once for all layers, so do this if any layer requests it:
	bool transcribe_modified_keys = false;

	for (auto *input = g_input; input; input = input->Prev)
	{
		if (input->InProgress() && input->IsInteresting(aEvent))
		{
			if (   aKeyUp && input->ScriptObject && input->ScriptObject->onKeyUp
				&& ( ((input->KeySC[aSC] | input->KeyVK[aVK]) & INPUT_KEY_NOTIFY)
					|| input->NotifyNonText && !((input->KeyVK[aVK]) & INPUT_KEY_IS_TEXT) )   )
			{
				PostMessage(g_hWnd, AHK_INPUT_KEYUP, (WPARAM)input, (aSC << 16) | aVK);
			}
			if (aKeyUp && (input->KeySC[aSC] & INPUT_KEY_DOWN_SUPPRESSED))
			{
				input->KeySC[aSC] &= ~INPUT_KEY_DOWN_SUPPRESSED;
				return false;
			}
			if (aKeyUp && (input->KeyVK[aVK] & INPUT_KEY_DOWN_SUPPRESSED))
			{
				input->KeyVK[aVK] &= ~INPUT_KEY_DOWN_SUPPRESSED;
				return false;
			}

			if (input->TranscribeModifiedKeys)
				transcribe_modified_keys = true;
		}
	}

	// The checks above suppress key-up if key-down was suppressed and the Input is still active.
	// Otherwise, avoid suppressing key-up since it may result in the key getting stuck down.
	// At the very least, this is needed for cases where a user presses a #z hotkey, for example,
	// to initiate an Input.  When the user releases the LWIN/RWIN key during the input, that
	// up-event should not be suppressed otherwise the modifier key would get "stuck down".  
	if (aKeyUp)
		return true;

	static vk_type sPendingDeadKeyVK = 0;
	static sc_type sPendingDeadKeySC = 0; // Need to track this separately because sometimes default VK-to-SC mapping isn't correct.
	static bool sPendingDeadKeyUsedShift = false;
	static bool sPendingDeadKeyUsedAltGr = false;
	
	bool transcribe_key = true;
	
	// Don't unconditionally transcribe modified keys such as Ctrl-C because calling ToAsciiEx() on
	// some such keys (e.g. Ctrl-LeftArrow or RightArrow if I recall correctly), disrupts the native
	// function of those keys.  That is the reason for the existence of the
	// g_input.TranscribeModifiedKeys option.
	// Fix for v1.0.38: Below now uses g_modifiersLR_logical vs. g_modifiersLR_physical because
	// it's the logical state that determines what will actually be produced on the screen and
	// by ToAsciiEx() below.  This fixes the Input command to properly capture simulated
	// keystrokes even when they were sent via hotkey such #c or a hotstring for which the user
	// might still be holding down a modifier, such as :*:<t>::Test (if '>' requires shift key).
	// It might also fix other issues.
	if ((g_modifiersLR_logical & ~(MOD_LSHIFT | MOD_RSHIFT)) // At least one non-Shift modifier is down (Shift may also be down).
		&& !transcribe_modified_keys
		&& !((g_modifiersLR_logical & (MOD_LALT | MOD_RALT)) && (g_modifiersLR_logical & (MOD_LCONTROL | MOD_RCONTROL))))
		// Since in some keybd layouts, AltGr (Ctrl+Alt) will produce valid characters (such as the @ symbol,
		// which is Ctrl+Alt+Q in the German/IBM layout and Ctrl+Alt+2 in the Spanish layout), an attempt
		// will now be made to transcribe all of the following modifier combinations:
		// - Anything with no modifiers at all.
		// - Anything that uses ONLY the shift key.
		// - Anything with Ctrl+Alt together in it, including Ctrl+Alt+Shift, etc. -- but don't do
		//   "anything containing the Alt key" because that causes weird side-effects with
		//   Alt+LeftArrow/RightArrow and maybe other keys too).
		// Older comment: If any modifiers except SHIFT are physically down, don't transcribe the key since
		// most users wouldn't want that.  An additional benefit of this policy is that registered hotkeys will
		// normally be excluded from the input (except those rare ones that have only SHIFT as a modifier).
		// Note that ToAsciiEx() will translate ^i to a tab character, !i to plain i, and many other modified
		// letters as just the plain letter key, which we don't want.
		transcribe_key = false;

	// v1.1.28.00: active_window is set to the focused control, if any, so that the hotstring buffer is reset
	// when the focus changes between controls, not just between windows.
	// v1.1.28.01: active_window is left as the active window; the above is not done because it disrupts
	// hotstrings when the first keypress causes a change in focus, such as to enter editing mode in Excel.
	// See Get_active_window_keybd_layout macro definition for related comments.
	HWND active_window = GetForegroundWindow(); // Set default in case there's no focused control.
	HKL active_window_keybd_layout = GetKeyboardLayout(GetFocusedCtrlThread(NULL, active_window));

	// Univeral Windows Platform apps apparently have their own handling for dead keys:
	//  - Dead key followed by Esc produces Chr(27), unlike non-UWP apps.
	//  - Pressing a dead key in a UWP app does not leave it in the keyboard layout's buffer,
	//    so to get the correct result here we must translate the dead key again, first.
	//  - Pressing a non-dead key disregards any dead key which was placed into the buffer by
	//    calling ToUnicodeEx, and it is left in the buffer.  To get the correct result for the
	//    next call, we must NOT reinsert it into the buffer (see dead_key_sequence_complete).
	static bool sUwpAppFocused = false;
	static HWND sUwpHwndChecked = 0;
	if (sUwpHwndChecked != active_window)
	{
		sUwpHwndChecked = active_window;
		TCHAR class_name[28];
		GetClassName(active_window, class_name, _countof(class_name));
		sUwpAppFocused = !_tcsicmp(class_name, _T("ApplicationFrameWindow"));
	}

	int char_count;
	TCHAR ch[3] = { 0 };
	BYTE key_state[256];
	memcpy(key_state, g_PhysicalKeyState, 256);

	if (sPendingDeadKeyVK && sUwpAppFocused && aVK != VK_PACKET)
	{
		AdjustKeyState(key_state
			, (sPendingDeadKeyUsedAltGr ? MOD_LCONTROL|MOD_RALT : 0)
			| (sPendingDeadKeyUsedShift ? MOD_RSHIFT : 0)); // Left vs Right Shift probably doesn't matter in this context.
		// If it turns out there was already a dead key in the buffer, the second call puts it back.
		if (ToUnicodeOrAsciiEx(sPendingDeadKeyVK, sPendingDeadKeySC, key_state, ch, 0, active_window_keybd_layout) > 0)
			ToUnicodeOrAsciiEx(sPendingDeadKeyVK, sPendingDeadKeySC, key_state, ch, 0, active_window_keybd_layout);
		sPendingDeadKeyVK = 0; // Don't reinsert it afterward (see above).
	}

	// Provide the correct logical modifier and CapsLock state for any translation below.
	AdjustKeyState(key_state, g_modifiersLR_logical);
	if (IsKeyToggledOn(VK_CAPITAL))
		key_state[VK_CAPITAL] |= STATE_ON;
	else
		key_state[VK_CAPITAL] &= ~STATE_ON;

	if (aVK == VK_PACKET)
	{
		// VK_PACKET corresponds to a SendInput event with the KEYEVENTF_UNICODE flag.
#ifdef UNICODE
		char_count = 1; // SendInput only supports a single 16-bit character code.
		ch[0] = (WCHAR)aEvent.scanCode; // No translation needed.
#else
		// Convert the Unicode character to ANSI, dropping any that can't be converted.
		char_count = WideCharToMultiByte(CP_ACP, WC_NO_BEST_FIT_CHARS, (WCHAR *)&aEvent.scanCode, 1, (CHAR *)ch, _countof(ch), NULL, NULL);
#endif
	}
	else if (transcribe_key)
	{
		char_count = ToUnicodeOrAsciiEx(aVK, aEvent.scanCode  // Uses the original scan code, not the adjusted "sc" one.
			, key_state, ch, g_MenuIsVisible ? 1 : 0, active_window_keybd_layout);

		if ((g_modifiersLR_logical & (MOD_LCONTROL | MOD_RCONTROL)) == 0) // i.e. must not replace '\r' with '\n' if it is the result of Ctrl+M.
		{
			if (ch[0] == '\r')  // Translate \r to \n since \n is more typical and useful in Windows.
				ch[0] = '\n';
			if (ch[1] == '\r')  // But it's never referred to if byte_count < 2
				ch[1] = '\n';
		}
	}
	else
	{
		char_count = 0;
	}

	// If Backspace is pressed after a dead key, ch[0] is the "dead" char and ch[1] is '\b'.
	// Testing shows that this can be handled a number of ways (we only support 1 & 2):
	// 1. Most Win32 apps perform backspacing and THEN insert ch[0].
	// 2. UWP apps perform backspacing and discard the pending dead key.
	// 3. VS2015 performs backspacing and leaves the dead key in the buffer.
	// 4. MarkdownPad 2 prints the dead char as if Space was pressed, and does no backspacing.
	// 5. Unconfirmed: some apps might do nothing; i.e. print the dead char and then delete it.
	if (aVK == VK_BACK && char_count /*&& !g_modifiersLR_logical*/) // Modifier state is checked mostly for backward-compatibility.
	{
		if (sUwpAppFocused)
		{
			char_count = 0;
			sPendingDeadKeyVK = 0;
		}
		else // Assume standard Win32 behaviour as described above.
			--char_count; // Remove '\b' to simplify the backspacing and collection stages.
	}
	
	if (!CollectInputHook(aEvent, aVK, aSC, ch, char_count, aIsIgnored))
		return false; // Suppress.
	
	// More notes about dead keys: The dead key behavior of Enter/Space/Backspace is already properly
	// maintained when an Input or hotstring monitoring is in effect.  In addition, keys such as the
	// following already work okay (i.e. the user can press them in between the pressing of a dead
	// key and it's finishing/base/trigger key without disrupting the production of diacritical letters)
	// because ToAsciiEx() finds no translation-to-char for them:
	// pgup/dn/home/end/ins/del/arrowkeys/f1-f24/etc.
	// Note that if a pending dead key is followed by the press of another dead key (including itself),
	// the sequence should be triggered and both keystrokes should appear in the active window.
	// That case has been tested too, and works okay with the layouts tested so far.

	// Dead keys in Danish layout as they appear on a US English keyboard: Equals and Plus /
	// Right bracket & Brace / probably others.

	// SUMMARY OF DEAD KEY ISSUE:
	// Calling ToAsciiEx() on the dead key itself doesn't disrupt anything. The disruption occurs on the next key
	// (for which the dead key is pending): ToAsciiEx() consumes previous/pending dead key, which causes the
	// active window's call of ToAsciiEx() to fail to see a dead key. So unless the program reinserts the dead key
	// after the call to ToAsciiEx() but before allowing the dead key's successor key to pass through to the
	// active window, that window would see a non-diacritic like "u" instead of û.  In other words, the program
	// "uses up" the dead key to populate its own hotstring buffer, depriving the active window of the dead key.
	//
	// JAVA ISSUE: Hotstrings are known to disrupt dead keys in Java apps on some systems (though not my XP one).
	// I spent several hours on it but was unable to solve it using anything other than a Sleep(20) after the
	// reinsertion of the dead key (and PhiLho reports that even that didn't fully solve it).  A Sleep here in the
	// hook would probably do more harm than good, so is avoided for now.  Other approaches:
	// 1) Send a simulated substitute for the successor key rather than allowing the hook to pass it through.
	//    Maybe that would somehow put things in a better order for Java.  However, there might be side-effects to
	//    that, such as in DirectX games.
	// 2) Have main thread (rather than hook thread) reinsert the dead key and its successor key (hook would have
	//    suppressed both), which allows the main thread to do a Sleep or MsgSleep.  Such a Sleep be more effective
	//    because the main thread's priority is lower than that of the hook's, allowing better round-robin.
	// 
	// If this key is a dead key or there's a dead key pending and this incoming key is capable of
	// completing/triggering it, do a workaround for the side-effects of ToAsciiEx().  This workaround
	// allows dead keys to continue to operate properly in the user's foreground window, while still
	// being capturable by the Input command and recognizable by any defined hotstrings whose
	// abbreviations use diacritical letters:
	bool dead_key_sequence_complete = sPendingDeadKeyVK && char_count > 0;
	if (char_count < 0) // It's a dead key, and it doesn't complete a sequence since in that case char_count would be >= 1.
	{
		// Since above did not return, treat_as_visible must be true.
		sPendingDeadKeyVK = aVK;
		sPendingDeadKeySC = aSC;
		sPendingDeadKeyUsedShift = g_modifiersLR_logical & (MOD_LSHIFT | MOD_RSHIFT);
		// Detect AltGr as fully and completely as possible in case the current keyboard layout
		// doesn't even have an AltGr key.  The section above which references sPendingDeadKeyUsedAltGr
		// relies on this check having been done here.  UPDATE:
		// v1.0.35.10: Allow Ctrl+Alt to be seen as AltGr too, which allows users to press Ctrl+Alt+Deadkey
		// rather than AltGr+Deadkey.  It might also resolve other issues.  This change seems okay since
		// the mere fact that this IS a dead key (as checked above) should mean that it's a deadkey made
		// manifest through AltGr.
		// Previous method:
		//sPendingDeadKeyUsedAltGr = (g_modifiersLR_logical & (MOD_LCONTROL | MOD_RALT)) == (MOD_LCONTROL | MOD_RALT);
		sPendingDeadKeyUsedAltGr = (g_modifiersLR_logical & (MOD_LCONTROL | MOD_RCONTROL))
			&& (g_modifiersLR_logical & (MOD_LALT | MOD_RALT));

		// Lexikos: Testing shows that calling ToUnicodeEx with the VK/SC of a dead key
		// acts the same as actually pressing that key.  Calling it once when there is
		// no pending dead key places the dead key in the keyboard layout's buffer and
		// returns -1; calling it again consumes the dead key and returns either 1 or 2,
		// depending on the keyboard layout.  For instance:
		//	- Passing vkC0 twice with US-International gives the string "``".
		//  - Passing vkBA twice with Neo2 gives just the combining version of "^".
		// 
		// Normally ToUnicodeEx would be called by the active window (after the hook
		// returns), thus placing the dead key in the buffer.  Since our call above
		// has already done that, we need to remove the dead key from the buffer
		// before returning.  The benefits of this over the old method include:
		//  - The hook is not called recursively since no extra keystrokes are generated.
		//  - It's probably faster for the same reason.
		//  - It works correctly even when there are multiple scripts with hotstrings,
		//    all calling ToUnicodeEx in sequence.
		//
		// The other half of this workaround can be found by searching for "if (dead_key_sequence_complete)".
		//
		ToUnicodeOrAsciiEx(aVK, aEvent.scanCode, key_state, ch, g_MenuIsVisible ? 1 : 0, active_window_keybd_layout);
		return true; // Visible.
	}
	
	// Hotstrings monitor neither ignored input nor input that is invisible due to suppression by
	// the Input command.  One reason for not monitoring ignored input is to avoid any chance of
	// an infinite loop of keystrokes caused by one hotstring triggering itself directly or
	// indirectly via a different hotstring:
	if (Hotstring::sEnabledCount && !aIsIgnored)
	{
		switch (aVK)
		{
		case VK_LEFT:
		case VK_RIGHT:
		case VK_DOWN:
		case VK_UP:
		case VK_NEXT:
		case VK_PRIOR:
		case VK_HOME:
		case VK_END:
			// Reset hotstring detection if the user seems to be navigating within an editor.  This is done
			// so that hotstrings do not fire in unexpected places.
			if (g_HSBufLength)
			{
				*g_HSBuf = '\0';
				g_HSBufLength = 0;
			}
			break;
		case VK_BACK:
			// v1.0.21: Only true (unmodified) backspaces are recognized by the below.  Another reason to do
			// this is that ^backspace has a native function (delete word) different than backspace in many editors.
			// Fix for v1.0.38: Below now uses g_modifiersLR_logical vs. physical because it's the logical state
			// that determines whether the backspace behaves like an unmodified backspace.  This solves the issue
			// of the Input command collecting simulated backspaces as real characters rather than recognizing
			// them as a means to erase the previous character in the buffer.
			if (!g_modifiersLR_logical && g_HSBufLength)
				g_HSBuf[--g_HSBufLength] = '\0';
			// Fall through to the check below in case this {BS} completed a dead key sequence.
			break;
		}
		if (char_count > 0
			&& !CollectHotstring(aEvent, ch, char_count, active_window, pKeyHistoryCurr, aHotstringWparamToPost, aHotstringLparamToPost))
		{
			sPendingDeadKeyVK = 0; // Avoid reinserting it later (see "dead_key_sequence_complete" below).
			return false; // Suppress.
		}
	}

	// Fix for v1.0.37.06: The following section was moved beneath the hotstring section so that
	// the hotstring section has a chance to bypass reinsertion of the dead key below. This fixes
	// wildcard hotstrings whose final character is diacritic, which would otherwise have the
	// dead key reinserted below, which in turn would cause the hotstring's first backspace to fire
	// the dead key (which kills the backspace, turning it into the dead key character itself).
	// For example:
	// :*:jsá::jsmith@somedomain.com
	// On the Spanish (Mexico) keyboard layout, one would type accent (English left bracket) followed by
	// the letter "a" to produce á.
	if (dead_key_sequence_complete)
	{
		// Fix for v1.1.34.03: Avoid reinserting the dead char if it wasn't actually in the buffer
		// (which can happen if there's another keyboard hook that removed it due to a suppressed
		// hotstring end-char).
		TCHAR new_ch[2];
		int new_char_count = ToUnicodeOrAsciiEx(aVK, aEvent.scanCode, key_state, new_ch, g_MenuIsVisible ? 1 : 0, active_window_keybd_layout);
		if (new_char_count < 0)
			// aVK is also a dead key and wasn't in the buffer, so take it back out.  This also implies
			// that sPendingDeadKeyVK needs to be reinserted, since the buffer state apparently differed
			// between our two ToUnicode() calls, and sPendingDeadKeyVK is probably the reason.
			ToUnicodeOrAsciiEx(aVK, aEvent.scanCode, key_state, new_ch, g_MenuIsVisible ? 1 : 0, active_window_keybd_layout);
		if (new_char_count != char_count || ch[0] != (new_ch[0] == '\r' ? '\n' : new_ch[0])) // Translation differs, likely due to pending dead key having been removed.
		{
			// Since our earlier call to ToUnicodeOrAsciiEx has removed the pending dead key from the
			// buffer, we need to put it back for the active window or the next hook in the chain.
			// This is not needed when ch (the character or characters produced by combining the dead
			// key with the last keystroke) is being suppressed, since in that case we don't want the
			// dead key back in the buffer.
			ZeroMemory(key_state, 256);
			AdjustKeyState(key_state
				, (sPendingDeadKeyUsedAltGr ? MOD_LCONTROL | MOD_RALT : 0)
				| (sPendingDeadKeyUsedShift ? MOD_RSHIFT : 0)); // Left vs Right Shift probably doesn't matter in this context.
			TCHAR temp_ch[2];
			ToUnicodeOrAsciiEx(sPendingDeadKeyVK, sPendingDeadKeySC, key_state, temp_ch, 0, active_window_keybd_layout);
			sPendingDeadKeyVK = 0;
		}
	}

	return true; // Visible.
}



bool CollectHotstring(KBDLLHOOKSTRUCT &aEvent, TCHAR ch[], int char_count, HWND active_window
	, KeyHistoryItem *pKeyHistoryCurr, WPARAM &aHotstringWparamToPost, LPARAM &aHotstringLparamToPost)
{
	bool suppress_hotstring_final_char = false; // Set default.

	if (active_window != g_HShwnd)
	{
		// Since the buffer tends to correspond to the text to the left of the caret in the
		// active window, if the active window changes, it seems best to reset the buffer
		// to avoid misfires.
		g_HShwnd = active_window;
		*g_HSBuf = '\0';
		g_HSBufLength = 0;
	}
	else if (HS_BUF_SIZE - g_HSBufLength < 3) // Not enough room for up-to-2 chars.
	{
		// Make room in buffer by removing chars from the front that are no longer needed for HS detection:
		// Bug-fixed the below for v1.0.21:
		g_HSBufLength = (int)_tcslen(g_HSBuf + HS_BUF_DELETE_COUNT);  // The new length.
		tmemmove(g_HSBuf, g_HSBuf + HS_BUF_DELETE_COUNT, g_HSBufLength + 1); // +1 to include the zero terminator.
	}

	g_HSBuf[g_HSBufLength++] = ch[0];
	if (char_count > 1)
		// MSDN: "This usually happens when a dead-key character (accent or diacritic) stored in the
		// keyboard layout cannot be composed with the specified virtual key to form a single character."
		g_HSBuf[g_HSBufLength++] = ch[1];
	g_HSBuf[g_HSBufLength] = '\0';

	if (g_HSBufLength)
	{
		TCHAR *cphs, *cpbuf, *cpcase_start, *cpcase_end;
		int case_capable_characters;
		bool first_char_with_case_is_upper, first_char_with_case_has_gone_by;
		CaseConformModes case_conform_mode;

		// Searching through the hot strings in the original, physical order is the documented
		// way in which precedence is determined, i.e. the first match is the only one that will
		// be triggered.
		for (HotstringIDType u = 0; u < Hotstring::sHotstringCount; ++u)
		{
			Hotstring &hs = *Hotstring::shs[u];  // For performance and convenience.
			if (hs.mSuspended)
				continue;
			if (hs.mEndCharRequired)
			{
				if (g_HSBufLength <= hs.mStringLength) // Ensure the string is long enough for loop below.
					continue;
				if (!_tcschr(g_EndChars, g_HSBuf[g_HSBufLength - 1])) // It's not an end-char, so no match.
					continue;
				cpbuf = g_HSBuf + g_HSBufLength - 2; // Init once for both loops. -2 to omit end-char.
			}
			else // No ending char required.
			{
				if (g_HSBufLength < hs.mStringLength) // Ensure the string is long enough for loop below.
					continue;
				cpbuf = g_HSBuf + g_HSBufLength - 1; // Init once for both loops.
			}
			cphs = hs.mString + hs.mStringLength - 1; // Init once for both loops.
			// Check if this item is a match:
			if (hs.mCaseSensitive)
			{
				for (; cphs >= hs.mString; --cpbuf, --cphs)
					if (*cpbuf != *cphs)
						break;
			}
			else // case insensitive
				// v1.0.43.03: Using CharLower vs. tolower seems the best default behavior (even though slower)
				// so that languages in which the higher ANSI characters are common will see "Ä" == "ä", etc.
				for (; cphs >= hs.mString; --cpbuf, --cphs)
					if (ltolower(*cpbuf) != ltolower(*cphs)) // v1.0.43.04: Fixed crash by properly casting to UCHAR (via macro).
						break;

			// Check if one of the loops above found a matching hotstring (relies heavily on
			// short-circuit boolean order):
			if (   cphs >= hs.mString // One of the loops above stopped early due discovering "no match"...
				// ... or it did but the "?" option is not present to protect from the fact that
				// what lies to the left of this hotstring abbreviation is an alphanumeric character:
				|| !hs.mDetectWhenInsideWord && cpbuf >= g_HSBuf && IsHotstringWordChar(*cpbuf)
				// ... v1.0.41: Or it's a perfect match but the right window isn't active or doesn't exist.
				// In that case, continue searching for other matches in case the script contains
				// hotstrings that would trigger simultaneously were it not for the "only one" rule.
				|| !HotCriterionAllowsFiring(hs.mHotCriterion, hs.mName)   )
				continue; // No match or not eligible to fire.
				// v1.0.42: The following scenario defeats the ability to give criterion hotstrings
				// precedence over non-criterion:
				// A global/non-criterion hotstring is higher up in the file than some criterion hotstring,
				// but both are eligible to fire at the same instant.  In v1.0.41, the global one would
				// take precedence because it's higher up (and this behavior is preserved not just for
				// backward compatibility, but also because it might be more flexible -- this is because
				// unlike hotkeys, variants aren't stored under a parent hotstring, so we don't know which
				// ones are exact dupes of each other (same options+abbreviation).  Thus, it would take
				// extra code to determine this at runtime; and even if it were added, it might be
				// more flexible not to do it; instead, to let the script determine (even by resorting to
				// #IfWinNOTActive) what precedence hotstrings have with respect to each other.

			//////////////////////////////////////////////////////////////
			// MATCHING HOTSTRING WAS FOUND (since above didn't continue).
			//////////////////////////////////////////////////////////////

			// Now that we have a match, see if its InputLevel is allowed. If not,
			// consider the key ignored (rather than continuing to search for more matches).
			if (!HotInputLevelAllowsFiring(hs.mInputLevel, aEvent.dwExtraInfo, &pKeyHistoryCurr->event_type))
				break;

			// Since default KeyDelay is 0, and since that is expected to be typical, it seems
			// best to unconditionally post a message rather than trying to handle the backspacing
			// and replacing here.  This is because a KeyDelay of 0 might be fairly slow at
			// sending keystrokes if the system is under heavy load, in which case we would
			// not be returning to our caller in a timely fashion, which would case the OS to
			// think the hook is unresponsive, which in turn would cause it to timeout and
			// route the key through anyway (testing confirms this).
			if (!hs.mConformToCase)
				case_conform_mode = CASE_CONFORM_NONE;
			else
			{
				// Find out what case the user typed the string in so that we can have the
				// replacement produced in similar case:
				cpcase_end = g_HSBuf + g_HSBufLength;
				if (hs.mEndCharRequired)
					--cpcase_end;
				// Bug-fix for v1.0.19: First find out how many of the characters in the abbreviation
				// have upper and lowercase versions (i.e. exclude digits, punctuation, etc):
				for (case_capable_characters = 0, first_char_with_case_is_upper = first_char_with_case_has_gone_by = false
					, cpcase_start = cpcase_end - hs.mStringLength
					; cpcase_start < cpcase_end; ++cpcase_start)
					if (IsCharLower(*cpcase_start) || IsCharUpper(*cpcase_start)) // A case-capable char.
					{
						if (!first_char_with_case_has_gone_by)
						{
							first_char_with_case_has_gone_by = true;
							if (IsCharUpper(*cpcase_start))
								first_char_with_case_is_upper = true; // Override default.
						}
						++case_capable_characters;
					}
				if (!case_capable_characters) // All characters in the abbreviation are caseless.
					case_conform_mode = CASE_CONFORM_NONE;
				else if (case_capable_characters == 1)
					// Since there is only a single character with case potential, it seems best as
					// a default behavior to capitalize the first letter of the replacement whenever
					// that character was typed in uppercase.  The behavior can be overridden by
					// turning off the case-conform mode.
					case_conform_mode = first_char_with_case_is_upper ? CASE_CONFORM_FIRST_CAP : CASE_CONFORM_NONE;
				else // At least two characters have case potential. If all of them are upper, use ALL_CAPS.
				{
					if (!first_char_with_case_is_upper) // It can't be either FIRST_CAP or ALL_CAPS.
						case_conform_mode = CASE_CONFORM_NONE;
					else // First char is uppercase, and if all the others are too, this will be ALL_CAPS.
					{
						case_conform_mode = CASE_CONFORM_FIRST_CAP; // Set default.
						// Bug-fix for v1.0.19: Changed !IsCharUpper() below to IsCharLower() so that
						// caseless characters such as the @ symbol do not disqualify an abbreviation
						// from being considered "all uppercase":
						for (cpcase_start = cpcase_end - hs.mStringLength; cpcase_start < cpcase_end; ++cpcase_start)
							if (IsCharLower(*cpcase_start)) // Use IsCharLower to better support chars from non-English languages.
								break; // Any lowercase char disqualifies CASE_CONFORM_ALL_CAPS.
						if (cpcase_start == cpcase_end) // All case-possible characters are uppercase.
							case_conform_mode = CASE_CONFORM_ALL_CAPS;
						//else leave it at the default set above.
					}
				}
			}

			if (hs.mDoBackspace || hs.mOmitEndChar) // Fix for v1.0.37.07: Added hs.mOmitEndChar so that B0+O will omit the ending character.
			{
				// Have caller suppress this final key pressed by the user, since it would have
				// to be backspaced over anyway.  Even if there is a visible Input command in
				// progress, this should still be okay since the input will still see the key,
				// it's just that the active window won't see it, which is okay since once again
				// it would have to be backspaced over anyway.  UPDATE: If an Input is in progress,
				// it should not receive this final key because otherwise the hotstring's backspacing
				// would backspace one too few times from the Input's point of view, thus the input
				// would have one extra, unwanted character left over (namely the first character
				// of the hotstring's abbreviation).  However, this method is not a complete
				// solution because it fails to work under a situation such as the following:
				// A hotstring script is started, followed by a separate script that uses the
				// Input command.  The Input script's hook will take precedence (since it was
				// started most recently), thus when the Hotstring's script's hook does sends
				// its replacement text, the Input script's hook will get a hold of it first
				// before the Hotstring's script has a chance to suppress it.  In other words,
				// The Input command will capture the ending character and then there will
				// be insufficient backspaces sent to clear the abbreviation out of it.  This
				// situation is quite rare so for now it's just mentioned here as a known limitation.
				suppress_hotstring_final_char = true;
			}

			// Post the message rather than sending it, because Send would need
			// SendMessageTimeout(), which is undesirable because the whole point of
			// making this hook thread separate from the main thread is to have it be
			// maximally responsive (especially to prevent mouse cursor lag).
			// Put the end char in the LOWORD and the case_conform_mode in the HIWORD.
			// Casting to UCHAR might be necessary to avoid problems when MAKELONG
			// casts a signed char to an unsigned WORD.
			// UPDATE: In v1.0.42.01, the message is posted later (by our caller) to avoid
			// situations in which the message arrives and is processed by the main thread
			// before we finish processing the hotstring's final keystroke here.  This avoids
			// problems with a script calling GetKeyState() and getting an inaccurate value
			// because the hook thread is either pre-empted or is running in parallel
			// (multiprocessor) and hasn't yet returned 1 or 0 to determine whether the final
			// keystroke is suppressed or passed through to the active window.
			// UPDATE: In v1.0.43, the ending character is not put into the Lparam when
			// hs.mDoBackspace is false.  This is because:
			// 1) When not backspacing, it's more correct that the ending character appear where the
			//    user typed it rather than appearing at the end of the replacement.
			// 2) Two ending characters would appear in pre-1.0.43 versions: one where the user typed
			//    it and one at the end, which is clearly incorrect.
			aHotstringWparamToPost = u; // Override the default set by caller.
			aHotstringLparamToPost = MAKELONG(
				hs.mEndCharRequired  // v1.0.48.04: Fixed to omit "&& hs.mDoBackspace" so that A_EndChar is set properly even for option "B0" (no backspacing).
					? g_HSBuf[g_HSBufLength - 1]  // Used by A_EndChar and Hotstring::DoReplace().
					: 0
				, case_conform_mode);

			// Clean up.
			// The keystrokes to be sent by the other thread upon receiving the message prepared above
			// will not be received by this function because:
			// 1) CollectInput() is not called for simulated keystrokes.
			// 2) The keyboard hook is absent during a SendInput hotstring.
			// 3) The keyboard hook does not receive SendPlay keystrokes (if hotstring is of that type).
			// Consequently, the buffer should be adjusted below to ensure it's in the right state to work
			// in situations such as the user typing two hotstrings consecutively where the ending
			// character of the first is used as a valid starting character (non-alphanumeric) for the next.
			if (hs.mReplacement)
			{
				// Since the buffer no longer reflects what is actually on screen to the left
				// of the caret position (since a replacement is about to be done), reset the
				// buffer, except for any end-char (since that might legitimately form part
				// of another hot string adjacent to the one just typed).  The end-char
				// sent by DoReplace() won't be captured (since it's "ignored input", which
				// is why it's put into the buffer manually here):
				if (hs.mEndCharRequired)
				{
					*g_HSBuf = g_HSBuf[g_HSBufLength - 1];
					g_HSBufLength = 1; // The buffer will be terminated to reflect this length later below.
				}
				else
					g_HSBufLength = 0; // The buffer will be terminated to reflect this length later below.
			}
			else if (hs.mDoBackspace)
			{
				// It's *not* a replacement, but we're doing backspaces, so adjust buf for backspaces
				// and the fact that the final char of the HS (if no end char) or the end char
				// (if end char required) will have been suppressed and never made it to the
				// active window.  A simpler way to understand is to realize that the buffer now
				// contains (for recognition purposes, in its right side) the hotstring and its
				// end char (if applicable), so remove both:
				g_HSBufLength -= hs.mStringLength;
				if (hs.mEndCharRequired)
					--g_HSBufLength; // The buffer will be terminated to reflect this length later below.
			}

			// v1.0.38.04: Fixed the following mDoReset section by moving it beneath the above because
			// the above relies on the fact that the buffer has not yet been reset.
			// v1.0.30: mDoReset was added to prevent hotstrings such as the following
			// from firing twice in a row, if you type 11 followed by another 1 afterward:
			//:*?B0:11::
			//MsgBox,0,test,%A_ThisHotkey%,1 ; Show which key was pressed and close the window after a second.
			//return
			// There are probably many other uses for the reset option (albeit obscure, but they have
			// been brought up in the forum at least twice).
			if (hs.mDoReset)
				g_HSBufLength = 0; // Further below, the buffer will be terminated to reflect this change.

			// In case the above changed the value of g_HSBufLength, terminate the buffer at that position:
			g_HSBuf[g_HSBufLength] = '\0';

			break; // Somewhere above would have done "continue" if a match wasn't found.
		} // for()
	} // if buf not empty
	return !suppress_hotstring_final_char;
}



bool CollectInputHook(KBDLLHOOKSTRUCT &aEvent, const vk_type aVK, const sc_type aSC, TCHAR aChar[], int aCharCount
	, bool aIsIgnored)
{
	auto *input = g_input;
	for (; input; input = input->Prev)
	{
		if (!input->InProgress() || !input->IsInteresting(aEvent))
			continue;
		
		UCHAR key_flags = input->KeyVK[aVK] | input->KeySC[aSC];
		
		// aCharCount is negative for dead keys, which are treated as text but not collected.
		bool treat_as_text = aCharCount && !(key_flags & INPUT_KEY_IGNORE_TEXT);
		bool collect_chars = treat_as_text && aCharCount > 0;

		// Determine visibility based on options and whether the key produced text.
		// Negative aCharCount (dead key) is treated as text in this context.
		bool visible;
		if (key_flags & INPUT_KEY_VISIBILITY_MASK)
			visible = key_flags & INPUT_KEY_VISIBLE;
		else if (kvk[aVK].as_modifiersLR || kvk[aVK].pForceToggle)
			visible = true; // Do not suppress modifiers or toggleable keys unless specified by KeyOpt().
		else
			visible = treat_as_text ? input->VisibleText : input->VisibleNonText;

		if (key_flags & END_KEY_ENABLED) // A terminating keystroke has now occurred unless the shift state isn't right.
		{
			bool end_if_shift_is_down = key_flags & END_KEY_WITH_SHIFT;
			bool end_if_shift_is_not_down = key_flags & END_KEY_WITHOUT_SHIFT;
			bool shift_is_down = g_modifiersLR_logical & (MOD_LSHIFT | MOD_RSHIFT);
			if (shift_is_down ? end_if_shift_is_down : end_if_shift_is_not_down)
			{
				// The shift state is correct to produce the desired end-key.
				input->EndByKey(aVK, aSC, input->KeySC[aSC] && (aSC || !input->KeyVK[aVK]), shift_is_down && !end_if_shift_is_not_down);
				if (!visible)
					break;
				continue;
			}
		}
		
		if (aVK == VK_BACK && !g_modifiersLR_logical && input->BackspaceIsUndo)
		{
			if (input->BufferLength)
				input->Buffer[--input->BufferLength] = '\0';
			visible = input->VisibleText; // Override VisibleNonText.
			// Fall through to the check below in case this {BS} completed a dead key sequence.
		}

		if (collect_chars)
			input->CollectChar(aChar, aCharCount);

		if (input->NotifyNonText)
		{
			// These flags enable key-up events to be classified as text or non-text based on
			// whether key-down produced text.
			if (treat_as_text)
				input->KeyVK[aVK] |= INPUT_KEY_IS_TEXT;
			else
				input->KeyVK[aVK] &= ~INPUT_KEY_IS_TEXT; // In case keyboard layout has changed or similar.
		}
		
		// Posting the notifications after CollectChar() might reduce the odds of a race condition.
		if (((key_flags & INPUT_KEY_NOTIFY) || input->NotifyNonText && !treat_as_text)
			&& input->ScriptObject && input->ScriptObject->onKeyDown)
		{
			// input is passed because the alternative would require the main thread to
			// iterate through the Input chain and determine which ones should be notified.
			// This would mean duplicating much of the logic that's used here, and would be
			// complicated by the possibility of an Input being terminated while OnKeyDown
			// is being executed (and thereby breaking the list).
			// This leaves room only for the bare essential parameters: aVK and aSC.
			PostMessage(g_hWnd, AHK_INPUT_KEYDOWN, (WPARAM)input, (aSC << 16) | aVK);
		}
		// Seems best to not collect dead key chars by default; if needed, OnDeadChar
		// could be added, or the script could mark each dead key for OnKeyDown.
		if (collect_chars && input->ScriptObject && input->ScriptObject->onChar)
		{
			PostMessage(g_hWnd, AHK_INPUT_CHAR, (WPARAM)input, ((TBYTE)aChar[1] << 16) | (TBYTE)aChar[0]);
		}

		if (!visible)
			break;
	}
	if (input) // Early break (invisible input).
	{
		if (aSC)
			input->KeySC[aSC] |= INPUT_KEY_DOWN_SUPPRESSED;
		else
			input->KeyVK[aVK] |= INPUT_KEY_DOWN_SUPPRESSED;
		return false;
	}
	return true;
}



bool input_type::IsInteresting(KBDLLHOOKSTRUCT &aEvent)
{
	return MinSendLevel == 0 ? true : HotInputLevelAllowsFiring(MinSendLevel - 1, aEvent.dwExtraInfo, NULL);
}



void input_type::CollectChar(TCHAR *ch, int char_count)
{
	const auto buffer = Buffer; // Marginally reduces code size.
	const auto match = this->match;

	for (int i = 0; i < char_count; ++i)
	{
		if (CaseSensitive ? _tcschr(EndChars, ch[i]) : ltcschr(EndChars, ch[i]))
		{
			EndByChar(ch[i]);
			return;
		}
		if (BufferLength == BufferLengthMax)
		{
			if (!BufferLength) // For L0, collect nothing but allow OnChar, etc.
				return;
			break;
		}
		buffer[BufferLength++] = ch[i];
		buffer[BufferLength] = '\0';
	}

	// Check if the buffer now matches any of the key phrases, if there are any:
	if (FindAnywhere)
	{
		if (CaseSensitive)
		{
			for (UINT i = 0; i < MatchCount; ++i)
			{
				if (_tcsstr(buffer, match[i]))
				{
					EndByMatch(i);
					return;
				}
			}
		}
		else // Not case sensitive.
		{
			for (UINT i = 0; i < MatchCount; ++i)
			{
				// v1.0.43.03: Changed lstrcasestr to strcasestr because it seems unlikely to break any existing
				// scripts and is also more useful given that Input with match-list is pretty rarely used,
				// and even when it is used, match lists are usually short (so performance isn't impacted much
				// by this change).
				if (lstrcasestr(buffer, match[i]))
				{
					EndByMatch(i);
					return;
				}
			}
		}
	}
	else // Exact match is required
	{
		if (CaseSensitive)
		{
			for (UINT i = 0; i < MatchCount; ++i)
			{
				if (!_tcscmp(buffer, match[i]))
				{
					EndByMatch(i);
					return;
				}
			}
		}
		else // Not case sensitive.
		{
			for (UINT i = 0; i < MatchCount; ++i)
			{
				// v1.0.43.03: Changed to locale-insensitive search.  See similar v1.0.43.03 comment above for more details.
				if (!lstrcmpi(buffer, match[i]))
				{
					EndByMatch(i);
					return;
				}
			}
		}
	}

	// Otherwise, no match found.
	if (BufferLength >= BufferLengthMax)
		EndByLimit();
}



bool IsHotstringWordChar(TCHAR aChar)
// Returns true if aChar would be part of a word if followed by a word char.
// aChar itself may be a word char or a nonspacing mark which combines with
// the next character (the first character of a potential hotstring match).
{
	// IsCharAlphaNumeric is used for simplicity and to preserve old behaviour
	// (with the only exception being the one added below), in case it's what
	// users have come to expect.  Note that checking for C1_ALPHA or C3_ALPHA
	// and C1_DIGIT is not equivalent: Michael S. Kaplan wrote that the real
	// conditions are "(C1_ALPHA && ! (C3_HIRAGANA | C3_KATAKANA) || C1_DIGIT)" -- https://web.archive.org/web/20130627015450/http://blogs.msdn.com/b/michkap/archive/2007/06/19/3396819.aspx
	if (IsCharAlphaNumeric(aChar))
		return true;
	WORD char_type;
	if (GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE3, &aChar, 1, &char_type))
	{
		// Nonspacing marks combine with the following character, so would visually
		// appear to be part of the word.  This should fix detection of words beginning
		// with or containing Arabic nonspacing diacritics, for example.
		if (char_type & C3_NONSPACING)
			return true;
	}
	return false;
}



void UpdateKeybdState(KBDLLHOOKSTRUCT &aEvent, const vk_type aVK, const sc_type aSC, bool aKeyUp, bool aIsSuppressed)
// Caller has ensured that vk has been translated from neutral to left/right if necessary.
// Always use the parameter vk rather than event.vkCode because the caller or caller's caller
// might have adjusted vk, namely to make it a left/right specific modifier key rather than a
// neutral one.
{
	// If this function was called from SuppressThisKey(), these comments apply:
	// Currently SuppressThisKey is only called with a modifier in the rare case
	// when sDisguiseNextLWinUp/RWinUp is in effect.  But there may be other cases in the
	// future, so we need to make sure the physical state of the modifiers is updated
	// in our tracking system even though the key is being suppressed:
	modLR_type modLR;
	if (modLR = kvk[aVK].as_modifiersLR) // Update our tracking of LWIN/RWIN/RSHIFT etc.
	{
		// Caller has ensured that vk has been translated from neutral to left/right if necessary
		// (e.g. VK_CONTROL -> VK_LCONTROL). For this reason, always use the parameter vk rather
		// than the raw event.vkCode.
		// Below excludes KEY_IGNORE_ALL_EXCEPT_MODIFIER since that type of event shouldn't be ignored by
		// this function.  UPDATE: KEY_PHYS_IGNORE is now considered to be something that shouldn't be
		// ignored in this case because if more than one instance has the hook installed, it is
		// possible for g_modifiersLR_logical_non_ignored to say that a key is down in one instance when
		// that instance's g_modifiersLR_logical doesn't say it's down, which is definitely wrong.  So it
		// is now omitted below:
		bool is_not_ignored = (aEvent.dwExtraInfo != KEY_IGNORE);
		bool is_fake_shift = aEvent.scanCode == SC_FAKE_LSHIFT || aEvent.scanCode == SC_FAKE_RSHIFT;
		bool is_fake_ctrl = aEvent.scanCode == SC_FAKE_LCTRL; // AltGr.
		// For backward-compatibility, fake LCtrl is marked as physical.
		bool event_is_physical = !is_fake_shift && KeybdEventIsPhysical(aEvent.flags, aVK, aKeyUp);

		if (aKeyUp)
		{
			// Keep track of system-generated Shift-up events (as part of a workaround for
			// Shift becoming stuck due to interaction between Send and the system handling
			// of shift-numpad combinations).  Find "fake shift" for more details.
			if (is_fake_shift)
				g_modifiersLR_numpad_mask |= modLR;
			if (!aIsSuppressed)
			{
				g_modifiersLR_logical &= ~modLR;
				// Even if is_not_ignored == true, this is updated unconditionally on key-up events
				// to ensure that g_modifiersLR_logical_non_ignored never says a key is down when
				// g_modifiersLR_logical says its up, which might otherwise happen in cases such
				// as alt-tab.  See this comment further below, where the operative word is "relied":
				// "key pushed ALT down, or relied upon it already being down, so go up".  UPDATE:
				// The above is no longer a concern because KeyEvent() now defaults to the mode
				// which causes our var "is_not_ignored" to be true here.  Only the Send command
				// overrides this default, and it takes responsibility for ensuring that the older
				// comment above never happens by forcing any down-modifiers to be up if they're
				// not logically down as reflected in g_modifiersLR_logical.  There's more
				// explanation for g_modifiersLR_logical_non_ignored in keyboard_mouse.h:
				if (is_not_ignored)
					g_modifiersLR_logical_non_ignored &= ~modLR;
			}
			if (event_is_physical) // Note that ignored events can be physical via KEYEVENT_PHYS()
			{
				g_modifiersLR_physical &= ~modLR;
				g_PhysicalKeyState[aVK] = 0;
				if (!is_fake_ctrl)
					g_modifiersLR_ctrlaltdel_mask &= ~modLR;
				// If a modifier with an available neutral VK has been released, update the state
				// of the neutral VK to be that of the opposite key (the one that wasn't released):
				switch (aVK)
				{
				case VK_LSHIFT:   g_PhysicalKeyState[VK_SHIFT] = g_PhysicalKeyState[VK_RSHIFT]; break;
				case VK_RSHIFT:   g_PhysicalKeyState[VK_SHIFT] = g_PhysicalKeyState[VK_LSHIFT]; break;
				case VK_LCONTROL: g_PhysicalKeyState[VK_CONTROL] = g_PhysicalKeyState[VK_RCONTROL]; break;
				case VK_RCONTROL: g_PhysicalKeyState[VK_CONTROL] = g_PhysicalKeyState[VK_LCONTROL]; break;
				case VK_LMENU:    g_PhysicalKeyState[VK_MENU] = g_PhysicalKeyState[VK_RMENU]; break;
				case VK_RMENU:    g_PhysicalKeyState[VK_MENU] = g_PhysicalKeyState[VK_LMENU]; break;
				}
			}
			g_modifiersLR_last_pressed = 0;
		}
		else // Modifier key was pressed down.
		{
			g_modifiersLR_numpad_mask &= ~modLR;
			if (!aIsSuppressed)
			{
				g_modifiersLR_logical |= modLR;
				if (is_not_ignored)
					g_modifiersLR_logical_non_ignored |= modLR;
			}
			if (event_is_physical)
			{
				g_modifiersLR_physical |= modLR;
				g_PhysicalKeyState[aVK] = STATE_DOWN;
				if (!is_fake_ctrl)
					g_modifiersLR_ctrlaltdel_mask |= modLR;
				// If a modifier with an available neutral VK has been pressed down (unlike LWIN & RWIN),
				// update the state of the neutral VK to be down also:
				switch (aVK)
				{
				case VK_LSHIFT:
				case VK_RSHIFT:   g_PhysicalKeyState[VK_SHIFT] = STATE_DOWN; break;
				case VK_LCONTROL:
				case VK_RCONTROL: g_PhysicalKeyState[VK_CONTROL] = STATE_DOWN; break;
				case VK_LMENU:
				case VK_RMENU:    g_PhysicalKeyState[VK_MENU] = STATE_DOWN; break;
				}
			}
			// See comments in GetModifierLRState() for details about the following.
			g_modifiersLR_last_pressed = modLR;
			g_modifiersLR_last_pressed_time = GetTickCount();
		}
	} // vk is a modifier key.
}



bool KeybdEventIsPhysical(DWORD aEventFlags, const vk_type aVK, bool aKeyUp)
// Always use the parameter vk rather than event.vkCode because the caller or caller's caller
// might have adjusted vk, namely to make it a left/right specific modifier key rather than a
// neutral one.
{
	// MSDN: "The keyboard input can come from the local keyboard driver or from calls to the keybd_event
	// function. If the input comes from a call to keybd_event, the input was "injected"".
	// My: This also applies to mouse events, so use it for them too:
	if (aEventFlags & LLKHF_INJECTED)
		return false;
	// So now we know it's a physical event.  But certain SHIFT key-down events are driver-generated.
	// We want to be able to tell the difference because the Send command and other aspects
	// of keyboard functionality need us to be accurate about which keys the user is physically
	// holding down at any given time:
	if (   (aVK == VK_LSHIFT || aVK == VK_RSHIFT) && !aKeyUp   )
	{
		// If the corresponding mask bit is set, the key was temporarily "released" by the system
		// as part of translating a shift-numpad combination to its unshifted counterpart, and this
		// event is the fake key-down which follows the release of the numpad key.  The system uses
		// standard scancodes for this specific case, not SC_FAKE_LSHIFT or SC_FAKE_RSHIFT.
		if (g_modifiersLR_numpad_mask & (aVK == VK_LSHIFT ? MOD_LSHIFT : MOD_RSHIFT))
			return false;
	}

	// Otherwise, it's physical.
	// v1.0.42.04:
	// The time member of the incoming event struct has been observed to be wrongly zero sometimes, perhaps only
	// for AltGr keyboard layouts that generate LControl events when RAlt is pressed (but in such cases, I think
	// it's only sometimes zero, not always).  It might also occur during simulation of Alt+Numpad keystrokes
	// to support {Asc NNNN}.  In addition, SendInput() is documented to have the ability to set its own timestamps;
	// if it's callers put in a bad timestamp, it will probably arrive here that way too.  Thus, use GetTickCount().
	// More importantly, when a script or other application simulates an AltGr keystroke (either down or up),
	// the LControl event received here is marked as physical by the OS or keyboard driver.  This is undesirable
	// primarily because it makes g_TimeLastInputPhysical inaccurate, but also because falsely marked physical
	// events can impact the script's calls to GetKeyState("LControl", "P"), etc.
	g_TimeLastInputPhysical = g_TimeLastInputKeyboard = GetTickCount();
	return true;
}


/////////////////////////////////////////////////////////////////////////////////////////


struct hk_sorted_type
{
	mod_type modifiers;
	modLR_type modifiersLR;
	// Keep sub-32-bit members contiguous to save memory without having to sacrifice performance of
	// 32-bit alignment:
	bool AllowExtraModifiers;
	vk_type vk;
	sc_type sc;
	HotkeyIDType id_with_flags;
};



int sort_most_general_before_least(const void *a1, const void *a2)
// The only items whose order are important are those with the same suffix.  For a given suffix,
// we want the most general modifiers (e.g. CTRL) to appear closer to the top of the list than
// those with more specific modifiers (e.g. CTRL-ALT).  To make qsort() perform properly, it seems
// best to sort by vk/sc then by generality.
{
	hk_sorted_type &b1 = *(hk_sorted_type *)a1; // For performance and convenience.
	hk_sorted_type &b2 = *(hk_sorted_type *)a2;
	if (b1.vk != b2.vk)
		return b1.vk - b2.vk;
	if (b1.sc != b2.sc)
		return b1.sc - b2.sc;

	// If the above didn't return, we now know that a1 and a2 have the same vk's or sc's.  So
	// we use a tie-breaker to cause the most general keys to appear closer to the top of the
	// list than less general ones.  This should result in a given suffix being grouped together
	// after the sort.  Within each suffix group, the most general modifiers should appear first.

	// This part is basically saying that keys that don't allow extra modifiers can always be processed
	// after all other keys:
	if (b1.AllowExtraModifiers && !b2.AllowExtraModifiers)
		return -1;  // Indicate that a1 is smaller, so that it will go to the top.
	if (!b1.AllowExtraModifiers && b2.AllowExtraModifiers)
		return 1;

	// However the order of suffixes that don't allow extra modifiers, among themselves, may be important.
	// Thus we don't return a zero if both have AllowExtraModifiers = 0.
	// Example: User defines ^a, but also defines >^a.  What should probably happen is that >^a forces ^a
	// to fire only when <^a occurs.

	// v1.1.33.03: Compare number of modifiers.  This supersedes some previous checks for when
	// a1's modifiers are a subset of a2's or vice versa (since the subset would always have
	// fewer bits).  This new method helps prioritize combinations which overlap but have a
	// different number of modifiers, such as "*<^a" vs. "*<^>^a".
	UINT nmodLR_a1 = popcount8(b1.modifiersLR);
	UINT nmodLR_a2 = popcount8(b2.modifiersLR);
	UINT nmod_a1 = popcount8(b1.modifiers) + nmodLR_a1;
	UINT nmod_a2 = popcount8(b2.modifiers) + nmodLR_a2;
	if (nmod_a1 != nmod_a2)
		return nmod_a1 - nmod_a2;
	if (nmodLR_a1 != nmodLR_a2)
		return nmodLR_a1 - nmodLR_a2;
	// v1.1.33.03: Sort by modifier value so that key-up hotkeys end up immediately after their
	// counterparts, otherwise we get odd results like Alt+Shift+A firing "*!a" and "*+a up"
	// instead of "*!a" and "*!a up" or "*+a" and "*+a up".
	if (b1.modifiers != b2.modifiers)
		return b1.modifiers - b2.modifiers; // !^+#
	if (b1.modifiersLR != b2.modifiersLR)
		return b1.modifiersLR - b2.modifiersLR; // <^>^<!>!<+>+<#>#

	// v1.0.38.03: The following check is added to handle a script containing hotkeys
	// such as the following (in this order):
	// *MButton::
	// *Mbutton Up::
	// MButton::
	// MButton Up::
	// What would happen before is that the qsort() would sometimes cause "MButton Up" from the
	// list above to be processed prior to "MButton", which would set hotkey_up[*MButton's ID]
	// to be MButton Up's ID.  Then when "MButton" was processed, it would set its_table_entry
	// to MButton's ID, but hotkey_up[MButton's ID] would be wrongly left INVALID when it should
	// have received a copy of the asterisk hotkey ID's counterpart key-up ID.  However, even
	// giving it a copy would not be quite correct because then *MButton's ID would wrongly
	// be left associated with MButton's Up's ID rather than *MButton Up's.  By solving the
	// problem here in the sort rather than copying the ID, both bugs are resolved.
	// v1.1.33.03: The scenario above would now also be prevented by checks in ChangeHookState
	// which avoid pairing a key-up hotkey with a more permissive key-down hotkey, but keeping
	// this might help ensure key-up hotkeys are matched optimally when there is overlap.
	//if ((b1.id_with_flags & HOTKEY_KEY_UP) != (b2.id_with_flags & HOTKEY_KEY_UP))
	//	return (b1.id_with_flags & HOTKEY_KEY_UP) ? 1 : -1; // Put key-up hotkeys higher in the list than their down counterparts (see comment above).

	// v1.1.33.03: Getting to this point should mean that a1 and a2 have the same modifiers,
	// vk and sc, but they might have different up/down status and key name (Esc/Escape/vk1B).
	// Ensure predictability by putting them in an order based on id_with_flags.
	return b1.id_with_flags - b2.id_with_flags;
}



void SetModifierAsPrefix(vk_type aVK, sc_type aSC, bool aAlwaysSetAsPrefix = false)
// The caller has already ensured that vk and/or sc is a modifier such as VK_CONTROL.
{
	if (aVK)
	{
		switch (aVK)
		{
		case VK_MENU:
		case VK_SHIFT:
		case VK_CONTROL:
			// Since the user is configuring both the left and right counterparts of a key to perform a suffix action,
			// it seems best to always consider those keys to be prefixes so that their suffix action will only fire
			// when the key is released.  That way, those keys can still be used as normal modifiers.
			// UPDATE for v1.0.29: But don't do it if there is a corresponding key-up hotkey for this neutral
			// modifier, which allows a remap such as the following to succeed:
			// Control::Send {LWin down}
			// Control up::Send {LWin up}
			if (!aAlwaysSetAsPrefix)
			{
				for (int i = 0; i < Hotkey::sHotkeyCount; ++i)
				{
					Hotkey &h = *Hotkey::shk[i]; // For performance and convenience.
					if (h.mVK == aVK && h.mKeyUp && !h.mModifiersConsolidatedLR && !h.mModifierVK && !h.mModifierSC
						&& !h.IsCompletelyDisabled())
						return; // Since caller didn't specify aAlwaysSetAsPrefix==true, don't make this key a prefix.
				}
			}
			switch (aVK)
			{
			case VK_MENU:
				kvk[VK_MENU].used_as_prefix = PREFIX_FORCED;
				kvk[VK_LMENU].used_as_prefix = PREFIX_FORCED;
				kvk[VK_RMENU].used_as_prefix = PREFIX_FORCED;
				ksc[SC_LALT].used_as_prefix = PREFIX_FORCED;
				ksc[SC_RALT].used_as_prefix = PREFIX_FORCED;
				break;
			case VK_SHIFT:
				kvk[VK_SHIFT].used_as_prefix = PREFIX_FORCED;
				kvk[VK_LSHIFT].used_as_prefix = PREFIX_FORCED;
				kvk[VK_RSHIFT].used_as_prefix = PREFIX_FORCED;
				ksc[SC_LSHIFT].used_as_prefix = PREFIX_FORCED;
				ksc[SC_RSHIFT].used_as_prefix = PREFIX_FORCED;
				break;
			case VK_CONTROL:
				kvk[VK_CONTROL].used_as_prefix = PREFIX_FORCED;
				kvk[VK_LCONTROL].used_as_prefix = PREFIX_FORCED;
				kvk[VK_RCONTROL].used_as_prefix = PREFIX_FORCED;
				ksc[SC_LCONTROL].used_as_prefix = PREFIX_FORCED;
				ksc[SC_RCONTROL].used_as_prefix = PREFIX_FORCED;
				break;
			}
			break;

		default:  // vk is a left/right modifier key such as VK_LCONTROL or VK_LWIN:
			if (aAlwaysSetAsPrefix)
				kvk[aVK].used_as_prefix = PREFIX_ACTUAL;
			else
				if (Hotkey::FindHotkeyContainingModLR(kvk[aVK].as_modifiersLR)) // Fixed for v1.0.35.13 (used to be aSC vs. aVK).
					kvk[aVK].used_as_prefix = PREFIX_ACTUAL;
				// else allow its suffix action to fire when key is pressed down,
				// under the fairly safe assumption that the user hasn't configured
				// the opposite key to also be a key-down suffix-action (but even
				// if the user has done this, it's an explicit override of the
				// safety checks here, so probably best to allow it).
		}
		return;
	}
	// Since above didn't return, using scan code instead of virtual key:
	if (aAlwaysSetAsPrefix)
		ksc[aSC].used_as_prefix = PREFIX_ACTUAL;
	else
		if (Hotkey::FindHotkeyContainingModLR(ksc[aSC].as_modifiersLR))
			ksc[aSC].used_as_prefix = PREFIX_ACTUAL;
}



void ChangeHookState(Hotkey *aHK[], int aHK_count, HookType aWhichHook, HookType aWhichHookAlways)
// Caller must verify that aWhichHook and aWhichHookAlways accurately reflect the hooks that should
// be active when we return.  For example, the caller must have already taken into account which
// hotkeys/hotstrings are suspended, disabled, etc.
//
// Caller should always be the main thread, never the hook thread.
// One reason is that this function isn't thread-safe.  Another is that new/delete/malloc/free
// themselves might not be thread-safe when the single-threaded CRT libraries are in effect
// (not using multi-threaded libraries due to a 3.5 KB increase in compressed code size).
//
// The input params are unnecessary because could just access directly by using Hotkey::shk[].
// But aHK is a little more concise.
// aWhichHookAlways was added to force the hooks to be installed (or stay installed) in the case
// of #InstallKeybdHook and #InstallMouseHook.  This is so that these two commands will always
// still be in effect even if hotkeys are suspended, so that key history can still be monitored via
// the hooks.
// Returns the set of hooks that are active after processing is complete.
{
	// Determine the set of hooks that should be activated or deactivated.
	HookType hooks_to_be_active = aWhichHook | aWhichHookAlways; // Bitwise union.

	if (!hooks_to_be_active) // No need to check any further in this case.  Just remove all hooks.
	{
		AddRemoveHooks(0); // Remove all hooks.
		return;
	}

	// Even if hooks_to_be_active indicates no change to hook status, we still need to continue in case
	// this is a suspend or unsuspend operation.  In both of those cases, though the hook(s)
	// may already be active, the hotkey configuration probably needs to be updated.
	// Related: Even if aHK_count is zero, still want to install the hook(s) whenever
	// aWhichHookAlways specifies that they should be.  This is done so that the
	// #InstallKeybdHook and #InstallMouseHook directives can have the hooks installed just
	// for use with something such as the KeyHistory feature, or for Hotstrings, Numlock AlwaysOn,
	// the Input command, and possibly others.

	// Now we know that at least one of the hooks is a candidate for activation.
	// Set up the arrays process all of the hook hotkeys even if the corresponding hook won't
	// become active (which should only happen if g_IsSuspended is true
	// and it turns out there are no suspend-hotkeys that are handled by the hook).

	// These arrays are dynamically allocated so that memory is conserved in cases when
	// the user doesn't need the hook at all (i.e. just normal registered hotkeys).
	// This is a waste of memory if there are no hook hotkeys, but currently the operation
	// of the hook relies upon these being allocated, even if the arrays are all clean
	// slates with nothing in them (it could check if the arrays are NULL but then the
	// performance would be slightly worse for the "average" script).  Presumably, the
	// caller is requesting the keyboard hook with zero hotkeys to support the forcing
	// of Num/Caps/ScrollLock always on or off (a fairly rare situation, probably):
	if (!kvk)  // Since it's an initialized global, this indicates that all 4 objects are not yet allocated.
	{
		if (   !(kvk = new key_type[VK_ARRAY_COUNT])
			|| !(ksc = new key_type[SC_ARRAY_COUNT])
			|| !(kvkm = new HotkeyIDType[KVKM_SIZE])
			|| !(kscm = new HotkeyIDType[KSCM_SIZE])
			|| !(hotkey_up = (HotkeyIDType *)malloc(Hotkey::shkMax * sizeof(HotkeyIDType)))   )
		{
			// At least one of the allocations failed.
			// Keep all 4 objects in sync with one another (i.e. either all allocated, or all not allocated):
			FreeHookMem(); // Since none of the hooks is active, just free any of the above memory that was partially allocated.
			return;
		}

		// Done once immediately after allocation to init attributes such as pForceToggle and as_modifiersLR,
		// which are zero for most keys:
		ZeroMemory(kvk, VK_ARRAY_COUNT * sizeof(key_type));
		ZeroMemory(ksc, SC_ARRAY_COUNT * sizeof(key_type));

		// Below is also a one-time-only init:
		// This attribute is exists for performance reasons (avoids a function call in the hook
		// procedure to determine this value):
		kvk[VK_CONTROL].as_modifiersLR = MOD_LCONTROL | MOD_RCONTROL;
		kvk[VK_LCONTROL].as_modifiersLR = MOD_LCONTROL;
		kvk[VK_RCONTROL].as_modifiersLR = MOD_RCONTROL;
		kvk[VK_MENU].as_modifiersLR = MOD_LALT | MOD_RALT;
		kvk[VK_LMENU].as_modifiersLR = MOD_LALT;
		kvk[VK_RMENU].as_modifiersLR = MOD_RALT;
		kvk[VK_SHIFT].as_modifiersLR = MOD_LSHIFT | MOD_RSHIFT;
		kvk[VK_LSHIFT].as_modifiersLR = MOD_LSHIFT;
		kvk[VK_RSHIFT].as_modifiersLR = MOD_RSHIFT;
		kvk[VK_LWIN].as_modifiersLR = MOD_LWIN;
		kvk[VK_RWIN].as_modifiersLR = MOD_RWIN;

		// This is a bit iffy because it's far from certain that these particular scan codes
		// are really modifier keys on anything but a standard English keyboard.  However,
		// long years of use haven't shown this to be a problem, and there are certainly other
		// parts of the code that do not support custom layouts remapping the modifier keys.
		ksc[SC_LCONTROL].as_modifiersLR = MOD_LCONTROL;
		ksc[SC_RCONTROL].as_modifiersLR = MOD_RCONTROL;
		ksc[SC_LALT].as_modifiersLR = MOD_LALT;
		ksc[SC_RALT].as_modifiersLR = MOD_RALT;
		ksc[SC_LSHIFT].as_modifiersLR = MOD_LSHIFT;
		ksc[SC_RSHIFT].as_modifiersLR = MOD_RSHIFT;
		ksc[SC_LWIN].as_modifiersLR = MOD_LWIN;
		ksc[SC_RWIN].as_modifiersLR = MOD_RWIN;

		// Use the address rather than the value, so that if the global var's value
		// changes during runtime, ours will too:
		kvk[VK_SCROLL].pForceToggle = &g_ForceScrollLock;
		kvk[VK_CAPITAL].pForceToggle = &g_ForceCapsLock;
		kvk[VK_NUMLOCK].pForceToggle = &g_ForceNumLock;
	}

	// Init only those attributes which reflect the hotkey's definition, not those that reflect
	// the key's current status (since those are initialized only if the hook state is changing
	// from OFF to ON (later below):
	int i;
	for (i = 0; i < VK_ARRAY_COUNT; ++i)
		RESET_KEYTYPE_ATTRIB(kvk[i])
	for (i = 0; i < SC_ARRAY_COUNT; ++i)
		RESET_KEYTYPE_ATTRIB(ksc[i]) // Note: ksc not kvk.

	// Indicate here which scan codes should override their virtual keys:
	for (i = 0; i < g_key_to_sc_count; ++i)
		if (g_key_to_sc[i].sc > 0 && g_key_to_sc[i].sc <= SC_MAX)
			ksc[g_key_to_sc[i].sc].sc_takes_precedence = true;

	// These have to be initialized with element value INVALID.
	// Don't use FillMemory because the array elements are too big (bigger than bytes):
	for (i = 0; i < KVKM_SIZE; ++i) // Simplify by viewing 2-dimensional array as a 1-dimensional array.
		kvkm[i] = HOTKEY_ID_INVALID;
	for (i = 0; i < KSCM_SIZE; ++i) // Simplify by viewing 2-dimensional array as a 1-dimensional array.
		kscm[i] = HOTKEY_ID_INVALID;
	for (i = 0; i < Hotkey::shkMax; ++i)
		hotkey_up[i] = HOTKEY_ID_INVALID;

	hk_sorted_type *hk_sorted = new hk_sorted_type[Hotkey::sHotkeyCount];
	int hk_sorted_count = 0;
	key_type *pThisKey = NULL;
	for (i = 0; i < aHK_count; ++i)
	{
		Hotkey &hk = *aHK[i]; // For performance and convenience.

		// If it's not a hook hotkey (e.g. it was already registered with RegisterHotkey() or it's a joystick
		// hotkey) don't process it here.  Similarly, if g_IsSuspended is true, we won't include it unless it's
		// exempt from suspension:
		if (   !HK_TYPE_IS_HOOK(hk.mType)
			|| (g_IsSuspended && !hk.IsExemptFromSuspend())
			|| hk.IsCompletelyDisabled()   ) // Listed last for short-circuit performance.
			continue;

		// Rule out the possibility of obnoxious values right away, preventing array-out-of bounds, etc.:
		if ((!hk.mVK && !hk.mSC) || hk.mVK > VK_MAX || hk.mSC > SC_MAX)
			continue;

		if (!hk.mVK)
		{
			// scan codes don't need something like the switch stmt below because they can't be neutral.
			// In other words, there's no scan code equivalent for something like VK_CONTROL.
			// In addition, SC_LCONTROL, for example, doesn't also need to change the kvk array
			// for VK_LCONTROL because the hook knows to give the scan code precedence, and thus
			// look it up only in the ksc array in that case.
			pThisKey = ksc + hk.mSC;
			// For some scan codes this was already set above.  But to support explicit scan code hotkeys,
			// such as "SC102::MsgBox", make sure it's set for every hotkey that uses an explicit scan code.
			pThisKey->sc_takes_precedence = true;
		}
		else
		{
			pThisKey = kvk + hk.mVK;
			// Keys that have a neutral as well as a left/right counterpart must be
			// fully initialized since the hook can receive the left, the right, or
			// the neutral (neutral only if another app calls KeyEvent(), probably).
			// There are several other switch stmts in this function like the below
			// that serve a similar purpose.  The alternative to doing all these
			// switch stmts is to always translate left/right vk's (whose sc's don't
			// take precedence) in the KeyboardProc() itself.  But that would add
			// the overhead of a switch stmt to *every* keypress ever made on the
			// system, so it seems better to set up everything correctly here since
			// this init section is only done once.
			switch (hk.mVK)
			{
			case VK_MENU:
				// It's not strictly necessary to init all of these, since the
				// hook currently never handles VK_RMENU, for example, by its
				// vk (it uses sc instead).  But it's safest to do all of them
				// in case future changes ever ruin that assumption:
				kvk[VK_LMENU].used_as_suffix = true;
				kvk[VK_RMENU].used_as_suffix = true;
				ksc[SC_LALT].used_as_suffix = true;
				ksc[SC_RALT].used_as_suffix = true;
				if (hk.mKeyUp) // Fix for v1.1.07.03: Set only if true in case there was already an "up" hotkey.
				{
					kvk[VK_LMENU].used_as_key_up = true;
					kvk[VK_RMENU].used_as_key_up = true;
					ksc[SC_LALT].used_as_key_up = true;
					ksc[SC_RALT].used_as_key_up = true;
				}
				break;
			case VK_SHIFT:
				// The neutral key itself is also set to be a suffix further below.
				kvk[VK_LSHIFT].used_as_suffix = true;
				kvk[VK_RSHIFT].used_as_suffix = true;
				ksc[SC_LSHIFT].used_as_suffix = true;
				ksc[SC_RSHIFT].used_as_suffix = true;
				if (hk.mKeyUp) // Fix for v1.1.07.03: Set only if true in case there was already an "up" hotkey.
				{
					kvk[VK_LSHIFT].used_as_key_up = true;
					kvk[VK_RSHIFT].used_as_key_up = true;
					ksc[SC_LSHIFT].used_as_key_up = true;
					ksc[SC_RSHIFT].used_as_key_up = true;
				}
				break;
			case VK_CONTROL:
				kvk[VK_LCONTROL].used_as_suffix = true;
				kvk[VK_RCONTROL].used_as_suffix = true;
				ksc[SC_LCONTROL].used_as_suffix = true;
				ksc[SC_RCONTROL].used_as_suffix = true;
				if (hk.mKeyUp) // Fix for v1.1.07.03: Set only if true in case there was already an "up" hotkey.
				{
					kvk[VK_LCONTROL].used_as_key_up = true;
					kvk[VK_RCONTROL].used_as_key_up = true;
					ksc[SC_LCONTROL].used_as_key_up = true;
					ksc[SC_RCONTROL].used_as_key_up = true;
				}
				break;
			// Later might want to add cases for VK_LCONTROL and such, but for right now,
			// these keys should never come up since they're done by scan code?
			}
		}

		pThisKey->used_as_suffix = true;
		HotkeyIDType hotkey_id_with_flags = hk.mID;

		if (hk.mKeyUp)
		{
			pThisKey->used_as_key_up = true;
			hotkey_id_with_flags |= HOTKEY_KEY_UP;
		}

		bool hk_is_custom_combo = hk.mModifierVK || hk.mModifierSC;

		// If this is a naked (unmodified) modifier key, make it a prefix if it ever modifies any
		// other hotkey.  This processing might be later combined with the hotkeys activation function
		// to eliminate redundancy / improve efficiency, but then that function would probably need to
		// init everything else here as well:
		if (pThisKey->as_modifiersLR && !hk.mModifiersConsolidatedLR && !hk_is_custom_combo
			&& !(hk.mNoSuppress & AT_LEAST_ONE_VARIANT_HAS_TILDE)) // v1.0.45.02: ~Alt, ~Control, etc. should fire upon press-down, not release (broken by 1.0.44's PREFIX_FORCED, but I think it was probably broken in pre-1.0.41 too).
			SetModifierAsPrefix(hk.mVK, hk.mSC);

		if (hk_is_custom_combo)
		{
			if (hk.mModifierVK)
			{
				if (kvk[hk.mModifierVK].as_modifiersLR)
					// The hotkey's ModifierVK is itself a modifier.
					SetModifierAsPrefix(hk.mModifierVK, 0, true);
				else
				{
					kvk[hk.mModifierVK].used_as_prefix = PREFIX_ACTUAL;
					if (hk.mNoSuppress & NO_SUPPRESS_PREFIX)
						kvk[hk.mModifierVK].no_suppress |= AT_LEAST_ONE_COMBO_HAS_TILDE;
				}
			}
			else //if (hk.mModifierSC)
			{
				if (ksc[hk.mModifierSC].as_modifiersLR)  // Fixed for v1.0.35.13 (used to be kvk vs. ksc).
					// The hotkey's ModifierSC is itself a modifier.
					SetModifierAsPrefix(0, hk.mModifierSC, true);
				else
				{
					ksc[hk.mModifierSC].used_as_prefix = PREFIX_ACTUAL;
					if (hk.mNoSuppress & NO_SUPPRESS_PREFIX)
						ksc[hk.mModifierSC].no_suppress |= AT_LEAST_ONE_COMBO_HAS_TILDE;
					// For some scan codes this was already set above.  But to support explicit scan code prefixes,
					// such as "SC118 & SC122::MsgBox", make sure it's set for every prefix that uses an explicit
					// scan code:
					ksc[hk.mModifierSC].sc_takes_precedence = true;
				}
			}
			// Insert this hotkey at the front of the linked list of hotkeys which use this suffix key.
			hk.mNextHotkey = pThisKey->first_hotkey;
			pThisKey->first_hotkey = hk.mID;
			continue;
		}

		// At this point, since the above didn't "continue", this hotkey is one without a ModifierVK/SC.
		// Put it into a temporary array, which will be later sorted:
		hk_sorted[hk_sorted_count].id_with_flags = hk.mHookAction ? hk.mHookAction : hotkey_id_with_flags;
		hk_sorted[hk_sorted_count].vk = hk.mVK;
		hk_sorted[hk_sorted_count].sc = hk.mSC;
		hk_sorted[hk_sorted_count].modifiers = hk.mModifiers;
		hk_sorted[hk_sorted_count].modifiersLR = hk.mModifiersLR;
		hk_sorted[hk_sorted_count].AllowExtraModifiers = hk.mAllowExtraModifiers;
		++hk_sorted_count;
	}

	if (hk_sorted_count)
	{
		// It's necessary to get them into this order to avoid problems that would be caused by
		// AllowExtraModifiers:
		qsort((void *)hk_sorted, hk_sorted_count, sizeof(hk_sorted_type), sort_most_general_before_least);

		// For each hotkey without a ModifierVK/SC (which override normal modifiers), expand its modifiers and
		// modifiersLR into its column in the kvkm or kscm arrays.

		mod_type modifiers, i_modifiers_merged;
		modLR_type i_modifiersLR_excluded;
		int modifiersLR;  // Don't make this modLR_type to avoid integer overflow, since it's a loop-counter.
		bool prev_hk_is_key_up, this_hk_is_key_up;
		HotkeyIDType prev_hk_id, this_hk_id;

		for (i = 0; i < hk_sorted_count; ++i)
		{
			hk_sorted_type &this_hk = hk_sorted[i]; // For performance and convenience.
			this_hk_is_key_up = this_hk.id_with_flags & HOTKEY_KEY_UP;
			this_hk_id = this_hk.id_with_flags & HOTKEY_ID_MASK;

			if (this_hk_id <= HOTKEY_ID_MAX) // It's a valid ID and not an ALT_TAB action.
			{
				// Insert this hotkey at the front of the list of hotkeys that use this suffix key.
				// This enables fallback between overlapping hotkeys, such as LCtrl & a, <^+a, ^+a.
				pThisKey = this_hk.vk ? kvk + this_hk.vk : ksc + this_hk.sc;
				// Insert after any custom combos.
				HotkeyIDType *first = &pThisKey->first_hotkey;
				while (*first != HOTKEY_ID_INVALID && (aHK[*first]->mModifierVK || aHK[*first]->mModifierSC))
					first = &aHK[*first]->mNextHotkey;
				aHK[this_hk_id]->mNextHotkey = *first;
				*first = this_hk_id;
			}

			i_modifiers_merged = this_hk.modifiers;
			if (this_hk.modifiersLR)
				i_modifiers_merged |= ConvertModifiersLR(this_hk.modifiersLR);
			
			// Fixed for v1.1.27.00: Calculate the modifiersLR bits which are NOT allowed to be set.
			// This fixes <^A erroneously taking over <>^A, and reduces the work that must be done
			// on each iteration of the loop below.
			i_modifiersLR_excluded = this_hk.AllowExtraModifiers ? 0
				: ~(this_hk.modifiersLR | ConvertModifiers(this_hk.modifiers));

			for (modifiersLR = 0; modifiersLR <= MODLR_MAX; ++modifiersLR)  // For each possible LR value.
			{
				if (modifiersLR & i_modifiersLR_excluded) // Checked first to avoid the ConvertModifiersLR call in many cases.
					continue;
				modifiers = ConvertModifiersLR(modifiersLR);
				// Below is true if modifiersLR is a superset of i's modifier value.  In other words,
				// modifiersLR has the minimum required keys.  It may also have some extraneous keys,
				// but only if they were not excluded by the check above, in which case they are allowed.
				if (i_modifiers_merged != (modifiers & i_modifiers_merged))
					continue;

				// In addition to the above, modifiersLR must also have the *specific* left or right keys
				// found in i's modifiersLR.  In other words, i's modifiersLR must be a perfect subset
				// of modifiersLR:
				if (this_hk.modifiersLR) // make sure that any more specific left/rights are also present.
					if (this_hk.modifiersLR != (modifiersLR & this_hk.modifiersLR))
						continue;

				// scan codes don't need the switch() stmt below because, for example,
				// the hook knows to look up left-control by only SC_LCONTROL, not VK_LCONTROL.
				bool do_cascade = this_hk.vk;

				// If above didn't "continue", modifiersLR is a valid hotkey combination so set it as such:
				HotkeyIDType &its_table_entry = this_hk.vk ? Kvkm(modifiersLR, this_hk.vk) : Kscm(modifiersLR, this_hk.sc);
				if (its_table_entry == HOTKEY_ID_INVALID) // Since there is no ID currently in the slot, key-up/down doesn't matter.
					its_table_entry = this_hk.id_with_flags;
				else
				{
					prev_hk_id = its_table_entry & HOTKEY_ID_MASK;
					if (this_hk_id >= Hotkey::sHotkeyCount || prev_hk_id >= Hotkey::sHotkeyCount) // AltTab hotkey.
						continue; // Exclude AltTab hotkeys since hotkey_up[] and shk[] can't be used.
					prev_hk_is_key_up = its_table_entry & HOTKEY_KEY_UP;
					if (this_hk_is_key_up && !prev_hk_is_key_up) // Override any existing key-up hotkey for this down hotkey ID, e.g. "LButton Up" takes precedence over "*LButton Up".
					{
						Hotkey &prev_hk = *Hotkey::shk[prev_hk_id];
						// v1.1.33.03: Since modifiers aren't checked when hotkey_to_fire_upon_release is used
						// to fire a key-up hotkey, avoid setting setting this_hk as prev_hk's up hotkey when:
						//   a) prev_hk permits modifiers that this_hk does not permit (i.e. requires to be up).
						//   b) this_hk requires modifiers that prev_hk does not require (i.e. might not be pressed).
						//
						//	a up::    ; Doesn't permit any modifiers.
						//	*a::      ; Permits all modifiers, so shouldn't necessarily fire "a up".
						//	<^b up::  ; Doesn't permit RCtrl.
						//	^b::      ; Permits RCtrl, so shouldn't necessarily fire "<^b up".
						//	*^c up::  ; Requires Ctrl.
						//	*+c::     ; Doesn't require Ctrl, so shouldn't necessarily fire "^c up".
						//
						// Note that prev_hk.mModifiersConsolidatedLR includes all LR modifiers that CAN be down,
						// but some might not be required, so might not be down (e.g. ^b has MOD_LCTRL|MOD_RCTRL).
						// However, if either LCTRL or RCTRL is set there, we know CTRL will be down, so the result
						// of ConvertModifiersLR() tells us which neutral modifiers will definitely be down.
						// prev_hk.mModifiers is checked first to avoid the function call where possible.
						if (  !((prev_hk.mAllowExtraModifiers ? MODLR_MAX : prev_hk.mModifiersConsolidatedLR) & i_modifiersLR_excluded)
							&& !(modLR_type)(this_hk.modifiersLR & ~prev_hk.mModifiersLR)
							&& (!(mod_type)(this_hk.modifiers & ~prev_hk.mModifiers)
							 || !(mod_type)(this_hk.modifiers & ~ConvertModifiersLR(prev_hk.mModifiersConsolidatedLR)))  )
						{
							hotkey_up[prev_hk_id] = this_hk.id_with_flags;
							do_cascade = false;  // Every place the down-hotkey ID already appears, it will point to this same key-up hotkey.
						}
						else
						{
							// v1.1.33.03: Override the lower-priority key-down hotkey which was already present.
							// Hotkey::FindPairedHotkey will be used to locate a key-down hotkey to fire based on
							// current modifier state.
							its_table_entry = this_hk.id_with_flags;
						}
					}
					else 
					{
						HotkeyIDType new_up_id;
						if (!this_hk_is_key_up && prev_hk_is_key_up)
							// Swap them so that the down-hotkey is in the main array and the up in the secondary:
							new_up_id = its_table_entry;
						else
							if (prev_hk_is_key_up || hotkey_up[this_hk_id] != HOTKEY_ID_INVALID)
								// Both are key-up hotkeys, or this_hk already has a key-up hotkey, in which case it
								// isn't overwritten since there's no guarantee the new one is more appropriate, and
								// it can cause the effect of swapping hotkey_up[] between two values repeatedly.
								new_up_id = HOTKEY_ID_INVALID;
							else // Both are key-down hotkeys.
								// Fix for v1.0.40.09: Also copy the previous hotkey's corresponding up-hotkey (if any)
								// so that this hotkey will have that same one.  This also solves the issue of a hotkey
								// such as "^!F1" firing twice (once for down and once for up) when "*F1" and "*F1 up"
								// are both hotkeys.  Instead, the "*F1 up" hotkey should fire upon release of "^!F1"
								// so that the behavior is consistent with the case where "*F1" isn't present as a hotkey.
								// This fix doesn't appear to break anything else, most notably it still allows a hotkey
								// such as "^!F1 up" to take precedence over "*F1 up" because in such a case, this
								// code would never have executed because prev_hk_is_key_up would be true but
								// this_hk_is_key_up would be false.  Note also that sort_most_general_before_least()
								// has put key-up hotkeys after their key-down counterparts in the list.
								// v1.1.33.03: Without this "^!F1" won't fire twice, but it also won't fire "*F1 up".
								new_up_id = hotkey_up[prev_hk_id];

						if (new_up_id != HOTKEY_ID_INVALID)
						{
							Hotkey &new_up_hk = *Hotkey::shk[new_up_id & HOTKEY_ID_MASK];
							// v1.1.33.03: Since modifiers aren't checked when hotkey_to_fire_upon_release is used
							// to fire a key-up hotkey, avoid setting setting new_up_hk as this_hk's up hotkey when:
							//   a) this_hk permits modifiers that new_up_hk does not.
							//   b) new_up_hk requires modifiers that this_hk does not.
							//
							//	<^a up::  ; Does not permit RCtrl.
							//	^a::      ; Permits RCtrl, so shouldn't necessarily fire "<^a up".
							//	*!1 up::  ; Requires Alt.
							//	*<^1::    ; Doesn't require Alt, so shouldn't necessarily fire "*!1 up".
							//
							// ~i_modifiersLR_excluded already accounts for this_hk.AllowExtraModifiers.
							//if (  !(modLR_type)(~i_modifiersLR_excluded & (new_up_hk.mAllowExtraModifiers ? 0 : ~new_up_hk.mModifiersConsolidatedLR))  )
							if (  (new_up_hk.mAllowExtraModifiers || !(modLR_type)(~i_modifiersLR_excluded & ~new_up_hk.mModifiersConsolidatedLR))
								&& !(new_up_hk.mModifiers & ~i_modifiers_merged) && !(new_up_hk.mModifiersLR & ~this_hk.modifiersLR)  )
								hotkey_up[this_hk_id] = new_up_id;
						}

						// Either both are key-up hotkeys or both are key-down hotkeys.  this overrides prev.
						its_table_entry = this_hk.id_with_flags;
					}
				}

				if (do_cascade)
				{
					switch (this_hk.vk)
					{
					case VK_MENU:
					case VK_LMENU: // In case the program is ever changed to support these VKs directly.
						Kvkm(modifiersLR, VK_LMENU) = this_hk.id_with_flags;
						Kscm(modifiersLR, SC_LALT) = this_hk.id_with_flags;
						if (this_hk.vk == VK_LMENU)
							break;
						//else fall through so that VK_MENU also gets the right side set below:
					case VK_RMENU:
						Kvkm(modifiersLR, VK_RMENU) = this_hk.id_with_flags;
						Kscm(modifiersLR, SC_RALT) = this_hk.id_with_flags;
						break;
					case VK_SHIFT:
					case VK_LSHIFT:
						Kvkm(modifiersLR, VK_LSHIFT) = this_hk.id_with_flags;
						Kscm(modifiersLR, SC_LSHIFT) = this_hk.id_with_flags;
						if (this_hk.vk == VK_LSHIFT)
							break;
						//else fall through so that VK_SHIFT also gets the right side set below:
					case VK_RSHIFT:
						Kvkm(modifiersLR, VK_RSHIFT) = this_hk.id_with_flags;
						Kscm(modifiersLR, SC_RSHIFT) = this_hk.id_with_flags;
						break;
					case VK_CONTROL:
					case VK_LCONTROL:
						Kvkm(modifiersLR, VK_LCONTROL) = this_hk.id_with_flags;
						Kscm(modifiersLR, SC_LCONTROL) = this_hk.id_with_flags;
						if (this_hk.vk == VK_LCONTROL)
							break;
						//else fall through so that VK_CONTROL also gets the right side set below:
					case VK_RCONTROL:
						Kvkm(modifiersLR, VK_RCONTROL) = this_hk.id_with_flags;
						Kscm(modifiersLR, SC_RCONTROL) = this_hk.id_with_flags;
						break;
					} // switch()
				} // if (do_cascade)
			}
		}
	}

	delete[] hk_sorted;

	// Support "Control", "Alt" and "Shift" as suffix keys by appending their lists of
	// custom combos to the lists used by their left and right versions.  This avoids the
	// need for the hook to detect these keys and perform a search through a second list.
	// This must be done after all custom combos have been processed above, since they
	// might be defined in any order, but the neutral hotkeys must be placed last.
	if (kvk[VK_SHIFT].used_as_suffix) // Skip the following unless Shift, LShift or RShift was used as a suffix.
		LinkKeysForCustomCombo(VK_SHIFT, VK_LSHIFT, VK_RSHIFT);
	if (kvk[VK_CONTROL].used_as_suffix)
		LinkKeysForCustomCombo(VK_CONTROL, VK_LCONTROL, VK_RCONTROL);
	if (kvk[VK_MENU].used_as_suffix)
		LinkKeysForCustomCombo(VK_MENU, VK_LMENU, VK_RMENU);

	// Add or remove hooks, as needed.  No change is made if the hooks are already in the correct state.
	AddRemoveHooks(hooks_to_be_active);
}



bool HookAdjustMaxHotkeys(Hotkey **&aHK, int &aCurrentMax, int aNewMax)
{
	Hotkey **new_shk = (Hotkey **)malloc(aNewMax * sizeof(Hotkey *));
	if (!new_shk)
		return false;
	HotkeyIDType *new_hotkey_up = NULL;
	if (   hotkey_up // No allocation needed if the hooks haven't been active yet.
		&& !(new_hotkey_up = (HotkeyIDType *)malloc(aNewMax * sizeof(HotkeyIDType)))   )
	{
		free(new_shk);
		return false;
	}
	// From here on, success is certain.
	if (aCurrentMax)
	{
		memcpy(new_shk, aHK, aCurrentMax * sizeof(Hotkey *));
		if (hotkey_up)
			memcpy(new_hotkey_up, hotkey_up, aCurrentMax * sizeof(HotkeyIDType));
			// No initialization is needed for the new portion since the hook won't be aware
			// of any new hotkeys prior to ChangeHookState(), which also refreshes hotkey_up.
	}
	Hotkey **old_shk = aHK;
	HotkeyIDType *old_hotkey_up = hotkey_up;
	aHK = new_shk;
	hotkey_up = new_hotkey_up;
	// At this stage, old_shk and new_shk are interchangeable.  To avoid race conditions with
	// the hook thread, wait for it to reach a known idle state before freeing the old array.
	WaitHookIdle();
	aCurrentMax = aNewMax;
	free(old_shk);
	free(old_hotkey_up);
	return true;
}



HotkeyIDType &CustomComboLast(HotkeyIDType *aFirst)
{
	for (; *aFirst != HOTKEY_ID_INVALID; aFirst = &Hotkey::shk[*aFirst]->mNextHotkey);
	return *aFirst;
}

void LinkKeysForCustomCombo(vk_type aNeutral, vk_type aLeft, vk_type aRight)
{
	HotkeyIDType first_neutral = kvk[aNeutral].first_hotkey;
	if (first_neutral == HOTKEY_ID_INVALID)
		return;
	// Append the neutral key's list to the lists of the left and right keys.
	CustomComboLast(&kvk[aLeft].first_hotkey) = first_neutral;
	CustomComboLast(&kvk[aRight].first_hotkey) = first_neutral;
}



void AddRemoveHooks(HookType aHooksToBeActive, bool aChangeIsTemporary)
// Caller has ensured that any static memory arrays used by the hook functions have been allocated.
// Caller is always the main thread, never the hook thread because this function isn't thread-safe
// and it also calls PeekMessage() for the main thread.
{
	HookType hooks_active_orig = GetActiveHooks();
	if (aHooksToBeActive == hooks_active_orig) // It's already in the right state.
		return;

	if (!hooks_active_orig) // Neither hook is active now but at least one will be or the above would have returned.
	{
		// Assert: sThreadHandle should be NULL at this point.  The only way this isn't true is if
		// a previous call to AddRemoveHooks() timed out while waiting for the hook thread to exit,
		// which seems far too rare to add extra code for.

		// CreateThread() vs. _beginthread():
		// It's not necessary to link to the multi-threading C runtime (which bloats the code by 3.5 KB
		// compressed) as long as the new thread doesn't call any C-library functions that aren't thread-safe
		// (in addition to the C functions that obviously use static data, calls to things like malloc(),
		// new, and other memory management functions probably aren't thread-safe unless the multi-threaded
		// library is used). The memory leak described in MSDN for ExitThread() applies only to the
		// multi-threaded libraries (multiple sources confirm this), so it isn't a concern either.
		// That's true even if the program is linked against the multi-threaded DLLs (MSVCRT.dll) rather
		// than the libraries (e.g. for a minimum-sized SC.bin file), as confirmed by the following quotes:
		// "This applies only to the static-link version of the runtime. For this and other reasons, I
		// *highly* recommend using the DLL runtime, which lets you use CreateThread() without prejudice.
		// Confirmation from MSDN: "Another work around is to link the *executable* to the CRT in a *DLL*
		// instead of the static CRT."
		//
		// The hooks are designed to make minimal use of C-library calls, currently calling only things
		// like memcpy() and strlen(), which are thread safe in the single-threaded library (according to
		// their source code).  However, the hooks may indirectly call other library functions via calls
		// to KeyEvent() and other functions, which has already been reviewed for thread-safety but needs
		// to be kept in mind as changes are made in the future.
		//
		// CreateThread's second parameter is the new thread's initial stack size. The stack will grow
		// automatically if more is needed, so it's kept small here to greatly reduce the amount of
		// memory used by the hook thread.  The XP Task Manager's "VM Size" column (which seems much
		// more accurate than "Mem Usage") indicates that a new thread consumes 28 KB + its stack size.
		if (!aChangeIsTemporary) // Caller has ensured that thread already exists when aChangeIsTemporary==true.
			if (sThreadHandle = CreateThread(NULL, 8*1024, HookThreadProc, NULL, 0, &g_HookThreadID))
				SetThreadPriority(sThreadHandle, THREAD_PRIORITY_TIME_CRITICAL); // See below for explanation.
			// The above priority level seems optimal because if some other process has high priority,
			// the keyboard and mouse hooks will still take precedence, which avoids the mouse cursor
			// and keystroke lag that would otherwise occur (confirmed through testing).  Due to their
			// return-ASAP nature, the hooks are an ideal candidate for almost-realtime priority because
			// they run only rarely and only for tiny bursts of time.
			// Note that the above must also be done in such a way that it works on NT4, which doesn't support
			// below-normal and above-normal process priorities, nor perhaps other aspects of priority.
			// So what is the actual priority given to the hooks by the OS?  Assuming that the script's
			// process is set to NORMAL_PRIORITY_CLASS (which is the default), the following applies:
			// First of all, a definition: "base priority" is the actual/net priority of the thread.
			// It determines how the OS will schedule a thread relative to all other threads on the system.
			// So in a sense, if you look only at base priority, the thread's process's priority has no
			// bearing on how the thread will get scheduled (except to the extent that it contributes
			// to the calculation of the base priority itself).  Here are some common base priorities
			// along with where the hook priority (15) fits in:
			// 7 = NORMAL_PRIORITY_CLASS process + THREAD_PRIORITY_NORMAL thread.
			// 9 = NORMAL_PRIORITY_CLASS process + THREAD_PRIORITY_HIGHEST thread.
			// 13 = HIGH_PRIORITY_CLASS process + THREAD_PRIORITY_NORMAL thread.
			// 15 = (ANY)_PRIORITY_CLASS process + THREAD_PRIORITY_TIME_CRITICAL thread. <-- Seems like the optimal compromise.
			// 15 = HIGH_PRIORITY_CLASS process + THREAD_PRIORITY_HIGHEST thread.
			// 24 = REALTIME_PRIORITY_CLASS process + THREAD_PRIORITY_NORMAL thread.
		else // Failed to create thread.  Seems to rare to justify the display of an error.
		{
			FreeHookMem(); // If everything's designed right, there should be no hooks now (even if there is, they can't be functional because their thread is nonexistent).
			return;
		}
	}
	//else there is at least one hook already active, which guarantees that the hook thread exists (assuming
	// everything is designed right).

	// Above has ensured that the hook thread now exists, so send it the status-change message.

	// Post the AHK_CHANGE_HOOK_STATE message to the new thread to put the right hooks into effect.
	// If both hooks are to be deactivated, AHK_CHANGE_HOOK_STATE also causes the hook thread to exit.
	// PostThreadMessage() has been observed to fail, such as when a script replaces a previous instance
	// of itself via #SingleInstance.  I think this happens because the new thread hasn't yet had a
	// chance to create its message queue via GetMessage().  So rather than using something like
	// WaitForSingleObject() -- which might not be reliable due to split-second timing of when the
	// queue actually gets created -- just keep retrying until time-out or PostThreadMessage() succeeds.
	for (int i = 0; i < 50 && !PostThreadMessage(g_HookThreadID, AHK_CHANGE_HOOK_STATE, aHooksToBeActive, !aChangeIsTemporary); ++i)
		Sleep(10); // Should never execute if thread already existed before this function was called.
		// Above: Sleep(10) seems better than Sleep(0), which would max the CPU while waiting.
		// MUST USE Sleep vs. MsgSleep, otherwise an infinite recursion of ExitApp is possible.
		// This can be reproduced by running a script consisting only of the line #InstallMouseHook
		// and then exiting via the tray menu.  I tried fixing it in TerminateApp with the following,
		// but it's just not enough.  So rather than spend a long time on it, it's fixed directly here:
			// Because of the below, our callers must NOT assume that an exit will actually take place.
			//static is_running = false;
			//if (is_running)
			//	return OK;
			//is_running = true; // Since we're exiting, there should be no need to set it to false further below.

	// If it times out I think it's realistically impossible that the new thread really exists because
	// if it did, it certainly would have had time to execute GetMessage() in all but extreme/theoretical
	// cases.  Therefore, no thread check/termination attempt is done.  Alternatively, a check for
	// GetExitCodeThread() could be done followed by closing the handle and setting it to NULL, but once
	// again the code size doesn't seem worth it for a situation that is probably impossible.
	//
	// Also, a timeout itself seems too rare (perhaps even impossible) to justify a warning dialog.
	// So do nothing, which retains the current values of g_KeybdHook and g_MouseHook.

	// For safety, serialize the termination of the hook thread so that this function can't be called
	// again by the main thread before the hook thread has had a chance to exit in response to the
	// previous call.  This improves reliability, especially by ensuring a clean exit (if our caller
	// is about to exit the app via exit(), which otherwise might not cleanly close all threads).
	// UPDATE: Also serialize all changes to the hook status so that our caller can rely on the new
	// hook state being in effect immediately.  For example, the Input command installs the keyboard
	// hook and it's more maintainable if we ensure the status is correct prior to returning.
	MSG msg;
	DWORD exit_code, start_time;
	bool problem_activating_hooks;
	for (problem_activating_hooks = false, start_time = GetTickCount();;) // For our caller, wait for hook thread to update the status of the hooks.
	{
		if (aHooksToBeActive) // Wait for the hook thread to activate the specified hooks.
		{
			// In this mode, the hook thread knows we want a report of success or failure via message.
			if (PeekMessage(&msg, NULL, AHK_CHANGE_HOOK_STATE, AHK_CHANGE_HOOK_STATE, PM_REMOVE))
			{
				if (msg.wParam) // The hook thread indicated failure to activate one or both of the hooks.
				{
					// This is done so that the MsgBox warning won't be shown until after these loops finish,
					// which seems safer to prevent any parts of the script from running as a result
					// the MsgBox pumping hotkey messages and such, which could result in a script
					// subroutine launching while we're in here:
					problem_activating_hooks = true;
					if (!GetActiveHooks() && !aChangeIsTemporary) // The failure is such that no hooks are now active, and thus (due to the mode) the hook thread will exit.
					{
						// Convert this loop into the mode that waits for the hook thread to exit.
						// This allows the thread handle to be closed and the memory to be freed.
						aHooksToBeActive = 0;
						continue;
					}
					// It failed but one hook is still active, or the change is temporary.  Either way,
					// we're done waiting.  Fall through to "break" below.
				}
				//else it successfully changed the state.
				// In either case, we're done waiting:
				break;
			}
			//else no AHK_CHANGE_HOOK_STATE message has arrived yet, so keep waiting until it does or timeout occurs.
		}
		else // The hook thread has been asked to deactivate both hooks.
		{
			if (aChangeIsTemporary) // The thread will not terminate in this mode, it will just remove its hooks.
			{
				if (!GetActiveHooks()) // The hooks have been deactivated.
					break; // Don't call FreeHookMem() because caller doesn't want that when aChangeIsTemporary==true.
			}
			else // Wait for the thread to terminate.
			{
				GetExitCodeThread(sThreadHandle, &exit_code);
				if (exit_code != STILL_ACTIVE) // The hook thread is now gone.
				{
					// Do the following only if it actually exited (i.e. not if this loop timed out):
					CloseHandle(sThreadHandle); // Release our reference to it to allow the OS to delete the thread object.
					sThreadHandle = NULL;
					FreeHookMem(); // There should be no hooks now (even if there is, they can't be functional because their thread is nonexistent).
					break;
				}
			}
		}
		if (GetTickCount() - start_time > 500) // DWORD subtraction yields correct result even when TickCount has wrapped.
			break;
		// v1.0.43: The following sleeps for 0 rather than some longer time because:
		// 1) In nearly all cases, this loop should do only one iteration because a Sleep(0) should guaranty
		//    that the hook thread will get a timeslice before our thread gets another.  In fact, it might not
		//    do any iterations if the system preempts the main thread immediately when a message is posted to
		//    a higher priority thread (especially one in its own process).
		// 2) SendKeys()'s SendInput mode relies on fast removal of hook to prevent a 10ms or longer delay before
		//    the keystrokes get sent.  Such a delay would be quite undesirable in cases where response time is
		//    critical, such as in games.
		// Testing shows that removing the Sleep() entirely does not help performance.  The following was measured
		// when the CPU was under heavy load from a cpu-maxing utility:
		//   Loop 10  ; Keybd hook must be installed for this test to be meaningful.
		//      SendInput {Shift}
		Sleep(0); // Not MsgSleep (see the "Sleep(10)" above for why).
	}
	// If the above loop timed out without the hook thread exiting (if it was asked to exit), sThreadHandle
	// is left as non-NULL to reflect this condition.

	// In case mutex create/open/close can be a high-overhead operation, do it only when the hook isn't
	// being quickly/temporarily removed then added back again.
	if (!aChangeIsTemporary)
	{
		if (g_KeybdHook && !(hooks_active_orig & HOOK_KEYBD)) // The keyboard hook has been newly added.
			sKeybdMutex = CreateMutex(NULL, FALSE, KEYBD_MUTEX_NAME); // Create-or-open this mutex and have it be unowned.
		else if (!g_KeybdHook && (hooks_active_orig & HOOK_KEYBD))  // The keyboard hook has been newly removed.
		{
			CloseHandle(sKeybdMutex);
			sKeybdMutex = NULL;
		}
		if (g_MouseHook && !(hooks_active_orig & HOOK_MOUSE)) // The mouse hook has been newly added.
			sMouseMutex = CreateMutex(NULL, FALSE, MOUSE_MUTEX_NAME); // Create-or-open this mutex and have it be unowned.
		else if (!g_MouseHook && (hooks_active_orig & HOOK_MOUSE))  // The mouse hook has been newly removed.
		{
			CloseHandle(sMouseMutex);
			sMouseMutex = NULL;
		}
	}

	// For maintainability, it seems best to display the MsgBox only at the very end.
	if (problem_activating_hooks)
	{
		// Prevent hotkeys and other subroutines from running (which could happen via MsgBox's message pump)
		// to avoid the possibility that the script will continue to call this function recursively, resulting
		// in an infinite stack of MsgBoxes. This approach is similar to that used in Hotkey::Perform()
		// for the #MaxHotkeysPerInterval warning dialog:
		g_AllowInterruption = FALSE; 
		// Below is a generic message to reduce code size.  Failure is rare, but has been known to happen when
		// certain types of games are running).
		MsgBox(_T("Warning: The keyboard and/or mouse hook could not be activated; ")
			_T("some parts of the script will not function."));
		g_AllowInterruption = TRUE;
	}
}



bool SystemHasAnotherKeybdHook()
{
	if (sKeybdMutex)
		CloseHandle(sKeybdMutex); // But don't set it to NULL because we need its value below as a flag.
	HANDLE mutex = CreateMutex(NULL, FALSE, KEYBD_MUTEX_NAME); // Create() vs. Open() has enough access to open the mutex if it exists.
	DWORD last_error = GetLastError();
	// Don't check g_KeybdHook because in the case of aChangeIsTemporary, it might be NULL even though
	// we want a handle to the mutex maintained here.
	if (sKeybdMutex) // It was open originally, so update the handle the the newly opened one.
		sKeybdMutex = mutex;
	else if (mutex) // Keep it closed because the system tracks how many handles there are, deleting the mutex when zero.
		CloseHandle(mutex);  // This facilitates other instances of the program getting the proper last_error value.
	return last_error == ERROR_ALREADY_EXISTS;
}



bool SystemHasAnotherMouseHook()
{
	if (sMouseMutex)
		CloseHandle(sMouseMutex); // But don't set it to NULL because we need its value below as a flag.
	HANDLE mutex = CreateMutex(NULL, FALSE, MOUSE_MUTEX_NAME); // Create() vs. Open() has enough access to open the mutex if it exists.
	DWORD last_error = GetLastError();
	// Don't check g_MouseHook because in the case of aChangeIsTemporary, it might be NULL even though
	// we want a handle to the mutex maintained here.
	if (sMouseMutex) // It was open originally, so update the handle the the newly opened one.
		sMouseMutex = mutex;
	else if (mutex) // Keep it closed because the system tracks how many handles there are, deleting the mutex when zero.
		CloseHandle(mutex);  // This facilitates other instances of the program getting the proper last_error value.
	return last_error == ERROR_ALREADY_EXISTS;
}



DWORD WINAPI HookThreadProc(LPVOID aUnused)
// The creator of this thread relies on the fact that this function always exits its thread
// when both hooks are deactivated.
{
	MSG msg;
	bool problem_activating_hooks;

	for (;;) // Infinite loop for pumping messages in this thread. This thread will exit via any use of "return" below.
	{
		if (GetMessage(&msg, NULL, 0, 0) == -1) // -1 is an error, 0 means WM_QUIT.
			continue; // Probably happens only when bad parameters are passed to GetMessage().

		switch (msg.message)
		{
		case WM_QUIT:
			// After this message, fall through to the next case below so that the hooks will be removed before
			// exiting this thread.
			msg.wParam = 0; // Indicate to AHK_CHANGE_HOOK_STATE that both hooks should be deactivated.
		// ********
		// NO BREAK IN ABOVE, FALL INTO NEXT CASE:
		// ********
		case AHK_CHANGE_HOOK_STATE: // No blank line between this in the above to indicate fall-through.
			// In this case, wParam contains the bitwise set of hooks that should be active.
			problem_activating_hooks = false;
			if (msg.wParam & HOOK_KEYBD) // Activate the keyboard hook (if it isn't already).
			{
				if (!g_KeybdHook)
				{
					// v1.0.39: Reset *before* hook is installed to avoid any chance that events can
					// flow into the hook prior to the reset:
					if (msg.lParam) // Sender of msg. is signaling that reset should be done.
						ResetHook(false, HOOK_KEYBD, true);
					if (   !(g_KeybdHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeybdProc, g_hInstance, 0))   )
						problem_activating_hooks = true;
				}
			}
			else // Caller specified that the keyboard hook is to be deactivated (if it isn't already).
				if (g_KeybdHook)
					if (UnhookWindowsHookEx(g_KeybdHook) || GetLastError() == ERROR_INVALID_HOOK_HANDLE) // Check last error in case the OS has already removed the hook.
						g_KeybdHook = NULL;

			if (msg.wParam & HOOK_MOUSE) // Activate the mouse hook (if it isn't already).
			{
				if (!g_MouseHook)
				{
					if (msg.lParam) // Sender of msg. is signaling that reset should be done.
						ResetHook(false, HOOK_MOUSE, true);
					if (   !(g_MouseHook = SetWindowsHookEx(WH_MOUSE_LL, LowLevelMouseProc, g_hInstance, 0))   )
						problem_activating_hooks = true;
				}
			}
			else // Caller specified that the mouse hook is to be deactivated (if it isn't already).
				if (g_MouseHook)
					if (UnhookWindowsHookEx(g_MouseHook) || GetLastError() == ERROR_INVALID_HOOK_HANDLE) // Check last error in case the OS has already removed the hook.
						g_MouseHook = NULL;

			// Upon failure, don't display MsgBox here because although MsgBox's own message pump would
			// service the hook that didn't fail (if it's active), it's best to avoid any blocking calls
			// here so that this event loop will continue to run.  For example, the script or OS might
			// ask this thread to terminate, which it couldn't do cleanly if it was in a blocking call.
			// Instead, send a reply back to the caller.
			// It's safe to post directly to thread because the creator of this thread should be
			// explicitly waiting for this message (so there's no chance that a MsgBox msg pump
			// will discard the message unless the caller has timed out, which seems impossible
			// in this case).
			if (msg.wParam) // The caller wants a reply only when it didn't ask us to terminate via deactivating both hooks.
				PostThreadMessage(g_MainThreadID, AHK_CHANGE_HOOK_STATE, problem_activating_hooks, 0);
			//else this is WM_QUIT or the caller wanted this thread to terminate.  Send no reply.

			// If caller passes true for msg.lParam, it wants a permanent change to hook state; so in that case, terminate this
			// thread whenever neither hook is no longer present.
			if (msg.lParam && !(g_KeybdHook || g_MouseHook)) // Both hooks are inactive (for whatever reason).
				return 0; // Thread is no longer needed. The "return" automatically calls ExitThread().
				// 1) Due to this thread's non-GUI nature, there doesn't seem to be any need to call
				// the somewhat mysterious PostQuitMessage() here.
				// 2) For thread safety and maintainability, it seems best to have the caller take
				// full responsibility for freeing the hook's memory.
			break;

		case AHK_HOOK_SYNC:
			sHookSyncd = true;
			break;

		} // switch (msg.message)
	} // for(;;)
}



void ResetHook(bool aAllModifiersUp, HookType aWhichHook, bool aResetKVKandKSC)
// Caller should ensure that aWhichHook indicates at least one of the hooks (not none).
{
	if (pPrefixKey)
	{
		// Reset pPrefixKey only if the corresponding hook is being reset.  This fixes
		// custom combo mouse hotkeys breaking when the prefix key does something which
		// causes the keyboard hook to be reset, or vice versa.
		bool is_mouse_key = pPrefixKey >= kvk && pPrefixKey <= kvk + VK_ARRAY_COUNT && IsMouseVK((vk_type)(pPrefixKey - kvk));
		if (aWhichHook & (is_mouse_key ? HOOK_MOUSE : HOOK_KEYBD))
			pPrefixKey = NULL;
	}

	if (aWhichHook & HOOK_MOUSE)
	{
		// Initialize some things, a very limited subset of what is initialized when the
		// keyboard hook is installed (see its comments).  This is might not everything
		// we should initialize, so further study is justified in the future:
#ifdef FUTURE_USE_MOUSE_BUTTONS_LOGICAL
		g_mouse_buttons_logical = 0;
#endif
		g_PhysicalKeyState[VK_LBUTTON] = 0;
		g_PhysicalKeyState[VK_RBUTTON] = 0;
		g_PhysicalKeyState[VK_MBUTTON] = 0;
		g_PhysicalKeyState[VK_XBUTTON1] = 0;
		g_PhysicalKeyState[VK_XBUTTON2] = 0;
		// These are not really valid, since they can't be in a physically down state, but it's
		// probably better to have a false value in them:
		g_PhysicalKeyState[VK_WHEEL_DOWN] = 0;
		g_PhysicalKeyState[VK_WHEEL_UP] = 0;
		// Lexikos: Support horizontal scrolling in Windows Vista and later.
		g_PhysicalKeyState[VK_WHEEL_LEFT] = 0;
		g_PhysicalKeyState[VK_WHEEL_RIGHT] = 0;

		if (aResetKVKandKSC)
		{
			ResetKeyTypeState(kvk[VK_LBUTTON]);
			ResetKeyTypeState(kvk[VK_RBUTTON]);
			ResetKeyTypeState(kvk[VK_MBUTTON]);
			ResetKeyTypeState(kvk[VK_XBUTTON1]);
			ResetKeyTypeState(kvk[VK_XBUTTON2]);
			ResetKeyTypeState(kvk[VK_WHEEL_DOWN]);
			ResetKeyTypeState(kvk[VK_WHEEL_UP]);
			// Lexikos: Support horizontal scrolling in Windows Vista and later.
			ResetKeyTypeState(kvk[VK_WHEEL_LEFT]);
			ResetKeyTypeState(kvk[VK_WHEEL_RIGHT]);
		}
	}

	if (aWhichHook & HOOK_KEYBD)
	{
		// Doesn't seem necessary to ever init g_KeyHistory or g_KeyHistoryNext here, since they were
		// zero-filled on startup.  But we do want to reset the below whenever the hook is being
		// installed after a (probably long) period during which it wasn't installed.  This is
		// because we don't know the current physical state of the keyboard and such:

		g_modifiersLR_physical = 0;  // Best to make this zero, otherwise keys might get stuck down after a Send.
		g_modifiersLR_numpad_mask = 0;
		g_modifiersLR_ctrlaltdel_mask = 0;
		g_modifiersLR_logical = g_modifiersLR_logical_non_ignored = (aAllModifiersUp ? 0 : GetModifierLRState(true));

		ZeroMemory(g_PhysicalKeyState, sizeof(g_PhysicalKeyState));

		sDisguiseNextMenu = false;
		sUndisguisedMenuInEffect = false;

		// On Windows Vista and later, this definitely only works if the classic alt-tab menu
		// has been restored via the registry.  A non-NULL result is probably only helpful for
		// enabling the Esc key workaround in the hook (even though that isn't as critical on
		// Windows 7 as it was on XP, since on 7 the user can dismiss it with physical Esc).
		// A NULL result is probably more common, such as if it's been a while since the hook
		// was removed (or Alt was released).  If the *classic* alt-tab menu isn't in use,
		// this at least serves to reset sAltTabMenuIsVisible to false:
		sAltTabMenuIsVisible = (FindWindow(_T("#32771"), NULL) != NULL);

		*g_HSBuf = '\0';
		g_HSBufLength = 0;
		g_HShwnd = 0; // It isn't necessary to determine the actual window/control at this point since the buffer is already empty.

		if (aResetKVKandKSC)
		{
			int i;
			for (i = 0; i < VK_ARRAY_COUNT; ++i)
				if (!IsMouseVK(i))  // Don't do mouse VKs since those must be handled by the mouse section.
					ResetKeyTypeState(kvk[i]);
			for (i = 0; i < SC_ARRAY_COUNT; ++i)
				ResetKeyTypeState(ksc[i]);
		}
	}
}



HookType GetActiveHooks()
{
	HookType hooks_currently_active = 0;
	if (g_KeybdHook)
		hooks_currently_active |= HOOK_KEYBD;
	if (g_MouseHook)
		hooks_currently_active |= HOOK_MOUSE;
	return hooks_currently_active;
}



void FreeHookMem()
// For maintainability, only the main thread should ever call this function.
// This is because new/delete/malloc/free themselves might not be thread-safe
// when the single-threaded CRT libraries are in effect (not using multi-threaded
// libraries due to a 3.5 KB increase in compressed code size).
{
	if (kvk)
	{
		delete [] kvk;
		kvk = NULL;
	}
	if (ksc)
	{
		delete [] ksc;
		ksc = NULL;
	}
	if (kvkm)
	{
		delete [] kvkm;
		kvkm = NULL;
	}
	if (kscm)
	{
		delete [] kscm;
		kscm = NULL;
	}
	if (hotkey_up)
	{
		free(hotkey_up);
		hotkey_up = NULL;
	}
}



void ResetKeyTypeState(key_type &key)
{
	key.is_down = false;
	key.it_put_alt_down = false;
	key.it_put_shift_down = false;
	key.down_performed_action = false;
	key.was_just_used = 0;
	key.hotkey_to_fire_upon_release = HOTKEY_ID_INVALID;
	// ABOVE line was added in v1.0.48.03 to fix various ways in which the hook didn't receive the key-down
	// hotkey that goes with this key-up, resulting in hotkey_to_fire_upon_release being left at its initial
	// value of zero (which is a valid hotkey ID).  Examples include:
	// The hotkey command being used to create a key-up hotkey while that key is being held down.
	// The script being reloaded or (re)started while the key is being held down.
}



void GetHookStatus(LPTSTR aBuf, int aBufSize)
// aBufSize is an int so that any negative values passed in from caller are not lost.
{
	TCHAR LRhText[128], LRpText[128];
	sntprintfcat(aBuf, aBufSize,
		_T("Modifiers (Hook's Logical) = %s\r\n")
		_T("Modifiers (Hook's Physical) = %s\r\n") // Font isn't fixed-width, so don't bother trying to line them up.
		_T("Prefix key is down: %s\r\n")
		, ModifiersLRToText(g_modifiersLR_logical, LRhText)
		, ModifiersLRToText(g_modifiersLR_physical, LRpText)
		, pPrefixKey ? _T("yes") : _T("no"));

	if (!g_KeybdHook)
		sntprintfcat(aBuf, aBufSize, _T("\r\n")
			_T("NOTE: Only the script's own keyboard events are shown\r\n")
			_T("(not the user's), because the keyboard hook isn't installed.\r\n"));

	// Add the below even if key history is already disabled so that the column headings can be seen.
	sntprintfcat(aBuf, aBufSize, 
		_T("\r\nNOTE: To disable the key history shown below, add the line \"#KeyHistory 0\" ")
		_T("anywhere in the script.  The same method can be used to change the size ")
		_T("of the history buffer.  For example: #KeyHistory 100  (Default is 40, Max is 500)")
		_T("\r\n\r\nThe oldest are listed first.  VK=Virtual Key, SC=Scan Code, Elapsed=Seconds since the previous event")
		_T(".  Types: h=Hook Hotkey, s=Suppressed (blocked), i=Ignored because it was generated by an AHK script")
		_T(", a=Artificial, #=Disabled via #IfWinActive/Exist, U=Unicode character (SendInput).\r\n\r\n")
		_T("VK  SC\tType\tUp/Dn\tElapsed\tKey\t\tWindow\r\n")
		_T("-------------------------------------------------------------------------------------------------------------"));

	if (g_KeyHistory)
	{
		// Start at the oldest key, which is KeyHistoryNext:
		TCHAR KeyName[128];
		int item, i;
		LPTSTR title_curr = _T(""), title_prev = _T("");
		for (item = g_KeyHistoryNext, i = 0; i < g_MaxHistoryKeys; ++i, ++item)
		{
			if (item >= g_MaxHistoryKeys)
				item = 0;
			title_prev = title_curr;
			title_curr = g_KeyHistory[item].target_window;
			if (g_KeyHistory[item].vk == VK_PACKET) // Unicode character probably sent via SendInput.
			{
				sntprintfcat(aBuf, aBufSize, _T("\r\nE7 %04X\t%c\t%c\t%0.2f\t%c              \t%s")
					, g_KeyHistory[item].sc
					, g_KeyHistory[item].event_type
					, g_KeyHistory[item].key_up ? _T('u') : _T('d')
					, g_KeyHistory[item].elapsed_time
#ifdef UNICODE
					, (wchar_t)g_KeyHistory[item].sc
#else
					, g_KeyHistory[item].sc & ~0x7f ? ' ' : (char)g_KeyHistory[item].sc
#endif
					, _tcscmp(title_curr, title_prev) ? title_curr : _T("") // Display title only when it changes.
					);
			}
			else if (g_KeyHistory[item].vk || g_KeyHistory[item].sc)
				sntprintfcat(aBuf, aBufSize, _T("\r\n%02X  %03X\t%c\t%c\t%0.2f\t%-15s\t%s")
					, g_KeyHistory[item].vk, g_KeyHistory[item].sc
					// It can't be both ignored and suppressed, so display only one:
					, g_KeyHistory[item].event_type
					, g_KeyHistory[item].key_up ? _T('u') : _T('d')
					, g_KeyHistory[item].elapsed_time
					, GetKeyName(g_KeyHistory[item].vk, g_KeyHistory[item].sc, KeyName, _countof(KeyName))
					, _tcscmp(title_curr, title_prev) ? title_curr : _T("") // Display title only when it changes.
					);
		}
	}
}



void WaitHookIdle()
// Wait until the hook has reached a known idle state (i.e. finished any processing
// that it was in the middle of, though it could start something new immediately after).
{
	if (!sThreadHandle)
		return;
	sHookSyncd = false;
	PostThreadMessage(g_HookThreadID, AHK_HOOK_SYNC, 0, 0);
	while (!sHookSyncd)
		SLEEP_WITHOUT_INTERRUPTION(0);
}
