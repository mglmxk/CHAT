#include "../includes/ring_buffer.hpp"
#include <iostream>
#include <vector>
#include <deque>
#include <list>
#include <chrono>
#include <string>
#include <map>
#include <unordered_map>
#include <random>

using Clock = std::chrono::high_resolution_clock;
using Ms = std::chrono::milliseconds;

void benchmark_history_containers() {
    std::cout << "=== Бенчмарк 1: История (добавление + вытеснение + чтение) ===\n";
    
    constexpr size_t CAPACITY = 1000;
    constexpr size_t ITERATIONS = 100000;
    
    {
        RingBuffer<int> rb(CAPACITY);
        auto start = Clock::now();
        for (size_t i = 0; i < ITERATIONS; ++i) {
            rb.push_back(i);
        }
        auto end = Clock::now();
        std::cout << "RingBuffer (vector): " 
                  << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
        
        size_t sum = 0;
        for (size_t i = 0; i < 100; ++i) {
            sum += rb[CAPACITY - 1 - i];
        }
        std::cout << "  Sum of last 100: " << sum << "\n";
    }
    
    {
        std::deque<int> dq;
        auto start = Clock::now();
        for (size_t i = 0; i < ITERATIONS; ++i) {
            dq.push_back(i);
            if (dq.size() > CAPACITY) dq.pop_front();
        }
        auto end = Clock::now();
        std::cout << "std::deque: " 
                  << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
    }
    
    {
        std::list<int> lst;
        auto start = Clock::now();
        for (size_t i = 0; i < ITERATIONS; ++i) {
            lst.push_back(i);
            if (lst.size() > CAPACITY) lst.pop_front();
        }
        auto end = Clock::now();
        std::cout << "std::list: " 
                  << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
    }
    
    std::cout << "\n";
}

void benchmark_map_containers() {
    std::cout << "=== Бенчмарк 2: map vs unordered_map ===\n";
    
    for (size_t size : {1000, 100000, 1000000}) {
        std::cout << "--- Размер: " << size << " записей ---\n";
        
        {
            std::map<std::string, int> m;
            auto start = Clock::now();
            for (size_t i = 0; i < size; ++i) {
                m["key_" + std::to_string(i)] = i;
            }
            auto end = Clock::now();
            std::cout << "map вставка: " 
                      << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
            
            start = Clock::now();
            for (size_t i = 0; i < size; ++i) {
                auto it = m.find("key_" + std::to_string(i));
                (void)it;
            }
            end = Clock::now();
            std::cout << "map поиск: " 
                      << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
            
            start = Clock::now();
            int sum = 0;
            for (const auto& [k, v] : m) sum += v;
            end = Clock::now();
            std::cout << "map обход: " 
                      << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
        }
        
        {
            std::unordered_map<std::string, int> m;
            auto start = Clock::now();
            for (size_t i = 0; i < size; ++i) {
                m["key_" + std::to_string(i)] = i;
            }
            auto end = Clock::now();
            std::cout << "unordered_map вставка: " 
                      << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
            
            start = Clock::now();
            for (size_t i = 0; i < size; ++i) {
                auto it = m.find("key_" + std::to_string(i));
                (void)it;
            }
            end = Clock::now();
            std::cout << "unordered_map поиск: " 
                      << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
            
            start = Clock::now();
            int sum = 0;
            for (const auto& [k, v] : m) sum += v;
            end = Clock::now();
            std::cout << "unordered_map обход: " 
                      << std::chrono::duration_cast<Ms>(end - start).count() << " ms\n";
        }
        
        std::cout << "\n";
    }
}

int main() {
    benchmark_history_containers();
    benchmark_map_containers();
    return 0;
}