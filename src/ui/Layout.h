#pragma once
// Layout: ve Sidebar ben trai (chi hien chuc nang vai tro duoc phep) va Thanh tren (ten + vai tro + Dang xuat).
#include <string>
#include "AuthService.h"
#include "ManHinh.h"
#include "raylib.h"

namespace Layout {
    const float RONG_SIDEBAR = 250.0f;
    const float CAO_TOPBAR   = 68.0f;

    // Ve sidebar + topbar.
    // - doiManHinh: duoc gan man hinh moi neu nguoi dung bam menu.
    // - bamDangXuat: true neu nguoi dung bam nut Dang xuat.
    // Tra ve hinh chu nhat vung noi dung con lai (da tru sidebar va topbar).
    Rectangle veKhungChinh(const AuthService& auth, ManHinh hienTai, ManHinh& doiManHinh, bool& bamDangXuat);
}
