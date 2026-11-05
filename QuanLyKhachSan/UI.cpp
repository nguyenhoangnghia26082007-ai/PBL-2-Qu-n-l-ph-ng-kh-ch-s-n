#include "UI.h"
#include "LuuTru.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <iostream>

using namespace std;

namespace {

string docDong(const string& nhac) {
    cout << nhac;
    string s;
    if (!getline(cin, s)) { cout << "\n"; exit(0); }
    size_t a = s.find_first_not_of(" \t\r"), b = s.find_last_not_of(" \t\r");
    return a == string::npos ? "" : s.substr(a, b - a + 1);
}

string docMa(const string& nhac) {
    string s = docDong(nhac);
    transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)toupper(c); });
    return s;
}

long long docSo(const string& nhac, long long lo, long long hi) {
    while (true) {
        string s = docDong(nhac);
        try {
            size_t pos = 0;
            long long v = stoll(s, &pos);
            if (pos == s.size() && v >= lo && v <= hi) return v;
        } catch (...) {}
        cout << "  Gia tri khong hop le (" << lo << " - " << hi << ").\n";
    }
}

double docThuc(const string& nhac) {
    while (true) {
        string s = docDong(nhac);
        try {
            size_t pos = 0;
            double v = stod(s, &pos);
            if (pos == s.size() && v > 0) return v;
        } catch (...) {}
        cout << "  Vui long nhap so duong.\n";
    }
}

Date docNgay(const string& nhac) {
    while (true) {
        string s = docDong(nhac + " (dd/mm/yyyy, Enter = hom nay): ");
        if (s.empty()) {                       // bam Enter => lay ngay hom nay
            Date h = Date::homNay();
            cout << "  -> " << h.str() << "\n";
            return h;
        }
        int d, m, y;
        if (sscanf(s.c_str(), "%d/%d/%d", &d, &m, &y) == 3 && Date(d, m, y).hopLe())
            return Date(d, m, y);
        cout << "  Ngay khong hop le.\n";
    }
}

void tieuDe(const string& t) { cout << "\n===== " << t << " =====\n"; }
void tamDung() { docDong("\nNhan Enter de tiep tuc..."); }

} // namespace

UngDung::UngDung() : trinhIn("KHACH SAN NHAN NGHIA", "Pleiku, Gia Lai", "0123456789") {
    // Co file du lieu thi doc lai; chua co (lan chay dau) thi nap du lieu mau roi tao file luon.
    if (LuuTru::tai(*this)) {
        cout << "Da doc du lieu tu file.\n";
    } else {
        duLieuMau();
        luu();
        cout << "Chua co file du lieu: da tao du lieu mau va ghi ra file.\n";
    }
}

void UngDung::luu() const {
    if (!LuuTru::luu(*this)) cerr << "Canh bao: khong ghi duoc file du lieu!\n";
}

KhachHang* UngDung::timKhach(const string& ma) const {
    for (auto& k : dsKhach) if (k->getMa() == ma) return k.get();
    return nullptr;
}
NhanVien* UngDung::timNhanVien(const string& ma) const {
    for (auto& n : dsNhanVien) if (n->getMa() == ma) return n.get();
    return nullptr;
}
DichVu* UngDung::timDichVu(const string& ma) const {
    for (auto& d : dsDichVu) if (d->getMa() == ma) return d.get();
    return nullptr;
}

void UngDung::duLieuMau() {
    Date homNay = Date::homNay();
    dsPhong.themCuoi(PhongFactory::taoPhong("DON", "P101", 400000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("DON", "P102", 400000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("DOI", "P103", 600000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("DOI", "P104", 650000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("SUITE", "P201", 1200000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("VIP", "P301", 2000000, homNay));

    dsKhach.push_back(make_unique<KhachHang>("KH01", "Do Thanh Nhan", "012345678901", "0905123456", "nhan@mail.com"));
    dsKhach.push_back(make_unique<KhachHang>("KH02", "Le Minh Anh", "098765432109", "0912345678", "anh@mail.com"));

    dsNhanVien.push_back(make_unique<NhanVien>("NV01", "Nguyen Hoang Nghia", ChucVu::LE_TAN, Date(1, 6, 2025)));
    dsNhanVien.push_back(make_unique<NhanVien>("NV02", "Tran Thu Ha", ChucVu::THU_NGAN, Date(1, 7, 2025)));
    dsNhanVien.push_back(make_unique<NhanVien>("NV03", "Pham Quang Ly", ChucVu::QUAN_LY, Date(1, 1, 2025)));

    dsDichVu.push_back(make_unique<DichVuAnUong>("DV01", "Buffet sang", 150000));
    dsDichVu.push_back(make_unique<DichVuGiatUi>("DV02", "Giat ui", 40000));
    dsDichVu.push_back(make_unique<DichVuSpa>("DV03", "Massage", 300000, 1.2));
    dsDichVu.push_back(make_unique<DichVuDuaDon>("DV04", "Don san bay", 15000));
}

void UngDung::chay() {
    while (true) {
        tieuDe("QUAN LY KHACH SAN");
        cout << "1. Quan ly phong\n2. Nhan phong / Dich vu / Tra phong\n3. Khach hang\n"
                "4. Nhan vien\n5. Hoa don & Thanh toan\n6. Thong ke\n0. Thoat\n";
        switch (docSo("Chon: ", 0, 6)) {
            case 1: menuPhong(); break;
            case 2: menuLuuTru(); break;
            case 3: menuKhachHang(); break;
            case 4: menuNhanVien(); break;
            case 5: menuHoaDon(); break;
            case 6: menuThongKe(); break;
            default: luu(); cout << "Tam biet!\n"; return;
        }
    }
}

// ---------------- Phong ----------------
void UngDung::menuPhong() {
    while (true) {
        tieuDe("QUAN LY PHONG");
        cout << "1. Danh sach phong\n2. Them phong\n3. Xoa phong\n4. Sap xep theo don gia\n"
                "5. Bat/tat bao tri\n0. Quay lai\n";
        switch (docSo("Chon: ", 0, 5)) {
            case 1: dsPhong.hienThi(); tamDung(); break;
            case 2: {
                cout << "Loai phong: DON / DOI / SUITE / VIP\n";
                string loai = docMa("Loai: ");
                string ma = docMa("Ma phong: ");
                long long gia = docSo("Gia co ban (VND/dem): ", 1, 1000000000LL);
                Date ngayDua = docNgay("Ngay dua vao su dung");
                Phong* p = PhongFactory::taoPhong(loai, ma, gia, ngayDua);
                if (!p) cout << "Loai phong khong hop le.\n";
                else if (!dsPhong.themCuoi(p)) { cout << "Ma phong da ton tai.\n"; delete p; }
                else cout << "Da them phong " << ma << ".\n";
                tamDung(); break;
            }
            case 3: {
                string ma = docMa("Ma phong can xoa: ");
                bool coHoaDon = false;
                for (auto* h : qlHoaDon.tatCa()) if (h->getLuotThue()->getMaPhong() == ma) coHoaDon = true;
                if (coHoaDon) cout << "Khong xoa duoc: phong da co hoa don.\n";
                else cout << (dsPhong.xoaTheoMa(ma) ? "Da xoa.\n" : "Khong xoa duoc (khong ton tai hoac dang co khach).\n");
                tamDung(); break;
            }
            case 4: {
                bool tang = docSo("1. Tang dan  2. Giam dan: ", 1, 2) == 1;
                dsPhong.sapXepTheoDonGia(tang);
                dsPhong.hienThi(); tamDung(); break;
            }
            case 5: {
                Phong* p = dsPhong.timTheoMa(docMa("Ma phong: "));
                if (!p) { cout << "Khong tim thay phong.\n"; tamDung(); break; }
                bool bat = docSo("1. Bat bao tri  2. Tat bao tri: ", 1, 2) == 1;
                cout << (p->baoTri(bat) ? "Da cap nhat.\n" : "Khong the doi trang thai luc nay.\n");
                tamDung(); break;
            }
            default: return;
        }
        luu(); // moi lan them/sua/xoa deu ghi lai vao file
    }
}

// ---------------- Luu tru ----------------
void UngDung::menuLuuTru() {
    while (true) {
        tieuDe("NHAN PHONG / DICH VU / TRA PHONG");
        cout << "1. Nhan phong\n2. Them dich vu cho phong dang o\n3. Tra phong\n"
                "4. Danh muc dich vu\n0. Quay lai\n";
        switch (docSo("Chon: ", 0, 4)) {
            case 1: nhanPhong(); break;
            case 2: themDichVuChoPhong(); break;
            case 3: traPhong(); break;
            case 4: for (auto& d : dsDichVu) d->hienThi(); tamDung(); break;
            default: return;
        }
        luu(); // moi lan them/sua/xoa deu ghi lai vao file
    }
}

void UngDung::nhanPhong() {
    dsPhong.hienThi();
    Phong* p = dsPhong.timTheoMa(docMa("Ma phong: "));
    if (!p) { cout << "Khong tim thay phong.\n"; tamDung(); return; }
    KhachHang* kh = timKhach(docMa("Ma khach hang: "));
    if (!kh) { cout << "Khong tim thay khach hang (them o muc Khach hang).\n"; tamDung(); return; }
    Date ngay = docNgay("Ngay nhan");
    cout << (p->nhanPhong(kh, ngay) ? "Nhan phong thanh cong.\n" : "Phong khong o trang thai TRONG.\n");
    tamDung();
}

void UngDung::themDichVuChoPhong() {
    Phong* p = dsPhong.timTheoMa(docMa("Ma phong: "));
    if (!p || !p->getLuotThueHienTai()) { cout << "Phong khong ton tai hoac chua co khach.\n"; tamDung(); return; }
    for (auto& d : dsDichVu) d->hienThi();
    DichVu* dv = timDichVu(docMa("Ma dich vu: "));
    if (!dv) { cout << "Khong tim thay dich vu.\n"; tamDung(); return; }
    double sl = docThuc("So luong: ");
    if (p->themDichVu(*dv, sl))
        cout << "Da them: " << dinhDangTien(p->getLuotThueHienTai()->getDichVu().back().getThanhTien()) << "\n";
    else cout << "Khong them duoc.\n";
    tamDung();
}

void UngDung::traPhong() {
    Phong* p = dsPhong.timTheoMa(docMa("Ma phong: "));
    if (!p || !p->getLuotThueHienTai()) { cout << "Phong khong ton tai hoac chua co khach.\n"; tamDung(); return; }
    Date ngay = docNgay("Ngay tra");
    if (ngay - p->getLuotThueHienTai()->getNgayNhan() < 0)
        cout << "Ngay tra truoc ngay nhan, he thong tinh toi thieu 1 dem.\n";
    LuotThue* lt = p->getLuotThueHienTai();
    long long tien = p->traPhong(ngay);
    cout << "Da tra phong. " << lt->soDem() << " dem, tien phong " << dinhDangTien(tien)
         << ", dich vu " << dinhDangTien(lt->tienDichVu()) << ".\n";
    // Tra phong xong lap hoa don luon de khong bi bo sot luot thue chua co hoa don.
    if (docSo("Lap hoa don ngay bay gio? (1. Co  2. De sau): ", 1, 2) == 1) {
        lapHoaDonChoLuot(lt);
    } else {
        cout << "Nho vao muc Hoa don & Thanh toan de lap hoa don sau.\n";
    }
    tamDung();
}

// ---------------- Khach hang ----------------
void UngDung::menuKhachHang() {
    while (true) {
        tieuDe("KHACH HANG");
        cout << "1. Danh sach\n2. Them khach hang\n3. Cap nhat thong tin\n0. Quay lai\n";
        switch (docSo("Chon: ", 0, 3)) {
            case 1: for (auto& k : dsKhach) k->hienThi(); tamDung(); break;
            case 2: {
                string ma = docMa("Ma KH: ");
                if (ma.empty() || timKhach(ma)) { cout << "Ma rong hoac da ton tai.\n"; tamDung(); break; }
                string ten = docDong("Ho ten: "), cccd = docDong("CCCD: "),
                       sdt = docDong("So dien thoai: "), email = docDong("Email: ");
                if (ten.empty()) { cout << "Ho ten khong duoc rong.\n"; tamDung(); break; }
                dsKhach.push_back(make_unique<KhachHang>(ma, ten, cccd, sdt, email));
                cout << "Da them.\n"; tamDung(); break;
            }
            case 3: {
                KhachHang* kh = timKhach(docMa("Ma KH: "));
                if (!kh) { cout << "Khong tim thay.\n"; tamDung(); break; }
                string ten = docDong("Ho ten moi: "), sdt = docDong("SDT moi: "), email = docDong("Email moi: ");
                cout << (kh->capNhatThongTin(ten, sdt, email) ? "Da cap nhat.\n" : "Ho ten/SDT khong duoc rong.\n");
                tamDung(); break;
            }
            default: return;
        }
        luu(); // moi lan them/sua/xoa deu ghi lai vao file
    }
}

// ---------------- Nhan vien ----------------
void UngDung::menuNhanVien() {
    while (true) {
        tieuDe("NHAN VIEN");
        cout << "1. Danh sach\n2. Them nhan vien\n0. Quay lai\n";
        switch (docSo("Chon: ", 0, 2)) {
            case 1: for (auto& n : dsNhanVien) n->hienThi(); tamDung(); break;
            case 2: {
                string ma = docMa("Ma NV: ");
                if (ma.empty() || timNhanVien(ma)) { cout << "Ma rong hoac da ton tai.\n"; tamDung(); break; }
                string ten = docDong("Ho ten: ");
                if (ten.empty()) { cout << "Ho ten khong duoc rong.\n"; tamDung(); break; }
                cout << "1. LE_TAN  2. THU_NGAN  3. QUAN_LY  4. KE_TOAN\n";
                ChucVu cv = static_cast<ChucVu>(docSo("Chuc vu: ", 1, 4) - 1);
                Date ngay = docNgay("Ngay vao lam");
                dsNhanVien.push_back(make_unique<NhanVien>(ma, ten, cv, ngay));
                cout << "Da them.\n"; tamDung(); break;
            }
            default: return;
        }
        luu(); // moi lan them/sua/xoa deu ghi lai vao file
    }
}

// ---------------- Hoa don ----------------
void UngDung::menuHoaDon() {
    while (true) {
        tieuDe("HOA DON & THANH TOAN");
        cout << "1. Lap hoa don\n2. Thanh toan\n3. Huy hoa don\n4. In hoa don\n"
                "5. Danh sach hoa don\n6. In bien lai\n0. Quay lai\n";
        switch (docSo("Chon: ", 0, 6)) {
            case 1: lapHoaDon(); break;
            case 2: thanhToan(); break;
            case 3: {
                string ma = docMa("Ma hoa don: ");
                NhanVien* nv = timNhanVien(docMa("Ma nhan vien thuc hien (QUAN_LY): "));
                if (!nv) cout << "Khong tim thay nhan vien.\n";
                else if (!nv->coTheHuyHoaDon())
                    cout << "Khong co quyen: " << nv->getHoTen() << " la " << tenChucVu(nv->getChucVu())
                         << ", chi QUAN_LY moi duoc huy hoa don.\n";
                else cout << (qlHoaDon.huyHoaDon(ma, nv) ? "Da huy.\n"
                                                         : "Khong huy duoc (khong ton tai hoac da co thanh toan).\n");
                tamDung(); break;
            }
            case 4: inHoaDon(); break;
            case 5: qlHoaDon.hienThi(); tamDung(); break;
            case 6: inBienLai(); break;
            default: return;
        }
        luu(); // moi lan them/sua/xoa deu ghi lai vao file
    }
}

void UngDung::lapHoaDon() {
    Phong* p = dsPhong.timTheoMa(docMa("Ma phong da tra: "));
    LuotThue* lt = p ? p->getLuotThueCuoi() : nullptr;
    if (!lt || !lt->daTra()) { cout << "Phong khong ton tai hoac chua tra phong.\n"; tamDung(); return; }
    lapHoaDonChoLuot(lt);
    tamDung();
}

bool UngDung::lapHoaDonChoLuot(LuotThue* lt) {
    NhanVien* nv = timNhanVien(docMa("Ma nhan vien lap hoa don: "));
    if (!nv) { cout << "Khong tim thay nhan vien.\n"; return false; }
    Date ngay = docNgay("Ngay lap");
    HoaDon* hd = qlHoaDon.taoHoaDon(lt, nv, ngay);
    if (!hd) { cout << "Luot thue nay da co hoa don.\n"; return false; }
    cout << trinhIn.inHoaDon(hd);
    cout << "Ma hoa don: " << hd->getMa() << " (vao muc Thanh toan de thu tien).\n";
    return true;
}

void UngDung::thanhToan() {
    HoaDon* hd = qlHoaDon.timTheoMa(docMa("Ma hoa don: "));
    if (!hd) { cout << "Khong tim thay hoa don.\n"; tamDung(); return; }
    if (hd->getTrangThai() == TrangThaiHoaDon::DA_HUY || hd->conPhaiTra() == 0) {
        cout << "Hoa don da huy hoac da thanh toan du.\n"; tamDung(); return;
    }
    cout << "Con phai tra: " << dinhDangTien(hd->conPhaiTra()) << "\n";
    long long tien = docSo("So tien thanh toan: ", 1, hd->conPhaiTra());
    cout << "1. TIEN_MAT  2. CHUYEN_KHOAN  3. THE  4. VI_DIEN_TU  5. QR\n";
    HinhThucThanhToan ht = static_cast<HinhThucThanhToan>(docSo("Hinh thuc: ", 1, 5) - 1);
    NhanVien* nv = timNhanVien(docMa("Ma nhan vien thu ngan: "));
    if (!nv) { cout << "Khong tim thay nhan vien.\n"; tamDung(); return; }
    if (!nv->coTheThuTien()) {
        cout << "Khong co quyen: " << nv->getHoTen() << " la " << tenChucVu(nv->getChucVu())
             << ", chi THU_NGAN hoac QUAN_LY moi duoc thu tien.\n";
        tamDung(); return;
    }
    string ref = ht == HinhThucThanhToan::TIEN_MAT ? "" : docDong("So tham chieu giao dich: ");
    Date ngay = docNgay("Ngay thanh toan");
    if (!qlHoaDon.thanhToan(hd->getMa(), tien, ht, nv, ngay, ref)) {
        cout << "Thanh toan that bai (thieu so tham chieu hoac so tien khong hop le).\n";
    } else {
        cout << trinhIn.inBienLai(hd->getThanhToan().back());
        if (hd->daTatToan() && daCongDiem.insert(hd->getMa()).second) {
            int diem = (int)(hd->getTongCong() / 100000);
            hd->getKhach()->congDiem(diem);
            cout << "Hoa don da thanh toan du. Cong " << diem << " diem cho khach (hang: "
                 << tenHang(hd->getKhach()->getHang()) << ").\n";
        }
    }
    tamDung();
}

void UngDung::inHoaDon() {
    HoaDon* hd = qlHoaDon.timTheoMa(docMa("Ma hoa don: "));
    if (!hd) { cout << "Khong tim thay hoa don.\n"; tamDung(); return; }
    cout << trinhIn.inHoaDon(hd);
    string file = "hoadon_" + hd->getMa() + ".txt";
    if (docSo("Luu ra file " + file + "? (1. Co  2. Khong): ", 1, 2) == 1)
        cout << (trinhIn.xuatPDF(hd, file) ? "Da luu.\n" : "Khong ghi duoc file.\n");
    tamDung();
}

void UngDung::inBienLai() {
    HoaDon* hd = qlHoaDon.timTheoMa(docMa("Ma hoa don: "));
    if (!hd) { cout << "Khong tim thay hoa don.\n"; tamDung(); return; }
    if (hd->getThanhToan().empty()) cout << "Hoa don chua co thanh toan nao.\n";
    for (auto* t : hd->getThanhToan()) cout << trinhIn.inBienLai(t);
    tamDung();
}

// ---------------- Thong ke ----------------
// Dinh nghia (thong nhat trong toan he thong):
//  - DOANH THU = tien phong + dich vu - giam gia cua cac hoa don chua huy (chua VAT).
//  - THUC THU  = tien da thu (gom VAT, tru phan hoan) theo ngay thu tien.
void UngDung::menuThongKe() {
    while (true) {
        tieuDe("THONG KE");
        cout << "1. Tong doanh thu (theo hoa don)\n2. Doanh thu theo loai phong\n"
                "3. Tien thuc thu theo ngay\n4. Hoa don theo nhan vien\n0. Quay lai\n";
        switch (docSo("Chon: ", 0, 4)) {
            case 1: {
                int chuaLap = 0;
                for (Phong* p : dsPhong.tatCa())
                    for (LuotThue* lt : p->getLichSu())
                        if (lt->daTra() && !qlHoaDon.timTheoLuotThue(lt)) ++chuaLap;
                cout << "Tong doanh thu (sau giam gia, chua VAT, hoa don chua huy): "
                     << dinhDangTien(qlHoaDon.tongDoanhThu()) << "\n";
                if (chuaLap > 0)
                    cout << "Luu y: con " << chuaLap << " luot thue da tra phong nhung chua co hoa don hieu luc, "
                            "chua tinh vao doanh thu.\n";
                tamDung(); break;
            }
            case 2:
                for (auto& [loai, dt] : qlHoaDon.doanhThuTheoLoai()) cout << "  " << loai << ": " << dinhDangTien(dt) << "\n";
                tamDung(); break;
            case 3: {
                Date ngay = docNgay("Ngay");
                cout << "Thuc thu (gom VAT, tru hoan): " << dinhDangTien(qlHoaDon.thucThuNgay(ngay)) << "\n";
                tamDung(); break;
            }
            case 4: {
                NhanVien* nv = timNhanVien(docMa("Ma nhan vien: "));
                if (!nv) { cout << "Khong tim thay nhan vien.\n"; tamDung(); break; }
                auto ds = qlHoaDon.timTheoNhanVienLap(nv->getMa());
                cout << nv->getHoTen() << " da lap " << ds.size() << " hoa don.\n";
                for (auto* h : ds) h->hienThi();
                tamDung(); break;
            }
            default: return;
        }
    }
}
