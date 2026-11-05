// Layout.cpp: Sidebar loc theo quyen + Topbar hien ten, vai tro va nut Dang xuat.
#include "Layout.h"
#include "PhanQuyen.h"
#include "Theme.h"
#include "Widgets.h"

namespace {
struct MucMenu {
    ManHinh manHinh;
    Quyen quyenCan;
    const char* nhan;
};

const MucMenu DANH_SACH_MENU[] = {
    {ManHinh::TONG_QUAN,      Quyen::XEM_TONG_QUAN,     "Tổng quan"},
    {ManHinh::NHAN_TRA_PHONG, Quyen::NHAN_TRA_PHONG,    "Nhận / Trả phòng"},
    {ManHinh::KHACH_HANG,     Quyen::QUAN_LY_KHACH,     "Khách hàng"},
    {ManHinh::HOA_DON,        Quyen::QUAN_LY_HOA_DON,   "Hóa đơn & TT"},
    {ManHinh::DOANH_THU,      Quyen::XEM_DOANH_THU,     "Doanh thu"},
    {ManHinh::NHAN_VIEN,      Quyen::QUAN_LY_NHAN_VIEN, "Nhân viên"},
    {ManHinh::TAI_KHOAN,      Quyen::QUAN_LY_TAI_KHOAN, "Tài khoản"},
};
} // namespace

Rectangle Layout::veKhungChinh(const AuthService& auth, ManHinh hienTai, ManHinh& doiManHinh, bool& bamDangXuat) {
    doiManHinh = hienTai;
    bamDangXuat = false;
    float w = (float)GetScreenWidth(), h = (float)GetScreenHeight();

    // ---------- 1. Sidebar ben trai ----------
    Rectangle sb = {0, 0, RONG_SIDEBAR, h};
    DrawRectangleRec(sb, Theme::SIDEBAR);

    // Logo / ten khach san
    Theme::Chu("KS BÁCH KHOA", 24, 20, Theme::CO_VUA, WHITE, true);
    Theme::Chu("54 Nguyễn Lương Bằng, ĐN", 24, 48, Theme::CO_NHO - 2, Theme::CHU_PHU);
    DrawLine(20, 78, (int)RONG_SIDEBAR - 20, 78, Color{30, 41, 59, 255});

    float y = 96;
    for (const auto& m : DANH_SACH_MENU) {
        if (!auth.coQuyen(m.quyenCan)) continue; // an chuc nang khong du quyen

        Rectangle nut = {14, y, RONG_SIDEBAR - 28, 46};
        bool chon = (hienTai == m.manHinh);
        bool hover = Widgets::diChuotVao(nut);
        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

        Color nen = chon ? Theme::SIDEBAR_CHON : (hover ? Theme::SIDEBAR_HOVER : BLANK);
        if (nen.a) DrawRectangleRounded(nut, Widgets::boGoc(nut, 10), 8, nen);
        Theme::Chu(m.nhan, nut.x + 18, nut.y + 12, Theme::CO_THUONG, chon ? WHITE : Theme::CHU_SIDEBAR, chon);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) doiManHinh = m.manHinh;
        y += 54;
    }

    // ---------- 2. Thanh tren (Topbar) ----------
    Rectangle tb = {RONG_SIDEBAR, 0, w - RONG_SIDEBAR, CAO_TOPBAR};
    DrawRectangleRec(tb, Theme::TRANG);
    DrawLine((int)tb.x, (int)CAO_TOPBAR - 1, (int)w, (int)CAO_TOPBAR - 1, Theme::VIEN);

    // Tieu de man hinh hien tai ben trai topbar
    const char* tieuDe = "Hệ thống quản lý";
    for (const auto& m : DANH_SACH_MENU) if (m.manHinh == hienTai) { tieuDe = m.nhan; break; }
    Theme::Chu(tieuDe, tb.x + 28, 20, Theme::CO_VUA, Theme::CHU, true);

    // Thong tin nguoi dung + vai tro + nut Dang xuat ben phai
    const Phien& p = auth.phienHienTai();
    Rectangle nutDX = {w - 140, 14, 116, 40};
    if (VeNut(nutDX, "Đăng xuất", KieuNut::PHU, true, Theme::CO_NHO + 1)) bamDangXuat = true;

    std::string vt = PhanQuyen::tenVaiTro(p.vaiTro);
    Vector2 ktVT = Theme::DoChu(vt, Theme::CO_NHO, true);
    Rectangle nhanVT = {nutDX.x - ktVT.x - 36, 20, ktVT.x + 20, 28};
    Color mauVT = p.vaiTro == VaiTro::QUAN_LY ? Theme::CHINH :
                  p.vaiTro == VaiTro::LE_TAN  ? Theme::THANH_CONG : Theme::CANH_BAO;
    DrawRectangleRounded(nhanVT, 1.0f, 8, Fade(mauVT, 0.14f));
    Theme::ChuGiua(vt, nhanVT, Theme::CO_NHO, mauVT, true);

    Vector2 ktTen = Theme::DoChu(p.hoTen, Theme::CO_THUONG, true);
    Theme::Chu(p.hoTen, nhanVT.x - ktTen.x - 16, 22, Theme::CO_THUONG, Theme::CHU, true);

    // ---------- 3. Vung noi dung ----------
    Rectangle vung = {RONG_SIDEBAR, CAO_TOPBAR, w - RONG_SIDEBAR, h - CAO_TOPBAR};
    DrawRectangleRec(vung, Theme::NEN);
    return vung;
}
