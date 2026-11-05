// App.cpp: vong lap chinh raylib + định nghĩa RAYGUI_IMPLEMENTATION duy nhat tai day.
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "App.h"
#include "AccountScreen.h"
#include "CustomerScreen.h"
#include "DashboardScreen.h"
#include "InvoiceScreen.h"
#include "Layout.h"
#include "RevenueScreen.h"
#include "RoomScreen.h"
#include "StaffScreen.h"
#include "Theme.h"

void App::chuyenManHinh(ManHinh m) {
    hienTai = m;
    if (m == ManHinh::DANG_NHAP) manDangNhap.Enter();
    else if (auto it = dsManHinh.find(m); it != dsManHinh.end()) it->second->Enter();
}

ManHinh App::manHinhMacDinhSauDangNhap() const {
    // Buong phong khong co quyen xem Tong quan -> vao thang Nhan / Tra phong
    return auth.coQuyen(Quyen::XEM_TONG_QUAN) ? ManHinh::TONG_QUAN : ManHinh::NHAN_TRA_PHONG;
}

void App::chay() {
    SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);
    InitWindow(1280, 780, "Quản Lý Khách Sạn Bách Khoa - Đà Nẵng (PBL)");
    SetWindowMinSize(1024, 660);
    SetTargetFPS(60);
    SetExitKey(KEY_NULL); // khong tat cua so khi nhan ESC

    Theme::napFont();
    Theme::apDungRaygui();
    manDangNhap.napTaiNguyen();

    std::string tb1 = auth.khoiDong();
    std::string tb2 = service.khoiDong();
    if (!tb1.empty()) ThongBao::hien(tb1, ThongBao::Loai::THONG_TIN);
    if (!tb2.empty()) ThongBao::hien(tb2, ThongBao::Loai::THONG_TIN);

    // Dang ky 7 man hinh he thong (So do phong + Nhan/Tra phong da gop lam 1 tai NHAN_TRA_PHONG)
    dsManHinh[ManHinh::TONG_QUAN] = std::make_unique<DashboardScreen>(service);
    dsManHinh[ManHinh::NHAN_TRA_PHONG] = std::make_unique<RoomScreen>(auth, service);
    dsManHinh[ManHinh::KHACH_HANG] = std::make_unique<CustomerScreen>(service);
    dsManHinh[ManHinh::HOA_DON] = std::make_unique<InvoiceScreen>(auth, service);
    dsManHinh[ManHinh::DOANH_THU] = std::make_unique<RevenueScreen>(auth, service);
    dsManHinh[ManHinh::NHAN_VIEN] = std::make_unique<StaffScreen>(service);
    dsManHinh[ManHinh::TAI_KHOAN] = std::make_unique<AccountScreen>(auth);

    chuyenManHinh(ManHinh::DANG_NHAP);

    while (!WindowShouldClose()) {
        Widgets::batDauKhung(hopDangXuat.mo);

        // ---- Update ----
        if (hienTai == ManHinh::DANG_NHAP) {
            manDangNhap.Update();
            if (manDangNhap.layVaXoaCoThanhCong()) chuyenManHinh(manHinhMacDinhSauDangNhap());
        } else if (auto it = dsManHinh.find(hienTai); it != dsManHinh.end()) {
            it->second->Update();
        }

        // ---- Draw ----
        BeginDrawing();
        ClearBackground(Theme::NEN);

        if (hienTai == ManHinh::DANG_NHAP) {
            manDangNhap.Draw({0, 0, (float)GetScreenWidth(), (float)GetScreenHeight()});
        } else {
            ManHinh tiep = hienTai;
            bool bamDX = false;
            Rectangle vung = Layout::veKhungChinh(auth, hienTai, tiep, bamDX);
            if (auto it = dsManHinh.find(hienTai); it != dsManHinh.end()) it->second->Draw(vung);
            if (tiep != hienTai) chuyenManHinh(tiep);
            if (bamDX) hopDangXuat.moHop("Đăng xuất", "Bạn có chắc muốn đăng xuất khỏi phiên làm việc?", "Đăng xuất", true);
        }

        if (hopDangXuat.ve() == 1) {
            auth.dangXuat();
            chuyenManHinh(ManHinh::DANG_NHAP);
            ThongBao::hien("Đã đăng xuất.", ThongBao::Loai::THONG_TIN);
        }
        ThongBao::ve(hienTai == ManHinh::DANG_NHAP ? 0.0f : Layout::CAO_TOPBAR);
        Widgets::ketThucKhung();
        EndDrawing();
    }

    service.luu();
    manDangNhap.giaiPhong();
    Theme::giaiPhongFont();
    CloseWindow();
}
