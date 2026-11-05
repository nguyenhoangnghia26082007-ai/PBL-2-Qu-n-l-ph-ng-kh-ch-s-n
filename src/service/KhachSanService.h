#pragma once
// KhachSanService: nghiep vu khach san cho UI. Moi ham deu kiem tra quyen truoc khi lam.
// Ben trong goi lai dung cac lop/ham cua ban console (Phong, NhanVien, HoaDon, LuuTru...).
#include <string>
#include <vector>
#include "AuthService.h"
#include "KetQua.h"
#include "KhachSanData.h"

struct ThongKeNhanh {
    int tongPhong = 0, phongTrong = 0, phongCoKhach = 0, phongBaoTri = 0;
    int soKhach = 0;
    bool coDoanhThu = false;
    long long doanhThu = 0;
};

struct PhongHienThi {
    std::string ma, maLoai;
    TinhTrangPhong tinhTrang = TinhTrangPhong::TRONG;
    long long donGia = 0;
    std::string tenKhach;
};

// Thong tin nhan vien de hien thi va chinh sua tren giao dien
struct NhanVienHienThi {
    std::string ma, hoTen, cccd, sdt;
    ChucVu chucVu = ChucVu::LE_TAN;
    Date ngayVaoLam;
    int soHoaDonDaLap = 0;
};

// ---- Cac struct ho tro So do phong & Nhan / Tra phong (Buoc 2) ----
struct DichVuDaDungItem {
    std::string tenDV;
    double soLuong = 0;
    long long donGia = 0, thanhTien = 0;
    Date thoiDiem;
};

struct ChiTietPhongHienThi {
    std::string ma, maLoai, tenLoai;
    int sucChua = 0;
    long long giaCoBan = 0, donGiaHienTai = 0;
    TinhTrangPhong tinhTrang = TinhTrangPhong::TRONG;
    Date ngayDuaVaoSuDung;

    // Neu dang co khach
    bool coKhach = false;
    std::string maLT, maKH, tenKhach, sdtKhach, cccdKhach;
    Date ngayNhan;
    int soDemTamTinh = 0;
    long long tienPhongTamTinh = 0, tienDichVuTamTinh = 0, tongTienTamTinh = 0;
    std::vector<DichVuDaDungItem> dsDichVu;
};

struct DichVuMenuHienThi {
    std::string ma, ten, loai;
    long long donGia = 0;
};

struct KhachHangNhanh {
    std::string ma, hoTen, sdt, cccd;
};

// ---- Cac struct ho tro Khach hang (Buoc 3) ----
struct KhachHangHienThi {
    std::string ma, hoTen, cccd, sdt, email, diaChi;
    int diem = 0;
    HangThanhVien hang = HangThanhVien::THUONG;
    std::string tenHang;
    int phanTramGiam = 0;
};

// ---- Cac struct ho tro Hoa don & Doanh thu (Buoc 4) ----
struct DongHoaDonHienThi {
    std::string moTa;
    double soLuong = 0;
    long long donGia = 0, thanhTien = 0;
};

struct LichSuThanhToanHienThi {
    std::string maGD;
    long long soTien = 0;
    std::string hinhThuc;
    Date thoiDiem;
    std::string tenNhanVienThu;
    std::string soThamChieu;
};

struct HoaDonHienThi {
    std::string maHD, maPhong, tenKhach, sdtKhach, tenNVLap;
    Date ngayLap;
    long long tongCong = 0, daThanhToan = 0, conPhaiTra = 0;
    TrangThaiHoaDon trangThai = TrangThaiHoaDon::CHUA_THANH_TOAN;
    std::string tenTrangThai;
};

struct ChiTietHoaDonHienThi {
    std::string maHD, maPhong, maLT;
    std::string tenKhach, sdtKhach, cccdKhach, diaChiKhach;
    std::string tenHangKhach;
    std::string tenNVLap;
    Date ngayLap, ngayNhan, ngayTra;
    int soDem = 0;
    long long tienPhong = 0, tienDichVu = 0, giamGia = 0, thueVAT = 0, tongCong = 0;
    long long daThanhToan = 0, conPhaiTra = 0;
    TrangThaiHoaDon trangThai = TrangThaiHoaDon::CHUA_THANH_TOAN;
    std::string tenTrangThai;
    std::vector<DongHoaDonHienThi> dsDong;
    std::vector<LichSuThanhToanHienThi> dsThanhToan;
};

struct DoanhThuLoaiPhongHienThi {
    std::string loaiPhong, tenLoai;
    long long doanhThu = 0;
    float phanTram = 0.0f;
};

struct ThongKeDoanhThuChiTiet {
    long long tongDoanhThu = 0;
    long long thucThuHomNay = 0;
    int tongSoHoaDon = 0, hoaDonDaTatToan = 0, hoaDonChuaThanhToan = 0, hoaDonDaHuy = 0;
    std::vector<DoanhThuLoaiPhongHienThi> doanhThuTheoLoai;
};

class KhachSanService {
    AuthService& auth;
    KhachSanData data;
public:
    explicit KhachSanService(AuthService& a) : auth(a) {}

    std::string khoiDong();
    bool luu() const;

    KetQua thongKeNhanh(ThongKeNhanh& kq) const;
    KetQua danhSachPhong(std::vector<PhongHienThi>& kq) const;

    // ---- Nghiep vu So do phong & Nhan / Tra phong (Buoc 2) ----
    KetQua chiTietPhong(const std::string& maPhong, ChiTietPhongHienThi& kq) const;
    KetQua nhanPhong(const std::string& maPhong, const std::string& maKH, const std::string& ngayNhanStr);
    KetQua themDichVuChoPhong(const std::string& maPhong, const std::string& maDV, double soLuong);
    KetQua traPhong(const std::string& maPhong, const std::string& ngayTraStr, std::string& maHDMoi);
    KetQua doiBaoTri(const std::string& maPhong, bool bat);
    KetQua themPhong(const std::string& loai, const std::string& maPhong, long long giaCoBan, const std::string& ngayDuaStr);
    KetQua xoaPhong(const std::string& maPhong);

    KetQua danhSachDichVu(std::vector<DichVuMenuHienThi>& kq) const;
    KetQua danhSachKhachHangNhanh(std::vector<KhachHangNhanh>& kq) const;

    // ---- Nghiep vu Khach hang & Hang thanh vien (Buoc 3) ----
    KetQua danhSachKhachHang(std::vector<KhachHangHienThi>& kq) const;
    KetQua themKhachHang(const std::string& ma, const std::string& hoTen, const std::string& cccd,
                         const std::string& sdt, const std::string& email, const std::string& diaChi);
    KetQua capNhatKhachHang(const std::string& ma, const std::string& hoTen, const std::string& sdt,
                            const std::string& email, const std::string& diaChi);
    KetQua xoaKhachHang(const std::string& ma);

    // ---- Nghiep vu Hoa don, Thanh toan & Doanh thu (Buoc 4) ----
    KetQua danhSachHoaDon(std::vector<HoaDonHienThi>& kq) const;
    KetQua chiTietHoaDon(const std::string& maHD, ChiTietHoaDonHienThi& kq) const;
    KetQua thanhToanHoaDon(const std::string& maHD, long long soTien, HinhThucThanhToan ht,
                           const std::string& soThamChieu, const std::string& ngayStr);
    KetQua huyHoaDon(const std::string& maHD);
    KetQua xuatHoaDonTxt(const std::string& maHD, std::string& noiDungFile) const;
    KetQua thongKeBaoCaoDoanhThu(ThongKeDoanhThuChiTiet& kq) const;

    // ---- Quan ly nhan vien (chi QUAN_LY) ----
    KetQua danhSachNhanVien(std::vector<NhanVienHienThi>& kq) const;
    KetQua themNhanVien(const std::string& ma, const std::string& hoTen, const std::string& cccd,
                        const std::string& sdt, ChucVu chucVu, const std::string& ngayStr);
    KetQua capNhatNhanVien(const std::string& ma, const std::string& hoTen, const std::string& cccd,
                           const std::string& sdt, ChucVu chucVu, const std::string& ngayStr);
    KetQua xoaNhanVien(const std::string& ma);
};
