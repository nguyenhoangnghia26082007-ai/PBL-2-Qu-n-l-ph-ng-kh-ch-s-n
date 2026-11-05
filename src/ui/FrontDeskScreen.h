#pragma once
// FrontDeskScreen: Man hinh chuyen trach ban Le tan (Nhan phong / Tra phong / Goi dich vu).
// Tach biet hoan toan voi So do phong de khong bi trung lap giao dien.
#include <string>
#include <vector>
#include "AuthService.h"
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class FrontDeskScreen : public ManHinhCoSo {
    AuthService& auth;
    KhachSanService& service;

    // Danh sach cac phong dang co khach o (Active stays)
    std::vector<PhongHienThi> dsPhongDangO;
    std::vector<PhongHienThi> dsPhongTrong;
    std::vector<KhachHangNhanh> dsKhach;
    std::vector<DichVuMenuHienThi> dsDichVuMenu;

    OTextNhap oTimKiem;
    int tabThaoTac = 0; // 0: Nhan phong moi (Check-in), 1: Tra phong & Dich vu (Check-out)

    // Form Check-in (Nhan phong)
    int phongTrongChonIdx = 0;
    std::string maKhachNhan, tenKhachNhan;
    OTextNhap oNgayNhan;
    bool moPopupChonKhach = false;
    OTextNhap oTimKhach;

    // Form Phong dang o & Tra phong / Dich vu
    std::string maPhongDangChon;
    ChiTietPhongHienThi chiTietDangO;
    int dichVuChonIdx = 0;
    OTextNhap oSoLuongDV;
    OTextNhap oNgayTra;
    HopXacNhan hopXacNhanTraPhong;

    void taiLai();
    void chonPhongDangO(const std::string& maPhong);
public:
    FrontDeskScreen(AuthService& a, KhachSanService& s);
    void Enter() override;
    void Update() override;
    void Draw(Rectangle vung) override;
};
