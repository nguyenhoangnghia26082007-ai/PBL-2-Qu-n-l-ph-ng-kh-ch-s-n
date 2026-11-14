#pragma once
#include <string>
#include "Common.h"

enum class ChucVu { LE_TAN, THU_NGAN, QUAN_LY, KE_TOAN };

std::string tenChucVu(ChucVu c);

class NhanVien {
    std::string maNV, hoTen, cccd, soDienThoai;
    Date ngayVaoLam;
    ChucVu chucVu;
    int soHoaDonDaLap = 0;
public:
    NhanVien(std::string ma, std::string ten, ChucVu cv, Date ngay = Date());
    void ghiNhanHoaDon() { ++soHoaDonDaLap; }
    const std::string& getMa() const { return maNV; }
    const std::string& getHoTen() const { return hoTen; }
    ChucVu getChucVu() const { return chucVu; }
    int getSoHoaDonDaLap() const { return soHoaDonDaLap; }
    // Phan quyen theo chuc vu (Le tan, Thu ngan, Quan ly deu co the thu tien khach)
    bool coTheThuTien() const {
        return chucVu == ChucVu::THU_NGAN || chucVu == ChucVu::QUAN_LY || chucVu == ChucVu::LE_TAN;
    }
    bool coTheHuyHoaDon() const { return chucVu == ChucVu::QUAN_LY; }
    const std::string& getCCCD() const { return cccd; }
    const std::string& getSDT() const { return soDienThoai; }
    const Date& getNgayVaoLam() const { return ngayVaoLam; }
    // Khoi phuc tu file
    void khoiPhuc(const std::string& cccd_, const std::string& sdt, int soHD) {
        cccd = cccd_; soDienThoai = sdt; soHoaDonDaLap = soHD;
    }
    // Cap nhat thong tin nhan vien (Quan ly su dung tren giao dien)
    bool capNhatThongTin(const std::string& ten, const std::string& cccd_, const std::string& sdt,
                         ChucVu cv, Date ngay) {
        if (ten.empty()) return false;
        hoTen = ten; cccd = cccd_; soDienThoai = sdt; chucVu = cv; ngayVaoLam = ngay;
        return true;
    }
    void hienThi() const;
};
