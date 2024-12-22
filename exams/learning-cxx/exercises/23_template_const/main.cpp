#include "../exercise.h"
#include <cstring>
#include <iostream>

// READ: 模板非类型实参 <https://zh.cppreference.com/w/cpp/language/template_parameters#%E6%A8%A1%E6%9D%BF%E9%9D%9E%E7%B1%BB%E5%9E%8B%E5%AE%9E%E5%8F%82>

template<unsigned int N, class T>
struct Tensor {
    unsigned int shape[N];  // 存储每个维度的大小
    T *data;                // 存储数据

    // 构造函数: 初始化 shape，并分配数据内存
    Tensor(unsigned int const shape_[N]) {
        // 复制 shape 数组
        std::memcpy(shape, shape_, N * sizeof(unsigned int));

        // 计算总的元素个数
        unsigned int size = 1;
        for (unsigned int i = 0; i < N; ++i) {
            size *= shape[i];
        }

        // 为数据分配内存
        data = new T[size];
        std::memset(data, 0, size * sizeof(T));  // 初始化数据为 0
    }

    // 析构函数: 释放分配的内存
    ~Tensor() {
        delete[] data;
    }

    // 禁止复制和移动构造函数
    Tensor(Tensor const &) = delete;
    Tensor(Tensor &&) noexcept = delete;

    // 通过多维索引计算线性索引
    unsigned int index(unsigned int indices[N]) const {
        unsigned int idx = 0;
        unsigned int stride = 1;
        for (int i = N - 1; i >= 0; --i) {
            idx += indices[i] * stride;
            stride *= shape[i];  // 更新 stride
        }
        return idx;
    }

    // 运算符[]: 用来通过多维索引访问元素
    T &operator[](unsigned int indices[N]) {
        return data[index(indices)];
    }

    // 运算符+=: 对两个 Tensor 进行元素级别加法
    Tensor &operator+=(Tensor const &other) {
        // 假设两个 Tensor 的形状相同
        unsigned int totalSize = 1;
        for (unsigned int i = 0; i < N; ++i) {
            totalSize *= shape[i];  // 计算总元素个数
        }

        // 对每个元素执行加法操作
        for (unsigned int i = 0; i < totalSize; ++i) {
            data[i] += other.data[i];
        }
        return *this;
    }
};

// ---- 不要修改以下代码 ----
int main(int argc, char **argv) {
    {
        unsigned int shape[]{2, 3, 4, 5};
        auto tensor = Tensor<4, int>(shape);

        unsigned int i0[]{0, 0, 0, 0};
        tensor[i0] = 1;
        ASSERT(tensor[i0] == 1, "tensor[i0] should be 1");
        ASSERT(tensor.data[0] == 1, "tensor[i0] should be 1");

        unsigned int i1[]{1, 2, 3, 4};
        tensor[i1] = 2;
        ASSERT(tensor[i1] == 2, "tensor[i1] should be 2");
        ASSERT(tensor.data[119] == 2, "tensor[i1] should be 2");
    }
    {
        unsigned int shape[]{7, 8, 128};
        auto tensor = Tensor<3, float>(shape);

        unsigned int i0[]{0, 0, 0};
        tensor[i0] = 1.f;
        ASSERT(tensor[i0] == 1.f, "tensor[i0] should be 1");
        ASSERT(tensor.data[0] == 1.f, "tensor[i0] should be 1");

        unsigned int i1[]{3, 4, 99};
        tensor[i1] = 2.f;
        ASSERT(tensor[i1] == 2.f, "tensor[i1] should be 2");
        ASSERT(tensor.data[3683] == 2.f, "tensor[i1] should be 2");
    }
    return 0;
}
