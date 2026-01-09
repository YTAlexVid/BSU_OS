#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <libproc.h>
#include <csignal>
#include <unistd.h>


void trim(std::string &s) {
    size_t end = s.find_last_not_of(" \n\r\t\f\v");
    s.erase(end + 1);
    size_t start = s.find_first_not_of(" \n\r\t\f\v");
    s.erase(0, start);
}
bool isSpaces(std::string s) {
    trim(s);
    return s.empty();
}

std::vector<std::string> split(const std::string &s, char seperator) {
    std::vector<std::string> output;
    std::string::size_type prev_pos = 0, pos = 0;
    while ((pos = s.find(seperator, pos)) != std::string::npos) {
        std::string substring = s.substr(prev_pos, pos - prev_pos);
        if (isSpaces(substring)) {
            output.push_back(substring);
        }
        prev_pos = ++pos;
    }
    std::string substring = s.substr(prev_pos, pos - prev_pos);
    if (isSpaces(substring)) {
        output.push_back(substring);
    }
    return output;
}

std::vector<int> parse_PIDs(const std::string &input) {
    std::vector<std::string> pid_strings = split(input, '|');
    std::vector<int> pids;
    for (const auto &pid_str: pid_strings) {
        try {
            int pid = std::stoi(pid_str);
            pids.push_back(pid);
        } catch (const std::invalid_argument &) {
            std::cerr << "Invalid PID: " << pid_str << std::endl;
        }
    }
    return pids;
}

void callKiller(const std::vector<std::string> &argsList) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to fork process!" << std::endl;
        return;
    }
    if (pid == 0) {
        std::vector<char *> args;
        args.reserve(argsList.size() + 2);
        std::string filename = "./Killer";
        args.push_back(const_cast<char *>(filename.c_str()));
        for (const auto &arg: argsList) {
            args.push_back(const_cast<char *>(arg.c_str()));
        }
        args.push_back(nullptr);
        execvp("./Killer", args.data());
        std::cerr << "Failed to start Killer. Error: " << errno << std::endl;
        exit(1);
    }
    waitpid(pid, nullptr, 0);

}

void processesStatusesByPIDs(const std::vector<int> &pids) {
    for (const auto &pid: pids) {
        if (kill(pid, 0) == 0 || errno == EPERM)
            std::cout << "Process with PID " << pid << " is running." << std::endl;
        else std::cout << "Process with PID " << pid << " is NOT running." << std::endl;
    }
}

char *toLowerCase(char *str) {
    for (char *p = str; *p; ++p) {
        *p = tolower(*p);
    }
    return str;
}

void processesStatusesByNames(const std::vector<std::string> &names) {
    std::unordered_set<std::string> processList;

    int procCount = proc_listallpids(nullptr, 0);
    if (procCount <= 0) {
        std::cerr << "Failed to get processes list!" << std::endl;
    }
    std::vector<pid_t> pids(procCount / sizeof(pid_t) + 10);
    procCount = proc_listallpids(pids.data(), pids.size() * sizeof(pid_t));

    for (int i = 0; i < procCount; ++i) {
        if (pids[i] == 0) continue;
        char name[PROC_PIDPATHINFO_MAXSIZE];
        if (proc_name(pids[i], name, sizeof(name)) > 0) {
            processList.insert(std::string(toLowerCase(name)));
        }
    }

    for (const auto &name: names) {
        if (processList.find(name) != processList.end()) {
            std::cout << "Process \"" << name << "\" is running." << std::endl;
        } else {
            std::cout << "Process \"" << name << "\" is NOT running." << std::endl;
        }
    }
}

int main() {
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

    std::vector<std::string> args;
    std::string envVar;
    for (const auto &pid: PIDs) {
        args.emplace_back("--id");
        args.push_back(std::to_string(pid));
    }
    for (const auto &name: Names) {
        args.emplace_back("--name");
        args.push_back(name);
    }
    for (const auto &env: Envs) {
        envVar += env + "|";
    }

    std::cout << "\n\nKiller output:" << std::endl;

    if (!args.empty() || !Envs.empty()) {
        setenv("PROC_TO_KILL", envVar.c_str(), 1);
        std::cout << std::flush;
        std::cerr << std::flush;
        callKiller(args);
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
