#pragma once
#include <string>
#include "Common.h"

class DichVu {
protected:
    std::string maDV, tenDV;
    long long donGia;
    std::string donViTinh;
public:
    DichVu(std::string ma, std::string ten, long long gia, std::string dvt);
    virtual ~DichVu() = default;
    virtual long long tinhTien(double soLuong) const = 0;
    virtual std::string loaiDV() const = 0;  // ANUONG / GIATUI / SPA / DUADON (dung khi luu file)
    virtual double thamSo() const = 0;       // tham so rieng cua tung loai (phi, he so...)
    const std::string& getMa() const { return maDV; }
    const std::string& getTen() const { return tenDV; }
    long long getDonGia() const { return donGia; }
    virtual void hienThi() const;
};

class DichVuAnUong : public DichVu {
    double phiPhucVu; // 0.05 = 5%
public:
    DichVuAnUong(std::string ma, std::string ten, long long gia, double phi = 0.05);
    long long tinhTien(double soLuong) const override;
    std::string loaiDV() const override { return "ANUONG"; }
    double thamSo() const override { return phiPhucVu; }
};

class DichVuGiatUi : public DichVu {
    long long phiToiThieu;
public:
    DichVuGiatUi(std::string ma, std::string ten, long long gia, long long phiTT = 30000);
    long long tinhTien(double soLuong) const override;
    std::string loaiDV() const override { return "GIATUI"; }
    double thamSo() const override { return (double)phiToiThieu; }
};

class DichVuSpa : public DichVu {
    double heSoNgoaiGio;
public:
    DichVuSpa(std::string ma, std::string ten, long long gia, double he = 1.0);
    long long tinhTien(double soLuong) const override;
    std::string loaiDV() const override { return "SPA"; }
    double thamSo() const override { return heSoNgoaiGio; }
};

class DichVuDuaDon : public DichVu {
    long long phiMoCua;
public:
    DichVuDuaDon(std::string ma, std::string ten, long long gia, long long phiMo = 20000);
    long long tinhTien(double soLuong) const override;
    std::string loaiDV() const override { return "DUADON"; }
    double thamSo() const override { return (double)phiMoCua; }
};

class SuDungDichVu {
    const DichVu* dv;
    std::string tenDichVu;
    double soLuong;
    Date thoiDiem;
    long long thanhTien;
public:
    SuDungDichVu(const DichVu& d, double sl, Date t);
    // Khoi phuc tu file: giu nguyen thanh tien da chot luc phat sinh
    SuDungDichVu(const DichVu& d, double sl, Date t, long long tt);
    const DichVu* getDichVu() const { return dv; }
    const std::string& getTenDichVu() const { return tenDichVu; }
    double getSoLuong() const { return soLuong; }
    const Date& getThoiDiem() const { return thoiDiem; }
    long long getThanhTien() const { return thanhTien; }
};
