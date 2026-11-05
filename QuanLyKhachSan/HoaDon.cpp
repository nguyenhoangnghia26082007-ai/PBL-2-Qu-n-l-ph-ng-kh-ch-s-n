#include "HoaDon.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

std::string tenTrangThaiHD(TrangThaiHoaDon t) {
    switch (t) {
        case TrangThaiHoaDon::CHUA_THANH_TOAN: return "CHUA_THANH_TOAN";
        case TrangThaiHoaDon::THANH_TOAN_MOT_PHAN: return "THANH_TOAN_MOT_PHAN";
        case TrangThaiHoaDon::DA_THANH_TOAN: return "DA_THANH_TOAN";
        default: return "DA_HUY";
    }
}

std::string tenHinhThuc(HinhThucThanhToan h) {
    switch (h) {
        case HinhThucThanhToan::TIEN_MAT: return "TIEN_MAT";
        case HinhThucThanhToan::CHUYEN_KHOAN: return "CHUYEN_KHOAN";
        case HinhThucThanhToan::THE: return "THE";
        case HinhThucThanhToan::VI_DIEN_TU: return "VI_DIEN_TU";
        default: return "QR";
    }
}

// ---------- DongHoaDon ----------
DongHoaDon::DongHoaDon(std::string mt, double sl, long long dg, long long tt)
    : moTa(std::move(mt)), soLuong(sl), donGia(dg), thanhTien(tt) {}

// ---------- ThanhToanPhong ----------
ThanhToanPhong::ThanhToanPhong(std::string ma, HoaDon* hd, long long tien, HinhThucThanhToan ht,
                               Date t, NhanVien* nv, std::string tc)
    : maGD(std::move(ma)), hoaDon(hd), soTien(tien), hinhThuc(ht), thoiDiem(t),
      nhanVienThu(nv), soThamChieu(std::move(tc)) {}

bool ThanhToanPhong::xuLy() {
    if (daXuLy_ || !hoaDon || !nhanVienThu || soTien <= 0) return false;
    if (hinhThuc != HinhThucThanhToan::TIEN_MAT && soThamChieu.empty()) return false;
    daXuLy_ = true;
    return true;
}

bool ThanhToanPhong::hoanTien() {
    if (!daXuLy_ || daHoan_) return false;
    daHoan_ = true;
    return true;
}

std::string ThanhToanPhong::inBienLai() const {
    std::ostringstream os;
    os << "BIEN LAI " << maGD << " | HD " << hoaDon->getMa() << " | " << thoiDiem.str() << "\n"
       << "  So tien : " << dinhDangTien(soTien) << (daHoan_ ? " (DA HOAN)" : "") << "\n"
       << "  Hinh thuc: " << tenHinhThuc(hinhThuc)
       << (soThamChieu.empty() ? "" : " | Ref: " + soThamChieu) << "\n"
       << "  Thu ngan: " << nhanVienThu->getHoTen() << "\n";
    return os.str();
}

// ---------- HoaDon ----------
HoaDon::HoaDon(std::string ma, LuotThue* lt, NhanVien* nv, Date ngay)
    : maHD(std::move(ma)), ngayLap(ngay), luotThue(lt), nhanVienLap(nv) {
    taoDongHoaDon();
    apDungGiamGia(lt->getKhach()->getHang());
    nv->ghiNhanHoaDon();
}

HoaDon::HoaDon(std::string ma, LuotThue* lt, NhanVien* nv, Date ngay, long long phong, long long dv,
               long long giam, long long vat, long long tong, bool huy)
    : maHD(std::move(ma)), ngayLap(ngay), luotThue(lt), nhanVienLap(nv), tienPhong(phong),
      tienDichVu(dv), giamGia(giam), thueVAT(vat), tongCong(tong), daHuy_(huy) {}

HoaDon::~HoaDon() { for (auto* t : thanhToan) delete t; }

void HoaDon::taoDongHoaDon() {
    chiTiet.clear();
    tienPhong = luotThue->tienPhong();
    tienDichVu = luotThue->tienDichVu();
    chiTiet.emplace_back("Tien phong " + luotThue->getMaPhong() + " (" +
                         std::to_string(luotThue->soDem()) + " dem)",
                         luotThue->soDem(), luotThue->getDonGiaChot(), tienPhong);
    for (auto& dv : luotThue->getDichVu()) {
        long long dg = dv.getSoLuong() > 0 ? std::llround(dv.getThanhTien() / dv.getSoLuong()) : 0;
        chiTiet.emplace_back(dv.getTenDichVu(), dv.getSoLuong(), dg, dv.getThanhTien());
    }
}

void HoaDon::apDungGiamGia(HangThanhVien hang) {
    giamGia = (tienPhong + tienDichVu) * tyLeGiamGia(hang) / 100;
    tinhTongCong();
}

long long HoaDon::tinhTongCong() {
    long long truocThue = tienPhong + tienDichVu - giamGia;
    thueVAT = std::llround(truocThue * 0.10);
    return tongCong = truocThue + thueVAT;
}

bool HoaDon::themThanhToan(ThanhToanPhong* tt) {
    if (!tt || daHuy_ || tt->getHoaDon() != this) return false;
    if (tt->getSoTien() > conPhaiTra()) return false;
    if (!tt->xuLy()) return false;
    thanhToan.push_back(tt);
    return true;
}

long long HoaDon::daThanhToanTien() const {
    long long s = 0;
    for (auto* t : thanhToan) s += t->getSoTienThuc();
    return s;
}

long long HoaDon::conPhaiTra() const { return std::max(0LL, tongCong - daThanhToanTien()); }

bool HoaDon::daTatToan() const { return !daHuy_ && conPhaiTra() == 0; }

bool HoaDon::huy() {
    if (daHuy_ || daThanhToanTien() > 0) return false;
    daHuy_ = true;
    return true;
}

TrangThaiHoaDon HoaDon::getTrangThai() const {
    if (daHuy_) return TrangThaiHoaDon::DA_HUY;
    long long paid = daThanhToanTien();
    if (paid >= tongCong) return TrangThaiHoaDon::DA_THANH_TOAN;
    return paid > 0 ? TrangThaiHoaDon::THANH_TOAN_MOT_PHAN : TrangThaiHoaDon::CHUA_THANH_TOAN;
}

void HoaDon::hienThi() const {
    std::cout << maHD << " | " << luotThue->getKhach()->getHoTen() << " | "
              << dinhDangTien(tongCong) << " | " << tenTrangThaiHD(getTrangThai())
              << " | NV lap: " << nhanVienLap->getHoTen() << "\n";
}

// ---------- QuanLyHoaDon ----------
QuanLyHoaDon::~QuanLyHoaDon() { dsHoaDon.duyet([](HoaDon* h) { delete h; }); }

std::string QuanLyHoaDon::soHieu(const char* tienTo, int n) {
    std::ostringstream os;
    os << tienTo << std::setfill('0') << std::setw(4) << n;
    return os.str();
}

std::vector<HoaDon*> QuanLyHoaDon::tatCa() const {
    std::vector<HoaDon*> r;
    dsHoaDon.duyet([&](HoaDon* h) { r.push_back(h); });
    return r;
}

HoaDon* QuanLyHoaDon::timTheoMa(const std::string& maHD) const {
    return dsHoaDon.timNeu([&](HoaDon* h) { return h->getMa() == maHD; });
}

HoaDon* QuanLyHoaDon::timTheoLuotThue(const LuotThue* lt) const {
    return dsHoaDon.timNeu([&](HoaDon* h) {
        return h->getLuotThue() == lt && h->getTrangThai() != TrangThaiHoaDon::DA_HUY;
    });
}

HoaDon* QuanLyHoaDon::taoHoaDon(LuotThue* lt, NhanVien* nv, Date ngay) {
    if (!lt || !nv || !lt->daTra()) return nullptr;
    if (timTheoLuotThue(lt)) return nullptr;
    auto* hd = new HoaDon(soHieu("HD", ++demHD), lt, nv, ngay);
    dsHoaDon.themCuoi(hd);
    return hd;
}

bool QuanLyHoaDon::thanhToan(const std::string& maHD, long long tien, HinhThucThanhToan ht,
                             NhanVien* thuNgan, Date ngay, const std::string& soThamChieu) {
    HoaDon* hd = timTheoMa(maHD);
    if (!hd) return false;
    if (!thuNgan || !thuNgan->coTheThuTien()) return false; // chi THU_NGAN / QUAN_LY duoc thu tien
    auto* tt = new ThanhToanPhong(soHieu("GD", ++demGD), hd, tien, ht, ngay, thuNgan, soThamChieu);
    if (!hd->themThanhToan(tt)) { delete tt; return false; }
    return true;
}

bool QuanLyHoaDon::huyHoaDon(const std::string& maHD, const NhanVien* nguoiHuy) {
    if (!nguoiHuy || !nguoiHuy->coTheHuyHoaDon()) return false; // chi QUAN_LY duoc huy
    HoaDon* hd = timTheoMa(maHD);
    return hd && hd->huy();
}

std::vector<HoaDon*> QuanLyHoaDon::timTheoKhach(const std::string& maKH) const {
    std::vector<HoaDon*> r;
    dsHoaDon.duyet([&](HoaDon* h) { if (h->getKhach()->getMa() == maKH) r.push_back(h); });
    return r;
}

std::vector<HoaDon*> QuanLyHoaDon::timTheoNhanVienLap(const std::string& maNV) const {
    std::vector<HoaDon*> r;
    dsHoaDon.duyet([&](HoaDon* h) { if (h->getNhanVienLap()->getMa() == maNV) r.push_back(h); });
    return r;
}

long long QuanLyHoaDon::tongDoanhThu() const {
    long long s = 0;
    dsHoaDon.duyet([&](HoaDon* h) { s += h->getDoanhThu(); });
    return s;
}

std::map<std::string, long long> QuanLyHoaDon::doanhThuTheoLoai() const {
    std::map<std::string, long long> m;
    dsHoaDon.duyet([&](HoaDon* h) {
        if (h->getTrangThai() == TrangThaiHoaDon::DA_HUY) return;
        m[h->getLuotThue()->getPhong()->layTenLoaiPhong()] += h->getDoanhThu();
    });
    return m;
}

long long QuanLyHoaDon::thucThuNgay(const Date& ngay) const {
    long long s = 0;
    dsHoaDon.duyet([&](HoaDon* h) {
        if (h->getTrangThai() == TrangThaiHoaDon::DA_HUY) return;
        for (auto* t : h->getThanhToan())
            if (t->getThoiDiem() == ngay) s += t->getSoTienThuc();
    });
    return s;
}

void QuanLyHoaDon::hienThi() const { dsHoaDon.duyet([](HoaDon* h) { h->hienThi(); }); }

// ---------- TrinhInHoaDon ----------
TrinhInHoaDon::TrinhInHoaDon(std::string ten, std::string dc, std::string mst, std::string kg)
    : tenKhachSan(std::move(ten)), diaChi(std::move(dc)), maSoThue(std::move(mst)), khoGiay(std::move(kg)) {}

std::string TrinhInHoaDon::dinhDangTien(long long v) { return ::dinhDangTien(v); }

std::string TrinhInHoaDon::inHoaDon(const HoaDon* hd) const {
    std::ostringstream os;
    const std::string line(60, '-');
    os << line << "\n" << tenKhachSan << "\n" << diaChi << " | MST: " << maSoThue
       << " | Kho: " << khoGiay << "\n" << line << "\n"
       << "HOA DON " << hd->getMa() << "   Ngay lap: " << hd->getNgayLap().str() << "\n"
       << "Khach hang : " << hd->getKhach()->getHoTen() << " (" << hd->getKhach()->getMa() << ")\n"
       << "Nhan vien  : " << hd->getNhanVienLap()->getHoTen() << "\n" << line << "\n";
    for (auto& d : hd->getChiTiet())
        os << std::left << std::setw(30) << d.getMoTa() << std::right << std::setw(8) << d.getSoLuong()
           << std::setw(20) << dinhDangTien(d.getThanhTien()) << "\n";
    os << line << "\n"
       << "Tam tinh : " << dinhDangTien(hd->getTienPhong() + hd->getTienDichVu()) << "\n"
       << "Giam gia : " << dinhDangTien(hd->getGiamGia()) << "\n"
       << "VAT 10%  : " << dinhDangTien(hd->getThueVAT()) << "\n"
       << "TONG CONG: " << dinhDangTien(hd->getTongCong()) << "\n"
       << "Da tra   : " << dinhDangTien(hd->daThanhToanTien()) << "\n"
       << "Con lai  : " << dinhDangTien(hd->conPhaiTra()) << "\n"
       << "Trang thai: " << tenTrangThaiHD(hd->getTrangThai()) << "\n" << line << "\n";
    return os.str();
}

bool TrinhInHoaDon::xuatPDF(const HoaDon* hd, const std::string& duongDan) const {
    std::ofstream f(duongDan);
    if (!f) return false;
    f << inHoaDon(hd);
    return true;
}

std::string TrinhInHoaDon::inBienLai(const ThanhToanPhong* tt) const {
    return tenKhachSan + "\n" + tt->inBienLai();
}
