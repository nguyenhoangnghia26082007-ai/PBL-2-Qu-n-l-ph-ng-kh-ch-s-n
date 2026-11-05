#pragma once
// App: dieu phoi cua so, chuyen man hinh theo enum ManHinh, hop xac nhan dang xuat.
#include <map>
#include <memory>
#include "AuthService.h"
#include "KhachSanService.h"
#include "LoginScreen.h"
#include "ManHinh.h"
#include "Widgets.h"

class App {
    AuthService auth;
    KhachSanService service{auth};
    LoginScreen manDangNhap{auth};
    std::map<ManHinh, std::unique_ptr<ManHinhCoSo>> dsManHinh;
    ManHinh hienTai = ManHinh::DANG_NHAP;
    HopXacNhan hopDangXuat;

    void chuyenManHinh(ManHinh m);
    ManHinh manHinhMacDinhSauDangNhap() const;
public:
    void chay();
};
