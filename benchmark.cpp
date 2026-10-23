#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

const int RUNS = 7;

struct Result {
    std::string name;
    std::vector<double> times;
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

int main() {
    std::array<std::string, 3> programs = {
        "./matrix_O0",
        "./matrix_O2",
        "./matrix_O3"
    };

    std::array<std::string, 3> names = {
        "-O0",
        "-O2",
        "-O3"
    };

    std::array<Result, 3> results;

    for (int p = 0; p < 3; ++p) {
        results[p].name = names[p];
    }

    std::cout << std::fixed << std::setprecision(6);

    for (int run = 1; run <= RUNS; ++run) {
        std::cout << "\n=== Run " << run << " ===\n";

        for (int p = 0; p < 3; ++p) {
            std::string command =
                programs[p] + " > benchmark_output.txt";

            auto start = std::chrono::steady_clock::now();
            int status = std::system(command.c_str());
            auto end = std::chrono::steady_clock::now();

            if (status != 0) {
                std::cerr << "Error running " << programs[p] << '\n';
                return 1;
            }

            // Читаємо час самої функції з виводу програми.
            // Для цього програма має виводити рядок "Time: ... s".
            FILE* file = std::fopen("benchmark_output.txt", "r");

            if (file == nullptr) {
                std::cerr << "Cannot open benchmark output\n";
                return 1;
            }

            char line[256];
            double elapsed = -1.0;

            while (std::fgets(line, sizeof(line), file)) {
                double value;

                if (std::sscanf(line, "Time: %lf s", &value) == 1) {
                    elapsed = value;
                    break;
                }
            }

            std::fclose(file);

            if (elapsed < 0.0) {
                std::cerr << "Cannot read execution time\n";
                return 1;
            }

            results[p].times.push_back(elapsed);

            std::cout << names[p] << ": "
                      << elapsed << " s\n";
        }
    }

    std::cout << "\n========== RESULTS ==========\n";
    std::cout << std::left
              << std::setw(10) << "Version"
              << std::right
              << std::setw(14) << "Mean (s)"
              << std::setw(14) << "Median (s)"
              << std::setw(14) << "Min (s)"
              << std::setw(14) << "Max (s)"
              << std::setw(14) << "Speedup"
              << '\n';

    double baseline = mean(results[0].times);

    for (const auto& result : results) {
        double avg = mean(result.times);
        double med = median(result.times);

        auto minmax = std::minmax_element(
            result.times.begin(), result.times.end()
        );

        double speedup = baseline / avg;

        std::cout << std::left
                  << std::setw(10) << result.name
                  << std::right
                  << std::setw(14) << avg
                  << std::setw(14) << med
                  << std::setw(14) << *minmax.first
                  << std::setw(14) << *minmax.second
                  << std::setw(14) << speedup
                  << '\n';
    }

    std::cout << "\nSpeedup is relative to -O0.\n";

    return 0;
}