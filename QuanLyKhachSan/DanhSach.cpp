#include "DanhSach.h"

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
