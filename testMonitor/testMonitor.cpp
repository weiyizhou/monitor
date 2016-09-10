/* Author: Weiyi Zhou 5/31/2016
Checks if a process is running every five minutes. 
If the process is running, nothing occurs. 
If the process is not running, the program uses ShellExecute() to run the program
*/
#include "testMonitor.h"
using namespace std;

LPCTSTR getIniName() { //returns path to ini file, which must be placed in the same location as the executable
    DWORD len = 4;
    for (;;) {
        LPTSTR fileName = new TCHAR[len];
        if (len == ::GetModuleFileName(NULL, fileName, len)) {
            delete fileName;
            len *= 2;
        }
        else {
            ::lstrcpy(fileName + lstrlen(fileName) - 3, L"ini");
            return fileName;
        }
    }
}

int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow) {
	LPCTSTR iName = getIniName();
	
	WCHAR name[256];
	GetPrivateProfileString( L"data", L"name", NULL, name, 256, iName);
	WCHAR path[256];
	GetPrivateProfileString( L"data", L"path", NULL, path, 256, iName);
	int timeLen = GetPrivateProfileInt( L"data", L"time", NULL, iName);
	PROCESSENTRY32W entry;
    entry.dwSize = sizeof(PROCESSENTRY32W);
	int isRunning = 0;
	int counter = 0; //counter for tracking duplicate processes
    while (1) {
		counter = 0; //reset counter
		isRunning = 0; //reset mark for process running
		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL); // check if the process is already running
		if (Process32First(snapshot, &entry) == TRUE) {
			while (Process32Next(snapshot, &entry) == TRUE) { //iterate through running processes
				if (_wcsicmp(entry.szExeFile, name) == 0) { //actual comparison
					isRunning = 1; //mark that the process is running
				}
				if (_wcsicmp(entry.szExeFile, L"authCFG.exe") == 0) { //actual comparison
					counter += 1; //increment count of copies of this program running
					if (counter > 1) { //if more than one copy of this program is running, terminate this copy
						HANDLE hDup = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
						DWORD eCode;
						GetExitCodeProcess(hDup, &eCode);
						TerminateProcess(hDup, eCode);
						CloseHandle(hDup);

					}
				}
			}
		}
		if (isRunning == 0) { //if the process is not running...
			ShellExecute(NULL, L"open", path, NULL, NULL, SW_SHOWDEFAULT); //...run it
		}
		CloseHandle(snapshot);
		Sleep(timeLen); //wait timeLen sec before checking again
		
	}
	return 1;
}

