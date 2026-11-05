#pragma once
// RevenueScreen: Thong ke bao cao doanh thu theo loai phong, thuc thu theo ngay, thong ke hoa don.
#include "AuthService.h"
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class RevenueScreen : public ManHinhCoSo {
    AuthService& auth;
    KhachSanService& service;

    ThongKeDoanhThuChiTiet tk;
    void taiLai();
public:
    RevenueScreen(AuthService& a, KhachSanService& s);
    void Enter() override;
    void Draw(Rectangle vung) override;
};
