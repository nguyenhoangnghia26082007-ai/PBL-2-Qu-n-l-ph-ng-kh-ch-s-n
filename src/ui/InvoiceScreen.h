#pragma once
// InvoiceScreen: Quan ly hoa don, thanh toan tien phong & dich vu, in hoa don, huy hoa don.
#include <string>
#include <vector>
#include "AuthService.h"
#include "HoaDon.h"
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class InvoiceScreen : public ManHinhCoSo {
    AuthService& auth;
    KhachSanService& service;

    std::vector<HoaDonHienThi> dsHoaDon;
    std::vector<HoaDonHienThi> dsHoaDonLoc;
    int boLocTrangThai = 0; // 0: Tat ca, 1: Chua xong, 2: Da xong, 3: Da huy
    OTextNhap oTimKiem;

    std::string maHDChon;
    ChiTietHoaDonHienThi chiTiet;

    // Form thanh toan
    OTextNhap oSoTienThu, oSoThamChieu, oNgayThu;
    int hinhThucChonIdx = 0; // 0: TIEN_MAT, 1: CHUYEN_KHOAN, 2: THE, 3: VI_DIEN_TU, 4: QR

    HopXacNhan hopHuyHD;

    // Xem phieu hoa don tren app (Modal Receipt Preview)
    bool moModalXemIn = false;
    std::string noiDungXemIn;

    void taiLai();
    void apDungLoc();
    void chonHoaDon(const std::string& maHD);
public:
    InvoiceScreen(AuthService& a, KhachSanService& s);
    void Enter() override;
    void Update() override;
    void Draw(Rectangle vung) override;
};
