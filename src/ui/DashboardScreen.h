#pragma once
// DashboardScreen: trang Tong quan (cac the so lieu + luoi so do phong nhanh).
#include <vector>
#include "KhachSanService.h"
#include "ManHinh.h"

class DashboardScreen : public ManHinhCoSo {
    KhachSanService& service;
    ThongKeNhanh tk{};
    std::vector<PhongHienThi> dsPhong;
public:
    explicit DashboardScreen(KhachSanService& s) : service(s) {}
    void Enter() override;
    void Draw(Rectangle vung) override;
};
