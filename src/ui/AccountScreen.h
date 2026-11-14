#pragma once
// AccountScreen: man hinh Quan ly tai khoan (mo khoa tai khoan bi khoa 5 lan, doi mat khau, sua thong tin, them/xoa).
#include <vector>
#include "AuthService.h"
#include "ManHinh.h"
#include "Widgets.h"

class AccountScreen : public ManHinhCoSo {
    AuthService& auth;
    std::vector<TaiKhoan> ds;
    int dangChon = -1;

    OTextNhap oTenDN, oMatKhau, oHoTen, oMaNV;
    VaiTro vaiTroChon = VaiTro::LE_TAN;
    HopXacNhan hopXoa;
    bool hienMatKhau = false; // false: che mat khau bang dau * (bam "Hiện" de xem)

    void taiLai();
    void chonTaiKhoan(int idx);
    void datFormThemMoi();
public:
    explicit AccountScreen(AuthService& a);
    void Enter() override;
    void Draw(Rectangle vung) override;
};
