#include "Header.h"

Long_uint::Long_uint() {
    data.fill(0);
}

Long_uint::Long_uint(uint32_t value) {
    data.fill(0);
    data[0] = value;
}

int Long_uint::cmp(const Long_uint& other) const {
    if (data.size() != other.data.size())
        return data.size() < other.data.size() ? -1 : 1;

    for (int i = (int)data.size() - 1; i >= 0; --i)
        if (data[i] != other.data[i])
            return data[i] < other.data[i] ? -1 : 1;

    return 0;
}


Long_uint Long_uint::add(const Long_uint& a, const Long_uint& b) {
    Long_uint c;
    uint64_t carry = 0;

    for (size_t i = 0; i < word; ++i) {
        uint64_t tmp = (uint64_t)a.data[i] + b.data[i] + carry;
        c.data[i] = (uint32_t)tmp;
        carry = tmp >> word_bit;
    }
    return c;
}

Long_uint Long_uint::subtract(const Long_uint& a, const Long_uint& b) {
    Long_uint c;
    int64_t borrow = 0;

    for (size_t i = 0; i < word; ++i) {
        int64_t tmp = (int64_t)a.data[i] - b.data[i] - borrow;
        if (tmp < 0) {
            tmp += (1LL << word_bit);
            borrow = 1;
        } else {
            borrow = 0;
        }
        c.data[i] = (uint32_t)tmp;
    }
    return c;
}

Long_uint Long_uint::bit_zsuv_l(size_t bits) const {
    Long_uint r;
    if (bits >= max_bit) return r; 
    size_t w = bits / word_bit;
    size_t b = bits % word_bit;
    
    for (size_t i = w; i < word; ++i) {
        size_t src_idx = i - w;
        r.data[i] = data[src_idx] << b;
        if (b && src_idx > 0) {
            r.data[i] |= data[src_idx - 1] >> (word_bit - b);
        }
    }
    return r;
}


Long_uint Long_uint::bit_zsuv_r(size_t bits) const {
    Long_uint r;
    size_t w = bits / word_bit;
    size_t b = bits % word_bit;

    for (size_t i = 0; i + w < word; ++i) {
        uint64_t part = data[i + w];
        if (b && i + w + 1 < word) part |= (uint64_t)data[i + w + 1] << word_bit;
        r.data[i] = (uint32_t)((part >> b) & 0xFFFFFFFFu);
    }
    return r;
}


int Long_uint::bit_length() const {
    for (int i = word - 1; i >= 0; --i) {
        if (data[i]) return i * word_bit + (31 - __builtin_clz(data[i]));
    }
    return 0;
}

Long_uint Long_uint::multiply(const Long_uint& a, const Long_uint& b) {
    Long_uint c;

    for (size_t i = 0; i < word; ++i) {
        uint64_t carry = 0;
        for (size_t j = 0; j + i < word; ++j) {
            uint64_t tmp = (uint64_t)a.data[i] * b.data[j] + c.data[i + j] + carry;
            c.data[i + j] = (uint32_t)tmp;
            carry = tmp >> word_bit;
        }
    }
    return c;
}

Long_uint Long_uint::square(const Long_uint& a) {
    return multiply(a, a);
}

void Long_uint::divmod(const Long_uint& A, const Long_uint& B, Long_uint& Q, Long_uint& R) {
    Q = Long_uint(0);
    R = Long_uint(0);

    if (B.is_zero()) return;

    for (int i = A.bit_length() - 1; i >= 0; --i) {
        R = R.bit_zsuv_l(1);
        if ((A.data[i / word_bit] >> (i % word_bit)) & 1) {
            R.data[0] |= 1; 
        }
        
        if (R.cmp(B) >= 0) {
            R = subtract(R, B);
            Q.data[i / word_bit] |= (1u << (i % word_bit));
        }
    }
}


Long_uint Long_uint::power(Long_uint base, Long_uint exp) {
    Long_uint result(1);

    while (!exp.is_zero()) {
        if (exp.is_odd()) result = multiply(result, base);
        base = square(base);
        exp = exp.bit_zsuv_r(1);
    }
    return result;
}


std::string Long_uint::to_hex() const {
    static const char* H = "0123456789ABCDEF";
    std::string s;
    for (int i = word - 1; i >= 0; --i) {
        for (int b = 28; b >= 0; b -= 4) {
            s.push_back(H[(data[i] >> b) & 0xF]);
        }
    }
    auto pos = s.find_first_not_of('0');
    return pos == std::string::npos ? "0" : s.substr(pos);
}

Long_uint Long_uint::from_hex(const std::string& str) {
    Long_uint x;
    for (char c : str) {
        uint32_t v = (c >= '0' && c <= '9') ? c - '0' : (c >= 'A' && c <= 'F') ? c - 'A' + 10 : (c >= 'a' && c <= 'f') ? c - 'a' + 10 : 0;
        x = x.bit_zsuv_l(4);
        x.data[0] |= v;
    }
    return x;
}

Long_uint Long_uint::gcd(Long_uint a, Long_uint b) {
    if (a.cmp(b) < 0) std::swap(a, b);
    Long_uint q, r;
    while (!b.is_zero()) {
        divmod(a, b, q, r);
        a = b;
        b = r;
    }
    return a;
}

Long_uint Long_uint::lcm(const Long_uint& a, const Long_uint& b) {
    Long_uint g = gcd(a, b);
    Long_uint q, r;
    divmod(multiply(a, b), g, q, r);
    return q;
}

Long_uint Long_uint::add_mod(const Long_uint& a, const Long_uint& b, const Long_uint& n) {
    Long_uint r = add(a, b);
    if (r.cmp(n) >= 0) r = subtract(r, n);
    return r;
}

Long_uint Long_uint::sub_mod(const Long_uint& a, const Long_uint& b, const Long_uint& n) {
    if (a.cmp(b) >= 0) return subtract(a, b);
    return subtract(add(a, n), b);
}

Long_uint Long_uint::mul_mod(const Long_uint& a, const Long_uint& b, const Long_uint& n, const Long_uint& m) {
    return barrett_reduce(multiply(a, b), n, m);
}

Long_uint Long_uint::sq_mod(const Long_uint& a, const Long_uint& n, const Long_uint& m) {
    return barrett_reduce(multiply(a, a), n, m);
}

Long_uint Long_uint::barrett_mu(const Long_uint& n) {
    int k = n.bit_length();
    Long_uint one(1);
    Long_uint b = one.bit_zsuv_l(2 * k);
    Long_uint q, r;
    Long_uint::divmod(b, n, q, r);
    return q; 
}

Long_uint Long_uint::barrett_reduce(const Long_uint& x, const Long_uint& n, const Long_uint& m) {
    int k = n.bit_length();
    Long_uint q = Long_uint::multiply(x, m);
    q = q.bit_zsuv_r(2 * k);
    Long_uint r = Long_uint::subtract(x, Long_uint::multiply(q, n));
    if (r.cmp(n) >= 0) r = Long_uint::subtract(r, n);
    if (r.cmp(n) >= 0) r = Long_uint::subtract(r, n);

    return r;
}

bool Long_uint::is_zero() const {
    for (uint32_t w : data) if (w != 0) return false;
    return true;
}

bool Long_uint::is_odd() const {
    return !data.empty() && (data[0] & 1);
}


Long_uint Long_uint::pow_mod(const Long_uint& a, const Long_uint& d,const Long_uint& n) {
    Long_uint mu = barrett_mu(n);
    Long_uint result(1);
    Long_uint base = barrett_reduce(a, n, mu);
    Long_uint exp = d;
    while (!exp.is_zero()) {
        if (exp.is_odd())  result = barrett_reduce(multiply(result, base), n, mu);
        base = barrett_reduce(multiply(base, base), n, mu);
        exp = exp.bit_zsuv_r(1);
    }
    return result;
}
