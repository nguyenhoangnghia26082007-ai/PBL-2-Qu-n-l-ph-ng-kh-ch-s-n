// RevenueScreen.cpp: Bieu do doanh thu theo thang/quy, theo loai phong, thuc thu hom nay, so luong hoa don.
#include "RevenueScreen.h"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include "Common.h"
#include "LuoiPhong.h"
#include "Theme.h"

RevenueScreen::RevenueScreen(AuthService& a, KhachSanService& s) : auth(a), service(s) {
    namChon = Date::homNay().y;
}

void RevenueScreen::taiTheoKy() {
    service.thongKeDoanhThuTheoKy(namChon, xemTheoQuy, dsKy, tongDoanhThuNam);
}

void RevenueScreen::taiLai() {
    service.thongKeBaoCaoDoanhThu(tk);
    taiTheoKy();
}

void RevenueScreen::Enter() {
    namChon = Date::homNay().y;
    taiLai();
}

namespace {
void veTheBaoCao(Rectangle r, const std::string& tieuDe, const std::string& giaTri, Color mau,
                 const std::string& phuDe = "") {
    Widgets::veThe(r, 14);
    DrawRectangleRounded({r.x, r.y, 6, r.height}, 1.0f, 6, mau);
    Theme::Chu(tieuDe, r.x + 20, r.y + 14, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    Theme::Chu(giaTri, r.x + 20, r.y + 42, Theme::CO_LON - 2, Theme::CHU, true);
    if (!phuDe.empty()) {
        Theme::Chu(phuDe, r.x + 20, r.y + 74, Theme::CO_NHO - 1, Theme::CHU_PHU);
    }
}
} // namespace

void RevenueScreen::Draw(Rectangle vung) {
    float x = vung.x + 24, y = vung.y + 16, w = vung.width - 48, h = vung.height - 32;

    // =========================================================================
    // 1. HANG THE THONG KE NHANH (TOP)
    // =========================================================================
    float khe = 18;
    float rongThe = (w - khe * 3) / 4.0f;
    float caoThe = 100;

    veTheBaoCao({x + 0 * (rongThe + khe), y, rongThe, caoThe}, "Tổng doanh thu",
                dinhDangTien(tk.tongDoanhThu), Theme::CANH_BAO,
                "Năm " + std::to_string(namChon) + ": " + dinhDangTien(tongDoanhThuNam));
    veTheBaoCao({x + 1 * (rongThe + khe), y, rongThe, caoThe}, "Thực thu hôm nay",
                dinhDangTien(tk.thucThuHomNay), Theme::THANH_CONG);
    veTheBaoCao({x + 2 * (rongThe + khe), y, rongThe, caoThe}, "HĐ đã tất toán",
                std::to_string(tk.hoaDonDaTatToan) + " / " + std::to_string(tk.tongSoHoaDon), Theme::CHINH);
    veTheBaoCao({x + 3 * (rongThe + khe), y, rongThe, caoThe}, "HĐ chưa thu đủ",
                std::to_string(tk.hoaDonChuaThanhToan), Theme::LOI);

    y += caoThe + 20;
    float caoVungConLai = h - (y - (vung.y + 16));

    // =========================================================================
    // 2. KHU VUC TRAI: DOANH THU THEO THANG / QUY
    //    KHU VUC PHAI: DOANH THU THEO LOAI PHONG & TINH TRANG HOA DON
    // =========================================================================
    float rongTrai = (w - 20) * 0.58f;
    float rongPhai = w - rongTrai - 20;

    Rectangle theKy = {x, y, rongTrai, caoVungConLai};
    Rectangle theBang = {x + rongTrai + 20, y, rongPhai, caoVungConLai};

    // -------------------------------------------------------------------------
    // Card Trai: Doanh thu chia theo Thang hoac Quy
    // -------------------------------------------------------------------------
    Widgets::veThe(theKy, 16);
    float bx = theKy.x + 24, by = theKy.y + 18, bw = theKy.width - 48;

    Theme::Chu(xemTheoQuy ? "Doanh thu theo quý" : "Doanh thu theo tháng",
               bx, by, Theme::CO_VUA, Theme::CHU, true);
    Theme::Chu("Năm " + std::to_string(namChon) + " • Tổng năm: " + dinhDangTien(tongDoanhThuNam),
               bx, by + 28, Theme::CO_NHO, Theme::CHU_PHU);

    // Bo chon Nam + Tab Thang / Quy o goc phai header
    float ctrlRight = bx + bw;
    Rectangle rBtnQuy   = {ctrlRight - 78, by + 4, 78, 32};
    Rectangle rBtnThang = {ctrlRight - 162, by + 4, 80, 32};
    Rectangle rNamSau   = {ctrlRight - 200, by + 4, 30, 32};
    Rectangle rNamNhan  = {ctrlRight - 258, by + 4, 56, 32};
    Rectangle rNamTruoc = {ctrlRight - 290, by + 4, 30, 32};

    if (VeNut(rNamTruoc, "<", KieuNut::PHU, true, Theme::CO_NHO)) {
        --namChon;
        taiTheoKy();
    }
    DrawRectangleRounded(rNamNhan, 0.2f, 6, Color{241, 245, 249, 255});
    Theme::ChuGiua(std::to_string(namChon), rNamNhan, Theme::CO_NHO, Theme::CHU, true);
    if (VeNut(rNamSau, ">", KieuNut::PHU, true, Theme::CO_NHO)) {
        ++namChon;
        taiTheoKy();
    }

    if (VeNut(rBtnThang, "Tháng", !xemTheoQuy ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 1)) {
        if (xemTheoQuy) { xemTheoQuy = false; taiTheoKy(); }
    }
    if (VeNut(rBtnQuy, "Quý", xemTheoQuy ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 1)) {
        if (!xemTheoQuy) { xemTheoQuy = true; taiTheoKy(); }
    }

    float cy = by + 64;
    const Color mauKy[4] = {Theme::CHINH, Theme::THANH_CONG, Theme::CANH_BAO, Color{147, 51, 234, 255}};

    long long maxDT = 0;
    for (const auto& item : dsKy) {
        if (item.doanhThu > maxDT) maxDT = item.doanhThu;
    }

    if (!xemTheoQuy) {
        // Hien thi 12 thang chia 2 cot (Thang 1..6 ben trai, Thang 7..12 ben phai)
        float colW = (bw - 24.0f) / 2.0f;
        float availH = theKy.height - (cy - theKy.y) - 16.0f;
        float rowH = std::max(44.0f, std::min(58.0f, availH / 6.0f));

        for (size_t i = 0; i < dsKy.size() && i < 12; ++i) {
            int col = (int)i / 6;
            int row = (int)i % 6;
            float mx = bx + col * (colW + 24.0f);
            float my = cy + row * rowH;

            const auto& item = dsKy[i];
            Color mau = (item.doanhThu > 0) ? mauKy[col % 2] : Theme::CHU_PHU;

            std::string nhanThang = "Tháng " + std::to_string(i + 1) + " (" + std::to_string(item.soHoaDon) + " HĐ)";
            Theme::Chu(nhanThang, mx, my, Theme::CO_NHO, Theme::CHU, item.doanhThu > 0);

            std::string strTien = dinhDangTien(item.doanhThu);
            Vector2 kt = Theme::DoChu(strTien, Theme::CO_NHO, true);
            Theme::Chu(strTien, mx + colW - kt.x, my, Theme::CO_NHO, mau, true);

            Rectangle rNen = {mx, my + 20, colW, 13};
            DrawRectangleRounded(rNen, 1.0f, 6, Color{241, 245, 249, 255});
            if (item.doanhThu > 0 && maxDT > 0) {
                float tyLe = (float)item.doanhThu / (float)maxDT;
                Rectangle rThanh = {mx, my + 20, std::max(colW * tyLe, 10.0f), 13};
                DrawRectangleRounded(rThanh, 1.0f, 6, mauKy[i % 4]);
            }
        }
    } else {
        // Hien thi 4 quy trong nam
        const char* tenQuy[4] = {
            "Quý 1 (Tháng 1 - 3)",
            "Quý 2 (Tháng 4 - 6)",
            "Quý 3 (Tháng 7 - 9)",
            "Quý 4 (Tháng 10 - 12)"
        };
        float availH = theKy.height - (cy - theKy.y) - 18.0f;
        float rowH = std::max(60.0f, std::min(78.0f, availH / 4.0f));

        for (size_t i = 0; i < dsKy.size() && i < 4; ++i) {
            const auto& item = dsKy[i];
            Color mau = mauKy[i % 4];
            float my = cy + i * rowH;

            std::string nhan = std::string(tenQuy[i]) + " • " + std::to_string(item.soHoaDon) + " hóa đơn";
            Theme::Chu(nhan, bx, my, Theme::CO_THUONG, Theme::CHU, true);

            float pt = (tongDoanhThuNam > 0) ? ((float)item.doanhThu / (float)tongDoanhThuNam * 100.0f) : 0.0f;
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(1) << pt << "% (" << dinhDangTien(item.doanhThu) << ")";
            std::string strDT = ss.str();
            Vector2 kt = Theme::DoChu(strDT, Theme::CO_THUONG, true);
            Theme::Chu(strDT, bx + bw - kt.x, my, Theme::CO_THUONG, mau, true);

            Rectangle rNen = {bx, my + 26, bw, 20};
            DrawRectangleRounded(rNen, 1.0f, 6, Color{241, 245, 249, 255});
            if (item.doanhThu > 0 && tongDoanhThuNam > 0) {
                float rongThanh = bw * (pt / 100.0f);
                Rectangle rThanh = {bx, my + 26, std::max(rongThanh, 14.0f), 20};
                DrawRectangleRounded(rThanh, 1.0f, 6, mau);
            }
        }
    }

    // -------------------------------------------------------------------------
    // Card Phai: Doanh thu theo loai phong & Tinh trang hoa don
    // -------------------------------------------------------------------------
    Widgets::veThe(theBang, 16);
    float px = theBang.x + 24, py = theBang.y + 18, pw = theBang.width - 48;
    Theme::Chu("Doanh thu theo loại phòng", px, py, Theme::CO_VUA, Theme::CHU, true);
    Theme::Chu("Tỷ lệ đóng góp từng hạng phòng", px, py + 28, Theme::CO_NHO, Theme::CHU_PHU);

    float lcy = py + 58;
    const Color mauCacLoai[4] = {Theme::CHINH, Theme::THANH_CONG, Theme::CANH_BAO, Color{147, 51, 234, 255}};

    for (size_t i = 0; i < tk.doanhThuTheoLoai.size(); ++i) {
        const auto& item = tk.doanhThuTheoLoai[i];
        Color mau = mauCacLoai[i % 4];

        Theme::Chu(LuoiPhong::tenLoai(item.loaiPhong), px, lcy, Theme::CO_NHO + 1, Theme::CHU, true);

        std::ostringstream ss;
        ss << std::fixed << std::setprecision(1) << item.phanTram << "% (" << dinhDangTien(item.doanhThu) << ")";
        std::string strDT = ss.str();
        Vector2 kt = Theme::DoChu(strDT, Theme::CO_NHO + 1, true);
        Theme::Chu(strDT, px + pw - kt.x, lcy, Theme::CO_NHO + 1, mau, true);

        lcy += 22;
        Rectangle rThanhNen = {px, lcy, pw, 14};
        DrawRectangleRounded(rThanhNen, 1.0f, 6, Color{241, 245, 249, 255});

        float rongThanh = (item.phanTram > 0.0f) ? (pw * (item.phanTram / 100.0f)) : 0.0f;
        if (rongThanh > 0.0f) {
            Rectangle rThanhTien = {px, lcy, std::max(rongThanh, 12.0f), 14};
            DrawRectangleRounded(rThanhTien, 1.0f, 6, mau);
        }

        lcy += 24;
    }

    lcy += 6;
    DrawLine((int)px, (int)lcy, (int)(px + pw), (int)lcy, Theme::VIEN);
    lcy += 12;

    Theme::Chu("Tình trạng hóa đơn:", px, lcy, Theme::CO_NHO + 1, Theme::CHU, true);
    lcy += 24;
    Theme::Chu("• Đã tất toán đủ: " + std::to_string(tk.hoaDonDaTatToan) + " hóa đơn", px, lcy, Theme::CO_NHO, Theme::THANH_CONG);
    lcy += 22;
    Theme::Chu("• Chưa thanh toán: " + std::to_string(tk.hoaDonChuaThanhToan) + " hóa đơn", px, lcy, Theme::CO_NHO, Theme::LOI);
    lcy += 22;
    Theme::Chu("• Đã hủy (bỏ qua): " + std::to_string(tk.hoaDonDaHuy) + " hóa đơn", px, lcy, Theme::CO_NHO, Theme::CHU_PHU);

    lcy += 26;
    if (lcy + 54 <= theBang.y + theBang.height - 12) {
        Rectangle rGhiChu = {px, lcy, pw, 54};
        DrawRectangleRounded(rGhiChu, 0.15f, 6, Color{248, 250, 252, 255});
        DrawRectangleRoundedLinesEx(rGhiChu, 0.15f, 6, 1.0f, Theme::VIEN);
        Theme::Chu("• Doanh thu = Phòng + DV - Giảm giá (chưa VAT)", rGhiChu.x + 10, rGhiChu.y + 8, Theme::CO_NHO - 1, Theme::CHU);
        Theme::Chu("• Thực thu = Tiền khách trả thực tế trong ngày", rGhiChu.x + 10, rGhiChu.y + 28, Theme::CO_NHO - 1, Theme::THANH_CONG);
    }
}
