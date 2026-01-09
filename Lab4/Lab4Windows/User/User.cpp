#include <iostream>
#include <string>
#include <vector>
#include <windows.h>
#include <TlHelp32.h>
#include <unordered_set>
#include <algorithm>




void trim(std::string& s) {
	size_t end = s.find_last_not_of(" \n\r\t\f\v");
	s.erase(end + 1);
	size_t start = s.find_first_not_of(" \n\r\t\f\v");
	s.erase(0, start);
}
bool isSpaces(std::string s) {
	trim(s);
	return s.empty();
}
std::vector<std::string> split(const std::string& s, char seperator)
{
	std::vector<std::string> output;
	std::string::size_type prev_pos = 0, pos = 0;
	while ((pos = s.find(seperator, pos)) != std::string::npos)
	{
		std::string substring = s.substr(prev_pos, pos - prev_pos);
		if (!isSpaces(substring)) {
			output.push_back(substring);
		}
		prev_pos = ++pos;
	}
	std::string substring = s.substr(prev_pos, pos - prev_pos);
	if (!isSpaces(substring)) {
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
			std::cerr << "Invalid PID: " << pid_str << std::endl;
		}
	}
	return pids;
}

void callKiller(const std::string& args) {
	STARTUPINFOA si = { sizeof(STARTUPINFOA) };
	PROCESS_INFORMATION pi;

    LPCSTR appName = "Killer.exe";
	BOOL opStatus = CreateProcessA(
		NULL,
		const_cast<char*>(args.c_str()),
		NULL,
		NULL,
		FALSE,
		0,
		NULL,
		NULL,
		&si,
		&pi
	);
	if (!opStatus) {
		std::cerr << "Failed to start Killer.exe with args: " << args << ". Error: " << GetLastError() << std::endl;
		return;
	}
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

void processesStatusesByPIDs(const std::vector<int>& pids) {
	for (const auto& pid : pids) {
		HANDLE hProcess = OpenProcess(PROCESS_QUERY_LIMITED_INFORMATION, FALSE, pid);
		bool isRunning = false;
		if (hProcess != NULL) {
			DWORD exitCode;
			BOOL result = GetExitCodeProcess(hProcess, &exitCode);
			isRunning = (result && exitCode == STILL_ACTIVE);
			CloseHandle(hProcess);
		}
		if (isRunning) std::cout << "Process with PID " << pid << " is running." << std::endl;
		else std::cout << "Process with PID " << pid << " is NOT running." << std::endl;
	}
}

char* toLowerCase(char* str) {
	for (char* p = str; *p; ++p) {
		*p = tolower(*p);
	}
	return str;
}

void processesStatusesByNames(const std::vector<std::string>& names) {

	std::unordered_set<std::string> processList;

	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (Process32First(snapshot, &entry) == TRUE) {
		while (Process32Next(snapshot, &entry) == TRUE) {
			char exeFile[MAX_PATH];
			wcstombs_s(nullptr, exeFile, MAX_PATH, entry.szExeFile, _TRUNCATE);
			processList.insert(std::string(toLowerCase(exeFile)));
		}
	}
	for (const auto& name : names) {
		if (processList.find(name) != processList.end()) {
			std::cout << "Process \"" << name << "\" is running." << std::endl;
		}
		else {
			std::cout << "Process \"" << name << "\" is NOT running." << std::endl;
		}
	}
	CloseHandle(snapshot);
}

int main()
{
	std::string PID_input;
	std::cout << "Enter (separated by |) PIDs to be terminated via --id:" << std::endl;
	std::getline(std::cin, PID_input);
	std::vector<int> PIDs = parse_PIDs(PID_input);

	std::string Name_input;
	std::cout << "Enter (separated by |) Process Names to be terminated via --name:" << std::endl;
	std::getline(std::cin, Name_input);
	std::transform(Name_input.begin(), Name_input.end(), Name_input.begin(), ::tolower);
	std::vector<std::string> Names = split(Name_input, '|');

	std::string Env_input;
	std::cout << "Enter (separated by |) Process Names to be terminated via env var:" << std::endl;
	std::getline(std::cin, Env_input);
	std::transform(Env_input.begin(), Env_input.end(), Env_input.begin(), ::tolower);
	std::vector<std::string> Envs = split(Env_input, '|');

	std::cout << "\n\nStatuses for PIDs:" << std::endl;
	processesStatusesByPIDs(PIDs);
	std::cout << "\nStatuses for Names:" << std::endl;
	processesStatusesByNames(Names);
	std::cout << "\nStatuses for Environment Variable:" << std::endl;
	processesStatusesByNames(Envs);

	std::string args;
	std::string envVar;
	for (const auto& pid : PIDs) {
		args += " --id " + std::to_string(pid);
	}
	for (const auto& name : Names) {
		args += " --name \"" + name + "\"";
	}
	for (const auto& env : Envs) {
		envVar += env + "|";
	}

	std::cout << "\n\nKiller output:" << std::endl;

	if (!args.empty() || !Envs.empty()) {
		SetEnvironmentVariableA("PROC_TO_KILL", envVar.c_str());
		std::cout << std::flush;
		std::cerr << std::flush;
		callKiller("Killer.exe" + args);
	}

	std::cout << "\n\nAfter termination attempt:" << std::endl;

	std::cout << "\n\nStatuses for PIDs:" << std::endl;
	processesStatusesByPIDs(PIDs);
	std::cout << "\nStatuses for Names:" << std::endl;
	processesStatusesByNames(Names);
	std::cout << "\nStatuses for Environment Variable:" << std::endl;
	processesStatusesByNames(Envs);

	std::cout << "\nPress Enter to exit..." << std::endl;
	std::cin.get();
	return 0;
}