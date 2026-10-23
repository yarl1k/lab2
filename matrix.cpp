#include <iostream>
#include <vector>
#include <chrono>
#include <random>

const int N = 1000;

void multiplyMatrices(const std::vector<double>& A, 
                      const std::vector<double>& B, 
                      std::vector<double>& C, int n) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;
            for (int k = 0; k < n; ++k) {
                sum += A[i * n + k] * B[k * n + j];
            }
            C[i * n + j] = sum;
        }
    }
}

int main() {
    std::vector<double> A(N * N), B(N * N), C(N * N, 0.0);
    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(1.0, 10.0);

    for (int i = 0; i < N * N; ++i) {
        A[i] = dist(rng);
        B[i] = dist(rng);
    }

    auto start = std::chrono::high_resolution_clock::now();
    multiplyMatrices(A, B, C, N);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> elapsed = end - start;
    std::cout << "Time: " << elapsed.count() << " s\n";

    double checksum = 0.0;
    for (int i = 0; i < 100; ++i) checksum += C[i * N + i];
    std::cout << "Checksum: " << checksum << "\n";

    return 0;
}