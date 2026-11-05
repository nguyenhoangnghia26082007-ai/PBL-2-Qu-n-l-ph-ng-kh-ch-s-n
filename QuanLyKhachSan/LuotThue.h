#pragma once
#include <string>
#include <vector>
#include "KhachHang.h"
#include "DichVu.h"

class Phong;

class LuotThue {
    std::string maLT;
    Phong* phong;
    std::string maPhong;
    Date ngayNhan, ngayTra;
    bool daTra_ = false;
    long long donGiaChot;
    KhachHang* khach;
    std::vector<SuDungDichVu> dichVu; // composition
public:
    LuotThue(std::string ma, Phong* p, std::string maP, KhachHang* kh, Date nhan, long long gia);

    bool themDichVu(const DichVu& dv, double soLuong, Date ngay = Date());
    long long dong(Date ngay);
    // Khoi phuc tu file (khong kiem tra nghiep vu)
    void khoiPhucTra(Date ngay, bool tra);
    void themDichVuDaCo(const DichVu& dv, double soLuong, Date ngay, long long thanhTien);
    int soDem() const;
    long long tienPhong() const;
    long long tienDichVu() const;
    long long tongTien() const { return tienPhong() + tienDichVu(); }

    const std::string& getMa() const { return maLT; }
    const std::string& getMaPhong() const { return maPhong; }
    Phong* getPhong() const { return phong; }
    KhachHang* getKhach() const { return khach; }
    bool daTra() const { return daTra_; }
    const Date& getNgayNhan() const { return ngayNhan; }
    const Date& getNgayTra() const { return ngayTra; }
    long long getDonGiaChot() const { return donGiaChot; }
    const std::vector<SuDungDichVu>& getDichVu() const { return dichVu; }
};
