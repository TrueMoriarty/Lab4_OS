#include <iostream>
#include <windows.h>
#include <fstream>
#include <string>

HANDLE CreateNewProcess(const std::string& exePath, const std::string& nameLog) {

	SECURITY_ATTRIBUTES securityAttributes = { 
		sizeof(securityAttributes), 
		nullptr, 
		true 
	};

	HANDLE logFileHandle = CreateFile(nameLog.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, &securityAttributes,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	STARTUPINFO infoStartup;
	PROCESS_INFORMATION processInformation;

	ZeroMemory(&infoStartup, sizeof(infoStartup));
	infoStartup.cb = sizeof(infoStartup);
	infoStartup.dwFlags |= STARTF_USESTDHANDLES;
	infoStartup.hStdOutput = logFileHandle;
	infoStartup.hStdError = NULL;
	infoStartup.hStdInput = NULL;

	ZeroMemory(&processInformation, sizeof(processInformation));

	if (CreateProcess(exePath.c_str(), NULL, NULL, NULL, true, NULL, NULL, NULL, &infoStartup, &processInformation))
		return processInformation.hProcess;

	return nullptr;
}

int main()
{
	std::cout << "START" << std::endl;

	const int pageSize = 4096;
	const int pageCount = 16;
	const int processCount = 16;
	const int hProcessCount = processCount / 2;

	HANDLE semaphoresWrite[pageCount];
	HANDLE semaphoresRead[pageCount];
	HANDLE IOMutex = CreateMutex(NULL, false, "IOMutex");
	HANDLE fileHandle = CreateFile("D:\\lab4\\text.txt", GENERIC_ALL, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	HANDLE fileMap = CreateFileMapping(fileHandle, NULL, PAGE_READWRITE, 0, pageSize * pageCount, "MAPPING");
	HANDLE processHandles[processCount];

	for (int i = 0; i < pageCount; i++)
	{
		std::string semaphoreName = "writeSemaphore_" + std::to_string(i);
		semaphoresWrite[i] = CreateSemaphore(NULL, 1, 1, semaphoreName.c_str());
		semaphoreName = "readSemaphore_" + std::to_string(i);
		semaphoresRead[i] = CreateSemaphore(NULL, 0, 1, semaphoreName.c_str());
	}

	for (int i = 0; i < hProcessCount; i++)
	{
		std::string nameLog = "D:\\lab4\\writeLogs\\writeLog_" + std::to_string(i) + ".txt";
		processHandles[i] = CreateNewProcess("C:\\Users\\arsla\\source\\repos\\Lab4_Writer\\Debug\\Lab4_Writer.exe",
			nameLog);
	}

	for (int i = 0; i < hProcessCount; i++)
	{
		std::string nameLog = "D:\\lab4\\readLogs\\readLog_" + std::to_string(i) + ".txt";
		processHandles[i + hProcessCount] = CreateNewProcess("C:\\Users\\arsla\\source\\repos\\Lab4_Reader\\Debug\\Lab4_Reader.exe",
			nameLog);
	}

	std::cout << "WAIT" << std::endl;

	WaitForMultipleObjects(processCount, processHandles, true, INFINITE);
	std::cout << "OVER" << std::endl;

	CloseHandle(IOMutex);
	CloseHandle(fileMap);
	CloseHandle(fileHandle);

	for (int i = 0; i < pageCount; i++)
	{
		CloseHandle(semaphoresWrite[i]);
		CloseHandle(semaphoresRead[i]);
	}
	return 0;
}