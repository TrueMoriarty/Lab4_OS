#include <iostream>
#include <fstream>
#include <windows.h>
#include <string>
#include <ctime>

void WriteLog(std::string data, HANDLE outHandle) {
	WriteFile(outHandle, data.c_str(), data.length(), NULL, NULL);
}

int main()
{

	const int pageSize = 4096;
	const int pageCount = 16;

	srand(time(NULL));
	HANDLE semaphoresWrite[pageCount];
	HANDLE semaphoresRead[pageCount];
	HANDLE IOMutex = OpenMutex(MUTEX_MODIFY_STATE | SYNCHRONIZE, FALSE, "IOMutex");
	HANDLE mapFile = OpenFileMapping(GENERIC_READ, FALSE, "MAPPING");
	LPVOID fileView = MapViewOfFile(mapFile, FILE_MAP_READ, 0, 0, pageSize * pageCount);
	HANDLE handleStdOut = GetStdHandle(STD_OUTPUT_HANDLE);

	DWORD page = 0;

	for (int i = 0; i < pageCount; i++) {
		std::string semaphoreName = "writeSemaphore_" + std::to_string(i);
		semaphoresWrite[i] = OpenSemaphore(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, semaphoreName.c_str());

		semaphoreName = "readSemaphore_" + std::to_string(i);
		semaphoresRead[i] = OpenSemaphore(SEMAPHORE_MODIFY_STATE | SYNCHRONIZE, FALSE, semaphoreName.c_str());
	}

	VirtualLock(fileView, pageSize * pageCount);

	for (int i = 0; i < 3; i++) {
		page = WaitForMultipleObjects(pageCount, semaphoresRead, FALSE, INFINITE);
		WriteLog("TAKE | Semaphore | " + std::to_string(GetTickCount()) + "\n", handleStdOut);

		WaitForSingleObject(IOMutex, INFINITE);
		WriteLog("TAKE | Mutex | " + std::to_string(GetTickCount()) + "\n", handleStdOut);

		Sleep(500 + (rand() % 1001));
		WriteLog("READ | Page: " + std::to_string(page) + " | " + std::to_string(GetTickCount()) + "\n", handleStdOut);

		ReleaseMutex(IOMutex);
		WriteLog("FREE | Mutex | " + std::to_string(GetTickCount()) + "\n", handleStdOut);

		ReleaseSemaphore(semaphoresWrite[page], 1, NULL);
		WriteLog("FREE | Semaphore | " + std::to_string(GetTickCount()) + "\n\n", handleStdOut);
	}

	CloseHandle(IOMutex);
	CloseHandle(mapFile);
	CloseHandle(fileView);
	CloseHandle(handleStdOut);

	return 0;
}