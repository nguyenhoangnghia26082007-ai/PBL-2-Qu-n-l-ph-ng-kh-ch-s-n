#pragma once
// PlaceholderScreen: man hinh giu cho cho cac buoc tiep theo (van hien so do phong cho SO_DO_PHONG).
#include <string>
#include <vector>
#include "KhachSanService.h"
#include "ManHinh.h"

class PlaceholderScreen : public ManHinhCoSo {
    KhachSanService& service;
    std::string tieuDe, moTa;
    bool hienLuoiPhong;
    std::vector<PhongHienThi> dsPhong;
    int dangChon = -1;
public:
    PlaceholderScreen(KhachSanService& s, std::string td, std::string mt, bool luoi = false)
        : service(s), tieuDe(std::move(td)), moTa(std::move(mt)), hienLuoiPhong(luoi) {}
    void Enter() override;
    void Draw(Rectangle vung) override;
};
