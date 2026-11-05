#pragma once
// ManHinh: enum trang thai man hinh + lop co so cho moi man hinh (Update/Draw).
#include "raylib.h"

enum class ManHinh {
    DANG_NHAP,
    TONG_QUAN,
    NHAN_TRA_PHONG,
    KHACH_HANG,
    HOA_DON,
    DOANH_THU,
    NHAN_VIEN,
    TAI_KHOAN
};

class ManHinhCoSo {
public:
    virtual ~ManHinhCoSo() = default;
    virtual void Enter() {}               // goi moi lan chuyen toi man hinh nay (nap lai du lieu)
    virtual void Update() {}              // xu ly phim tat / logic truoc khi ve
    virtual void Draw(Rectangle vung) = 0; // ve trong vung noi dung (da tru sidebar + thanh tren)
};
