//
// Created by Alex Saharchuk on 26/10/2025.
//

#include "MatrixMult.h"
#include "bufferedChannel.h"

#include <iostream>
#include <thread>
#include <mutex>
#include <vector>

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
    Matrix result(n, Array(n, 0));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            result[i][j] = rand() % 10001 - 5000;
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


void worker(Matrix &result, const Matrix &a, const Matrix &b, MutexMatrix &mutexes, BufferedChannel<Task> &channel,
            int blockSize) {
    while (true) {
        std::pair<Task, bool> task = channel.receive();
        if (task.second == false) break;
        int i = task.first.i;
        int j = task.first.j;
        int k = task.first.k;
        int n = a.size();
        int x = i * blockSize;
        int y = j * blockSize;
        int depth = k * blockSize;
        int blockN = std::min(n - x, blockSize);
        int blockM = std::min(n - y, blockSize);
        int blockDepth = std::min(n - depth, blockSize);

        Matrix blockResult(blockN, Array(blockM));
        for (int i = 0; i < blockN; ++i) {
            for (int j = 0; j < blockM; ++j) {
                for (int k = 0; k < blockDepth; ++k) {
                    blockResult[i][j] += a[x + i][depth + k]
                            * b[depth + k][y + j];
                }
            }
        }

        std::lock_guard lock(*mutexes[i][j]);
        addMatrix(result, blockResult, {x, y});
    }
}

Matrix multiplyMatrices(const Matrix &a, const Matrix &b, int blockSize, int workers_count) {
    int n = a.size();
    int blocks = n / blockSize + static_cast<int>(n % blockSize != 0);
    Matrix result(n, Array(n, 0));
    BufferedChannel<Task> channel(workers_count * 2);

    MutexMatrix mutexes(blocks);
    for (int i = 0; i < blocks; ++i) {
        mutexes[i].resize(blocks);
        for (int j = 0; j < blocks; ++j) {
            mutexes[i][j] = std::make_unique<std::mutex>();
        }
    }

    std::vector<std::thread> threads;
    threads.reserve(workers_count);
    for (int i = 0; i < workers_count; ++i) {
        threads.emplace_back(worker, std::ref(result), std::cref(a), std::cref(b), std::ref(mutexes), std::ref(channel), blockSize);
    }

    for (int i = 0; i < blocks; ++i) {
        for (int j = 0; j < blocks; ++j) {
            for (int k = 0; k < blocks; ++k) {
                channel.send({i, j, k});
            }
        }
    }
    channel.close();
    for (auto &t: threads) {
        t.join();
    }
    return result;
}

void addMatrix(Matrix &a, const Matrix &b, const Pair &pos) {
    for (int i = 0; i < b.size(); ++i) {
        for (int j = 0; j < b[0].size(); ++j) {
            a[pos.first + i][pos.second + j] += b[i][j];
        }
    }
}

void printMatrix(const Matrix &a) {
    for (const Array &row: a) {
        for (int i: row) {
            std::cout << i << "\t";
        }
        std::cout << "\n";
    }
}
