#pragma once
// PhanQuyen: bang quyen theo vai tro. Ca UI (an/hien menu) va Service (chan thao tac) deu dung bang nay.
#include <string>
#include "TaiKhoan.h"

enum class Quyen {
    XEM_TONG_QUAN,              // trang tong quan
    XEM_SO_DO_PHONG,            // xem luoi phong
    CAP_NHAT_TRANG_THAI_PHONG,  // don phong / bao tri
    QUAN_LY_PHONG,              // them / xoa / sua gia phong
    NHAN_TRA_PHONG,             // dat phong, check-in, check-out, them dich vu
    QUAN_LY_KHACH,              // khach hang
    QUAN_LY_HOA_DON,            // lap / thanh toan / in hoa don
    HUY_HOA_DON,                // huy hoa don
    XEM_DOANH_THU,              // thong ke doanh thu
    QUAN_LY_NHAN_VIEN,          // nhan vien
    QUAN_LY_TAI_KHOAN           // tai khoan dang nhap
};

namespace PhanQuyen {
    bool coQuyen(VaiTro vt, Quyen q);
    std::string tenVaiTro(VaiTro vt); // nhan tieng Viet: "Quản lý"...
}
