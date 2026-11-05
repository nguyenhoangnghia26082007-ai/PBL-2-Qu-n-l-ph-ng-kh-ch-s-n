#include "NhanVien.h"
#include <iostream>

std::string tenChucVu(ChucVu c) {
    switch (c) {
        case ChucVu::LE_TAN: return "LE_TAN";
        case ChucVu::THU_NGAN: return "THU_NGAN";
        case ChucVu::QUAN_LY: return "QUAN_LY";
        default: return "KE_TOAN";
    }
}

NhanVien::NhanVien(std::string ma, std::string ten, ChucVu cv, Date ngay)
    : maNV(std::move(ma)), hoTen(std::move(ten)), ngayVaoLam(ngay), chucVu(cv) {}

void NhanVien::hienThi() const {
    std::cout << maNV << " | " << hoTen << " | " << tenChucVu(chucVu)
              << " | da lap " << soHoaDonDaLap << " hoa don\n";
}
