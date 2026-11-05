#pragma once
// LuoiPhong: ve so do phong dang luoi, to mau theo trang thai. Dung chung cho Tong quan va So do phong.
#include <string>
#include <vector>
#include "KhachSanService.h"
#include "raylib.h"

namespace LuoiPhong {
    // Nhan tieng Viet cho du lieu cua ban console
    std::string tenLoai(const std::string& maLoai);
    std::string tenTrangThai(TinhTrangPhong t);
    Color mauTrangThai(TinhTrangPhong t);

    // Ve luoi trong vung; tra ve chi so phong vua bam (-1 neu khong bam). Tra ve chieu cao da dung qua caoDaDung.
    int ve(Rectangle vung, const std::vector<PhongHienThi>& ds, int dangChon = -1, float* caoDaDung = nullptr);
    // Chu giai mau (Trong / Co khach / Bao tri)
    void veChuGiai(float x, float y);
}
