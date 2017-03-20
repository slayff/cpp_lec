//A
#include "address.h"
#include <iostream>
#include <string>
#include <memory>

int main() {
    std::string line;
    while (getline(std::cin, line)) {
        std::unique_ptr<Address> address;
        try {
            address.reset(new Address());
            Parse(line, address.get());
            Unify(address.get());
            std::cout << Format(*address) << "\n";
        } catch (...) {
            std::cout << "exception\n";
        }
    }
}

//B
#include <iostream>
#include <stdexcept>
class Ball: public Figure3D {
 private:
    double radius;
 public:
    Ball(const Point3D& p, double r):Figure3D(p) {
        if (r < 0) {
            throw std::domain_error("Length must be non-negative");
        } else {
            radius = r;
        }
    }
    double Volume() const override {
        return 4 * PI * radius * radius * radius / 3;
    }
    ~Ball() override {
    }
};

class Cuboid: public Figure3D {
 private:
    double length, width, height;
 public:
    Cuboid(const Point3D& p, double l, double w, double h): Figure3D(p) {
        if (l < 0 || w < 0 || h < 0) {
            throw std::domain_error("Length must be non-negative");
        } else {
            length = l;
            width = w;
            height = h;
        }
    }

    double Volume() const override {
        return length * width * height;
    }
    ~Cuboid() override {
    }
};

//C
#include "animals.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <memory>

using Zoo = std::vector<std::unique_ptr<Animal>>;

Zoo CreateZoo() {
    Zoo zoo;
    std::string word;
    while (std::cin >> word) {
        if (word == "Tiger") {
            zoo.emplace_back(new Tiger);
        } else if (word == "Wolf") {
            zoo.emplace_back(new Wolf);
        } else if (word == "Fox") {
            zoo.emplace_back(new Fox);
        } else {
            throw std::runtime_error("Unknown animal!");
            }
    }
    return zoo;
}

void Process(const Zoo& zoo) {
    for (const auto& animal : zoo) {
        std::cout << animal->voice() << "\n";
    }
}

//D
Matrix& operator = (const Matrix& other) {
        for (size_t k = 0; k != rows; ++k) {
            delete[] data[k];
        }
        delete[] data;
        rows = other.rows;
        columns = other.columns;
        data = new T * [rows];
        size_t i = 0;
        try {
            for (; i != rows; ++i)
                data[i] = new T[columns];
        } catch (...) {
            for (size_t k = 0; k != i; ++k)
                delete [] data[k];
            delete [] data;
            throw;
        }
        for (i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
        return *this;
    }

Matrix(const Matrix& other) {
        rows = other.rows;
        columns = other.columns;
        data = new T * [rows];
        size_t i = 0;
        try {
            for (; i != rows; ++i)
                data[i] = new T[columns];
        } catch (...) {
            for (size_t k = 0; k != i; ++k)
                delete [] data[k];
            delete [] data;
            throw;
        }
        for (i = 0; i < rows; ++i) {
            for (size_t j = 0; j < columns; ++j) {
                data[i][j] = other.data[i][j];
            }
        }
}
