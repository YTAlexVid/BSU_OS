#include <iostream>
#include <string>
#include <thread>

#include "MatrixMult.h"

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;


//За кол-во работников принято N-1, где N - кол-во потоков процессора. -1 нужен для main thread.
//Размер канала = 2*(N-1)

int main() {
    int max_threads = std::thread::hardware_concurrency();
    std::cout << "Max threads available: " << max_threads << '\n';
    srand(time(nullptr));
    int N;
    do {
        std::cout << "Enter N >= 5 (matrix size): ";
        std::cin >> N;
    } while (N < 5);

    Matrix a = generateMatrix(N);
    Matrix b = generateMatrix(N);

    std::cout << "Print matrices? ('yes' to print): ";
    std::string print;
    std::cin >> print;
    if (print == "yes") {
        std::cout << "Matrix A:" << std::endl;
        printMatrix(a);
        std::cout << "Matrix B:" << std::endl;
        printMatrix(b);
    } {
        auto t1 = high_resolution_clock::now();
        Matrix result = mulMat(a, b);
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
        std::cout << "Result with normal mult: " << std::endl;
        std::cout << '\t' << ms_double.count() << "ms\n";
    }

    for (int k = N; k > 0; --k) {
        auto t1 = high_resolution_clock::now();
        Matrix result = multiplyMatrices(a, b, k, std::max(static_cast<unsigned>(1), std::thread::hardware_concurrency() - 1));
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << "-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << std::endl;
        std::cout << "Result with k = " << k << ":" << std::endl;
        int blocks = N / k + static_cast<int>(N % k != 0);
        int thread_count = blocks * blocks * blocks;
        std::cout << "Tasks Count: \t" << thread_count << std::endl;
        std::cout << '\t' << ms_double.count() << "ms\n";
    }

    return 0;
}
