#include <iostream>
#include <csignal>
#include <string>
#include <unistd.h>
#include <vector>
#include <libproc.h>
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

char* toLowerCase(char* str) {
	for (char* p = str; *p; ++p) {
		*p = tolower(*p);
	}
	return str;
}

void killProcessById(const char* PID_str)
{
	pid_t pid = 0;
	try {
		pid = std::stoi(PID_str);
	}
	catch (...) {
		std::cerr << "> Invalid PID: " << PID_str << std::endl;
		return;
	}

	if (kill(pid, SIGKILL) == 0) {
		int retries = 50;
		bool terminated = false;
		while (retries > 0) {
			if (kill(pid, 0) == -1 && errno == ESRCH) {
				terminated = true;
				break;
			}
			usleep(100000);
			--retries;
		}
		if(terminated) {
			std::cout << "> Process (PID = " << pid << ") terminated!" << std::endl;
		}
		else {
			std::cerr << "> Failed to wait for process (PID = " << pid << ") to terminate." << std::endl;
		}
	}
	else std::cerr << "> Failed to terminate process (PID = " << pid << ")! Error: " << errno << std::endl;
}


void killProcessesByNames(const std::unordered_set<std::string>& targets)
{
	int procCount = proc_listallpids(nullptr, 0);
	if (procCount <= 0) {
		std::cerr << "Failed to get processes list!" << std::endl;
	}
	std::vector<pid_t> pids(procCount / sizeof(pid_t) + 10);
	procCount = proc_listallpids(pids.data(), pids.size() * sizeof(pid_t));
	for (int i = 0; i < procCount / sizeof(pid_t); ++i) {
		char name[PROC_PIDPATHINFO_MAXSIZE];
		if (proc_name(pids[i], name, sizeof(name)) > 0) {
			std::string procName = toLowerCase(name);
			if (targets.count(procName) > 0) {
				pid_t pid = pids[i];
				if (kill(pid, SIGKILL) == 0) {
					int retries = 50;
					bool terminated = false;
					while (retries > 0) {
						if (kill(pid, 0) == -1 && errno == ESRCH) {
							terminated = true;
							break;
						}
						usleep(100000);
						--retries;
					}
					if(terminated) {
						std::cout << "> Process (Name = " << name << ") terminated!" << std::endl;
					}
					else {
						std::cerr << "> Failed to wait for process (Name = " << name << ") to terminate." << std::endl;
					}
				}
				else std::cerr << "> Failed to terminate process (Name = " << name << ")! Error: " << errno << std::endl;
			}
		}
	}
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
	char* envVar = getenv("PROC_TO_KILL");
	if (envVar) {
		std::string envStr(envVar);
		if (!envStr.empty()) {
			std::vector<std::string> procNames = split(envStr, '|');
			for (const auto& name : procNames) {
				targets.insert(name);
			}
		}
	}

	if (!targets.empty()) {
		killProcessesByNames(targets);
	}

	return 0;
}
