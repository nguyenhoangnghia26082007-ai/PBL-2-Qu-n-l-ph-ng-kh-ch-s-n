#pragma once
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "DanhSach.h"
#include "HoaDon.h"

class LuuTru;

// Giao dien menu tren console
class UngDung {
    friend class LuuTru; // LuuTru doc/ghi truc tiep cac danh sach de luu file
    std::vector<std::unique_ptr<KhachHang>> dsKhach;
    std::vector<std::unique_ptr<NhanVien>> dsNhanVien;
    std::vector<std::unique_ptr<DichVu>> dsDichVu;
    DanhSachPhong dsPhong;
    QuanLyHoaDon qlHoaDon;
    TrinhInHoaDon trinhIn;
    std::set<std::string> daCongDiem; // hoa don da cong diem thanh vien

    KhachHang* timKhach(const std::string& ma) const;
    NhanVien* timNhanVien(const std::string& ma) const;
    DichVu* timDichVu(const std::string& ma) const;

    void duLieuMau();
    void luu() const; // ghi toan bo du lieu ra file (goi sau moi thao tac thay doi)
    void menuPhong();
    void menuLuuTru();
    void menuKhachHang();
    void menuNhanVien();
    void menuHoaDon();
    void menuThongKe();

    void nhanPhong();
    void themDichVuChoPhong();
    void traPhong();
    void lapHoaDon();
    bool lapHoaDonChoLuot(LuotThue* lt); // hoi nhan vien + ngay roi lap hoa don cho luot da tra
    void thanhToan();
    void inHoaDon();
    void inBienLai();
public:
    UngDung();
    void chay();
};
