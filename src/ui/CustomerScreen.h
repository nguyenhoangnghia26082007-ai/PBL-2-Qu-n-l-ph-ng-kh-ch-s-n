#pragma once
// CustomerScreen: Quan ly khach hang, tim kiem, them moi, sua thong tin, tich diem & hang thanh vien.
#include <string>
#include <vector>
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class CustomerScreen : public ManHinhCoSo {
    KhachSanService& service;
    std::vector<KhachHangHienThi> dsKhach;
    std::vector<KhachHangHienThi> dsKhachLoc;
    int dangChon = -1;

    OTextNhap oTimKiem;
    OTextNhap oMa, oHoTen, oCCCD, oSDT, oEmail, oDiaChi;
    HopXacNhan hopXoa;

    void taiLai();
    void apDungLoc();
    void chonKhach(int idx);
    void datFormThemMoi();
public:
    explicit CustomerScreen(KhachSanService& s);
    void Enter() override;
    void Update() override;
    void Draw(Rectangle vung) override;
};
