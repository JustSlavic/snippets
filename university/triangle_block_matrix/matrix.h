#ifndef MATRIX_H
#define MATRIX_H

#include <cstdlib>
#include <vector>
#include <utility>
#include <random>
#include <fstream>
#include <cassert>
#include "block_matrix.h"


template <typename U, size_t D>
class block_matrix;


template <typename T>
class matrix {
    size_t size_{0};
    size_t dimension_{0};
    std::vector<T> data_;

public:
    matrix() = default;
    matrix(const matrix&) = default;
    matrix& operator=(const matrix&) = default;
    matrix(matrix&&) noexcept = default;
    matrix& operator=(matrix&&) noexcept = default;

    explicit matrix(size_t dimension, T default_value = T())
        : size_(dimension*dimension)
        , dimension_(dimension)
        , data_(dimension*dimension, default_value)
    {}

    static matrix from_triangle_matrix_file(const std::string& filename) {
        std::ifstream input{filename};
        if (!input.good()) throw std::runtime_error("bad file");

        size_t dimension;
        input >> dimension;

        matrix result(dimension);

        for (size_t i = 0; i < dimension; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                input >> result.data_[i + j*dimension];
                result.data_[j + i*dimension] = result.data_[i + j*dimension];
            }
        }

        return result;
    }

    matrix& transpose() {
        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < i; ++j) {
                std::swap(data_[i*dimension_ + j], data_[j*dimension_ + i]);
            }
        }
        return *this;
    }

    matrix multiply_parallel_i(const matrix& other) const {
        matrix result(dimension_);

        const T* lhs = this->data_.data();
        const T* rhs = other.data_.data();
        T* res = result.data_.data();

#pragma omp parallel for firstprivate(lhs, rhs, res)
        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
                for (size_t k = 0; k < dimension_; ++k) {
                    res[i*dimension_ + j] += lhs[i*dimension_ + k] * rhs[k*dimension_ + j];
                }
            }
        }

        return result;
    }

    matrix multiply_parallel_j(const matrix& other) const {
        matrix result(dimension_);

        const T* lhs = this->data_.data();
        const T* rhs = other.data_.data();
        T* res = result.data_.data();

        for (size_t i = 0; i < dimension_; ++i) {
#pragma omp parallel for firstprivate(i, lhs, rhs, res)
            for (size_t j = 0; j < dimension_; ++j) {
                    for (size_t k = 0; k < dimension_; ++k) {
                    res[i*dimension_ + j] += lhs[i*dimension_ + k] * rhs[k*dimension_ + j];
                }
            }
        }

        return result;
    }

    matrix multiply_parallel_k(const matrix& other) const {
        matrix result(dimension_);

        const T* lhs = this->data_.data();
        const T* rhs = other.data_.data();
        T* res = result.data_.data();

        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
#pragma omp parallel for firstprivate(i, j, lhs, rhs, res)
                for (size_t k = 0; k < dimension_; ++k) {
                    res[i*dimension_ + j] += lhs[i*dimension_ + k] * rhs[k*dimension_ + j];
                }
            }
        }

        return result;
    }

    matrix multiply_on_single_thread(const matrix& other) const {
        matrix result(dimension_);
        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
                for (size_t k = 0; k < dimension_; ++k) {
                    result.data_[i*dimension_ + j] += data_[i*dimension_ + k] * other.data_[k*dimension_ + j];
                }
            }
        }
        return result;
    }

    matrix multiply_on_single_thread_but_rhs_transposed(const matrix& other) const {
        matrix result(dimension_);
        for (size_t i = 0; i < dimension_; ++i) {
            for (size_t j = 0; j < dimension_; ++j) {
                for (size_t k = 0; k < dimension_; ++k) {
                    result.data_[i*dimension_ + j] += data_[i*dimension_ + k] * other.data_[j*dimension_ + k];
                }
            }
        }
        return result;
    }

    template <typename U>
    friend std::ostream& operator<<(std::ostream&, const matrix<U>&);
    friend matrix<double> generate_random_double_matrix(size_t dimension);
    template <typename U, size_t D>
    friend bool operator==(const matrix<U>& lhs, const block_matrix<U, D>& rhs);
    template <typename U>
    friend bool operator==(const matrix<U>& lhs, const matrix<U>& rhs);

    size_t size_in_bytes() const {
        return size_ * sizeof(T);
    }
};

template <typename T>
std::ostream& operator<<(std::ostream& os, const matrix<T>& m) {
    for (size_t i = 0; i < m.dimension_; ++i) {
        os << "| ";
        for (size_t j = 0; j < m.dimension_; ++j) {
            os << m.data_[i*m.dimension_ + j] << " ";
        }
        os << "|\n";
    }

    return os;
}

template <typename T>
bool operator==(const matrix<T>& lhs, const matrix<T>& rhs) {
    assert(lhs.dimension_ == rhs.dimension_);

    for (size_t i = 0; i < lhs.size_; ++i) {
        if (lhs.data_[i] != rhs.data_[i]) return false;
    }

    return true;
}

template <typename T, size_t block_dimension>
bool operator==(const matrix<T>& lhs, const block_matrix<T, block_dimension>& rhs) {
    assert(lhs.dimension_ == rhs.dimension_);
    size_t pierce_blocks_i = 0;

    // coordinates of a block
    for (size_t block_i = 0; block_i < (rhs.dimension_ / block_dimension); ++block_i) {
        for (size_t block_j = 0; block_j < (rhs.dimension_ / block_dimension); ++block_j) {

            // coordinates within original matrix
            for (size_t i = block_i * block_dimension; i < block_i * block_dimension + block_dimension; ++i) {
                for (size_t j = block_j * block_dimension; j < block_j * block_dimension + block_dimension; ++j) {
                    if (rhs.data_[pierce_blocks_i++] != lhs.data_[i*lhs.dimension_ + j]) {
                        return false;
                    }
                }
            }
        }
    }

    return true;
}

template <typename T, size_t block_dimension>
bool operator==(const block_matrix<T, block_dimension>& lhs, const matrix<T>& rhs) {
    return rhs == lhs;
}

matrix<double> generate_random_double_matrix(size_t dimension) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distribution(-20, 20);

    matrix<double> result(dimension);

    for (size_t i = 0; i < result.size_; ++i) {
        result.data_[i] = distribution(generator);
    }

    return result;
}

#endif //MATRIX_H
