#ifndef HEADER_H
#define HEADER_H

#include <array>
#include <string>
#include <cstdint>

class Long_uint {
public:
    static constexpr size_t word_bit = 32;
    static constexpr size_t max_bit = 2048;
    static constexpr size_t word = max_bit / word_bit;

    std::array<uint32_t, word> data{};

    Long_uint();
    Long_uint(uint32_t value);

    int cmp(const Long_uint& other) const;

    static Long_uint add(const Long_uint& a, const Long_uint& b);

    static Long_uint subtract(const Long_uint& a, const Long_uint& b);

    Long_uint bit_zsuv_l(size_t bits) const;
    Long_uint bit_zsuv_r(size_t bits) const;

    int bit_length() const;

    static Long_uint multiply(const Long_uint& a, const Long_uint& b);
    static Long_uint square(const Long_uint& a);

    static void divmod(const Long_uint& A, const Long_uint& B, Long_uint& Q, Long_uint& R);

    static Long_uint power(Long_uint base, Long_uint exp);

    std::string to_hex() const;
    static Long_uint from_hex(const std::string& str);


};

#endif 