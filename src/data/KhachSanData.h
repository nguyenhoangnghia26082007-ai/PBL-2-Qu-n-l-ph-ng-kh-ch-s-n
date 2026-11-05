#pragma once
// KhachSanData: kho du lieu trung tam (tach tu lop UngDung cua ban console).
// Chi chua du lieu + ham tim kiem, KHONG co cin/cout, KHONG kiem tra quyen.
#include <memory>
#include <set>
#include <string>
#include <vector>
#include "DanhSach.h"
#include "HoaDon.h"

struct KhachSanData {
    std::vector<std::unique_ptr<KhachHang>> dsKhach;
    std::vector<std::unique_ptr<NhanVien>> dsNhanVien;
    std::vector<std::unique_ptr<DichVu>> dsDichVu;
    DanhSachPhong dsPhong;
    QuanLyHoaDon qlHoaDon;
    TrinhInHoaDon trinhIn{
        "KHACH SAN BACH KHOA",
        "54 Nguyen Luong Bang, P. Hoa Khanh Bac, Q. Lien Chieu, TP. Da Nang",
        "02363842308"};
    std::set<std::string> daCongDiem; // hoa don da cong diem thanh vien

    KhachHang* timKhach(const std::string& ma) const;
    NhanVien* timNhanVien(const std::string& ma) const;
    DichVu* timDichVu(const std::string& ma) const;

    // Du lieu mau dung o lan chay dau tien (giong het ban console)
    void duLieuMau();
};
