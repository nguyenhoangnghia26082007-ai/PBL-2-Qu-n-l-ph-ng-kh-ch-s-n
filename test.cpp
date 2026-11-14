#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>
#include <chrono>
#include <thread>
#include <cmath>

#include "Common.h"
#include "DichVu.h"
#include "Phong.h"
#include "KhachHang.h"
#include "NhanVien.h"
#include "LuotThue.h"
#include "HoaDon.h"
#include "DanhSach.h"
#include "KhachSanData.h"
#include "LuuTru.h"
#include "TaiKhoan.h"
#include "PhanQuyen.h"
#include "AuthService.h"
#include "KhachSanService.h"

int g_passed = 0;
int g_failed = 0;

void kiemTra(bool dk, const std::string& tenCase, const std::string& moTaLoi = "") {
    if (dk) {
        ++g_passed;
        std::cout << "  [PASS] " << tenCase << "\n";
    } else {
        ++g_failed;
        std::cout << "  [FAIL] " << tenCase << " -> " << moTaLoi << "\n";
    }
}

// =========================================================================
// TEST 1: Date, Tinh ngay, Dinh dang tien
// =========================================================================
void testDateVaTien() {
    std::cout << "\n=== TEST NHOM 1: Date, Tinh ngay, Dinh dang tien ===\n";
    
    Date dMacDinh;
    kiemTra(dMacDinh.laMacDinh() && dMacDinh.d == 1 && dMacDinh.m == 1 && dMacDinh.y == 1970,
            "1.1 Date mac dinh phai la 01/01/1970");

    kiemTra(Date(29, 2, 2024).hopLe(), "1.2.1 29/02/2024 (nam nhuan) la hop le");
    kiemTra(Date(29, 2, 2000).hopLe(), "1.2.2 29/02/2000 (chia het 400) la hop le");
    kiemTra(!Date(29, 2, 1900).hopLe(), "1.2.3 29/02/1900 (khong chia het 400) khong hop le");
    kiemTra(!Date(29, 2, 2100).hopLe(), "1.2.4 29/02/2100 (khong chia het 400) khong hop le");
    kiemTra(!Date(29, 2, 2023).hopLe(), "1.2.5 29/02/2023 khong phai nam nhuan");

    kiemTra(!Date(31, 4, 2025).hopLe(), "1.2.6 Thang 4 khong co ngay 31");
    kiemTra(!Date(31, 6, 2025).hopLe(), "1.2.7 Thang 6 khong co ngay 31");
    kiemTra(!Date(31, 9, 2025).hopLe(), "1.2.8 Thang 9 khong co ngay 31");
    kiemTra(!Date(31, 11, 2025).hopLe(), "1.2.9 Thang 11 khong co ngay 31");
    kiemTra(Date(31, 1, 2025).hopLe(), "1.2.10 Thang 1 co ngay 31");
    kiemTra(Date(31, 12, 2025).hopLe(), "1.2.11 Thang 12 co ngay 31");

    Date a(5, 11, 2026);
    Date b(5, 11, 2026);
    kiemTra((a - b) == 0, "1.3.1 Cung ngay tru nhau bang 0");

    Date c(1, 3, 2024);
    Date d(28, 2, 2024);
    kiemTra((c - d) == 2, "1.3.2 01/03/2024 - 28/02/2024 = 2 (qua ngay 29/02)");

    Date e(1, 1, 2026);
    Date f(1, 1, 2025);
    kiemTra((e - f) == 365, "1.3.3 01/01/2026 - 01/01/2025 = 365 ngay");

    Date g(1, 1, 2025);
    Date h(5, 1, 2025);
    kiemTra((g - h) == -4, "1.3.4 Ngay truoc tru ngay sau cho gia tri am");

    kiemTra(dinhDangTien(0) == "0 VND", "1.4.1 Dinh dang 0 VND: " + dinhDangTien(0));
    kiemTra(dinhDangTien(500000) == "500.000 VND", "1.4.2 Dinh dang 500k: " + dinhDangTien(500000));
    kiemTra(dinhDangTien(1234567890) == "1.234.567.890 VND", "1.4.3 Dinh dang 1 ty: " + dinhDangTien(1234567890));
    kiemTra(dinhDangTien(-50000) == "-50.000 VND", "1.4.4 Dinh dang so am: " + dinhDangTien(-50000));
}

// =========================================================================
// TEST 2: Dich vu va da hinh tinh tien
// =========================================================================
void testDichVu() {
    std::cout << "\n=== TEST NHOM 2: Dich vu & Da hinh tinh tien ===\n";

    DichVuAnUong fnb("DV01", "Buffet", 150000, 0.05);
    kiemTra(fnb.tinhTien(1) == 157500, "2.1.1 F&B sl 1: 150k + 5% = 157.500 VND");
    kiemTra(fnb.tinhTien(2) == 315000, "2.1.2 F&B sl 2: 315.000 VND");

    DichVuGiatUi giat("DV02", "Giat", 40000, 30000);
    kiemTra(giat.tinhTien(0.5) == 30000, "2.2.1 Giat ui 0.5kg: 20k < phi toi thieu 30k -> tinh 30k");
    kiemTra(giat.tinhTien(2.0) == 80000, "2.2.2 Giat ui 2kg: 80k > 30k -> tinh 80k");

    DichVuSpa spa("DV03", "Massage", 300000, 1.2);
    kiemTra(spa.tinhTien(1) == 360000, "2.3.1 Spa he so 1.2: 300k * 1.2 = 360k");

    DichVuDuaDon xe("DV04", "Taxi", 15000, 20000);
    kiemTra(xe.tinhTien(10) == 170000, "2.4.1 Xe 10km: 20k mo cua + 15k*10 = 170k");
}

// =========================================================================
// TEST 3: Phong, PhongFactory, Trang thai phong
// =========================================================================
void testPhongVaTrangThai() {
    std::cout << "\n=== TEST NHOM 3: Phong, PhongFactory & Trang thai ===\n";

    std::unique_ptr<Phong> pDon(PhongFactory::taoPhong("DON", "P101", 400000));
    std::unique_ptr<Phong> pDoi(PhongFactory::taoPhong("DOI", "P102", 600000));
    std::unique_ptr<Phong> pSuite(PhongFactory::taoPhong("SUITE", "P201", 1200000));
    std::unique_ptr<Phong> pVIP(PhongFactory::taoPhong("VIP", "P301", 2000000));

    kiemTra(pDon && pDon->tinhDonGia() == 400000, "3.1.1 Tao PhongDon don gia dung 400k");
    kiemTra(pDoi && pDoi->tinhDonGia() == 600000, "3.1.2 Tao PhongDoi don gia dung 600k");
    kiemTra(pSuite && pSuite->tinhDonGia() == 2200000, "3.1.3 Tao PhongSuite don gia dung 2.2tr");
    kiemTra(pVIP && pVIP->tinhDonGia() == 2500000, "3.1.4 Tao PhongVIP don gia dung 2.5tr");

    Phong* pKhongHopLe = PhongFactory::taoPhong("PRESIDENT", "P999", 5000000);
    kiemTra(pKhongHopLe == nullptr, "3.1.5 Loai phong khong ton tai tra ve nullptr");

    KhachHang kh("KH01", "Nguyen Van A", "123456789012", "0901234567", "a@mail.com");
    kiemTra(pDon->getTinhTrang() == TinhTrangPhong::TRONG, "3.2.1 Phong moi tao phai TRONG");

    bool nhanOk = pDon->nhanPhong(&kh, Date(1, 10, 2026));
    kiemTra(nhanOk && pDon->getTinhTrang() == TinhTrangPhong::DANG_SU_DUNG,
            "3.2.2 Nhan phong thanh cong -> DANG_SU_DUNG");

    bool nhanLai = pDon->nhanPhong(&kh, Date(2, 10, 2026));
    kiemTra(!nhanLai, "3.2.3 Khong the nhan phong khi dang co khach");

    bool btLoi = pDon->baoTri(true);
    kiemTra(!btLoi, "3.2.4 Khong the dua phong DANG_SU_DUNG vao bao tri");

    long long tienP = pDon->traPhong(Date(3, 10, 2026));
    kiemTra(tienP == 800000 && pDon->getTinhTrang() == TinhTrangPhong::TRONG,
            "3.2.5 Tra phong sau 2 dem: tien phong 800k, trang thai ve TRONG");

    bool btOk = pDon->baoTri(true);
    kiemTra(btOk && pDon->getTinhTrang() == TinhTrangPhong::BAO_TRI,
            "3.2.6 Dua phong TRONG vao bao tri thanh cong");

    bool nhanKhiBT = pDon->nhanPhong(&kh, Date(4, 10, 2026));
    kiemTra(!nhanKhiBT, "3.2.7 Khong the nhan phong khi dang BAO_TRI");

    bool xongBT = pDon->baoTri(false);
    kiemTra(xongBT && pDon->getTinhTrang() == TinhTrangPhong::TRONG,
            "3.2.8 Hoan tat bao tri -> ve TRONG");
}

// =========================================================================
// TEST 4: Khach hang, Tich diem, Hang thanh vien, Giam gia
// =========================================================================
void testKhachHang() {
    std::cout << "\n=== TEST NHOM 4: Khach hang & Hang thanh vien ===\n";

    KhachHang kh("KH01", "Tran Thi B", "012345678901", "0905111222", "b@mail.com");
    kiemTra(kh.getHang() == HangThanhVien::THUONG && tyLeGiamGia(kh.getHang()) == 0,
            "4.1 Khach moi tao hang THUONG, giam gia 0%");

    kh.congDiem(120);
    kiemTra(kh.getHang() == HangThanhVien::BAC && tyLeGiamGia(kh.getHang()) == 3,
            "4.2 120 diem -> hang BAC (giam 3%)");

    kh.congDiem(400);
    kiemTra(kh.getHang() == HangThanhVien::VANG && tyLeGiamGia(kh.getHang()) == 5,
            "4.3 520 diem -> hang VANG (giam 5%)");

    kh.congDiem(500);
    kiemTra(kh.getHang() == HangThanhVien::KIM_CUONG && tyLeGiamGia(kh.getHang()) == 10,
            "4.4 1020 diem -> hang KIM_CUONG (giam 10%)");

    bool capNhatRong = kh.capNhatThongTin("", "0905111222", "b@mail.com");
    kiemTra(!capNhatRong, "4.5 Cap nhat ho ten rong bi tu choi");

    bool capNhatOk = kh.capNhatThongTin("Tran Thi Binh", "0905999888", "binh@mail.com");
    kiemTra(capNhatOk && kh.getHoTen() == "Tran Thi Binh" && kh.getSDT() == "0905999888",
            "4.6 Cap nhat thong tin thanh cong");
}

// =========================================================================
// TEST 5: Luot thue, Tinh dem, Tra cung ngay, Tra som hon
// =========================================================================
void testLuotThue() {
    std::cout << "\n=== TEST NHOM 5: Luot thue & Tinh so dem ===\n";

    KhachHang kh("KH01", "Pham C", "111222333444", "0901112233", "c@mail.com");
    DichVuAnUong dv("DV01", "Com phan", 100000, 0.0);

    {
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DON", "P101", 300000));
        p->nhanPhong(&kh, Date(10, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        assert(lt != nullptr);

        lt->themDichVu(dv, 2, Date(10, 10, 2026));
        long long tien = p->traPhong(Date(10, 10, 2026));
        kiemTra(lt->soDem() == 1, "5.1.1 Check-in va check-out cung ngay tinh 1 dem");
        kiemTra(tien == 300000, "5.1.2 Tien phong tra cung ngay = 300k");
        kiemTra(lt->tienDichVu() == 200000, "5.1.3 Tien dich vu = 200k");
        kiemTra(lt->tongTien() == 500000, "5.1.4 Tong tien tam tinh = 500k");
    }

    {
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DON", "P102", 300000));
        p->nhanPhong(&kh, Date(10, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        p->traPhong(Date(5, 10, 2026));
        kiemTra(lt->soDem() == 1, "5.2.1 Ngay tra < ngay nhan duoc xu ly ve 1 dem, khong loi am");
    }

    {
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DON", "P103", 300000));
        p->nhanPhong(&kh, Date(10, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        p->traPhong(Date(12, 10, 2026));
        bool themDV = lt->themDichVu(dv, 1);
        kiemTra(!themDV, "5.3 Luot thue da tra khong the them dich vu tiep");
    }
}

// =========================================================================
// TEST 6: Hoa don, VAT, Giam gia, Thanh toan tung phan, Huy hoa don, Doanh thu
// =========================================================================
void testHoaDonVaThanhToan() {
    std::cout << "\n=== TEST NHOM 6: Hoa don, Thanh toan, VAT & Doanh thu ===\n";

    KhachHang khThuong("KH01", "Khach Thuong", "111", "0901", "a@mail.com");
    KhachHang khVip("KH02", "Khach Vip", "222", "0902", "b@mail.com");
    khVip.datDiem(1000);

    NhanVien nvLeTan("NV01", "Le Tan", ChucVu::LE_TAN, Date(1, 1, 2025));
    NhanVien nvThuNgan("NV02", "Thu Ngan", ChucVu::THU_NGAN, Date(1, 1, 2025));
    NhanVien nvQuanLy("NV03", "Quan Ly", ChucVu::QUAN_LY, Date(1, 1, 2025));

    DichVuAnUong dv("DV01", "Buffet", 100000, 0.0);

    {
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DON", "P101", 500000));
        p->nhanPhong(&khThuong, Date(1, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        lt->themDichVu(dv, 2);
        p->traPhong(Date(3, 10, 2026));

        HoaDon hd("HD001", lt, &nvLeTan, Date(3, 10, 2026));
        kiemTra(hd.getTienPhong() == 1000000, "6.1.1 Tien phong 2 dem: 1.000.000 VND");
        kiemTra(hd.getTienDichVu() == 200000, "6.1.2 Tien dich vu: 200.000 VND");
        kiemTra(hd.getGiamGia() == 0, "6.1.3 Giam gia khach thuong = 0");
        kiemTra(hd.getThueVAT() == 120000, "6.1.4 VAT 10% = 120.000 VND");
        kiemTra(hd.getTongCong() == 1320000, "6.1.5 Tong cong = 1.320.000 VND");
        kiemTra(hd.getDoanhThu() == 1200000, "6.1.6 Doanh thu (chua VAT) = 1.200.000 VND");
        kiemTra(hd.getTrangThai() == TrangThaiHoaDon::CHUA_THANH_TOAN, "6.1.7 Trang thai ban dau CHUA_THANH_TOAN");
    }

    {
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DON", "P102", 1000000));
        p->nhanPhong(&khVip, Date(1, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        p->traPhong(Date(2, 10, 2026));

        HoaDon hd("HD002", lt, &nvLeTan, Date(2, 10, 2026));
        kiemTra(hd.getGiamGia() == 100000, "6.2.1 Giam gia 10% khach VIP: 100.000 VND");
        kiemTra(hd.getThueVAT() == 90000, "6.2.2 VAT tinh tren gia sau giam: 90.000 VND");
        kiemTra(hd.getTongCong() == 990000, "6.2.3 Tong cong = 990.000 VND");
        kiemTra(hd.getDoanhThu() == 900000, "6.2.4 Doanh thu (chua VAT) = 900.000 VND");
    }

    {
        QuanLyHoaDon ql;
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DON", "P103", 500000));
        p->nhanPhong(&khThuong, Date(1, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        p->traPhong(Date(2, 10, 2026));

        HoaDon* hd = ql.taoHoaDon(lt, &nvLeTan, Date(2, 10, 2026));
        kiemTra(hd != nullptr, "6.3.1 Tao hoa don thanh cong");

        HoaDon* hdTrung = ql.taoHoaDon(lt, &nvLeTan, Date(2, 10, 2026));
        kiemTra(hdTrung == nullptr, "6.3.2 Khong cho phep tao hoa don trung cho cung 1 luot thue");

        bool ttVuot = ql.thanhToan(hd->getMa(), 600000, HinhThucThanhToan::TIEN_MAT, &nvThuNgan, Date(2, 10, 2026));
        kiemTra(!ttVuot, "6.3.3 Khong the thu tien vuot qua con phai tra");

        bool ttPhan1 = ql.thanhToan(hd->getMa(), 200000, HinhThucThanhToan::TIEN_MAT, &nvThuNgan, Date(2, 10, 2026));
        kiemTra(ttPhan1 && hd->getTrangThai() == TrangThaiHoaDon::THANH_TOAN_MOT_PHAN,
                "6.3.4 Thu 200k tien mat -> THANH_TOAN_MOT_PHAN, con lai: " + std::to_string(hd->conPhaiTra()));

        bool ttThieuRef = ql.thanhToan(hd->getMa(), 350000, HinhThucThanhToan::CHUYEN_KHOAN, &nvThuNgan, Date(2, 10, 2026), "");
        kiemTra(!ttThieuRef, "6.3.5 Chuyen khoan khong co so tham chieu phai bi tu choi");

        bool ttDuRef = ql.thanhToan(hd->getMa(), 350000, HinhThucThanhToan::CHUYEN_KHOAN, &nvThuNgan, Date(2, 10, 2026), "REF12345");
        kiemTra(ttDuRef && hd->daTatToan() && hd->getTrangThai() == TrangThaiHoaDon::DA_THANH_TOAN,
                "6.3.6 Thu not 350k voi ref -> DA_THANH_TOAN, con phai tra: 0");

        bool ttThem = ql.thanhToan(hd->getMa(), 10000, HinhThucThanhToan::TIEN_MAT, &nvThuNgan, Date(2, 10, 2026));
        kiemTra(!ttThem, "6.3.7 Hoa don da tat toan khong the thu them");

        bool huyLoi = ql.huyHoaDon(hd->getMa(), &nvQuanLy);
        kiemTra(!huyLoi, "6.3.8 Khong the huy hoa don da co thanh toan");
    }

    {
        QuanLyHoaDon ql;
        std::unique_ptr<Phong> p(PhongFactory::taoPhong("DOI", "P104", 600000));
        p->nhanPhong(&khThuong, Date(1, 10, 2026));
        LuotThue* lt = p->getLuotThueHienTai();
        p->traPhong(Date(2, 10, 2026));

        HoaDon* hd = ql.taoHoaDon(lt, &nvLeTan, Date(2, 10, 2026));
        bool huyLeTan = ql.huyHoaDon(hd->getMa(), &nvLeTan);
        kiemTra(!huyLeTan, "6.4.1 Le tan khong co quyen huy hoa don");

        bool huyOk = ql.huyHoaDon(hd->getMa(), &nvQuanLy);
        kiemTra(huyOk && hd->getTrangThai() == TrangThaiHoaDon::DA_HUY,
                "6.4.2 Quan ly huy hoa don chua thanh toan thanh cong");

        kiemTra(hd->getDoanhThu() == 0, "6.4.3 Hoa don da huy doanh thu = 0");
        kiemTra(ql.tongDoanhThu() == 0, "6.4.4 Tong doanh thu loai bo hoa don da huy");

        bool ttHuy = ql.thanhToan(hd->getMa(), 100000, HinhThucThanhToan::TIEN_MAT, &nvThuNgan, Date(2, 10, 2026));
        kiemTra(!ttHuy, "6.4.5 Khong the thanh toan vao hoa don da huy");
    }
}

// =========================================================================
// TEST 7: Tai khoan, Dang nhap, Khoa 30s, Khoa 5 lan, Quan ly duy nhat
// =========================================================================
void testAuthService() {
    std::cout << "\n=== TEST NHOM 7: Tai khoan & Bao mat dang nhap ===\n";

    AuthService auth;
    auth.khoiDong();

    KetQua kqAdmin = auth.dangNhap("admin", "admin123");
    kiemTra(kqAdmin.ok && auth.daDangNhap() && auth.phienHienTai().vaiTro == VaiTro::QUAN_LY,
            "7.1 Dang nhap admin dung mat khau thanh cong");

    kiemTra(auth.coQuyen(Quyen::QUAN_LY_PHONG), "7.2.1 Quan ly co quyen QUAN_LY_PHONG");
    kiemTra(auth.coQuyen(Quyen::QUAN_LY_NHAN_VIEN), "7.2.2 Quan ly co quyen QUAN_LY_NHAN_VIEN");
    kiemTra(auth.coQuyen(Quyen::XEM_DOANH_THU), "7.2.3 Quan ly co quyen XEM_DOANH_THU");

    auth.dangXuat();
    kiemTra(!auth.daDangNhap(), "7.3 Dang xuat thanh cong");

    KetQua kqLT = auth.dangNhap("letan", "letan123");
    kiemTra(kqLT.ok && auth.phienHienTai().vaiTro == VaiTro::LE_TAN, "7.4 Dang nhap letan thanh cong");
    kiemTra(!auth.coQuyen(Quyen::QUAN_LY_NHAN_VIEN), "7.5 Le tan KHONG co quyen QUAN_LY_NHAN_VIEN");
    kiemTra(!auth.coQuyen(Quyen::XEM_DOANH_THU), "7.6 Le tan KHONG co quyen XEM_DOANH_THU");
    kiemTra(auth.coQuyen(Quyen::NHAN_TRA_PHONG), "7.7 Le tan co quyen NHAN_TRA_PHONG");
    auth.dangXuat();

    auth.dangNhap("admin", "admin123");
    auth.xoaTaiKhoan("testuser");
    auth.themTaiKhoan("testuser", "pass123", "Nguoi dung test", "", VaiTro::LE_TAN);
    auth.dangXuat();

    KetQua s1 = auth.dangNhap("testuser", "sai");
    KetQua s2 = auth.dangNhap("testuser", "sai");
    kiemTra(!s1.ok && !s2.ok, "7.8 Nhap sai lan 1 va 2 bi bao loi");

    KetQua s3 = auth.dangNhap("testuser", "sai");
    kiemTra(!s3.ok && auth.giayTamKhoaConLai("testuser") > 0,
            "7.9 Nhap sai lan 3 bi tam khoa 30 giay (con: " + std::to_string(auth.giayTamKhoaConLai("testuser")) + "s)");

    KetQua dungKhiKhoa = auth.dangNhap("testuser", "pass123");
    kiemTra(!dungKhiKhoa.ok, "7.10 Trong thoi gian tam khoa 30s, nhap dung mat khau van bi chan");

    auth.dangNhap("admin", "admin123");
    auth.xoaTaiKhoan("testuser");
    auth.dangXuat();
}

// =========================================================================
// TEST 8: KhachSanService - Quy trinh toan trinh va rang buoc du lieu
// =========================================================================
void testKhachSanServiceWorkflows() {
    std::cout << "\n=== TEST NHOM 8: Quy trinh nghiep vu & Rang buoc du lieu ===\n";

    AuthService auth;
    auth.khoiDong();
    auth.dangNhap("admin", "admin123");

    KhachSanService svc(auth);
    svc.khoiDong();

    svc.xoaPhong("P901");
    KetQua kqThemP = svc.themPhong("VIP", "P901", 3000000, "01/01/2026");
    kiemTra(kqThemP.ok, "8.1 Them phong moi P901 thanh cong: " + kqThemP.thongBao);

    KetQua kqTrungP = svc.themPhong("VIP", "P901", 3000000, "01/01/2026");
    kiemTra(!kqTrungP.ok, "8.2 Chan them phong trung ma P901: " + kqTrungP.thongBao);

    svc.xoaKhachHang("KH99");
    KetQua kqThemKH = svc.themKhachHang("KH99", "Hoang Thi Hoa", "999888777666", "0933999888", "hoa@mail.com", "Da Nang");
    kiemTra(kqThemKH.ok, "8.3 Them khach hang KH99 thanh cong");

    KetQua kqNhan = svc.nhanPhong("P901", "KH99", "10/11/2026");
    kiemTra(kqNhan.ok, "8.4 Nhan phong P901 cho KH99 thanh cong");

    KetQua kqXoaDangO = svc.xoaPhong("P901");
    kiemTra(!kqXoaDangO.ok, "8.5 Khong the xoa phong dang co khach: " + kqXoaDangO.thongBao);

    KetQua kqXoaKHDangO = svc.xoaKhachHang("KH99");
    kiemTra(!kqXoaKHDangO.ok, "8.6 Khong the xoa khach hang co lich su / dang thue: " + kqXoaKHDangO.thongBao);

    KetQua kqThemDV = svc.themDichVuChoPhong("P901", "DV01", 2);
    kiemTra(kqThemDV.ok, "8.7 Them 2 suat Buffet cho P901 thanh cong");

    std::string maHDMoi;
    KetQua kqTra = svc.traPhong("P901", "12/11/2026", maHDMoi);
    kiemTra(kqTra.ok && !maHDMoi.empty(), "8.8 Tra phong P901 thanh cong, tao hoa don: " + maHDMoi);

    KetQua kqXoaPCoHD = svc.xoaPhong("P901");
    kiemTra(!kqXoaPCoHD.ok, "8.9 Khong the xoa phong P901 vi da co hoa don: " + kqXoaPCoHD.thongBao);

    ChiTietHoaDonHienThi ct;
    svc.chiTietHoaDon(maHDMoi, ct);
    kiemTra(ct.conPhaiTra > 0, "8.10 Hoa don moi co cong no con phai tra: " + dinhDangTien(ct.conPhaiTra));

    KetQua kqThu = svc.thanhToanHoaDon(maHDMoi, ct.conPhaiTra, HinhThucThanhToan::TIEN_MAT, "", "12/11/2026");
    kiemTra(kqThu.ok, "8.11 Thu tat toan hoa don " + maHDMoi + " thanh cong: " + kqThu.thongBao);

    std::string ndFile;
    KetQua kqXuat = svc.xuatHoaDonTxt(maHDMoi, ndFile);
    kiemTra(kqXuat.ok && !ndFile.empty(), "8.12 Xuat hoa don " + maHDMoi + " ra file thanh cong");

    ThongKeDoanhThuChiTiet tk;
    svc.thongKeBaoCaoDoanhThu(tk);
    kiemTra(tk.tongDoanhThu > 0 && tk.hoaDonDaTatToan >= 1, "8.13 Bao cao doanh thu phan anh dung du lieu");
}

// =========================================================================
// TEST 9: LuuTru (Doc/Ghi file persistence)
// =========================================================================
void testLuuTruPersistence() {
    std::cout << "\n=== TEST NHOM 9: LuuTru (Doc / Ghi file) ===\n";

    KhachSanData data1;
    data1.duLieuMau();

    bool luuOk = LuuTru::luu(data1);
    kiemTra(luuOk, "9.1 Ghi du lieu ra file van ban thanh cong");

    KhachSanData data2;
    bool taiOk = LuuTru::tai(data2);
    kiemTra(taiOk, "9.2 Doc du lieu tu file van ban thanh cong");

    kiemTra(data2.dsPhong.tatCa().size() == data1.dsPhong.tatCa().size(),
            "9.3 So luong phong doc tu file khop hoan toan");
    kiemTra(data2.dsKhach.size() == data1.dsKhach.size(),
            "9.4 So luong khach hang doc tu file khop hoan toan");
    kiemTra(data2.dsNhanVien.size() == data1.dsNhanVien.size(),
            "9.5 So luong nhan vien doc tu file khop hoan toan");
    kiemTra(data2.dsDichVu.size() == data1.dsDichVu.size(),
            "9.6 So luong dich vu doc tu file khop hoan toan");
}

// =========================================================================
// TEST 10: EDGE CASES & CAC TRUONG HOP THUC TE NGUOI DUNG
// =========================================================================
void testEdgeCasesVaBaoMat() {
    std::cout << "\n=== TEST NHOM 10: Edge Cases & Kiem thu cac tinh huong thuc te ===\n";

    AuthService auth;
    auth.khoiDong();
    KhachSanService svc(auth);
    svc.khoiDong();

    // Case 10.1: Le tan (NV01) dang nhap va thu tien hoa don
    auth.dangNhap("admin", "admin123");
    svc.xoaPhong("P801");
    svc.xoaKhachHang("KH88");
    svc.themPhong("DON", "P801", 300000, "01/01/2026");
    svc.themKhachHang("KH88", "Le Van Test", "123123123123", "0900000000", "test@mail.com", "DN");
    svc.nhanPhong("P801", "KH88", "01/11/2026");
    std::string maHD;
    svc.traPhong("P801", "02/11/2026", maHD);
    
    ChiTietHoaDonHienThi ct;
    svc.chiTietHoaDon(maHD, ct);

    // Chuyen sang dang nhap bang tai khoan letan (NV01)
    auth.dangXuat();
    auth.dangNhap("letan", "letan123");
    KetQua kqThuLeTan = svc.thanhToanHoaDon(maHD, ct.conPhaiTra, HinhThucThanhToan::TIEN_MAT, "", "02/11/2026");
    kiemTra(kqThuLeTan.ok, "10.1 Le tan (NV01) co the thu tien hoa don theo phan quyen giao dien", kqThuLeTan.thongBao);

    // Case 10.2: Quan ly co maNV rong co the huy hoa don khong
    auth.dangNhap("admin", "admin123");
    svc.nhanPhong("P801", "KH88", "05/11/2026");
    std::string maHDHuy;
    svc.traPhong("P801", "06/11/2026", maHDHuy);

    // Tao tai khoan Quan ly moi khong co maNV
    auth.xoaTaiKhoan("admin_no_nv");
    auth.themTaiKhoan("admin_no_nv", "pass123", "Quan Ly Phu", "", VaiTro::QUAN_LY);
    auth.dangXuat();
    auth.dangNhap("admin_no_nv", "pass123");

    KetQua kqHuyQL = svc.huyHoaDon(maHDHuy);
    kiemTra(kqHuyQL.ok, "10.2 Quan ly khong co maNV (maNV rong) van co the huy hoa don thanh cong", kqHuyQL.thongBao);

    // Don dep
    auth.dangNhap("admin", "admin123");
    auth.xoaTaiKhoan("admin_no_nv");

    // Case 10.3: Null safety khi phong chua co luot thue
    std::unique_ptr<Phong> pMoi(PhongFactory::taoPhong("DON", "P999", 500000));
    kiemTra(pMoi->getLuotThueHienTai() == nullptr, "10.3.1 getLuotThueHienTai khi chua co luot thue tra ve nullptr");
    kiemTra(pMoi->getLuotThueCuoi() == nullptr, "10.3.2 getLuotThueCuoi khi chua co luot thue tra ve nullptr");
    kiemTra(pMoi->getSoLuotThue() == 0, "10.3.3 getSoLuotThue ban dau bang 0");

    // Case 10.4: DanhSachPhong
    DanhSachPhong dsP;
    dsP.themCuoi(PhongFactory::taoPhong("DON", "P01", 300000));
    dsP.themCuoi(PhongFactory::taoPhong("DOI", "P02", 500000));
    dsP.themDau(PhongFactory::taoPhong("VIP", "P00", 1000000));
    kiemTra(dsP.tatCa().size() == 3, "10.4.1 DanhSachPhong them cuoi va them dau co dung 3 phong");
    kiemTra(dsP.tatCa()[0]->getMa() == "P00", "10.4.2 Phong dau tien la P00");

    dsP.sapXepTheoDonGia(true);
    kiemTra(dsP.tatCa()[0]->getMa() == "P01", "10.4.3 Sau sap xep tang dan: P01 (300k) dung dau");
    kiemTra(dsP.tatCa()[2]->getMa() == "P00", "10.4.4 P00 (1tr) dung cuoi");

    dsP.sapXepTheoDonGia(false);
    kiemTra(dsP.tatCa()[0]->getMa() == "P00", "10.4.5 Sau sap xep giam dan: P00 (1tr) dung dau");

    bool xoaP01 = dsP.xoaTheoMa("P01");
    kiemTra(xoaP01 && dsP.tatCa().size() == 2, "10.5.1 Xoa phong P01 thanh cong, con 2 phong");
    bool xoaKoTonTai = dsP.xoaTheoMa("P999");
    kiemTra(!xoaKoTonTai, "10.5.2 Xoa phong khong ton tai tra ve false");

    // Case 10.6: Chan xoa nhan vien dang lien ket voi tai khoan dang nhap
    auth.dangNhap("admin", "admin123");
    KetQua kqXoaNV01 = svc.xoaNhanVien("NV01");
    kiemTra(!kqXoaNV01.ok, "10.6 Chan xoa nhan vien NV01 vi dang lien ket tai khoan 'letan': " + kqXoaNV01.thongBao);

    // Case 10.7: Tao phong tu dong lay gia co dinh theo loai phong + Nhan phong kem ngay tra du kien + Doanh thu theo thang/quy
    svc.xoaPhong("P802");
    long long giaCB = 0, giaDem = 0;
    KetQua kqGia = svc.giaPhongTheoLoai("SUITE", giaCB, giaDem);
    kiemTra(kqGia.ok && giaCB == 1200000 && giaDem == 2200000, "10.7.1 Gia phong SUITE tu dong: co ban 1.2tr, don gia 2.2tr/dem");

    KetQua kqThemAuto = svc.themPhong("SUITE", "P802", "01/11/2026");
    kiemTra(kqThemAuto.ok, "10.7.2 Tao phong P802 (SUITE) khong can nhap gia thanh cong");

    KetQua kqNhanSaiTra = svc.nhanPhong("P802", "KH88", "10/11/2026", "08/11/2026");
    kiemTra(!kqNhanSaiTra.ok, "10.7.3 Chan ngay tra du kien truoc ngay nhan phong");

    KetQua kqNhanCoTra = svc.nhanPhong("P802", "KH88", "10/11/2026", "13/11/2026");
    kiemTra(kqNhanCoTra.ok, "10.7.4 Nhan phong P802 kem ngay tra du kien 13/11/2026 thanh cong");

    ChiTietPhongHienThi ctP802;
    svc.chiTietPhong("P802", ctP802);
    kiemTra(ctP802.coNgayTraDuKien && ctP802.ngayTraDuKien == Date(13, 11, 2026) && ctP802.soDemTamTinh == 3,
            "10.7.5 Chi tiet phong luu dung ngay tra du kien 13/11/2026 va tam tinh 3 dem");

    std::vector<DoanhThuKyHienThi> dsThang, dsQuy;
    long long tongNamThang = 0, tongNamQuy = 0;
    KetQua kqDTThang = svc.thongKeDoanhThuTheoKy(2026, false, dsThang, tongNamThang);
    KetQua kqDTQuy = svc.thongKeDoanhThuTheoKy(2026, true, dsQuy, tongNamQuy);
    kiemTra(kqDTThang.ok && dsThang.size() == 12 && tongNamThang > 0,
            "10.7.6 Thong ke doanh thu theo 12 thang nam 2026 thanh cong");
    kiemTra(kqDTQuy.ok && dsQuy.size() == 4 && tongNamQuy == tongNamThang && dsQuy[3].doanhThu == tongNamThang,
            "10.7.7 Thong ke doanh thu theo 4 quy nam 2026 khop voi tong theo thang");

    // Case 10.8: Tu dong sap xep phong theo tang / so phong (P101 -> P105 -> P201 -> P202 -> P301)
    DanhSachPhong dsSapXep;
    dsSapXep.themCuoi(PhongFactory::taoPhong("DON", "P101", 400000));
    dsSapXep.themCuoi(PhongFactory::taoPhong("SUITE", "P201", 1200000));
    dsSapXep.themCuoi(PhongFactory::taoPhong("VIP", "P301", 2000000));
    dsSapXep.themCuoi(PhongFactory::taoPhong("DON", "P202", 400000));
    dsSapXep.themCuoi(PhongFactory::taoPhong("DON", "P105", 400000));
    dsSapXep.sapXepTheoMa(true);
    auto vSX = dsSapXep.tatCa();
    kiemTra(vSX.size() == 5 &&
            vSX[0]->getMa() == "P101" &&
            vSX[1]->getMa() == "P105" &&
            vSX[2]->getMa() == "P201" &&
            vSX[3]->getMa() == "P202" &&
            vSX[4]->getMa() == "P301",
            "10.8 Tu dong sap xep phong theo tang / so phong (P101 -> P105 -> P201 -> P202 -> P301)");
}

int main() {
    const char* dsFile[] = {
        "dulieu_dichvu.txt", "dulieu_hethong.txt", "dulieu_hoadon.txt",
        "dulieu_khach.txt", "dulieu_luotthue.txt", "dulieu_nhanvien.txt",
        "dulieu_phong.txt", "dulieu_taikhoan.txt"
    };
    std::vector<std::pair<std::string, std::string>> saoLuu;
    for (const char* f : dsFile) {
        std::ifstream in(f, std::ios::binary);
        if (in) {
            std::ostringstream ss;
            ss << in.rdbuf();
            saoLuu.push_back({f, ss.str()});
        }
    }

    std::cout << "========================================================\n";
    std::cout << "   KHOI CHAY TOAN BO CAC TEST CASE - QUAN LY KHACH SAN   \n";
    std::cout << "========================================================\n";

    testDateVaTien();
    testDichVu();
    testPhongVaTrangThai();
    testKhachHang();
    testLuotThue();
    testHoaDonVaThanhToan();
    testAuthService();
    testKhachSanServiceWorkflows();
    testLuuTruPersistence();
    testEdgeCasesVaBaoMat();

    for (const auto& p : saoLuu) {
        std::ofstream out(p.first, std::ios::binary | std::ios::trunc);
        out << p.second;
    }

    std::cout << "\n========================================================\n";
    std::cout << "TONG KET:\n";
    std::cout << "  Passed : " << g_passed << "\n";
    std::cout << "  Failed : " << g_failed << "\n";
    std::cout << "========================================================\n";

    return g_failed == 0 ? 0 : 1;
}
