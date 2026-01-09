#include <iostream>
#include <string>
#include <sstream>

const int N = 19; //Может и 20, не знаю, вроде красника отчислили

int main() {
    std::string line;
    std::getline(std::cin, line);
    std::stringstream ss(line);
    double x;
    while (ss >> x) {
        std::cout << x + N << " ";
    }
    std::cout << std::endl;
    return 0;
}