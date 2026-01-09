#include <iostream>
#include <string>
#include <vector>
#include "Windows.h"
#include <TlHelp32.h>
#include <unordered_set>

void trim(std::string& s) {
	size_t end = s.find_last_not_of(" \n\r\t\f\v");
	s.erase(end + 1);
	size_t start = s.find_first_not_of(" \n\r\t\f\v");
	s.erase(0, start);
}

std::vector<std::string> split(const std::string& s, char seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring = s.substr(prev_pos, pos - prev_pos);
		trim(substring);
		if (!substring.empty()) {
			output.push_back(substring);
		}
		prev_pos = ++pos;
	}
	std::string substring = s.substr(prev_pos, pos - prev_pos);
	trim(substring);
	if (!substring.empty()) {
		output.push_back(substring);
	}
	return output;
}

std::vector<int> parse_PIDs(const std::string& input)
{
	std::vector<std::string> pid_strings = split(input, '|');
	std::vector<int> pids;
	for (const auto& pid_str : pid_strings) {
		try {
			int pid = std::stoi(pid_str);
			pids.push_back(pid);
		}
		catch (const std::invalid_argument&) {
			std::cerr << "> Invalid PID: " << pid_str << std::endl;
		}
	}
	return pids;
}

void killProcessById(const char* PID_str)
{
	DWORD PID = 0; 
	sscanf_s(PID_str, "%lu", &PID);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if(hProcess == NULL) {
		std::cerr << "> Failed to open process (PID = " << PID << ") for termination! Error: " << GetLastError() << std::endl;
		return;
	}
	
	if (TerminateProcess(hProcess, -1)) {
		DWORD waitResult = WaitForSingleObject(hProcess, 5000); 
		if(waitResult == WAIT_OBJECT_0) {
			std::cout << "> Process (PID = " << PID << ") terminated!" << std::endl;
		} 
		else if (waitResult == WAIT_TIMEOUT) {
			std::cerr << "> Timeout while waiting for process (PID = " << PID << ") to terminate!" << std::endl;
		}
		else {
			std::cerr << "> Failed to wait for process (PID = " << PID << "). Error: " << GetLastError() << "\n";
		}
	}
	else std::cerr << "> Failed to terminate process (PID = " << PID << ")! Error: " << GetLastError() << std::endl;

	CloseHandle(hProcess);
}

char* toLowerCase(char* str) {
	for (char* p = str; *p; ++p) {
		*p = tolower(*p);
	}
	return str;
}

void killProcessesByNames(const std::unordered_set<std::string>& targets)
{
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			char exeFile[MAX_PATH];
			wcstombs_s(nullptr, exeFile, MAX_PATH, entry.szExeFile, _TRUNCATE);

			if (targets.count(std::string(toLowerCase(exeFile))) > 0) {
				HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, entry.th32ProcessID);
				if(hProcess == NULL) {
					std::cerr << "> Failed to open process (Exe Name = " << exeFile << ") for termination! Error: " << GetLastError() << std::endl;
					continue;
				}
				if (TerminateProcess(hProcess, -1)) {
					DWORD waitResult = WaitForSingleObject(hProcess, 5000);
					if (waitResult == WAIT_OBJECT_0) {
						std::cout << "> Process (Exe Name = " << exeFile << ") terminated!" << std::endl;
					}
					else if (waitResult == WAIT_TIMEOUT) {
						std::cerr << "> Timeout while waiting for process (Exe Name = " << exeFile << ") to terminate!" << std::endl;
					}
					else {
						std::cerr << "> Failed to wait for process (Exe Name = " << exeFile << "). Error: " << GetLastError() << "\n";
					}
				}
				else std::cerr << "> Failed to terminate process (Exe Name = " << exeFile << ")! Error: " << GetLastError() << std::endl;
				CloseHandle(hProcess);
				
			}
		}
	}
	CloseHandle(snapshot);
}

int main(int argc, char* argv[])
{
	std::unordered_set<std::string> targets;
	for (int i = 1; i < argc; ++i) {
		if (strcmp(argv[i], "--id") == 0) {
			if (i + 1 >= argc) return -1;
			killProcessById(argv[++i]);
		}
		else if (strcmp(argv[i], "--name") == 0) {
			if (i + 1 >= argc) return -1;
			targets.insert(argv[++i]);
		}
		else {
			std::cerr << "> Invalid argument " << argv[i] << std::endl;
		}
	}
	auto size = GetEnvironmentVariableA("PROC_TO_KILL", nullptr, 0);
	if(size == 0) {
		return 0;
	}
	std::string envVar(size - 1, 0);
	GetEnvironmentVariableA("PROC_TO_KILL", &envVar[0], size);
	std::vector<std::string> procNames = split(envVar, '|');
	for(const auto& name : procNames) {
		targets.insert(name);
	}
	killProcessesByNames(targets);
	return 0;
}
