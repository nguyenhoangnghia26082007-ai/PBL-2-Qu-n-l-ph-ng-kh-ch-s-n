#pragma once
#include <string>

struct Date {
    int d = 1, m = 1, y = 1970;
    Date() = default;
    Date(int d_, int m_, int y_) : d(d_), m(m_), y(y_) {}
    long long toDays() const;
    bool laMacDinh() const { return d == 1 && m == 1 && y == 1970; }
    bool operator==(const Date& o) const { return d == o.d && m == o.m && y == o.y; }
    std::string str() const;
    bool hopLe() const;          // ngay co ton tai that khong (vd 31/02 la sai)
    static Date homNay();        // ngay hien tai theo dong ho may
};
long long operator-(const Date& a, const Date& b);
std::string dinhDangTien(long long v);
