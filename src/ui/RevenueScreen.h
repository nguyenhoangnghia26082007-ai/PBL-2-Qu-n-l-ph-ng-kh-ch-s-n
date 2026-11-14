#pragma once
// RevenueScreen: Thong ke bao cao doanh thu theo thang/quy, theo loai phong, thuc thu theo ngay, thong ke hoa don.
#include <vector>
#include "AuthService.h"
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class RevenueScreen : public ManHinhCoSo {
    AuthService& auth;
    KhachSanService& service;

    ThongKeDoanhThuChiTiet tk;
    bool xemTheoQuy = false; // false: Theo thang (T1..T12), true: Theo quy (Q1..Q4)
    int namChon = 2026;
    std::vector<DoanhThuKyHienThi> dsKy;
    long long tongDoanhThuNam = 0;

    void taiLai();
    void taiTheoKy();
public:
    RevenueScreen(AuthService& a, KhachSanService& s);
    void Enter() override;
    void Draw(Rectangle vung) override;
};
