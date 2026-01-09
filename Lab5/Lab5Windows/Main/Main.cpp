#include <iostream>
#include <Windows.h>

HANDLE CreateProcessWithPipes(const char* processName, HANDLE readPipe, HANDLE writePipe)
{
	STARTUPINFOA si;
	GetStartupInfoA(&si);
	si.cb = sizeof(STARTUPINFO);
	si.hStdInput = readPipe;
	si.hStdOutput = writePipe;
    si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	si.dwFlags |= STARTF_USESTDHANDLES;
	PROCESS_INFORMATION pi;
	BOOL success = CreateProcessA(
		NULL,
		(LPSTR)processName,
		NULL,
		NULL,
		TRUE,
		0,
		NULL,
		NULL,
		&si,
		&pi);

	if (!success) {
		printf("Error creating %s: %d\n", processName, GetLastError());
		return NULL;
	}

	CloseHandle(pi.hThread);
	return pi.hProcess;
}

int main()
{
    HANDLE Pipe1_Read, Pipe1_Write;
    HANDLE Pipe2_Read, Pipe2_Write;
    HANDLE Pipe3_Read, Pipe3_Write;

    SECURITY_ATTRIBUTES sa;
    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    const char* streamIn = "input.txt";
    HANDLE fileIn = CreateFileA(streamIn, GENERIC_READ, 0,
        &sa, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (fileIn == INVALID_HANDLE_VALUE)
	{
		char tmp[256];
		GetCurrentDirectoryA(256, (LPSTR)tmp);
		printf("Could not open file <%s> located in <%s> (error %d)\n", streamIn, tmp, GetLastError());
		getchar();
		return 1;
	}

	CreatePipe(&Pipe1_Read, &Pipe1_Write, &sa, 0);
	HANDLE hProcM = CreateProcessWithPipes("ProcessM.exe", fileIn, Pipe1_Write);
	CloseHandle(fileIn);
	CloseHandle(Pipe1_Write);
	CloseHandle(hProcM);

	CreatePipe(&Pipe2_Read, &Pipe2_Write, &sa, 0);
	HANDLE hProcA = CreateProcessWithPipes("ProcessA.exe", Pipe1_Read, Pipe2_Write);
	CloseHandle(Pipe1_Read);
	CloseHandle(Pipe2_Write);
	CloseHandle(hProcA);

	CreatePipe(&Pipe3_Read, &Pipe3_Write, &sa, 0);
	HANDLE hProcP = CreateProcessWithPipes("ProcessP.exe", Pipe2_Read, Pipe3_Write);
	CloseHandle(Pipe2_Read);
	CloseHandle(Pipe3_Write);
	CloseHandle(hProcP);

	HANDLE hProcS = CreateProcessWithPipes("ProcessS.exe", Pipe3_Read, GetStdHandle(STD_OUTPUT_HANDLE));
	CloseHandle(Pipe3_Read);

	if (hProcS != NULL) {
		WaitForSingleObject(hProcS, INFINITE);
		CloseHandle(hProcS);
	}

	std::cout << "Press any key to exit..." << std::endl;
	getchar();
	
	return 0;
}