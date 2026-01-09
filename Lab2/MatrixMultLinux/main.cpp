#include <iostream>
#include <pthread.h>
#include <string>
#include <chrono>
#include <vector>

typedef std::vector<std::vector<int> > Matrix;
typedef std::vector<int> Array;
typedef std::pair<int, int> Pair;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::duration;
using std::chrono::milliseconds;

struct thread_args {
    Matrix *result;
    const Matrix *a;
    const Matrix *b;
    Pair aBlockCoords;
    Pair bBlockCoords;
    int blockSize;
    pthread_mutex_t *mutexId;
};

Matrix generateMatrix(int n);

void *multiplyMatricesBlocksInPlace(void *);

void printMatrix(const Matrix &);

void addMatrix(Matrix &, const Matrix &, const Pair &);

Matrix multiplyMatrices(const Matrix &, const Matrix &, int);

Matrix mulMat(const Matrix &, const Matrix &);

bool areEqual(const Matrix &, const Matrix &);

int main() {
    srand(time(nullptr));
    int N;
    do {
        std::cout << "Enter N >= 5: ";
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
    }

    //Для N=50 лучше 2, для N=100 лучше 5, для N=200 лучше 10
    int start_k;
    std::cout << "Enter start k (0 < k <= N): ";
    std::cin >> start_k;
    if (start_k <= 0 || start_k > N) {
        start_k = 1;
    }

    for (int k = start_k; k <= N; ++k) {
        auto t1 = high_resolution_clock::now();
        Matrix result = multiplyMatrices(a, b, k);
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
        std::cout << "Result with k = " << k << ":" << std::endl;
        int blocks = N / k + static_cast<int>(N % k != 0);
        int thread_count = blocks * blocks * blocks;
        std::cout << "Thread Count: \t" << thread_count << std::endl;
        std::cout << '\t' << ms_double.count() << "ms\n";
    } {
        auto t1 = high_resolution_clock::now();
        Matrix result = mulMat(a, b);
        auto t2 = high_resolution_clock::now();
        duration<double, std::milli> ms_double = t2 - t1;
        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
        std::cout << "Result with normal mult: " << std::endl;
        std::cout << '\t' << ms_double.count() << "ms\n";
        std::cout << "=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=" << std::endl;
    }

    return 0;
}

Matrix mulMat(const Matrix &m1, const Matrix &m2) {
    int n = m1.size();
    Matrix result(n, Array(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            for (int k = 0; k < n; ++k) {
                result[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return result;
}

Matrix generateMatrix(int n) {
    Matrix result(n, Array(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = rand() % 10001 - 5000;;
        }
    }
    return result;
}

bool areEqual(const Matrix &m1, const Matrix &m2) {
    if (m1.size() != m2.size()) return false;
    for (int i = 0; i < m1.size(); ++i) {
        if (m1[i].size() != m2[i].size()) return false;
        for (int j = 0; j < m1[i].size(); ++j) {
            if (m1[i][j] != m2[i][j]) return false;
        }
    }
    return true;
}

void printMatrix(const Matrix &a) {
    for (const Array &row: a) {
        for (int i: row) {
            std::cout << i << "\t";
        }
        std::cout << "\n";
    }
}

Matrix multiplyMatrices(const Matrix &a, const Matrix &b, int blockSize) {
    int n = a.size();
    int blocks = n / blockSize + static_cast<int>(n % blockSize != 0);
    std::vector<pthread_t> threads(blocks * blocks * blocks);
    std::vector<std::vector<pthread_mutex_t> > mutexes(blocks, std::vector<pthread_mutex_t>(blocks));
    for (int i = 0; i < blocks; ++i) {
        mutexes[i].resize(blocks);
        for (int j = 0; j < blocks; ++j) {
            pthread_mutex_init(&mutexes[i][j], nullptr);
        }
    }
    Matrix result(n, Array(n));
    for (int i = 0; i < blocks; ++i) {
        for (int j = 0; j < blocks; ++j) {
            for (int k = 0; k < blocks; ++k) {
                int index = blocks * blocks * i + blocks * j + k;
                thread_args *ta = new thread_args(&result, &a, &b,
                                                  Pair({i * blockSize, k * blockSize}),
                                                  Pair({k * blockSize, j * blockSize}),
                                                  blockSize, &mutexes[i][j]);
                pthread_create(&threads[index], nullptr, multiplyMatricesBlocksInPlace, ta);
            }
        }
    }
    for (auto &t: threads) {
        pthread_join(t, nullptr);
    }
    for (auto &row: mutexes) {
        for (auto &m: row) {
            pthread_mutex_destroy(&m);
        }
    }
    return result;
}

void *multiplyMatricesBlocksInPlace(void *args) {
    thread_args *ta = static_cast<thread_args *>(args);
    Matrix &result = *ta->result;
    const Matrix &a = *ta->a;
    const Matrix &b = *ta->b;
    Pair &aBlockCoords = ta->aBlockCoords;
    Pair &bBlockCoords = ta->bBlockCoords;
    int blockSize = ta->blockSize;

    int n = a.size();
    int blockN = std::min(n, aBlockCoords.first + blockSize) - aBlockCoords.first;
    int blockM = std::min(n, bBlockCoords.second + blockSize) - bBlockCoords.second;
    int depth = std::min(n, aBlockCoords.second + blockSize) - aBlockCoords.second;

    Matrix blockResult(blockN, Array(blockM));
    for (int i = 0; i < blockN; ++i) {
        for (int j = 0; j < blockM; ++j) {
            for (int k = 0; k < depth; ++k) {
                blockResult[i][j] += a[aBlockCoords.first + i][aBlockCoords.second + k]
                        * b[bBlockCoords.first + k][bBlockCoords.second + j];
            }
        }
    }

    pthread_mutex_lock(ta->mutexId);
    addMatrix(result, blockResult, {aBlockCoords.first, bBlockCoords.second});
    pthread_mutex_unlock(ta->mutexId);
    delete ta;
    return nullptr;
}

void addMatrix(Matrix &a, const Matrix &b, const Pair &pos) {
    for (int i = 0; i < b.size(); ++i) {
        for (int j = 0; j < b[0].size(); ++j) {
            a[pos.first + i][pos.second + j] += b[i][j];
        }
    }
}
