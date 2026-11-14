#include "LuuTru.h"
#include "KhachSanData.h"
#include <cmath>
#include <cstdio>
#include <fstream>
#include <functional>
#include <iostream>
#include <map>

namespace {

const char* F_DICHVU   = "dulieu_dichvu.txt";
const char* F_KHACH    = "dulieu_khach.txt";
const char* F_NHANVIEN = "dulieu_nhanvien.txt";
const char* F_PHONG    = "dulieu_phong.txt";
const char* F_LUOTTHUE = "dulieu_luotthue.txt";
const char* F_HOADON   = "dulieu_hoadon.txt";
const char* F_HETHONG  = "dulieu_hethong.txt";

using Cot = std::vector<std::string>;

// ---------- tien ich ghi ----------
std::string sach(std::string s) { // dam bao 1 truong khong chua ky tu phan cach
    for (char& c : s) if (c == '|' || c == '\n' || c == '\r') c = ' ';
    return s;
}

std::string so(double v) {
    char buf[64];
    std::snprintf(buf, sizeof buf, "%.10g", v);
    return buf;
}

std::string ghep(std::initializer_list<std::string> cot) {
    std::string r;
    bool dau = true;
    for (auto& c : cot) { if (!dau) r += '|'; r += sach(c); dau = false; }
    return r;
}

// ---------- tien ich doc ----------
Cot tach(const std::string& line) {
    Cot r;
    std::string cur;
    for (char c : line) {
        if (c == '|') { r.push_back(cur); cur.clear(); }
        else cur += c;
    }
    r.push_back(cur);
    return r;
}

bool toLL(const std::string& s, long long& v) {
    try { size_t p = 0; v = std::stoll(s, &p); return p == s.size(); } catch (...) { return false; }
}
bool toDbl(const std::string& s, double& v) {
    try { size_t p = 0; v = std::stod(s, &p); return p == s.size(); } catch (...) { return false; }
}
bool toDate(const std::string& s, Date& d) {
    int a, b, c;
    if (std::sscanf(s.c_str(), "%d/%d/%d", &a, &b, &c) != 3) return false;
    d = Date(a, b, c);
    return true;
}

// Doc tung dong cua file; f tra false neu dong loi (se bi bo qua va dem lai).
// File khong ton tai => coi nhu rong.
void docFile(const char* ten, int& soLoi, const std::function<bool(const Cot&)>& f) {
    std::ifstream in(ten);
    if (!in) return;
    std::string line;
    bool dau = true;
    while (std::getline(in, line)) {
        if (dau) { dau = false; if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3); } // bo BOM
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        if (!f(tach(line))) ++soLoi;
    }
}

// Trang thai phong luu bang chu de khong phu thuoc thu tu enum.
std::string tenTT(TinhTrangPhong t) { return tenTinhTrang(t); }

// Chap nhan ca ten (TRONG/DANG_SU_DUNG/BAO_TRI) lan so cu cua phien ban co DA_DAT
// (0=TRONG, 1=DA_DAT->TRONG, 2=DANG_SU_DUNG, 3=BAO_TRI).
bool docTT(const std::string& s, TinhTrangPhong& t) {
    if (s == "TRONG" || s == "DA_DAT") { t = TinhTrangPhong::TRONG; return true; }
    if (s == "DANG_SU_DUNG") { t = TinhTrangPhong::DANG_SU_DUNG; return true; }
    if (s == "BAO_TRI") { t = TinhTrangPhong::BAO_TRI; return true; }
    long long v;
    if (!toLL(s, v)) return false;
    if (v == 0 || v == 1) { t = TinhTrangPhong::TRONG; return true; }
    if (v == 2) { t = TinhTrangPhong::DANG_SU_DUNG; return true; }
    if (v == 3) { t = TinhTrangPhong::BAO_TRI; return true; }
    return false;
}

} // namespace

// =====================================================================
//  GHI
// =====================================================================
bool LuuTru::luu(const KhachSanData& a) {
    bool ok = true;
    auto mo = [&](const char* ten, const char* tieuDe, const std::function<void(std::ofstream&)>& ghi) {
        std::ofstream out(ten, std::ios::trunc);
        if (!out) { ok = false; return; }
        out << "# " << tieuDe << "\n";
        ghi(out);
        if (!out) ok = false;
    };

    mo(F_DICHVU, "loai(ANUONG|GIATUI|SPA|DUADON)|ma|ten|donGia|thamSo", [&](std::ofstream& o) {
        for (auto& d : a.dsDichVu)
            o << ghep({d->loaiDV(), d->getMa(), d->getTen(), std::to_string(d->getDonGia()), so(d->thamSo())}) << "\n";
    });

    mo(F_KHACH, "ma|hoTen|cccd|soDienThoai|email|diaChi|diemTichLuy", [&](std::ofstream& o) {
        for (auto& k : a.dsKhach)
            o << ghep({k->getMa(), k->getHoTen(), k->getCCCD(), k->getSDT(), k->getEmail(), k->getDiaChi(),
                       std::to_string(k->getDiem())}) << "\n";
    });

    mo(F_NHANVIEN, "ma|hoTen|cccd|soDienThoai|chucVu(0=LE_TAN,1=THU_NGAN,2=QUAN_LY,3=KE_TOAN)|ngayVaoLam|soHoaDonDaLap",
       [&](std::ofstream& o) {
        for (auto& n : a.dsNhanVien)
            o << ghep({n->getMa(), n->getHoTen(), n->getCCCD(), n->getSDT(),
                       std::to_string((int)n->getChucVu()), n->getNgayVaoLam().str(),
                       std::to_string(n->getSoHoaDonDaLap())}) << "\n";
    });

    mo(F_PHONG, "loai(DON|DOI|SUITE|VIP)|maPhong|giaCoBan|tinhTrang(TRONG|DANG_SU_DUNG|BAO_TRI)|ngayDuaVaoSuDung",
       [&](std::ofstream& o) {
        for (Phong* p : a.dsPhong.tatCa())
            o << ghep({p->maLoai(), p->getMa(), std::to_string(p->getGiaCoBan()),
                       tenTT(p->getTinhTrang()), p->getNgayDua().str()}) << "\n";
    });

    mo(F_LUOTTHUE, "LT|maLT|maPhong|maKH|ngayNhan|ngayTra|daTra(0/1)|donGiaChot   va   DV|maLT|maDV|soLuong|ngay|thanhTien",
       [&](std::ofstream& o) {
        for (Phong* p : a.dsPhong.tatCa())
            for (LuotThue* lt : p->getLichSu()) {
                o << ghep({"LT", lt->getMa(), lt->getMaPhong(), lt->getKhach()->getMa(), lt->getNgayNhan().str(),
                           lt->getNgayTra().str(), lt->daTra() ? "1" : "0", std::to_string(lt->getDonGiaChot())}) << "\n";
                for (auto& d : lt->getDichVu())
                    o << ghep({"DV", lt->getMa(), d.getDichVu()->getMa(), so(d.getSoLuong()),
                               d.getThoiDiem().str(), std::to_string(d.getThanhTien())}) << "\n";
            }
    });

    mo(F_HOADON,
       "HD|maHD|maLT|maNVLap|ngayLap|tienPhong|tienDichVu|giamGia|VAT|tongCong|daHuy(0/1)   "
       "DONG|maHD|moTa|soLuong|donGia|thanhTien   "
       "TT|maHD|maGD|soTien|hinhThuc(0..4)|ngay|maNVThu|soThamChieu|daHoan(0/1)",
       [&](std::ofstream& o) {
        for (HoaDon* h : a.qlHoaDon.tatCa()) {
            o << ghep({"HD", h->getMa(), h->getLuotThue()->getMa(), h->getNhanVienLap()->getMa(),
                       h->getNgayLap().str(), std::to_string(h->getTienPhong()), std::to_string(h->getTienDichVu()),
                       std::to_string(h->getGiamGia()), std::to_string(h->getThueVAT()),
                       std::to_string(h->getTongCong()),
                       h->getTrangThai() == TrangThaiHoaDon::DA_HUY ? "1" : "0"}) << "\n";
            for (auto& d : h->getChiTiet())
                o << ghep({"DONG", h->getMa(), d.getMoTa(), so(d.getSoLuong()),
                           std::to_string(d.getDonGia()), std::to_string(d.getThanhTien())}) << "\n";
            for (auto* t : h->getThanhToan())
                o << ghep({"TT", h->getMa(), t->getMa(), std::to_string(t->getSoTien()),
                           std::to_string((int)t->getHinhThuc()), t->getThoiDiem().str(),
                           t->getNhanVienThu()->getMa(), t->getSoThamChieu(), t->daHoan() ? "1" : "0"}) << "\n";
        }
    });

    // Ghi file he thong cuoi cung: no cung la dau hieu "du lieu da duoc khoi tao"
    mo(F_HETHONG, "DEM|demHoaDon|demGiaoDich   va   CONGDIEM|maHD (hoa don da cong diem thanh vien)",
       [&](std::ofstream& o) {
        o << ghep({"DEM", std::to_string(a.qlHoaDon.getDemHD()), std::to_string(a.qlHoaDon.getDemGD())}) << "\n";
        for (auto& ma : a.daCongDiem) o << ghep({"CONGDIEM", ma}) << "\n";
    });

    return ok;
}

// =====================================================================
//  DOC
// =====================================================================
bool LuuTru::tai(KhachSanData& a) {
    { std::ifstream t(F_HETHONG); if (!t) return false; } // chua tung luu => lan chay dau
    int loi = 0;

    // 1) dich vu
    docFile(F_DICHVU, loi, [&](const Cot& p) {
        long long gia; double th;
        if (p.size() < 5 || !toLL(p[3], gia) || !toDbl(p[4], th) || a.timDichVu(p[1])) return false;
        std::unique_ptr<DichVu> d;
        if (p[0] == "ANUONG")      d.reset(new DichVuAnUong(p[1], p[2], gia, th));
        else if (p[0] == "GIATUI") d.reset(new DichVuGiatUi(p[1], p[2], gia, std::llround(th)));
        else if (p[0] == "SPA")    d.reset(new DichVuSpa(p[1], p[2], gia, th));
        else if (p[0] == "DUADON") d.reset(new DichVuDuaDon(p[1], p[2], gia, std::llround(th)));
        else return false;
        a.dsDichVu.push_back(std::move(d));
        return true;
    });

    // 2) khach hang
    docFile(F_KHACH, loi, [&](const Cot& p) {
        long long diem;
        if (p.size() < 7 || p[0].empty() || !toLL(p[6], diem) || a.timKhach(p[0])) return false;
        auto k = std::make_unique<KhachHang>(p[0], p[1], p[2], p[3], p[4]);
        k->datDiaChi(p[5]);
        k->datDiem((int)diem);
        a.dsKhach.push_back(std::move(k));
        return true;
    });

    // 3) nhan vien
    docFile(F_NHANVIEN, loi, [&](const Cot& p) {
        long long cv, soHD; Date ngay;
        if (p.size() < 7 || p[0].empty() || !toLL(p[4], cv) || cv < 0 || cv > 3 || !toDate(p[5], ngay) ||
            !toLL(p[6], soHD) || a.timNhanVien(p[0])) return false;
        auto n = std::make_unique<NhanVien>(p[0], p[1], static_cast<ChucVu>(cv), ngay);
        n->khoiPhuc(p[2], p[3], (int)soHD);
        a.dsNhanVien.push_back(std::move(n));
        return true;
    });

    // 4) phong (trang thai ap dung sau khi da gan xong cac luot thue)
    struct ChoPhong { Phong* p; TinhTrangPhong tt; Date ngay; };
    std::vector<ChoPhong> cho;
    docFile(F_PHONG, loi, [&](const Cot& p) {
        long long gia; TinhTrangPhong tt; Date ngay;
        if (p.size() < 5 || !toLL(p[2], gia) || !docTT(p[3], tt) || !toDate(p[4], ngay))
            return false;
        Phong* ph = PhongFactory::taoPhong(p[0], p[1], gia);
        if (!ph) return false;
        if (!a.dsPhong.themCuoi(ph)) { delete ph; return false; }
        cho.push_back({ph, tt, ngay});
        return true;
    });
    a.dsPhong.sapXepTheoMa(true);

    // 5) luot thue + dich vu da dung
    std::map<std::string, LuotThue*> theoMaLT;
    docFile(F_LUOTTHUE, loi, [&](const Cot& p) {
        if (p.empty()) return false;
        if (p[0] == "LT") {
            long long tra, gia; Date nhan, ngayTra;
            if (p.size() < 8 || !toDate(p[4], nhan) || !toDate(p[5], ngayTra) || !toLL(p[6], tra) ||
                !toLL(p[7], gia)) return false;
            Phong* ph = a.dsPhong.timTheoMa(p[2]);
            KhachHang* kh = a.timKhach(p[3]);
            if (!ph || !kh || theoMaLT.count(p[1])) return false;
            auto* lt = new LuotThue(p[1], ph, p[2], kh, nhan, gia);
            lt->khoiPhucTra(ngayTra, tra == 1);
            ph->nhapLuotThue(lt);
            theoMaLT[p[1]] = lt;
            return true;
        }
        if (p[0] == "DV") {
            double sl; Date ngay; long long tien;
            if (p.size() < 6 || !toDbl(p[3], sl) || !toDate(p[4], ngay) || !toLL(p[5], tien)) return false;
            auto it = theoMaLT.find(p[1]);
            DichVu* dv = a.timDichVu(p[2]);
            if (it == theoMaLT.end() || !dv) return false;
            it->second->themDichVuDaCo(*dv, sl, ngay, tien);
            return true;
        }
        return false;
    });
    for (auto& c : cho) c.p->khoiPhuc(c.tt, c.ngay);

    // 6) hoa don, dong hoa don, thanh toan
    std::map<std::string, HoaDon*> theoMaHD;
    docFile(F_HOADON, loi, [&](const Cot& p) {
        if (p.empty()) return false;
        if (p[0] == "HD") {
            long long tp, tdv, giam, vat, tong, huy; Date ngay;
            if (p.size() < 11 || !toDate(p[4], ngay) || !toLL(p[5], tp) || !toLL(p[6], tdv) || !toLL(p[7], giam) ||
                !toLL(p[8], vat) || !toLL(p[9], tong) || !toLL(p[10], huy)) return false;
            auto it = theoMaLT.find(p[2]);
            NhanVien* nv = a.timNhanVien(p[3]);
            if (it == theoMaLT.end() || !nv || theoMaHD.count(p[1])) return false;
            auto* hd = new HoaDon(p[1], it->second, nv, ngay, tp, tdv, giam, vat, tong, huy == 1);
            a.qlHoaDon.nhapHoaDon(hd);
            theoMaHD[p[1]] = hd;
            return true;
        }
        auto it = theoMaHD.find(p.size() > 1 ? p[1] : "");
        if (it == theoMaHD.end()) return false;
        HoaDon* hd = it->second;
        if (p[0] == "DONG") {
            double sl; long long dg, tt;
            if (p.size() < 6 || !toDbl(p[3], sl) || !toLL(p[4], dg) || !toLL(p[5], tt)) return false;
            hd->themDongDaCo(DongHoaDon(p[2], sl, dg, tt));
            return true;
        }
        if (p[0] == "TT") {
            long long tien, ht, hoan; Date ngay;
            if (p.size() < 9 || !toLL(p[3], tien) || !toLL(p[4], ht) || ht < 0 || ht > 4 || !toDate(p[5], ngay) ||
                !toLL(p[8], hoan)) return false;
            NhanVien* nv = a.timNhanVien(p[6]);
            if (!nv) return false;
            auto* tt = new ThanhToanPhong(p[2], hd, tien, static_cast<HinhThucThanhToan>(ht), ngay, nv, p[7]);
            tt->khoiPhuc(true, hoan == 1);
            hd->nhapThanhToan(tt);
            return true;
        }
        return false;
    });

    // 7) he thong: bo dem ma, hoa don da cong diem
    docFile(F_HETHONG, loi, [&](const Cot& p) {
        if (p.empty()) return false;
        if (p[0] == "DEM") {
            long long h, g;
            if (p.size() < 3 || !toLL(p[1], h) || !toLL(p[2], g)) return false;
            a.qlHoaDon.datBoDem((int)h, (int)g);
            return true;
        }
        if (p[0] == "CONGDIEM" && p.size() >= 2) { a.daCongDiem.insert(p[1]); return true; }
        return false;
    });

    if (loi > 0) std::cerr << "Canh bao: bo qua " << loi << " dong du lieu loi khi doc file.\n";
    return true;
}
