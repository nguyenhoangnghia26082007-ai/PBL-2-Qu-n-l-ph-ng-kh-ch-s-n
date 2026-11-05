#pragma once
// LoginScreen: trang dang nhap chia doi (trai: anh khach san, phai: form dang nhap).
#include <string>
#include "AuthService.h"
#include "ManHinh.h"
#include "Widgets.h"

class LoginScreen : public ManHinhCoSo {
    AuthService& auth;
    Texture2D anh{};
    OTextNhap oTen, oMatKhau;
    bool hienMatKhau = false;
    std::string loi;           // dong bao loi mau do
    bool vuaThanhCong = false;

    void thuDangNhap();
public:
    explicit LoginScreen(AuthService& a);
    void napTaiNguyen();       // goi sau InitWindow
    void giaiPhong();

    void Enter() override;
    void Update() override;
    void Draw(Rectangle vung) override;

    // true dung 1 lan ngay sau khi dang nhap thanh cong
    bool layVaXoaCoThanhCong();
};
