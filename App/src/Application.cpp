#include "Application.h"

#include "Entity.h"
#include "Core/DynamicArray.h"
#include <vector>
#include <iostream>
#include <chrono>

using namespace Composia;

void Application::Start()
{
    const size_t N = 10000000;

    // Benchmark std::vector
    std::vector<int> vec;
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i)
        vec.push_back(i);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "std::vector push_back: "
        << std::chrono::duration<double, std::milli>(end - start).count()
        << " ms\n";

    // Benchmark DynamicArray
    Composia::Core::DynamicArray<int> arr;
    start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < N; ++i)
        arr.PushBack(i);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "DynamicArray push_back: "
        << std::chrono::duration<double, std::milli>(end - start).count()
        << " ms\n";
    
}