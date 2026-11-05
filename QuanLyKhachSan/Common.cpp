#include "Common.h"
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>

long long Date::toDays() const {
    long long yy = y - (m <= 2);
    long long era = (yy >= 0 ? yy : yy - 399) / 400;
    long long yoe = yy - era * 400;
    long long doy = (153LL * (m + (m > 2 ? -3 : 9)) + 2) / 5 + d - 1;
    long long doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
    return era * 146097 + doe - 719468;
}

std::string Date::str() const {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(2) << d << '/' << std::setw(2) << m << '/' << y;
    return os.str();
}

bool Date::hopLe() const {
    if (y < 1900 || y > 2100 || m < 1 || m > 12 || d < 1) return false;
    static const int ngayTrongThang[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int toiDa = ngayTrongThang[m - 1];
    bool nhuan = (y % 4 == 0 && y % 100 != 0) || y % 400 == 0;
    if (m == 2 && nhuan) toiDa = 29;
    return d <= toiDa;
}

Date Date::homNay() {
    std::time_t t = std::time(nullptr);      // so giay tu 01/01/1970 den bay gio
    std::tm* lt = std::localtime(&t);        // doi sang ngay gio theo may
    return Date(lt->tm_mday, lt->tm_mon + 1, lt->tm_year + 1900);
}

long long operator-(const Date& a, const Date& b) { return a.toDays() - b.toDays(); }

std::string dinhDangTien(long long v) {
    std::string s = std::to_string(std::llabs(v)), r;
    int c = 0;
    for (int i = (int)s.size() - 1; i >= 0; --i) {
        r.insert(r.begin(), s[i]);
        if (++c % 3 == 0 && i > 0) r.insert(r.begin(), '.');
    }
    return (v < 0 ? "-" : "") + r + " VND";
}
