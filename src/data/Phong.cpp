#include "Phong.h"
#include <iostream>
#include <cmath>

std::string tenTinhTrang(TinhTrangPhong t) {
    switch (t) {
        case TinhTrangPhong::TRONG: return "TRONG";
        case TinhTrangPhong::DANG_SU_DUNG: return "DANG_SU_DUNG";
        default: return "BAO_TRI";
    }
}

// ---------- Phong ----------
Phong::Phong(std::string ma, int sc, long long gia, Date ngay)
    : maPhong(std::move(ma)), sucChua(sc), giaCoBan(gia), ngayDuaVaoSuDung(ngay) {}

Phong::~Phong() { for (auto* l : lichSu) delete l; }

void Phong::khoiPhuc(TinhTrangPhong t, Date ngayDua) {
    ngayDuaVaoSuDung = ngayDua;
    LuotThue* cuoi = getLuotThueCuoi();
    bool dangO = cuoi && !cuoi->daTra();
    if (dangO) t = TinhTrangPhong::DANG_SU_DUNG;                 // con luot thue chua dong => dang co khach
    else if (t == TinhTrangPhong::DANG_SU_DUNG) t = TinhTrangPhong::TRONG; // khong co khach thi khong the dang o
    tinhTrang = t;
}

bool Phong::nhanPhong(KhachHang* kh, Date ngay) {
    if (!kh) return false;
    if (tinhTrang != TinhTrangPhong::TRONG) return false;
    std::string ma = "LT" + maPhong + "-" + std::to_string(lichSu.size() + 1);
    lichSu.push_back(new LuotThue(ma, this, maPhong, kh, ngay, tinhDonGia()));
    tinhTrang = TinhTrangPhong::DANG_SU_DUNG;
    return true;
}

long long Phong::traPhong(Date ngay) {
    if (tinhTrang != TinhTrangPhong::DANG_SU_DUNG) return 0;
    long long tien = lichSu.back()->dong(ngay);
    tinhTrang = TinhTrangPhong::TRONG;
    return tien;
}

bool Phong::baoTri(bool bat) {
    if (bat) {
        if (tinhTrang != TinhTrangPhong::TRONG) return false;
        tinhTrang = TinhTrangPhong::BAO_TRI;
        return true;
    }
    if (tinhTrang != TinhTrangPhong::BAO_TRI) return false;
    tinhTrang = TinhTrangPhong::TRONG;
    return true;
}

bool Phong::themDichVu(const DichVu& dv, double soLuong) {
    if (tinhTrang != TinhTrangPhong::DANG_SU_DUNG) return false;
    return lichSu.back()->themDichVu(dv, soLuong);
}

LuotThue* Phong::getLuotThueHienTai() const {
    return tinhTrang == TinhTrangPhong::DANG_SU_DUNG ? lichSu.back() : nullptr;
}

void Phong::hienThi() const {
    std::cout << maPhong << " | " << layTenLoaiPhong() << " | " << sucChua << " nguoi | "
              << dinhDangTien(tinhDonGia()) << "/dem | " << tenTinhTrang(tinhTrang)
              << " | tu " << (ngayDuaVaoSuDung.laMacDinh() ? std::string("chua ro") : ngayDuaVaoSuDung.str()) << "\n";
}

// ---------- Cac loai phong ----------
PhongDon::PhongDon(std::string ma, long long gia, bool bc) : Phong(std::move(ma), 1, gia), coBanCong(bc) {}
long long PhongDon::tinhDonGia() const { return giaCoBan + (coBanCong ? 50000 : 0); }
std::string PhongDon::layTenLoaiPhong() const { return "Phong Don"; }

PhongDoi::PhongDoi(std::string ma, long long gia, std::string kg)
    : Phong(std::move(ma), 2, gia), kieuGiuong(std::move(kg)) {}
long long PhongDoi::tinhDonGia() const { return giaCoBan + (kieuGiuong == "King" ? 100000 : 0); }
std::string PhongDoi::layTenLoaiPhong() const { return "Phong Doi"; }

PhongSuite::PhongSuite(std::string ma, long long gia, int spn, double he)
    : Phong(std::move(ma), 4, gia), soPhongNho(spn), heSoSuite(he) {}
long long PhongSuite::tinhDonGia() const {
    return std::llround(giaCoBan * heSoSuite) + (long long)soPhongNho * 200000;
}
std::string PhongSuite::layTenLoaiPhong() const { return "Phong Suite"; }

PhongVIP::PhongVIP(std::string ma, long long gia, long long pp)
    : Phong(std::move(ma), 3, gia), phuPhiVIP(pp) {}
long long PhongVIP::tinhDonGia() const { return giaCoBan + phuPhiVIP; }
std::string PhongVIP::layTenLoaiPhong() const { return "Phong VIP"; }

// ---------- PhongFactory ----------
std::map<std::string, PhongFactory::Creator>& PhongFactory::registry() {
    static std::map<std::string, Creator> r;
    return r;
}

void PhongFactory::macDinh() {
    static bool done = false;
    if (done) return;
    done = true;
    auto& r = registry();
    r.emplace("DON",   [](const std::string& m, long long g) -> Phong* { return new PhongDon(m, g); });
    r.emplace("DOI",   [](const std::string& m, long long g) -> Phong* { return new PhongDoi(m, g); });
    r.emplace("SUITE", [](const std::string& m, long long g) -> Phong* { return new PhongSuite(m, g); });
    r.emplace("VIP",   [](const std::string& m, long long g) -> Phong* { return new PhongVIP(m, g); });
}

void PhongFactory::dangKy(const std::string& loai, Creator c) { registry()[loai] = std::move(c); }

Phong* PhongFactory::taoPhong(const std::string& loai, const std::string& ma, long long gia, Date ngayDua) {
    macDinh();
    auto it = registry().find(loai);
    if (it == registry().end()) return nullptr;
    Phong* p = it->second(ma, gia);
    if (p) p->datNgayDua(ngayDua);
    return p;
}

long long PhongFactory::giaCoBanMacDinh(const std::string& loai) {
    if (loai == "DON") return 400000;
    if (loai == "DOI") return 600000;
    if (loai == "SUITE") return 1200000;
    if (loai == "VIP") return 2000000;
    return 0;
}
