#include "DanhSach.h"
#include <cctype>

namespace {
bool soSanhMaPhongTuNhien(const std::string& a, const std::string& b) {
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        unsigned char ca = (unsigned char)a[i];
        unsigned char cb = (unsigned char)b[j];
        if (std::isdigit(ca) && std::isdigit(cb)) {
            long long na = 0, nb = 0;
            while (i < a.size() && std::isdigit((unsigned char)a[i])) {
                na = na * 10 + (a[i] - '0');
                ++i;
            }
            while (j < b.size() && std::isdigit((unsigned char)b[j])) {
                nb = nb * 10 + (b[j] - '0');
                ++j;
            }
            if (na != nb) return na < nb;
        } else {
            char ua = (char)std::toupper(ca);
            char ub = (char)std::toupper(cb);
            if (ua != ub) return ua < ub;
            ++i;
            ++j;
        }
    }
    return a.size() < b.size();
}
} // namespace

DanhSachPhong::~DanhSachPhong() {
    dsPhong.duyet([](Phong* p) { delete p; });
}

Phong* DanhSachPhong::timTheoMa(const std::string& ma) const {
    return dsPhong.timNeu([&](Phong* p) { return p->getMa() == ma; });
}

bool DanhSachPhong::themDau(Phong* p) {
    if (!p || timTheoMa(p->getMa())) return false;
    dsPhong.themDau(p);
    return true;
}

bool DanhSachPhong::themCuoi(Phong* p) {
    if (!p || timTheoMa(p->getMa())) return false;
    dsPhong.themCuoi(p);
    return true;
}

bool DanhSachPhong::chenTaiViTri(int viTri, Phong* p) {
    if (!p || timTheoMa(p->getMa())) return false;
    return dsPhong.chenTaiViTri(viTri, p);
}

bool DanhSachPhong::xoaTheoMa(const std::string& ma) {
    Phong* p = timTheoMa(ma);
    if (!p || p->getTinhTrang() == TinhTrangPhong::DANG_SU_DUNG) return false;
    dsPhong.xoaNeu([&](Phong* x) { return x == p; });
    delete p;
    return true;
}

void DanhSachPhong::sapXepTheoMa(bool tangDan) {
    dsPhong.sapXep([&](Phong* a, Phong* b) {
        return tangDan ? soSanhMaPhongTuNhien(a->getMa(), b->getMa())
                       : soSanhMaPhongTuNhien(b->getMa(), a->getMa());
    });
}

void DanhSachPhong::sapXepTheoDonGia(bool tangDan) {
    dsPhong.sapXep([&](Phong* a, Phong* b) {
        return tangDan ? a->tinhDonGia() < b->tinhDonGia() : a->tinhDonGia() > b->tinhDonGia();
    });
}

void DanhSachPhong::hienThi() const {
    dsPhong.duyet([](Phong* p) { p->hienThi(); });
}

std::vector<Phong*> DanhSachPhong::tatCa() const {
    std::vector<Phong*> r;
    dsPhong.duyet([&](Phong* p) { r.push_back(p); });
    return r;
}
