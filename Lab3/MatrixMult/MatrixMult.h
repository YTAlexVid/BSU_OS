//
// Created by Alex Saharchuk on 26/10/2025.
//

#ifndef MATRIXMULT_H
#define MATRIXMULT_H
#include <vector>

#include "bufferedChannel.h"

typedef std::vector<std::vector<int> > Matrix;
typedef std::vector<int> Array;
typedef std::pair<int, int> Pair;
typedef std::vector<std::vector<std::shared_ptr<std::mutex> > > MutexMatrix;

struct Task {
    int i;
    int j;
    int k;
};

Matrix mulMat(const Matrix &, const Matrix &);

Matrix generateMatrix(int n);

bool areEqual(const Matrix &, const Matrix &);

void worker(Matrix &, const Matrix &, const Matrix &, MutexMatrix &, BufferedChannel<Task> &, int);

Matrix multiplyMatrices(const Matrix &, const Matrix &, int, int);

void addMatrix(Matrix &, const Matrix &, const Pair &);

void printMatrix(const Matrix &);

#endif //MATRIXMULT_H
