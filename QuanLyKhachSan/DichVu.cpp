#include "DichVu.h"
#include <iostream>
#include <algorithm>
#include <cmath>

DichVu::DichVu(std::string ma, std::string ten, long long gia, std::string dvt)
    : maDV(std::move(ma)), tenDV(std::move(ten)), donGia(gia), donViTinh(std::move(dvt)) {}

void DichVu::hienThi() const {
    std::cout << maDV << " | " << tenDV << " | " << dinhDangTien(donGia) << "/" << donViTinh << "\n";
}

DichVuAnUong::DichVuAnUong(std::string ma, std::string ten, long long gia, double phi)
    : DichVu(std::move(ma), std::move(ten), gia, "phan"), phiPhucVu(phi) {}
long long DichVuAnUong::tinhTien(double sl) const { return std::llround(donGia * sl * (1.0 + phiPhucVu)); }

DichVuGiatUi::DichVuGiatUi(std::string ma, std::string ten, long long gia, long long phiTT)
    : DichVu(std::move(ma), std::move(ten), gia, "kg"), phiToiThieu(phiTT) {}
long long DichVuGiatUi::tinhTien(double sl) const {
    return std::max(phiToiThieu, (long long)std::llround(donGia * sl));
}

DichVuSpa::DichVuSpa(std::string ma, std::string ten, long long gia, double he)
    : DichVu(std::move(ma), std::move(ten), gia, "gio"), heSoNgoaiGio(he) {}
long long DichVuSpa::tinhTien(double sl) const { return std::llround(donGia * sl * heSoNgoaiGio); }

DichVuDuaDon::DichVuDuaDon(std::string ma, std::string ten, long long gia, long long phiMo)
    : DichVu(std::move(ma), std::move(ten), gia, "km"), phiMoCua(phiMo) {}
long long DichVuDuaDon::tinhTien(double sl) const { return phiMoCua + (long long)std::llround(donGia * sl); }

SuDungDichVu::SuDungDichVu(const DichVu& d, double sl, Date t)
    : dv(&d), tenDichVu(d.getTen()), soLuong(sl), thoiDiem(t), thanhTien(d.tinhTien(sl)) {}

SuDungDichVu::SuDungDichVu(const DichVu& d, double sl, Date t, long long tt)
    : dv(&d), tenDichVu(d.getTen()), soLuong(sl), thoiDiem(t), thanhTien(tt) {}
