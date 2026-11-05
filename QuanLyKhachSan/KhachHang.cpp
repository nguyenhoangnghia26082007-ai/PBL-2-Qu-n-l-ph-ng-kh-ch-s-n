#include "KhachHang.h"
#include <iostream>

std::string tenHang(HangThanhVien h) {
    switch (h) {
        case HangThanhVien::THUONG: return "THUONG";
        case HangThanhVien::BAC: return "BAC";
        case HangThanhVien::VANG: return "VANG";
        default: return "KIM_CUONG";
    }
}

int tyLeGiamGia(HangThanhVien h) {
    switch (h) {
        case HangThanhVien::THUONG: return 0;
        case HangThanhVien::BAC: return 3;
        case HangThanhVien::VANG: return 5;
        default: return 10;
    }
}

KhachHang::KhachHang(std::string ma, std::string ten, std::string cccd_, std::string sdt, std::string email_)
    : maKH(std::move(ma)), hoTen(std::move(ten)), cccd(std::move(cccd_)),
      soDienThoai(std::move(sdt)), email(std::move(email_)) {}

bool KhachHang::capNhatThongTin(const std::string& ten, const std::string& sdt, const std::string& mail) {
    if (ten.empty() || sdt.empty()) return false;
    hoTen = ten; soDienThoai = sdt; email = mail;
    return true;
}

void KhachHang::congDiem(int diem) {
    if (diem > 0) { diemTichLuy += diem; capNhatHang(); }
}

void KhachHang::datDiem(int diem) {
    diemTichLuy = diem < 0 ? 0 : diem;
    capNhatHang();
}

void KhachHang::capNhatHang() {
    if (diemTichLuy >= 1000) hang = HangThanhVien::KIM_CUONG;
    else if (diemTichLuy >= 500) hang = HangThanhVien::VANG;
    else if (diemTichLuy >= 100) hang = HangThanhVien::BAC;
    else hang = HangThanhVien::THUONG;
}

void KhachHang::hienThi() const {
    std::cout << maKH << " | " << hoTen << " | " << soDienThoai << " | hang " << tenHang(hang)
              << " (" << diemTichLuy << " diem)\n";
}
