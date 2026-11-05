#pragma once
// RoomScreen: Man hinh thong nhat "Nhan / Tra phong" (So do phong + Check-in / Check-out / Dich vu / Bao tri).
#include <string>
#include <vector>
#include "AuthService.h"
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class RoomScreen : public ManHinhCoSo {
    AuthService& auth;
    KhachSanService& service;

    std::vector<PhongHienThi> dsPhong;
    std::vector<PhongHienThi> dsPhongLoc;
    int boLocTrangThai = 0; // 0: Tat ca, 1: Trong, 2: Dang co khach, 3: Bao tri
    OTextNhap oTimKiem;

    std::string maPhongChon;
    ChiTietPhongHienThi chiTiet;

    // Check-in (Nhan phong)
    std::string maKhachNhan, tenKhachNhan;
    OTextNhap oNgayNhan;
    bool moPopupChonKhach = false;
    OTextNhap oTimKhach;
    std::vector<KhachHangNhanh> dsKhach;

    // Dich vu phong
    std::vector<DichVuMenuHienThi> dsDichVuMenu;
    int dichVuChonIdx = 0;
    OTextNhap oSoLuongDV;

    // Check-out (Tra phong)
    OTextNhap oNgayTra;
    HopXacNhan hopXacNhanTraPhong;

    // Quan ly phong (Them / Xoa)
    bool moModalThemPhong = false;
    OTextNhap oMaPhongMoi, oGiaPhongMoi, oNgayDuaMoi;
    int loaiPhongChonIdx = 0; // 0: DON, 1: DOI, 2: SUITE, 3: VIP
    HopXacNhan hopXoaPhong;

    void taiLaiDuLieu();
    void apDungBoLoc();
    void chonPhong(const std::string& ma);
public:
    RoomScreen(AuthService& a, KhachSanService& s);
    void Enter() override;
    void Update() override;
    void Draw(Rectangle vung) override;
};
