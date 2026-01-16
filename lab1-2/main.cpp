#include <iostream>
#include <string>
#include <chrono>
#include <x86intrin.h> 
#include "Header.h"

void timer() {
    Long_uint a = Long_uint::from_hex("0123456789ABCDEF");
    Long_uint b = Long_uint::from_hex("0123456789ABCDEF");
    const int N = 10000;

    auto t1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) Long_uint::add(a, b);
    auto t2 = std::chrono::high_resolution_clock::now();

    auto t3 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) Long_uint::multiply(a, b);
    auto t4 = std::chrono::high_resolution_clock::now();

    Long_uint q, r;
    auto t5 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < N; ++i) Long_uint::divmod(a, b, q, r);
    auto t6 = std::chrono::high_resolution_clock::now();

    Long_uint exp(17);
    auto t7 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; ++i) Long_uint::power(a, exp);
    auto t8 = std::chrono::high_resolution_clock::now();

    auto add_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count() / N;
    auto mul_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count() / N;
    auto div_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t6 - t5).count() / N;
    auto pow_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(t8 - t7).count() / 100;

    std::cout << "\nTime:\n";
    std::cout << "Add: " << add_ns << " ns\n";
    std::cout << "Mul: " << mul_ns << " ns\n";
    std::cout << "Div: " << div_ns << " ns\n";
    std::cout << "Pow: " << pow_ns << " ns\n";
}

void timer_cycles() {
    Long_uint a = Long_uint::from_hex("0123456789ABCDEF");
    Long_uint b = Long_uint::from_hex("0123456789ABCDEF");

    const int N = 10000;
    Long_uint q, r;
    Long_uint exp(17);

    unsigned long long t1, t2;

    t1 = __rdtsc();
    for (int i = 0; i < N; ++i) Long_uint::add(a, b);
    t2 = __rdtsc();
    auto add_cycles = (t2 - t1) / N;

    t1 = __rdtsc();
    for (int i = 0; i < N; ++i) Long_uint::multiply(a, b);
    t2 = __rdtsc();
    auto mul_cycles = (t2 - t1) / N;

    t1 = __rdtsc();
    for (int i = 0; i < N; ++i)  Long_uint::divmod(a, b, q, r);
    t2 = __rdtsc();
    auto div_cycles = (t2 - t1) / N;

    t1 = __rdtsc();
    for (int i = 0; i < 100; ++i) Long_uint::power(a, exp);
    t2 = __rdtsc();
    auto pow_cycles = (t2 - t1) / 100;

    std::cout << "\nProcessor ticks:\n";
    std::cout << "Add: " << add_cycles << "\n";
    std::cout << "Mul: " << mul_cycles << "\n";
    std::cout << "Div: " << div_cycles << "\n";
    std::cout << "Pow: " << pow_cycles << "\n";
}



int main() {
    std::string sA, sB, sC, sX;
    
    std::cout << "Enter A (HEX): ";
    std::cin >> sA;

    std::cout << "Enter B (HEX): ";
    std::cin >> sB;

    std::cout << "Enter C (HEX): ";
    std::cin >> sC;

    std::cout << "Enter x (HEX): ";
    std::cin >> sX;

    Long_uint A = Long_uint::from_hex(sA);
    Long_uint B = Long_uint::from_hex(sB);
    Long_uint C = Long_uint::from_hex(sC);
    Long_uint x = Long_uint::from_hex(sX);

    Long_uint sum = Long_uint::add(A, B);
    Long_uint diff = Long_uint::subtract(A, B);
    Long_uint prod = Long_uint::multiply(A, B);

    Long_uint Q, R;
    Long_uint::divmod(A, B, Q, R);

    Long_uint sqrt = Long_uint::square(A);
    Long_uint pow = Long_uint::power(A, 2);

    Long_uint x_r = Long_uint::from_hex(sX);
    Long_uint x_l = Long_uint::from_hex(sX);

    Long_uint csum = Long_uint::multiply(C, sum);
    Long_uint sumc = Long_uint::multiply(sum, C);
    Long_uint ca = Long_uint::multiply(C, A);
    Long_uint cb = Long_uint::multiply(C, B);
    Long_uint ccsum = Long_uint::add(ca, cb);


    std::cout << "\nResults:\n";
    std::cout << "A + B = " << sum.to_hex() << "\n";
    std::cout << "A - B = " << diff.to_hex() << "\n";
    std::cout << "A * B = " << prod.to_hex() << "\n";
    std::cout << "A / B = " << Q.to_hex() << "\n";
    std::cout << "A % B = " << R.to_hex() << "\n";
    std::cout << "A * A = " << sqrt.to_hex() << "\n";
    std::cout << "A^2 = " << pow.to_hex() << "\n";
    std::cout << "x_r = " << x_r.bit_zsuv_r(4).to_hex() << "\n";
    std::cout << "x_l = " << x_l.bit_zsuv_l(4).to_hex() << "\n";
    std::cout << "C * (A + B) = " << csum.to_hex() << "\n";
    std::cout << "(A + B) * C = " << sumc.to_hex() << "\n";
    std::cout << "CA + CB = " << ccsum.to_hex() << "\n";

    timer();
    timer_cycles();

    return 0;
}
