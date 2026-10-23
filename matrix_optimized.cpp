#include <iostream>
#include <vector>
#include <chrono>
#include <random>

const int N = 1000;

void transposeMatrix(
    const std::vector<double>& B,
    std::vector<double>& BT,
    int n
) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            BT[j * n + i] = B[i * n + j];
        }
    }
}

void multiplyMatrices(
    const std::vector<double>& A,
    const std::vector<double>& BT,
    std::vector<double>& C,
    int n
) {
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            double sum = 0.0;

            for (int k = 0; k < n; ++k) {
                sum += A[i * n + k] * BT[j * n + k];
            }

            C[i * n + j] = sum;
        }
    }
}

int main() {
    std::vector<double> A(N * N);
    std::vector<double> B(N * N);
    std::vector<double> BT(N * N);
    std::vector<double> C(N * N, 0.0);

    std::mt19937 rng(42);
    std::uniform_real_distribution<double> dist(1.0, 10.0);

    for (int i = 0; i < N * N; ++i) {
        A[i] = dist(rng);
        B[i] = dist(rng);
    }

    auto transposeStart = std::chrono::steady_clock::now();

    transposeMatrix(B, BT, N);

    auto transposeEnd = std::chrono::steady_clock::now();

    auto multiplyStart = std::chrono::steady_clock::now();

    multiplyMatrices(A, BT, C, N);

    auto multiplyEnd = std::chrono::steady_clock::now();

    double transposeTime =
        std::chrono::duration<double>(
            transposeEnd - transposeStart
        ).count();

    double multiplyTime =
        std::chrono::duration<double>(
            multiplyEnd - multiplyStart
        ).count();

    double checksum = 0.0;

    for (int i = 0; i < 100; ++i) {
        checksum += C[i * N + i];
    }

    std::cout << "Transpose time: "
              << transposeTime << " s\n";

    std::cout << "Multiplication time: "
              << multiplyTime << " s\n";

    std::cout << "Total time: "
              << transposeTime + multiplyTime << " s\n";

    std::cout << "Checksum: "
              << checksum << '\n';

    return 0;
}