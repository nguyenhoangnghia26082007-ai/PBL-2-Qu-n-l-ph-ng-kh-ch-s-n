#pragma once
#include <string>
#include "Common.h"

enum class HangThanhVien { THUONG, BAC, VANG, KIM_CUONG };

std::string tenHang(HangThanhVien h);
int tyLeGiamGia(HangThanhVien h); // phan tram

class KhachHang {
    std::string maKH, hoTen, cccd, soDienThoai, email, diaChi;
    int diemTichLuy = 0;
    HangThanhVien hang = HangThanhVien::THUONG;
public:
    KhachHang(std::string ma, std::string ten, std::string cccd_, std::string sdt, std::string email_);
    bool capNhatThongTin(const std::string& ten, const std::string& sdt, const std::string& mail);
    void congDiem(int diem);
    void capNhatHang();
    const std::string& getMa() const { return maKH; }
    const std::string& getHoTen() const { return hoTen; }
    int getDiem() const { return diemTichLuy; }
    HangThanhVien getHang() const { return hang; }
    const std::string& getCCCD() const { return cccd; }
    const std::string& getSDT() const { return soDienThoai; }
    const std::string& getEmail() const { return email; }
    const std::string& getDiaChi() const { return diaChi; }
    void datDiaChi(const std::string& dc) { diaChi = dc; }
    void datDiem(int diem); // khoi phuc tu file, tu cap nhat hang
    void hienThi() const;
};
