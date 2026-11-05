// RevenueScreen.cpp: Bieu do doanh thu theo loai phong, thuc thu hom nay, so luong hoa don.
#include "RevenueScreen.h"
#include <iomanip>
#include <sstream>
#include "Common.h"
#include "Theme.h"

RevenueScreen::RevenueScreen(AuthService& a, KhachSanService& s) : auth(a), service(s) {}

void RevenueScreen::taiLai() {
    service.thongKeBaoCaoDoanhThu(tk);
}

void RevenueScreen::Enter() {
    taiLai();
}

namespace {
void veTheBaoCao(Rectangle r, const std::string& tieuDe, const std::string& giaTri, Color mau) {
    Widgets::veThe(r, 14);
    DrawRectangleRounded({r.x, r.y, 6, r.height}, 1.0f, 6, mau);
    Theme::Chu(tieuDe, r.x + 20, r.y + 18, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    Theme::Chu(giaTri, r.x + 20, r.y + 50, Theme::CO_LON - 2, Theme::CHU, true);
}
} // namespace

void RevenueScreen::Draw(Rectangle vung) {
    float x = vung.x + 24, y = vung.y + 16, w = vung.width - 48, h = vung.height - 32;

    // =========================================================================
    // 1. HANG THE THONG KE NHANH (TOP)
    // =========================================================================
    float khe = 18;
    float rongThe = (w - khe * 3) / 4.0f;
    float caoThe = 104;

    veTheBaoCao({x + 0 * (rongThe + khe), y, rongThe, caoThe}, "Tổng doanh thu",
                dinhDangTien(tk.tongDoanhThu), Theme::CANH_BAO);
    veTheBaoCao({x + 1 * (rongThe + khe), y, rongThe, caoThe}, "Thực thu hôm nay",
                dinhDangTien(tk.thucThuHomNay), Theme::THANH_CONG);
    veTheBaoCao({x + 2 * (rongThe + khe), y, rongThe, caoThe}, "HĐ đã tất toán",
                std::to_string(tk.hoaDonDaTatToan) + " / " + std::to_string(tk.tongSoHoaDon), Theme::CHINH);
    veTheBaoCao({x + 3 * (rongThe + khe), y, rongThe, caoThe}, "HĐ chưa thu đủ",
                std::to_string(tk.hoaDonChuaThanhToan), Theme::LOI);

    y += caoThe + 24;
    float caoVungConLai = h - (y - (vung.y + 16));

    // =========================================================================
    // 2. KHU VUC BIEU DO DOANH THU THEO LOAI PHONG & CHI TIET
    // =========================================================================
    float rongTrai = (w - 20) * 0.58f;
    float rongPhai = w - rongTrai - 20;

    Rectangle theBieuDo = {x, y, rongTrai, caoVungConLai};
    Rectangle theBang = {x + rongTrai + 20, y, rongPhai, caoVungConLai};

    // Card Trai: Bieu do cot / thanh ngang
    Widgets::veThe(theBieuDo, 16);
    float bx = theBieuDo.x + 24, by = theBieuDo.y + 20, bw = theBieuDo.width - 48;
    Theme::Chu("Doanh thu theo loại phòng", bx, by, Theme::CO_VUA, Theme::CHU, true);
    Theme::Chu("Tỷ lệ đóng góp doanh thu của từng hạng phòng", bx, by + 32, Theme::CO_NHO + 1, Theme::CHU_PHU);

    float cy = by + 80;
    const Color mauCacLoai[4] = {Theme::CHINH, Theme::THANH_CONG, Theme::CANH_BAO, Color{147, 51, 234, 255}};

    for (size_t i = 0; i < tk.doanhThuTheoLoai.size(); ++i) {
        const auto& item = tk.doanhThuTheoLoai[i];
        Color mau = mauCacLoai[i % 4];

        // Nhan loai phong + Doanh thu
        Theme::Chu(item.tenLoai + " (" + item.loaiPhong + ")", bx, cy, Theme::CO_THUONG, Theme::CHU, true);

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << item.phanTram << "% (" << dinhDangTien(item.doanhThu) << ")";
        std::string strDT = ss.str();
        Vector2 kt = Theme::DoChu(strDT, Theme::CO_THUONG, true);
        Theme::Chu(strDT, bx + bw - kt.x, cy, Theme::CO_THUONG, mau, true);

        // Thanh bieu do
        cy += 28;
        Rectangle rThanhNen = {bx, cy, bw, 22};
        DrawRectangleRounded(rThanhNen, 1.0f, 6, Color{241, 245, 249, 255});

        float rongThanh = (item.phanTram > 0.0f) ? (bw * (item.phanTram / 100.0f)) : 0.0f;
        if (rongThanh > 0.0f) {
            Rectangle rThanhTien = {bx, cy, std::max(rongThanh, 16.0f), 22};
            DrawRectangleRounded(rThanhTien, 1.0f, 6, mau);
        }

        cy += 44;
    }

    // Card Phai: Bang tong hop & Ghi chu dinh nghia he thong
    Widgets::veThe(theBang, 16);
    float px = theBang.x + 24, py = theBang.y + 20, pw = theBang.width - 48;
    Theme::Chu("Quy chuẩn thống kê", px, py, Theme::CO_VUA, Theme::CHU, true);
    Theme::Chu("Thống nhất toàn hệ thống PBL", px, py + 32, Theme::CO_NHO + 1, Theme::CHU_PHU);

    float gy = py + 76;
    Rectangle rGhiChu = {px, gy, pw, 130};
    DrawRectangleRounded(rGhiChu, 0.15f, 6, Color{248, 250, 252, 255});
    DrawRectangleRoundedLinesEx(rGhiChu, 0.15f, 6, 1.0f, Theme::VIEN);

    Theme::Chu("• DOANH THU = Tiền phòng + Dịch vụ - Giảm giá", rGhiChu.x + 14, rGhiChu.y + 14, Theme::CO_NHO + 1, Theme::CHU, true);
    Theme::Chu("  (Tính theo các hóa đơn chưa hủy, chưa gồm thuế VAT).", rGhiChu.x + 14, rGhiChu.y + 36, Theme::CO_NHO, Theme::CHU_PHU);

    Theme::Chu("• THỰC THU = Tiền khách đã trả thực tế trong ngày", rGhiChu.x + 14, rGhiChu.y + 68, Theme::CO_NHO + 1, Theme::THANH_CONG, true);
    Theme::Chu("  (Đã gồm VAT, trừ khoản hoàn tiền theo dòng tiền ngân quỹ).", rGhiChu.x + 14, rGhiChu.y + 90, Theme::CO_NHO, Theme::CHU_PHU);

    gy += 150;
    Theme::Chu("Tình trạng hóa đơn:", px, gy, Theme::CO_THUONG, Theme::CHU, true);
    gy += 30;
    Theme::Chu("• Đã tất toán đủ: " + std::to_string(tk.hoaDonDaTatToan) + " hóa đơn", px, gy, Theme::CO_NHO + 1, Theme::THANH_CONG);
    gy += 26;
    Theme::Chu("• Chưa thanh toán: " + std::to_string(tk.hoaDonChuaThanhToan) + " hóa đơn", px, gy, Theme::CO_NHO + 1, Theme::LOI);
    gy += 26;
    Theme::Chu("• Đã hủy (bỏ qua): " + std::to_string(tk.hoaDonDaHuy) + " hóa đơn", px, gy, Theme::CO_NHO + 1, Theme::CHU_PHU);
}
