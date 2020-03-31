#ifndef TRIANGLE_BLOCK_MATRIX_H
#define TRIANGLE_BLOCK_MATRIX_H

#include <cstdlib>
#include <string>
#include <fstream>
#include <cassert>
#include <utility>
#include <vector>

#include "block_matrix.h"


template <typename T, size_t block_dimension = 1>
class triangle_block_matrix {
    size_t size_{0};  // размер выделенной памяти
    size_t dimension_{0};  // размерность матрицы
    size_t block_size_{0};  // длина блока матрицы в памяти
    std::vector<size_t> block_mapping_;
    T* data_{nullptr};  // данные

public:
    // конструктор по-умолчанию
    triangle_block_matrix() = default;
    // деструктор
    ~triangle_block_matrix() { delete[] data_; }

    // копирующий конструктор
    triangle_block_matrix(const triangle_block_matrix& other)
        : size_(other.size_)
        , dimension_(other.dimension_)
        , block_size_(other.block_size_)
        , block_mapping_(other.block_mapping_)
    {
        data_ = new T[size_];
        for (size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }

    // копирующий оператор присваивания
    triangle_block_matrix& operator=(const triangle_block_matrix& other) noexcept {
        triangle_block_matrix(other).swap(*this);
        return *this;
    }

    // перемещающий конструктор
    triangle_block_matrix(triangle_block_matrix&& other) noexcept = default;

    // перемещающий оператор присваивания
    triangle_block_matrix& operator=(triangle_block_matrix&& other) noexcept {
        triangle_block_matrix(std::move(other)).swap(*this);
        return *this;
    }

    // конструктор нулевой матрицы
    explicit triangle_block_matrix(size_t dimension, T default_value = T()) {
        size_ = calc_size_(dimension);
        dimension_ = dimension;
        block_size_ = block_dimension*block_dimension;

        block_mapping_.resize(dimension * dimension / block_dimension / block_dimension);
        size_t piercing_numeration = 0;
        for (size_t i = 0; i < dimension / block_dimension; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                block_mapping_[i*dimension/block_dimension + j] = \
                block_mapping_[j*dimension/block_dimension + i] = piercing_numeration++;
            }
        }

        data_ = new T[size_];
        for (size_t i = 0; i < size_; ++i) data_[i] = default_value;
    }

    // статическая функция чтения из файла
    static triangle_block_matrix from_file(const std::string& filename) {
        std::ifstream input{filename};
        if (!input.good()) throw std::runtime_error("bad file");

        size_t dimension = 0;
        input >> dimension;

        triangle_block_matrix result(dimension, block_dimension);

        // заполнить квадратную матрицу полностью
        T* square_matrix = new T[dimension * dimension];
        for (size_t i = 0; i < dimension; ++i) {
            for (size_t j = 0; j <= i; ++j) {
                input >> square_matrix[i * dimension + j];
                square_matrix[j * dimension + i] = square_matrix[i * dimension + j];
            }
        }

        // перенести заполненные данные в блочный формат
        size_t pierce_blocks_i = 0;

        // coordinates of a block
        for (size_t block_i = 0; block_i < (dimension / block_dimension); ++block_i) {
            for (size_t block_j = 0; block_j <= block_i; ++block_j) {

                // coordinates within original matrix
                for (size_t i = block_i * block_dimension; i < block_i * block_dimension + block_dimension; ++i) {
                    for (size_t j = block_j * block_dimension; j < block_j * block_dimension + block_dimension; ++j) {
                        result.data_[pierce_blocks_i++] = square_matrix[i * dimension + j];
                    }
                }
            }
        }

        delete[] square_matrix;
        return result;
    }

    template <typename U, size_t b_dim>
    friend std::ostream& operator<<(std::ostream& os, const triangle_block_matrix<U, b_dim>& matrix);
    template<size_t D>
    friend triangle_block_matrix<double, D> generate_random_double_triangle_block_matrix(size_t dimension);

    block_matrix<T, block_dimension> multiply_on_single_thread(const triangle_block_matrix& other) {
        assert(dimension_ == other.dimension_);
        block_matrix<T, block_dimension> result(dimension_);

        size_t blocks_in_strip = dimension_ / block_dimension;
        for (size_t i = 0; i < blocks_in_strip; ++i) {
            for (size_t k = 0; k <= i; ++k) {
                for (size_t j = 0; j <= k; ++j) {
                    multiply_blocks(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                            );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]  * b[" << block_mapping_[k * blocks_in_strip + j] << "];" << std::endl;
                }
                for (size_t j = k + 1; j < blocks_in_strip; ++j) {
                    multiply_blocks_but_rhs_transposed(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                            );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]  * b[" << block_mapping_[k * blocks_in_strip + j] << "]t;" << std::endl;
                }
            }
            for (size_t k = i + 1; k < blocks_in_strip; ++k) {
                for (size_t j = 0; j <= k; ++j) {
                    multiply_blocks_but_lhs_transposed(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]t * b[" << block_mapping_[k * blocks_in_strip + j] << "];" << std::endl;
                }
                for (size_t j = k + 1; j < blocks_in_strip; ++j) {
                    multiply_blocks_but_lhs_and_rhs_transposed(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]t * b[" << block_mapping_[k * blocks_in_strip + j] << "]t;" << std::endl;
                }
            }
        }

        return result;
    }

    block_matrix<T, block_dimension> multiply_parallel_blocks(const triangle_block_matrix& other) {
        assert(dimension_ == other.dimension_);
        block_matrix<T, block_dimension> result(dimension_);

        const T* lhs = this->data_;
        const T* rhs = other.data_;
        T* res = result.data_;

        size_t blocks_in_strip = dimension_ / block_dimension;
        for (size_t i = 0; i < blocks_in_strip; ++i) {
            for (size_t k = 0; k <= i; ++k) {
#pragma omp parallel for firstprivate(i, k, lhs, rhs, res)
                for (size_t j = 0; j <= k; ++j) {
                    multiply_blocks(
                            lhs + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            rhs + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            res + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]  * b[" << block_mapping_[k * blocks_in_strip + j] << "];" << std::endl;
                }
#pragma omp parallel for firstprivate(i, k, lhs, rhs, res)
                for (size_t j = k + 1; j < blocks_in_strip; ++j) {
                    multiply_blocks_but_rhs_transposed(
                            lhs + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            rhs + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            res + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]  * b[" << block_mapping_[k * blocks_in_strip + j] << "]t;" << std::endl;
                }
            }
            for (size_t k = i + 1; k < blocks_in_strip; ++k) {
#pragma omp parallel for firstprivate(i, k, lhs, rhs, res)
                for (size_t j = 0; j <= k; ++j) {
                    multiply_blocks_but_lhs_transposed(
                            lhs + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            rhs + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            res + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]t * b[" << block_mapping_[k * blocks_in_strip + j] << "];" << std::endl;
                }
#pragma omp parallel for firstprivate(i, k, lhs, rhs, res)
                for (size_t j = k + 1; j < blocks_in_strip; ++j) {
                    multiply_blocks_but_lhs_and_rhs_transposed(
                            lhs + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            rhs + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            res + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]t * b[" << block_mapping_[k * blocks_in_strip + j] << "]t;" << std::endl;
                }
            }
        }

        return result;
    }

    block_matrix<T, block_dimension> multiply_but_block_multiplication_parallel(const triangle_block_matrix& other) {
        assert(dimension_ == other.dimension_);
        block_matrix<T, block_dimension> result(dimension_);

        size_t blocks_in_strip = dimension_ / block_dimension;
        for (size_t i = 0; i < blocks_in_strip; ++i) {
            for (size_t k = 0; k <= i; ++k) {
                for (size_t j = 0; j <= k; ++j) {
                    multiply_blocks_parallel(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]  * b[" << block_mapping_[k * blocks_in_strip + j] << "];" << std::endl;
                }
                for (size_t j = k + 1; j < blocks_in_strip; ++j) {
                    multiply_blocks_but_rhs_transposed_parallel(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]  * b[" << block_mapping_[k * blocks_in_strip + j] << "]t;" << std::endl;
                }
            }
            for (size_t k = i + 1; k < blocks_in_strip; ++k) {
                for (size_t j = 0; j <= k; ++j) {
                    multiply_blocks_but_lhs_transposed_parallel(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]t * b[" << block_mapping_[k * blocks_in_strip + j] << "];" << std::endl;
                }
                for (size_t j = k + 1; j < blocks_in_strip; ++j) {
                    multiply_blocks_but_lhs_and_rhs_transposed_parallel(
                            this->data_ + block_mapping_[i * blocks_in_strip + k]*block_size_,
                            other.data_ + block_mapping_[k * blocks_in_strip + j]*block_size_,
                            result.data_ + i*block_size_*blocks_in_strip + j*block_size_,
                            block_dimension
                    );
//                    std::cout << "c[" << i << "][" << j << "] += a[" << block_mapping_[i * blocks_in_strip + k] <<
//                              "]t * b[" << block_mapping_[k * blocks_in_strip + j] << "]t;" << std::endl;
                }
            }
        }

        return result;
    }

    void print_as_stored(std::ostream& os) {
        size_t block_start = 0;
        for (size_t block_i = 0; block_i < dimension_ / block_dimension; ++block_i) {
            os << "| ";
            for (size_t block_j = 0; block_j <= block_i; ++block_j) {
                print_block(data_ + block_start, block_dimension, os);
                os << ' ';
                block_start += block_size_;
            }
            os << "|\n";
        }
        os << std::endl;
    }

    size_t size_in_bytes() const {
        return size_ * sizeof(T);
    }

private:
    static size_t calc_size_(size_t dimension) {
        assert(dimension % block_dimension == 0);
        return dimension * (dimension + block_dimension) / 2;
    }

    void swap(triangle_block_matrix& other) {
        std::swap(size_, other.size_);
        std::swap(dimension_, other.dimension_);
        std::swap(block_size_, other.block_size_);
        std::swap(block_mapping_, other.block_mapping_);
        std::swap(data_, other.data_);
    }

    static void multiply_blocks(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[i*dim + k] * rhs[k*dim + j];
                }
            }
        }
    }

    static void multiply_blocks_but_lhs_transposed(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[k*dim + i] * rhs[k*dim + j];
                }
            }
        }
    }

    static void multiply_blocks_but_rhs_transposed(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[i*dim + k] * rhs[j*dim + k];
                }
            }
        }
    }

    static void multiply_blocks_but_lhs_and_rhs_transposed(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[k*dim + i] * rhs[j*dim + k];
                }
            }
        }
    }

    static void multiply_blocks_parallel(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
#pragma omp parallel for firstprivate(lhs, rhs, res, dim, i)
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[i*dim + k] * rhs[k*dim + j];
                }
            }
        }
    }

    static void multiply_blocks_but_lhs_transposed_parallel(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
#pragma omp parallel for firstprivate(lhs, rhs, res, dim, i)
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[k*dim + i] * rhs[k*dim + j];
                }
            }
        }
    }

    static void multiply_blocks_but_rhs_transposed_parallel(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
#pragma omp parallel for firstprivate(lhs, rhs, res, dim, i)
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[i*dim + k] * rhs[j*dim + k];
                }
            }
        }
    }

    static void multiply_blocks_but_lhs_and_rhs_transposed_parallel(const T* lhs, const T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
#pragma omp parallel for firstprivate(lhs, rhs, res, dim, i)
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[k*dim + i] * rhs[j*dim + k];
                }
            }
        }
    }

    static void print_block(T* block, size_t dim, std::ostream& os) {
        os << "[";
        for (size_t i = 0; i < dim*dim - 1; ++i) {
            os << block[i] << ", ";
        }
        os << block[dim*dim - 1] << "]";
    }
};


template <typename T, size_t block_dimension>
std::ostream& operator<<(std::ostream& os, const triangle_block_matrix<T, block_dimension>& matrix) {
    size_t strip_start = 0;

    // итерирование по координатам блока (по i, вниз)
    for (size_t block_i = 0; block_i < (matrix.dimension_ / block_dimension); ++ block_i) {
        size_t strip_width = block_dimension * (block_i + 1);
        size_t strip_height = block_dimension;
        strip_start += block_i * matrix.block_size_;

        // итерирование по координатам внутри одного блока (по i, вниз)
        for (size_t i = 0; i < block_dimension; ++i) {
            os << "| ";
            // итерирование по координатам блока (по j, вправо)
            for (size_t block_j = 0; block_j <= block_i; ++block_j) {
                // итерирование по координатам внутри одного блока (по j, вправо)
                for (size_t j = 0; j < block_dimension; ++j) {
                    os << matrix.data_[strip_start + block_j * matrix.block_size_ + i*block_dimension + j] << " ";
                }
            }
            // заполнение плейсхолдерами мест, для которых память не выделена
            for (size_t block_j = block_i + 1; block_j < (matrix.dimension_ / block_dimension); ++block_j) {
                for (size_t j = 0; j < block_dimension; ++j) {
                    os << "_ ";
                }
            }
            os << "|\n";
        }
    }

    return os;
}

template<size_t D>
triangle_block_matrix<double, D> generate_random_double_triangle_block_matrix(size_t dimension) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distribution(-20, 20);

    triangle_block_matrix<double, D> result(dimension);

    for (size_t i = 0; i < result.size_; ++i) {
        result.data_[i] = distribution(generator);
    }

    return result;
}


#endif //TRIANGLE_BLOCK_MATRIX_H
