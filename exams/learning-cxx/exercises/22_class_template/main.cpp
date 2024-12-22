#include "../exercise.h"
#include <cstring>
#include <iostream>

// READ: 类模板 <https://zh.cppreference.com/w/cpp/language/class_template>

template<class T>
struct Tensor4D {
    unsigned int shape[4];
    T *data;

    // 构造函数: 初始化 shape 和数据
    Tensor4D(unsigned int const shape_[4], T const *data_) {
        unsigned int size = 1;
        // 复制 shape 并计算数据总大小
        for (int i = 0; i < 4; ++i) {
            shape[i] = shape_[i];
            size *= shape_[i];
        }
        data = new T[size];
        std::memcpy(data, data_, size * sizeof(T));
    }

    ~Tensor4D() {
        delete[] data;
    }

    // 禁止复制和移动
    Tensor4D(Tensor4D const &) = delete;
    Tensor4D(Tensor4D &&) noexcept = delete;

    // 计算多维数组的线性索引
    unsigned int index(unsigned int indices[4]) const {
        unsigned int idx = 0;
        unsigned int stride = 1;
        for (int i = 3; i >= 0; --i) {
            idx += indices[i] * stride;
            stride *= shape[i];  // 更新 stride
        }
        return idx;
    }

    // 运算符[]: 通过多维索引访问元素
    T &operator[](unsigned int indices[4]) {
        return data[index(indices)];
    }

    // 运算符+=: 实现单向广播的加法
    Tensor4D &operator+=(Tensor4D const &others) {
        // 广播加法的实现
        for (int d = 0; d < 4; ++d) {
            if (shape[d] != others.shape[d] && others.shape[d] != 1) {
                throw std::invalid_argument("Shapes are not broadcastable.");
            }
        }

        // 对于每个元素，进行加法操作
        unsigned int totalSize = 1;
        for (int i = 0; i < 4; ++i) {
            totalSize *= shape[i];  // 计算总元素个数
        }

        // 广播加法：按元素相加
        for (unsigned int i = 0; i < totalSize; ++i) {
            unsigned int indices[4];
            unsigned int tmp = i;

            for (int j = 3; j >= 0; --j) {
                indices[j] = tmp % shape[j];
                tmp /= shape[j];
            }

            unsigned int broadcastedIndex[4];
            for (int d = 0; d < 4; ++d) {
                broadcastedIndex[d] = (others.shape[d] == 1) ? 0 : indices[d];
            }

            data[i] += others.data[others.index(broadcastedIndex)];
        }

        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{1, 2, 3, 4};
        // clang-format off
        int data[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        auto t0 = Tensor4D<int>(shape, data);
        auto t1 = Tensor4D<int>(shape, data);
        t0 += t1;  // 进行加法
        for (auto i = 0u; i < sizeof(data) / sizeof(*data); ++i) {
            ASSERT(t0.data[i] == data[i] * 2, "Tensor doubled by plus its self.");
        }
    }

    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        float d0[]{
            1, 1, 1, 1,
            2, 2, 2, 2,
            3, 3, 3, 3,

            4, 4, 4, 4,
            5, 5, 5, 5,
            6, 6, 6, 6};
        // clang-format on
        unsigned int s1[]{1, 2, 3, 1};
        // clang-format off
        float d1[]{
            6,
            5,
            4,

            3,
            2,
            1};
        // clang-format on

        auto t0 = Tensor4D<float>(s0, d0);
        auto t1 = Tensor4D<float>(s1, d1);
        t0 += t1;  // 进行加法
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == 7.f, "Every element of t0 should be 7 after adding t1 to it.");
        }
    }

    {
        unsigned int s0[]{1, 2, 3, 4};
        // clang-format off
        double d0[]{
             1,  2,  3,  4,
             5,  6,  7,  8,
             9, 10, 11, 12,

            13, 14, 15, 16,
            17, 18, 19, 20,
            21, 22, 23, 24};
        // clang-format on
        unsigned int s1[]{1, 1, 1, 1};
        double d1[]{1};

        auto t0 = Tensor4D<double>(s0, d0);
        auto t1 = Tensor4D<double>(s1, d1);
        t0 += t1;  // 进行加法
        for (auto i = 0u; i < sizeof(d0) / sizeof(*d0); ++i) {
            ASSERT(t0.data[i] == d0[i] + 1, "Every element of t0 should be incremented by 1 after adding t1 to it.");
        }
    }
}

