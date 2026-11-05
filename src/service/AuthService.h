#pragma once
// AuthService: dang nhap / dang xuat, khoa 30s khi sai 3 lan, khoa vinh vien khi sai 5 lan (Quan ly mo lai),
// va quan ly tai khoan (them / sua / doi mat khau / khoa / mo khoa / xoa).
#include <chrono>
#include <map>
#include <string>
#include <vector>
#include "KetQua.h"
#include "PhanQuyen.h"
#include "TaiKhoan.h"

struct Phien {
    bool daDangNhap = false;
    std::string tenDangNhap, hoTen, maNV;
    VaiTro vaiTro = VaiTro::BUONG_PHONG;
};

class AuthService {
    std::vector<TaiKhoan> dsTaiKhoan;
    Phien phien;
    int saiKhongTonTai = 0; // dem khi nhap ten tai khoan khong ton tai
    std::map<std::string, std::chrono::steady_clock::time_point> tamKhoaDen; // theo ten tai khoan

    TaiKhoan* tim(const std::string& tenDangNhap);
    const TaiKhoan* tim(const std::string& tenDangNhap) const;
    int demQuanLyHoatDong(const std::string& boQuaTen = "") const;
public:
    std::string khoiDong();
    bool luu() const;

    KetQua dangNhap(const std::string& tenDangNhap, const std::string& matKhau);
    void dangXuat();
    // So giay con tam khoa cua 1 tai khoan (0 neu khong bi tam khoa)
    int giayTamKhoaConLai(const std::string& tenDangNhap) const;

    const Phien& phienHienTai() const { return phien; }
    bool daDangNhap() const { return phien.daDangNhap; }
    bool coQuyen(Quyen q) const;
    KetQua yeuCau(Quyen q) const;

    // ---- Quan ly tai khoan (chi QUAN_LY) ----
    KetQua danhSachTaiKhoan(std::vector<TaiKhoan>& kq) const;
    KetQua themTaiKhoan(const std::string& tenDangNhap, const std::string& matKhau,
                        const std::string& hoTen, const std::string& maNV, VaiTro vaiTro);
    KetQua capNhatTaiKhoan(const std::string& tenDangNhap, const std::string& matKhauMoi,
                           const std::string& hoTen, const std::string& maNV, VaiTro vaiTro);
    KetQua datKhoaTaiKhoan(const std::string& tenDangNhap, bool khoa); // mo khoa se reset soLanSai = 0
    KetQua xoaTaiKhoan(const std::string& tenDangNhap);

    // Dong bo ten hien thi khi Quan ly sua ho ten nhan vien
    void dongBoHoTenTheoMaNV(const std::string& maNV, const std::string& hoTenMoi);
};
