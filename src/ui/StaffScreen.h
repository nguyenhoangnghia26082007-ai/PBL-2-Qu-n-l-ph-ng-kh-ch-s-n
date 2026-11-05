#pragma once
// StaffScreen: man hinh Quan ly nhan vien (danh sach + them moi + cap nhat thong tin + xoa).
#include <vector>
#include "KhachSanService.h"
#include "ManHinh.h"
#include "Widgets.h"

class StaffScreen : public ManHinhCoSo {
    KhachSanService& service;
    std::vector<NhanVienHienThi> ds;
    int dangChon = -1; // -1: che do them moi, >=0: dang sua nhan vien

    OTextNhap oMa, oHoTen, oCCCD, oSDT, oNgay;
    ChucVu chucVuChon = ChucVu::LE_TAN;
    HopXacNhan hopXoa;

    void taiLai();
    void chonNhanVien(int idx);
    void datFormThemMoi();
public:
    explicit StaffScreen(KhachSanService& s);
    void Enter() override;
    void Draw(Rectangle vung) override;
};
