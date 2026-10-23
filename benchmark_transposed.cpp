
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

const int RUNS = 7;

struct Result {
    std::string name;
    std::vector<double> transposeTimes;
    std::vector<double> multiplyTimes;
    std::vector<double> totalTimes;
};

double mean(const std::vector<double>& values) {
    double sum = 0.0;

    for (double value : values) {
        sum += value;
    }

    return sum / values.size();
}

double median(std::vector<double> values) {
    std::sort(values.begin(), values.end());
    return values[values.size() / 2];
}

bool readTimes(const std::string& filename,
               double& transpose,
               double& multiply,
               double& total) {
    FILE* file = std::fopen(filename.c_str(), "r");

    if (file == nullptr) {
        return false;
    }

    char line[256];
    bool foundTranspose = false;
    bool foundMultiply = false;
    bool foundTotal = false;

    while (std::fgets(line, sizeof(line), file)) {
        if (std::sscanf(line, "Transpose time: %lf s",
                        &transpose) == 1) {
            foundTranspose = true;
        } else if (std::sscanf(line, "Multiplication time: %lf s",
                               &multiply) == 1) {
            foundMultiply = true;
        } else if (std::sscanf(line, "Total time: %lf s",
                               &total) == 1) {
            foundTotal = true;
        }
    }

    std::fclose(file);

    return foundTranspose && foundMultiply && foundTotal;
}

int main() {
    const std::array<std::string, 3> programs = {
        "./matrix_transposed_O0",
        "./matrix_transposed_O2",
        "./matrix_transposed_O3"
    };

    const std::array<std::string, 3> names = {
        "-O0", "-O2", "-O3"
    };

    std::array<Result, 3> results;

    for (int i = 0; i < 3; ++i) {
        results[i].name = names[i];
    }

    std::cout << std::fixed << std::setprecision(6);

    for (int run = 1; run <= RUNS; ++run) {
        std::cout << "\n=== Run " << run << " ===\n";

        for (int p = 0; p < 3; ++p) {
            const std::string filename = "benchmark_output.txt";
            const std::string command =
                programs[p] + " > " + filename;

            int status = std::system(command.c_str());

            if (status != 0) {
                std::cerr << "Error running "
                          << programs[p] << '\n';
                return 1;
            }

            double transpose = 0.0;
            double multiply = 0.0;
            double total = 0.0;

            if (!readTimes(filename, transpose, multiply, total)) {
                std::cerr << "Cannot read timings from "
                          << programs[p] << '\n';
                return 1;
            }

            results[p].transposeTimes.push_back(transpose);
            results[p].multiplyTimes.push_back(multiply);
            results[p].totalTimes.push_back(total);

            std::cout << names[p]
                      << ": transpose = " << transpose
                      << " s, multiplication = " << multiply
                      << " s, total = " << total << " s\n";
        }
    }

    std::cout << "\n========== RESULTS ==========\n";

    std::cout << std::left
              << std::setw(10) << "Version"
              << std::right
              << std::setw(14) << "Transpose"
              << std::setw(14) << "Multiply"
              << std::setw(14) << "Total"
              << std::setw(14) << "Speedup"
              << '\n';

    const double baseline = mean(results[0].totalTimes);

    for (const auto& result : results) {
        const double transpose = mean(result.transposeTimes);
        const double multiply = mean(result.multiplyTimes);
        const double total = mean(result.totalTimes);
        const double speedup = baseline / total;

        std::cout << std::left
                  << std::setw(10) << result.name
                  << std::right
                  << std::setw(14) << transpose
                  << std::setw(14) << multiply
                  << std::setw(14) << total
                  << std::setw(13) << speedup << "x"
                  << '\n';
    }

    std::cout << "\nAll times are mean values over "
              << RUNS << " runs.\n";
    std::cout << "Speedup is relative to -O0 total time.\n";

    return 0;
}
