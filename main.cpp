#include <iostream>
#include <cmath>
#include <fstream>
#include <iomanip>


class Matrix {
private:
    double **value;
    int size;

public:
    explicit Matrix(int size) {
        this->size = size;

        value = new double*[size];

        for (int index = 0; index < size; index++) {
            auto array = new double[size];

            for (int aIndex = 0; aIndex < size; aIndex++) {
                array[aIndex] = 0;
            }

            value[index] = array;
        }
    }

    Matrix(double **value, int size) {
        this->value = value;
        this->size = size;
    }

    Matrix(std::initializer_list<std::initializer_list<double>> data) {
        size = 0;

        auto matrix = new double*[data.size()];

        for (auto string : data) {
            auto array = new double[string.size()];
            int counter = 0;

            for (auto val : string) {
                array[counter++] = val;
            }

            matrix[size++] = array;
        }

        this->value = matrix;
    }

    double* operator[] (int index) {
        return this->value[index];
    }

    [[nodiscard]] int getSize() const {
        return size;
    }

    Matrix transpose() {
        auto result = Matrix(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                result[j][i] = value[i][j];
            }
        }

        return result;
    }

    Matrix decompose() {
        auto result = Matrix(size);

        result[0][0] = std::sqrt(value[0][0]);

        for (int j = 1; j < size; j++) {
            result[j][0] = value[j][0] / result[0][0];
        }

        auto same = [&] (int i)
        {
            double sum = 0;

            for (int p = 0; p <= i - 1; p++) {
                sum += pow(result[i][p], 2);
            }

            return sqrt(value[i][i] - sum);
        };

        auto diff = [&] (int i, int j) {
            double sum = 0;

            for (int p = 0; p <= i - 1; p++) {
                sum += result[i][p] * result[j][p];
            }

            return (1 / result[i][i]) * (value[j][i] - sum);
        };

        for (int strIndex = 1; strIndex < size; strIndex++) {
            for (int colIndex = 1; colIndex <= strIndex; colIndex++) {
                result[strIndex][colIndex] = (strIndex == colIndex) ? same(strIndex) : diff(colIndex, strIndex);
            }
        }

        return result;
    }

    void print() {
        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                std::cout << value[i][j] << ' ';
            }
            std::cout << '\n';
        }
    }
};


Matrix* getMatrix(std::string filename, int *equationsCount) {
    std::ifstream file(filename, std::ios_base::in);

    if (!file.is_open()) {
        std::cout << "file " << filename << " was not open.\n";
        return nullptr;
    }

    int eMin, eMax;
    file >> eMin >> eMax;

    *equationsCount = std::abs(eMax - eMin) + 1;

    int size;
    double some;
    Matrix* result = (Matrix*)malloc(sizeof(Matrix) * *equationsCount);

    for (int index = 0; index < *equationsCount; index++) {
        file >> size >> some;

        result[index] = Matrix(size);

        for (int i = 0; i < size; i++) {
            for (int j = 0; j < size; j++) {
                file >> result[index][i][j];
            }
        }
    }

    file.close();

    return result;
}


double* solve(Matrix S, double* b) {
    auto result = new double[S.getSize()];

    for (int index = 0; index < S.getSize(); index++) {
        double c = 0;

        for (int end = 0; end < index; end++) {
            c += S[index][end] * result[end];
        }

        result[index] = (b[index] - c) / S[index][index];
    }

    return result;
}


double* solveT(Matrix S, double* b) {
    const int N = S.getSize();
    auto result = new double[N];

    for (int i = N - 1; i >= 0; i--) {
        double sum = 0;

        for (int j = i + 1; j < N; j++) {
            sum += S[i][j] * result[j];
        }

        result[i] = (b[i] - sum) /  S[i][i];
    }
}


int main() {
    int length = 0;

    auto matrsArr = getMatrix("../matrices/matricesA.matrs", &length);

    std::ofstream file("../matrices/roots.matrs");
    std::ifstream Bfile("../matrices/matricesB.vectrs", std::ios_base::in);

    for (int index = 0; index < length; index++) {
        auto S = matrsArr[index].decompose();

        double b[S.getSize()];

        for (int bIndex = 0; bIndex < S.getSize(); bIndex++)
            Bfile >> b[bIndex];

        auto x = solveT(S.transpose(), solve(S, b));

        for (int xIndex = 0; xIndex < S.getSize(); xIndex++)
            file << std::setprecision(15) << x[xIndex] << (xIndex == S.getSize() - 1 ? "" : " ");

        file << std::endl;
    }

    file.close();
    Bfile.close();

    auto L = Matrix({
        {10, 7, 5, 2},
        {7, 13, 11, 6},
        {5, 11, 19, 13},
        {2, 6, 13, 17}
    });

    double b[4] = {4, 5, 6, 7};

    auto y = solve(L.decompose(), b);

    auto x = solveT(L.decompose().transpose(), b);

    for (int index = 0; index < 4; index++) {
        std::cout << x[index] << ' ';
    }
    std::cout << std::endl;

    return 0;
}
