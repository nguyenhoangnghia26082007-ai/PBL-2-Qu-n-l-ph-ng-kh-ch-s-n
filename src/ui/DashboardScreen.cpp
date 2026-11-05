// DashboardScreen.cpp: hien the thong ke (ẩn doanh thu neu khong phai Quan ly) va so do phong.
#include "DashboardScreen.h"
#include "Common.h"
#include "LuoiPhong.h"
#include "Theme.h"
#include "Widgets.h"

void DashboardScreen::Enter() {
    service.thongKeNhanh(tk);
    service.danhSachPhong(dsPhong);
}

namespace {
void veTheSo(Rectangle r, const std::string& nhan, const std::string& giaTri, Color mauNhan) {
    Widgets::veThe(r, 14);
    DrawRectangleRounded({r.x, r.y, 6, r.height}, 1.0f, 6, mauNhan);
    Theme::Chu(nhan, r.x + 22, r.y + 18, Theme::CO_NHO + 2, Theme::CHU_PHU);
    Theme::Chu(giaTri, r.x + 22, r.y + 50, Theme::CO_LON, Theme::CHU, true);
}
} // namespace

void DashboardScreen::Draw(Rectangle vung) {
    float x = vung.x + 28, y = vung.y + 24, rong = vung.width - 56;

    // ---- Hang the thong ke ----
    int soThe = tk.coDoanhThu ? 4 : 3;
    float khe = 18;
    float rongThe = (rong - khe * (soThe - 1)) / soThe;
    float caoThe = 104;

    veTheSo({x + 0 * (rongThe + khe), y, rongThe, caoThe}, "Phòng trống",
            std::to_string(tk.phongTrong) + " / " + std::to_string(tk.tongPhong), Theme::THANH_CONG);
    veTheSo({x + 1 * (rongThe + khe), y, rongThe, caoThe}, "Đang có khách",
            std::to_string(tk.phongCoKhach), Theme::LOI);
    veTheSo({x + 2 * (rongThe + khe), y, rongThe, caoThe}, "Khách hàng",
            std::to_string(tk.soKhach), Theme::CHINH);
    if (tk.coDoanhThu) {
        veTheSo({x + 3 * (rongThe + khe), y, rongThe, caoThe}, "Tổng doanh thu",
                dinhDangTien(tk.doanhThu), Theme::CANH_BAO);
    }

    // ---- So do phong nhanh ----
    y += caoThe + 28;
    Theme::Chu("Trạng thái phòng hiện tại", x, y, Theme::CO_VUA, Theme::CHU, true);
    LuoiPhong::veChuGiai(x + 310, y + 3);

    y += 42;
    LuoiPhong::ve({x, y, rong, vung.height - (y - vung.y) - 20}, dsPhong);
}
