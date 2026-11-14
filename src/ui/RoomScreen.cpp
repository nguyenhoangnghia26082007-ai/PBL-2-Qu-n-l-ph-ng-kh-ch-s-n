// RoomScreen.cpp: Man hinh thong nhat "Nhan / Tra phong" (So do phong + Check-in / Check-out / Dich vu / Bao tri).
#include "RoomScreen.h"
#include <algorithm>
#include <cctype>
#include "Common.h"
#include "LuoiPhong.h"
#include "PhanQuyen.h"
#include "Theme.h"

namespace {
std::string vietHoa(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::toupper(c); });
    return s;
}
} // namespace

RoomScreen::RoomScreen(AuthService& a, KhachSanService& s) : auth(a), service(s) {
    oTimKiem.goiY = "Tìm mã phòng...";
    oTimKiem.toiDa = 16;

    oNgayNhan.toiDa = 12;
    oNgayTraDuKien.toiDa = 12;
    oNgayTraDuKien.goiY = "dd/mm/yyyy (tùy chọn)";
    oNgayTra.toiDa = 12;

    oTimKhach.goiY = "Tìm theo tên, SĐT, CCCD...";
    oTimKhach.toiDa = 32;

    oSoLuongDV.giaTri = "1";
    oSoLuongDV.toiDa = 6;

    oMaPhongMoi.goiY = "VD: P202";
    oMaPhongMoi.toiDa = 12;
    oNgayDuaMoi.toiDa = 12;
}

void RoomScreen::taiLaiDuLieu() {
    service.danhSachPhong(dsPhong);
    service.danhSachDichVu(dsDichVuMenu);
    service.danhSachKhachHangNhanh(dsKhach);
    apDungBoLoc();
    if (!maPhongChon.empty()) chonPhong(maPhongChon);
    else if (!dsPhongLoc.empty()) chonPhong(dsPhongLoc.front().ma);
}

void RoomScreen::apDungBoLoc() {
    dsPhongLoc.clear();
    std::string tuKhoa = vietHoa(oTimKiem.giaTri);

    for (const auto& p : dsPhong) {
        if (!tuKhoa.empty() && p.ma.find(tuKhoa) == std::string::npos) continue;

        if (boLocTrangThai == 1 && p.tinhTrang != TinhTrangPhong::TRONG) continue;
        if (boLocTrangThai == 2 && p.tinhTrang != TinhTrangPhong::DANG_SU_DUNG) continue;
        if (boLocTrangThai == 3 && p.tinhTrang != TinhTrangPhong::BAO_TRI) continue;

        dsPhongLoc.push_back(p);
    }
}

void RoomScreen::chonPhong(const std::string& ma) {
    maPhongChon = ma;
    KetQua kq = service.chiTietPhong(ma, chiTiet);
    if (!kq.ok) { maPhongChon.clear(); return; }

    oNgayNhan.giaTri = Date::homNay().str();
    oNgayTraDuKien.giaTri.clear();
    // Neu luc nhan phong da ghi ngay tra du kien thi dien san vao o ngay tra
    oNgayTra.giaTri = chiTiet.coNgayTraDuKien ? chiTiet.ngayTraDuKien.str() : Date::homNay().str();
    oSoLuongDV.giaTri = "1";
    if (!dsKhach.empty() && maKhachNhan.empty()) {
        maKhachNhan = dsKhach.front().ma;
        tenKhachNhan = dsKhach.front().hoTen;
    }
}

void RoomScreen::Enter() {
    boLocTrangThai = 0;
    oTimKiem.giaTri.clear();
    moPopupChonKhach = false;
    moModalThemPhong = false;
    taiLaiDuLieu();
}

void RoomScreen::Update() {
    apDungBoLoc();
}

void RoomScreen::Draw(Rectangle vung) {
    bool coModal = hopXacNhanTraPhong.mo || hopXoaPhong.mo || moModalThemPhong || moPopupChonKhach;
    if (coModal) Widgets::batDauKhung(true);

    float x = vung.x + 24, y = vung.y + 16, w = vung.width - 48, h = vung.height - 32;
    float rongPanelPhai = std::min(420.0f, std::max(370.0f, w * 0.40f));
    float rongKhuTrai = w - rongPanelPhai - 20.0f;

    // =========================================================================
    // KHU VUC BEN TRAI: Luoi so do phong truc quan
    // =========================================================================
    Rectangle theTrai = {x, y, rongKhuTrai, h};
    Widgets::veThe(theTrai, 16);

    // 1. Hang 1 Toolbar: Tim kiem phong + Nut + Them phong (tuyet doi khong chong de)
    float ty = theTrai.y + 16, tx = theTrai.x + 18, tw = theTrai.width - 36;
    float rongO_TK = std::min(220.0f, tw * 0.58f);
    VeOTextNhap({tx, ty, rongO_TK, 36}, oTimKiem);

    if (auth.coQuyen(Quyen::QUAN_LY_PHONG)) {
        float rThem = std::min(130.0f, tw * 0.38f);
        Rectangle nutThem = {tx + tw - rThem, ty, rThem, 36};
        if (VeNut(nutThem, "+ Thêm phòng", KieuNut::CHINH, true, Theme::CO_NHO)) {
            moModalThemPhong = true;
            oMaPhongMoi.giaTri.clear();
            oNgayDuaMoi.giaTri = Date::homNay().str();
            loaiPhongChonIdx = 0;
        }
    }

    // 2. Hang 2: 4 Tabs bo loc trang thai phong (kem cham mau trang thai)
    float gy = ty + 46;
    const char* dsTab[] = {"Tất cả", "Trống", "Có khách", "Bảo trì"};
    Color mauTabDots[] = {BLANK, Theme::THANH_CONG, Theme::LOI, Theme::CANH_BAO};
    float tabX = tx;
    for (int i = 0; i < 4; ++i) {
        Vector2 kt = Theme::DoChu(dsTab[i], Theme::CO_NHO - 1, true);
        float wTab = kt.x + (i > 0 ? 28.0f : 20.0f);
        Rectangle rTab = {tabX, gy, wTab, 32};
        bool chon = (boLocTrangThai == i);
        if (VeNut(rTab, dsTab[i], chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 1)) {
            boLocTrangThai = i;
            apDungBoLoc();
        }
        if (i > 0 && !chon) {
            DrawCircle((int)(rTab.x + 10), (int)(rTab.y + 16), 4.0f, mauTabDots[i]);
        }
        tabX += rTab.width + 6;
    }

    DrawLine((int)theTrai.x + 16, (int)gy + 42, (int)(theTrai.x + theTrai.width - 16), (int)gy + 42, Theme::VIEN);

    // 3. Luoi so do phong
    float ly = gy + 52;
    Rectangle vungLuoi = {tx, ly, tw, theTrai.height - (ly - theTrai.y) - 16};

    int idxHienTai = -1;
    for (int i = 0; i < (int)dsPhongLoc.size(); ++i) {
        if (dsPhongLoc[i].ma == maPhongChon) { idxHienTai = i; break; }
    }

    int bam = LuoiPhong::ve(vungLuoi, dsPhongLoc, idxHienTai);
    if (bam >= 0 && bam < (int)dsPhongLoc.size()) {
        chonPhong(dsPhongLoc[bam].ma);
    }

    // =========================================================================
    // KHU VUC BEN PHAI: Thao tac Nhan phong / Tra phong / Goi dich vu
    // =========================================================================
    Rectangle thePhai = {theTrai.x + theTrai.width + 20, y, rongPanelPhai, h};
    Widgets::veThe(thePhai, 16);

    float px = thePhai.x + 20, py = thePhai.y + 18, pw = thePhai.width - 40;

    if (maPhongChon.empty()) {
        Theme::Chu("Bảng thao tác phòng", px, py, Theme::CO_VUA, Theme::CHU, true);
        Theme::Chu("Bấm chọn 1 phòng bên trái để thực hiện thao tác:", px, py + 34, Theme::CO_THUONG, Theme::CHU_PHU);

        float hy = py + 80;
        const char* huongDan[] = {
            "• Phòng TRỐNG: Nhận phòng (Check-in), Đưa vào bảo trì.",
            "• Phòng CÓ KHÁCH: Gọi dịch vụ (ăn uống, giặt, spa...), Trả phòng.",
            "• Phòng BẢO TRÌ: Hoàn tất dọn dẹp để sẵn sàng đón khách.",
            "• Quản lý: Thêm phòng mới hoặc Xóa phòng trống."
        };
        for (const auto* hd : huongDan) {
            Theme::Chu(Theme::CatChu(hd, pw, Theme::CO_NHO), px, hy, Theme::CO_NHO, Theme::CHU);
            hy += 34;
        }

        // The tong hop
        Rectangle rThongKe = {px, thePhai.y + thePhai.height - 180, pw, 150};
        Widgets::veBong(rThongKe, 12, 3);
        DrawRectangleRounded(rThongKe, 0.15f, 6, Color{248, 250, 252, 255});
        DrawRectangleRoundedLinesEx(rThongKe, 0.15f, 6, 1.0f, Theme::VIEN);
        Theme::Chu("Tổng số phòng: " + std::to_string(dsPhong.size()), px + 16, rThongKe.y + 16, Theme::CO_THUONG, Theme::CHU, true);

        int cTrong = 0, cO = 0, cBT = 0;
        for (const auto& p : dsPhong) {
            if (p.tinhTrang == TinhTrangPhong::TRONG) ++cTrong;
            else if (p.tinhTrang == TinhTrangPhong::DANG_SU_DUNG) ++cO;
            else ++cBT;
        }
        Theme::Chu("• Phòng trống: " + std::to_string(cTrong), px + 16, rThongKe.y + 52, Theme::CO_NHO + 1, Theme::THANH_CONG, true);
        Theme::Chu("• Đang có khách: " + std::to_string(cO), px + 16, rThongKe.y + 82, Theme::CO_NHO + 1, Theme::LOI, true);
        Theme::Chu("• Đang bảo trì / dọn: " + std::to_string(cBT), px + 16, rThongKe.y + 112, Theme::CO_NHO + 1, Theme::CANH_BAO, true);

    } else {
        // DA CHON PHONG -> HIEN CHI TIET VA FORM THAO TAC
        Theme::Chu(chiTiet.ma, px, py, Theme::CO_LON, Theme::CHU, true);
        Theme::Chu(chiTiet.tenLoai + " • " + dinhDangTien(chiTiet.donGiaHienTai) + "/đêm", px, py + 38, Theme::CO_NHO + 1, Theme::CHU_PHU);

        std::string tenTT = LuoiPhong::tenTrangThai(chiTiet.tinhTrang);
        Color mauTT = LuoiPhong::mauTrangThai(chiTiet.tinhTrang);
        float rongHH = Widgets::rongBadge(tenTT, Theme::CO_NHO + 1, 11);
        Rectangle rHuyHieu = {px + pw - rongHH, py + 4, rongHH, 30};
        DrawBadge(tenTT, rHuyHieu, mauTT, Theme::CO_NHO + 1, 11);

        DrawLine((int)px, (int)py + 68, (int)(px + pw), (int)py + 68, Theme::VIEN);
        float cy = py + 78;

        // ---------------------------------------------------------------------
        // 1. PHONG TRONG: FORM CHECK-IN VA BAO TRI
        // ---------------------------------------------------------------------
        if (chiTiet.tinhTrang == TinhTrangPhong::TRONG) {
            Theme::Chu("NHẬN PHÒNG (CHECK-IN)", px, cy, Theme::CO_THUONG, Theme::THANH_CONG, true);
            cy += 28;

            Theme::Chu("Khách hàng nhận phòng:", px, cy, Theme::CO_NHO, Theme::CHU_PHU, true);
            cy += 22;
            Rectangle rKhach = {px, cy, pw - 90, 36};
            DrawRectangleRounded(rKhach, 0.2f, 6, Color{241, 245, 249, 255});
            std::string tenKhachHien = tenKhachNhan.empty() ? "(Chưa chọn khách)" : (maKhachNhan + " - " + tenKhachNhan);
            Theme::Chu(Theme::CatChu(tenKhachHien, rKhach.width - 16, Theme::CO_NHO), rKhach.x + 10, rKhach.y + 9, Theme::CO_NHO, Theme::CHU);

            Rectangle nutChonK = {rKhach.x + rKhach.width + 6, cy, 84, 36};
            if (VeNut(nutChonK, "Tìm khách", KieuNut::PHU, true, Theme::CO_NHO - 1)) {
                moPopupChonKhach = true;
                oTimKhach.giaTri.clear();
            }

            cy += 46;
            float rongNgay = (pw - 10) / 2.0f;
            Theme::Chu("Ngày nhận phòng:", px, cy, Theme::CO_NHO, Theme::CHU_PHU, true);
            Theme::Chu("Ngày trả dự kiến:", px + rongNgay + 10, cy, Theme::CO_NHO, Theme::CHU_PHU, true);
            cy += 22;
            VeOTextNhap({px, cy, rongNgay, 36}, oNgayNhan);
            VeOTextNhap({px + rongNgay + 10, cy, rongNgay, 36}, oNgayTraDuKien);

            cy += 48;
            if (VeNut({px, cy, pw, 44}, "Xác nhận nhận phòng", KieuNut::CHINH, !maKhachNhan.empty(), Theme::CO_THUONG)) {
                KetQua kq = service.nhanPhong(chiTiet.ma, maKhachNhan, oNgayNhan.giaTri, oNgayTraDuKien.giaTri);
                ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
                if (kq.ok) taiLaiDuLieu();
            }

            cy += 64;
            DrawLine((int)px, (int)cy, (int)(px + pw), (int)cy, Theme::VIEN);
            cy += 12;
            Theme::Chu("Thao tác khác:", px, cy, Theme::CO_NHO, Theme::CHU_PHU, true);
            cy += 24;

            if (auth.coQuyen(Quyen::CAP_NHAT_TRANG_THAI_PHONG)) {
                if (VeNut({px, cy, pw, 38}, "Đưa vào bảo trì / Dọn phòng", KieuNut::PHU, true, Theme::CO_NHO)) {
                    KetQua kq = service.doiBaoTri(chiTiet.ma, true);
                    ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
                    if (kq.ok) taiLaiDuLieu();
                }
                cy += 46;
            }

            if (auth.coQuyen(Quyen::QUAN_LY_PHONG)) {
                if (VeNut({px, cy, pw, 38}, "Xóa phòng này", KieuNut::NGUY_HIEM, true, Theme::CO_NHO)) {
                    hopXoaPhong.moHop("Xác nhận xóa phòng", "Bạn có chắc chắn muốn xóa phòng " + chiTiet.ma + "?", "Xóa", true);
                }
            }

        }
        // ---------------------------------------------------------------------
        // 2. PHONG DANG CO KHACH: THONG TIN KHACH, GOI DICH VU, TRA PHONG
        // ---------------------------------------------------------------------
        else if (chiTiet.tinhTrang == TinhTrangPhong::DANG_SU_DUNG) {
            // The khach
            Rectangle rKhachInfo = {px, cy, pw, 80};
            DrawRectangleRounded(rKhachInfo, 0.15f, 6, Color{248, 250, 252, 255});
            DrawRectangleRoundedLinesEx(rKhachInfo, 0.15f, 6, 1.0f, Theme::VIEN);

            Theme::Chu("Khách: " + chiTiet.tenKhach, rKhachInfo.x + 12, rKhachInfo.y + 10, Theme::CO_THUONG, Theme::CHU, true);
            Theme::Chu("SĐT: " + (chiTiet.sdtKhach.empty() ? "—" : chiTiet.sdtKhach) + " • CCCD: " + chiTiet.cccdKhach,
                       rKhachInfo.x + 12, rKhachInfo.y + 32, Theme::CO_NHO - 1, Theme::CHU_PHU);
            std::string dongNgay = "Nhận: " + chiTiet.ngayNhan.str() + " (" + std::to_string(chiTiet.soDemTamTinh) + " đêm)";
            if (chiTiet.coNgayTraDuKien) dongNgay += " • Dự kiến trả: " + chiTiet.ngayTraDuKien.str();
            Theme::Chu(Theme::CatChu(dongNgay, rKhachInfo.width - 24, Theme::CO_NHO), rKhachInfo.x + 12, rKhachInfo.y + 54,
                       Theme::CO_NHO, Theme::CHINH, true);

            cy += 92;

            // Form Goi dich vu
            Theme::Chu("THÊM DỊCH VỤ PHÒNG", px, cy, Theme::CO_NHO + 1, Theme::CHU, true);
            cy += 22;

            float rDV = (pw - 6 * 3) / 4.0f;
            for (int i = 0; i < (int)dsDichVuMenu.size() && i < 4; ++i) {
                Rectangle rBtn = {px + i * (rDV + 6), cy, rDV, 32};
                bool chon = (dichVuChonIdx == i);
                if (VeNut(rBtn, dsDichVuMenu[i].ten, chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 2))
                    dichVuChonIdx = i;
            }
            cy += 38;

            if (dichVuChonIdx >= 0 && dichVuChonIdx < (int)dsDichVuMenu.size()) {
                const auto& dv = dsDichVuMenu[dichVuChonIdx];
                Theme::Chu("Giá: " + dinhDangTien(dv.donGia), px, cy + 6, Theme::CO_NHO - 1, Theme::CHU_PHU);
                Theme::Chu("SL:", px + pw - 165, cy + 6, Theme::CO_NHO - 1, Theme::CHU_PHU, true);
                VeOTextNhap({px + pw - 138, cy, 48, 32}, oSoLuongDV);

                if (VeNut({px + pw - 82, cy, 82, 32}, "+ Thêm", KieuNut::CHINH, true, Theme::CO_NHO - 1)) {
                    double sl = std::atof(oSoLuongDV.giaTri.c_str());
                    if (sl <= 0) sl = 1.0;
                    KetQua kq = service.themDichVuChoPhong(chiTiet.ma, dv.ma, sl);
                    ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
                    if (kq.ok) {
                        oSoLuongDV.giaTri = "1";
                        taiLaiDuLieu();
                    }
                }
            }
            cy += 44;

            // Danh sach dich vu da dung
            Theme::Chu("Dịch vụ đã gọi (" + std::to_string(chiTiet.dsDichVu.size()) + "):", px, cy, Theme::CO_NHO, Theme::CHU_PHU, true);
            cy += 18;

            Rectangle rVungDV = {px, cy, pw, 70};
            DrawRectangleRounded(rVungDV, 0.12f, 6, Color{248, 250, 252, 255});
            DrawRectangleRoundedLinesEx(rVungDV, 0.12f, 6, 1.0f, Theme::VIEN);

            if (chiTiet.dsDichVu.empty()) {
                Theme::Chu("(Chưa có dịch vụ nào)", rVungDV.x + 12, rVungDV.y + 25, Theme::CO_NHO - 1, Theme::CHU_PHU);
            } else {
                float dy = rVungDV.y + 6;
                for (size_t i = 0; i < chiTiet.dsDichVu.size() && i < 3; ++i) {
                    const auto& d = chiTiet.dsDichVu[i];
                    std::string slStr = (d.soLuong == (long long)d.soLuong) ? std::to_string((long long)d.soLuong) : std::to_string(d.soLuong);
                    Theme::Chu("• " + d.tenDV + " (x" + slStr + ")", rVungDV.x + 8, dy, Theme::CO_NHO - 1, Theme::CHU);
                    Theme::Chu(dinhDangTien(d.thanhTien), rVungDV.x + rVungDV.width - 120, dy, Theme::CO_NHO - 1, Theme::CHU_PHU);
                    dy += 20;
                }
            }
            cy += 82;

            // Tam tinh va Tra phong
            DrawLine((int)px, (int)cy, (int)(px + pw), (int)cy, Theme::VIEN);
            cy += 8;
            Theme::Chu("Tạm tính: " + dinhDangTien(chiTiet.tongTienTamTinh), px, cy, Theme::CO_THUONG, Theme::LOI, true);
            Theme::Chu("(Phòng: " + dinhDangTien(chiTiet.tienPhongTamTinh) + " + DV: " + dinhDangTien(chiTiet.tienDichVuTamTinh) + ")",
                       px, cy + 22, Theme::CO_NHO - 1, Theme::CHU_PHU);

            cy += 48;
            Theme::Chu("Ngày trả:", px, cy + 8, Theme::CO_NHO, Theme::CHU_PHU, true);
            VeOTextNhap({px + 68, cy, 115, 36}, oNgayTra);

            Rectangle nutTraPhong = {px + 192, cy, pw - 192, 36};
            if (VeNut(nutTraPhong, "Trả phòng (Check-out)", KieuNut::NGUY_HIEM, true, Theme::CO_NHO)) {
                hopXacNhanTraPhong.moHop("Xác nhận trả phòng",
                    "Khách " + chiTiet.tenKhach + " trả phòng " + chiTiet.ma + "?\nHệ thống sẽ tự động lập hóa đơn thanh toán.",
                    "Trả phòng", true);
            }

        }
        // ---------------------------------------------------------------------
        // 3. PHONG DANG BAO TRI / DON DEP
        // ---------------------------------------------------------------------
        else {
            Rectangle rCanhBao = {px, cy, pw, 90};
            DrawRectangleRounded(rCanhBao, 0.15f, 6, Fade(Theme::CANH_BAO, 0.12f));
            DrawRectangleRoundedLinesEx(rCanhBao, 0.15f, 6, 1.2f, Theme::CANH_BAO);

            Theme::Chu("PHÒNG ĐANG BẢO TRÌ / ĐANG DỌN", rCanhBao.x + 14, rCanhBao.y + 14, Theme::CO_THUONG, Theme::CANH_BAO, true);
            Theme::Chu("Phòng tạm khóa để dọn vệ sinh hoặc sửa chữa thiết bị.",
                       rCanhBao.x + 14, rCanhBao.y + 42, Theme::CO_NHO, Theme::CHU);

            cy += 110;
            if (auth.coQuyen(Quyen::CAP_NHAT_TRANG_THAI_PHONG)) {
                if (VeNut({px, cy, pw, 46}, "Hoàn tất bảo trì -> Sẵn sàng đón khách", KieuNut::CHINH, true, Theme::CO_THUONG)) {
                    KetQua kq = service.doiBaoTri(chiTiet.ma, false);
                    ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
                    if (kq.ok) taiLaiDuLieu();
                }
            }
        }
    }

    // Modal Popup: Chon khach hang
    if (moPopupChonKhach) {
        Widgets::batDauHopThoai();
        float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.45f));

        Rectangle hop = {sw / 2 - 250, sh / 2 - 230, 500, 460};
        Widgets::veThe(hop, 16);
        Theme::Chu("Chọn khách hàng nhận phòng", hop.x + 24, hop.y + 18, Theme::CO_VUA, Theme::CHU, true);

        VeOTextNhap({hop.x + 24, hop.y + 56, hop.width - 48, 36}, oTimKhach);

        float ky = hop.y + 104;
        std::string tuKhoaK = vietHoa(oTimKhach.giaTri);
        int dem = 0;
        for (const auto& k : dsKhach) {
            if (!tuKhoaK.empty() &&
                vietHoa(k.hoTen).find(tuKhoaK) == std::string::npos &&
                vietHoa(k.ma).find(tuKhoaK) == std::string::npos &&
                k.sdt.find(tuKhoaK) == std::string::npos) {
                continue;
            }

            Rectangle rDong = {hop.x + 24, ky, hop.width - 48, 42};
            bool hover = Widgets::diChuotVao(rDong);
            if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

            DrawRectangleRounded(rDong, 0.2f, 6, hover ? Color{219, 234, 254, 255} : Color{248, 250, 252, 255});
            Theme::Chu(k.ma + " - " + k.hoTen, rDong.x + 12, rDong.y + 11, Theme::CO_NHO + 1, Theme::CHU, true);
            Theme::Chu(k.sdt.empty() ? k.cccd : k.sdt, rDong.x + rDong.width - 120, rDong.y + 11, Theme::CO_NHO, Theme::CHU_PHU);

            if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                maKhachNhan = k.ma;
                tenKhachNhan = k.hoTen;
                moPopupChonKhach = false;
                break;
            }

            ky += 46;
            if (++dem >= 6) break;
        }

        if (VeNut({hop.x + hop.width - 120, hop.y + hop.height - 46, 96, 34}, "Đóng", KieuNut::PHU)) {
            moPopupChonKhach = false;
        }
        Widgets::ketThucHopThoai();
    }

    // Modal: Them phong moi (Quan ly)
    if (moModalThemPhong) {
        Widgets::batDauHopThoai();
        float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.45f));

        Rectangle hop = {sw / 2 - 220, sh / 2 - 210, 440, 420};
        Widgets::veThe(hop, 16);
        Theme::Chu("Thêm phòng mới", hop.x + 24, hop.y + 20, Theme::CO_VUA, Theme::CHU, true);

        float my = hop.y + 64;
        Theme::Chu("Mã phòng (VD: P105):", hop.x + 24, my, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        VeOTextNhap({hop.x + 24, my + 22, hop.width - 48, 38}, oMaPhongMoi);

        my += 68;
        Theme::Chu("Loại phòng:", hop.x + 24, my, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        const char* dsLoai[] = {"DON", "DOI", "SUITE", "VIP"};
        float rLoai = (hop.width - 48 - 8 * 3) / 4.0f;
        for (int i = 0; i < 4; ++i) {
            Rectangle rBtn = {hop.x + 24 + i * (rLoai + 8), my + 22, rLoai, 36};
            bool chon = (loaiPhongChonIdx == i);
            if (VeNut(rBtn, dsLoai[i], chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO))
                loaiPhongChonIdx = i;
        }

        my += 68;
        // Gia phong co dinh theo loai: chi hien thi, khong cho nhap tay
        long long giaCoBan = 0, giaDem = 0;
        service.giaPhongTheoLoai(dsLoai[loaiPhongChonIdx], giaCoBan, giaDem);
        Theme::Chu("Giá phòng (tự động theo loại):", hop.x + 24, my, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        Rectangle rGia = {hop.x + 24, my + 22, hop.width - 48, 38};
        DrawRectangleRounded(rGia, Widgets::boGoc(rGia, 10), 10, Color{241, 245, 249, 255});
        DrawRectangleRoundedLinesEx(rGia, Widgets::boGoc(rGia, 10), 10, 1.2f, Theme::VIEN);
        std::string chuGia = dinhDangTien(giaDem) + "/đêm";
        if (giaDem != giaCoBan) chuGia += "  (cơ bản " + dinhDangTien(giaCoBan) + ")";
        Theme::Chu(Theme::CatChu(chuGia, rGia.width - 28, Theme::CO_NHO + 1), rGia.x + 14, rGia.y + 10,
                   Theme::CO_NHO + 1, Theme::CHU, true);

        my += 68;
        Theme::Chu("Ngày đưa vào sử dụng (dd/mm/yyyy):", hop.x + 24, my, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        VeOTextNhap({hop.x + 24, my + 22, hop.width - 48, 38}, oNgayDuaMoi);

        my += 74;
        Rectangle nutHuy = {hop.x + hop.width - 24 - 100 - 12 - 140, my, 100, 40};
        Rectangle nutXacNhan = {hop.x + hop.width - 24 - 140, my, 140, 40};

        if (VeNut(nutHuy, "Hủy", KieuNut::PHU)) moModalThemPhong = false;
        if (VeNut(nutXacNhan, "Tạo phòng", KieuNut::CHINH)) {
            KetQua kq = service.themPhong(dsLoai[loaiPhongChonIdx], oMaPhongMoi.giaTri, oNgayDuaMoi.giaTri);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) {
                moModalThemPhong = false;
                taiLaiDuLieu();
                chonPhong(vietHoa(oMaPhongMoi.giaTri));
            }
        }
        Widgets::ketThucHopThoai();
    }

    if (hopXacNhanTraPhong.ve() == 1) {
        std::string maHDMoi;
        KetQua kq = service.traPhong(chiTiet.ma, oNgayTra.giaTri, maHDMoi);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        taiLaiDuLieu();
    }

    if (hopXoaPhong.ve() == 1) {
        KetQua kq = service.xoaPhong(chiTiet.ma);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        if (kq.ok) { maPhongChon.clear(); taiLaiDuLieu(); }
    }
}
