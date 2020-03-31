#ifndef BLOCK_MATRIX_H
#define BLOCK_MATRIX_H


#include <cstdlib>
#include <cassert>
#include <string>
#include <fstream>
#include <utility>
#include <random>


template <typename U>
class matrix;


// объявление класса для того чтобы ниже сделать его дружественным
template <typename U, size_t D>
class triangle_block_matrix;


template <typename T, size_t block_dimension = 1>
class block_matrix {
    size_t size_{0};  // размер выделенной памяти
    size_t dimension_{0};  // размерность матрицы
    size_t block_size_{0};  // длина блока матрицы в памяти
    T* data_{nullptr};  // данные

public:
    // конструктор по-умолчанию
    block_matrix() = default;
    // деструктор
    ~block_matrix() { delete[] data_; }

    // копирующий конструктор
    block_matrix(const block_matrix& other)
            : size_(other.size_)
            , dimension_(other.dimension_)
            , block_size_(other.block_size_)
    {
        data_ = new T[size_];
        for (size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
    }

    // копирующий оператор присваивания
    block_matrix& operator=(const block_matrix& other) noexcept {
        block_matrix(other).swap(*this);
        return *this;
    }

    // перемещающий конструктор
    block_matrix(block_matrix&& other) noexcept = default;

    // перемещающий оператор присваивания
    block_matrix& operator=(block_matrix&& other) noexcept {
        block_matrix(std::move(other)).swap(*this);
        return *this;
    }

    // конструктор нулевой матрицы
    explicit block_matrix(size_t dimension, T default_value = T()) {
        size_ = calc_size_(dimension);
        dimension_ = dimension;
        block_size_ = block_dimension*block_dimension;

        data_ = new T[size_];
        for (size_t i = 0; i < size_; ++i) data_[i] = default_value;
    }

    // статическая функция чтения из файла
    static block_matrix from_triangle_matrix_file(const std::string& filename) {
        std::ifstream input{filename};
        if (!input.good()) throw std::runtime_error("bad file");

        size_t dimension = 0;
        input >> dimension;

        block_matrix result(dimension);

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
            for (size_t block_j = 0; block_j < (dimension / block_dimension); ++block_j) {

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

    bool operator==(const block_matrix& other) const {
        assert(dimension_ == other.dimension_);

        for (size_t i = 0; i < size_; ++i) {
            if (data_[i] != other.data_[i]) return false;
        }

        return true;
    }

    template <typename U, size_t D>
    friend std::ostream& operator<<(std::ostream& os, const block_matrix<U, D>& matrix);
    friend triangle_block_matrix<T, block_dimension>;
    template<size_t D>
    friend block_matrix<double, D> generate_random_double_block_matrix(size_t dimension);
    template <typename U, size_t D>
    friend bool operator==(const matrix<U>& lhs, const block_matrix<U, D>& rhs);

    block_matrix multiply_on_single_thread(const block_matrix& other) {
        assert(dimension_ == other.dimension_);
        block_matrix result(dimension_);

        size_t blocks_in_strip = dimension_ / block_dimension;
        for (size_t block_i = 0; block_i < blocks_in_strip; ++block_i) {
            for (size_t block_j = 0; block_j < blocks_in_strip; ++block_j) {
                for (size_t block_k = 0; block_k < blocks_in_strip; ++block_k) {
                    multiply_blocks(
                            data_ + block_i*block_size_*blocks_in_strip + block_k*block_size_,
                            other.data_ + block_k*block_size_*blocks_in_strip + block_j*block_size_,
                            result.data_ + block_i*block_size_*blocks_in_strip + block_j*block_size_,
                            block_dimension);
                }
            }
        }

        return result;
    }

    void print_as_stored(std::ostream& os) {
        size_t block_start = 0;
        for (size_t block_i = 0; block_i < dimension_ / block_dimension; ++block_i) {
            os << "| ";
            for (size_t block_j = 0; block_j < dimension_ / block_dimension; ++block_j) {
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
        return dimension * dimension;
    }

    void swap(block_matrix& other) {
        std::swap(size_, other.size_);
        std::swap(dimension_, other.dimension_);
        std::swap(block_size_, other.block_size_);
        std::swap(data_, other.data_);
    }

    static void multiply_blocks(T* lhs, T* rhs, T* res, size_t dim) {
        for (size_t i = 0; i < dim; ++i) {
            for (size_t j = 0; j < dim; ++j) {
                for (size_t k = 0; k < dim; ++k) {
                    res[i*dim + j] += lhs[i*dim + k] * rhs[k*dim + j];
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
std::ostream& operator<<(std::ostream& os, const block_matrix<T, block_dimension>& matrix) {
    // итерирование по координатам блока (по i, вниз)
    for (size_t block_i = 0; block_i < (matrix.dimension_ / block_dimension); ++ block_i) {
        size_t strip_start = block_i * matrix.block_size_ * (matrix.dimension_ / block_dimension);

        // итерирование по координатам внутри одного блока (по i, вниз)
        for (size_t i = 0; i < block_dimension; ++i) {
            os << "| ";
            // итерирование по координатам блока (по j, вправо)
            for (size_t block_j = 0; block_j < (matrix.dimension_ / block_dimension); ++block_j) {
                // итерирование по координатам внутри одного блока (по j, вправо)
                for (size_t j = 0; j < block_dimension; ++j) {
                    os << matrix.data_[strip_start + block_j * matrix.block_size_ + i*block_dimension + j] << " ";
                }
            }
            os << "|\n";
        }
    }

    return os;
}

template<size_t D>
block_matrix<double, D> generate_random_double_block_matrix(size_t dimension) {
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distribution(-20, 20);

    block_matrix<double, D> result(dimension);

    for (size_t i = 0; i < result.size_; ++i) {
        result.data_[i] = distribution(generator);
    }

    return result;
}


#endif //BLOCK_MATRIX_H
