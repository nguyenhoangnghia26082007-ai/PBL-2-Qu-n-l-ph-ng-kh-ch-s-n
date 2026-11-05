// FrontDeskScreen.cpp: Man hinh chuyen biet cho quay Le tan (Check-in, Check-out, Them dich vu).
#include "FrontDeskScreen.h"
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

FrontDeskScreen::FrontDeskScreen(AuthService& a, KhachSanService& s) : auth(a), service(s) {
    oTimKiem.goiY = "Tìm theo phòng, tên khách...";
    oTimKiem.toiDa = 32;

    oTimKhach.goiY = "Tìm khách hàng theo tên, SĐT...";
    oTimKhach.toiDa = 32;

    oNgayNhan.toiDa = 12;
    oNgayTra.toiDa = 12;

    oSoLuongDV.giaTri = "1";
    oSoLuongDV.toiDa = 6;
}

void FrontDeskScreen::taiLai() {
    std::vector<PhongHienThi> dsAll;
    service.danhSachPhong(dsAll);

    dsPhongDangO.clear();
    dsPhongTrong.clear();
    for (const auto& p : dsAll) {
        if (p.tinhTrang == TinhTrangPhong::DANG_SU_DUNG) dsPhongDangO.push_back(p);
        else if (p.tinhTrang == TinhTrangPhong::TRONG) dsPhongTrong.push_back(p);
    }

    service.danhSachKhachHangNhanh(dsKhach);
    service.danhSachDichVu(dsDichVuMenu);

    if (!maPhongDangChon.empty()) chonPhongDangO(maPhongDangChon);
    else if (!dsPhongDangO.empty()) chonPhongDangO(dsPhongDangO.front().ma);
}

void FrontDeskScreen::chonPhongDangO(const std::string& maPhong) {
    maPhongDangChon = maPhong;
    KetQua kq = service.chiTietPhong(maPhong, chiTietDangO);
    if (!kq.ok) { maPhongDangChon.clear(); return; }

    oNgayTra.giaTri = Date::homNay().str();
    oSoLuongDV.giaTri = "1";
}

void FrontDeskScreen::Enter() {
    oTimKiem.giaTri.clear();
    oNgayNhan.giaTri = Date::homNay().str();
    oNgayTra.giaTri = Date::homNay().str();
    moPopupChonKhach = false;
    phongTrongChonIdx = 0;
    dichVuChonIdx = 0;

    taiLai();
    if (!dsKhach.empty() && maKhachNhan.empty()) {
        maKhachNhan = dsKhach.front().ma;
        tenKhachNhan = dsKhach.front().hoTen;
    }
}

void FrontDeskScreen::Update() {}

void FrontDeskScreen::Draw(Rectangle vung) {
    if (hopXacNhanTraPhong.mo || moPopupChonKhach) Widgets::batDauKhung(true);

    float x = vung.x + 24, y = vung.y + 16, w = vung.width - 48, h = vung.height - 32;
    float rongPhai = 440.0f;
    float rongTrai = w - rongPhai - 20.0f;

    // =========================================================================
    // 1. BEN TRAI: DANH SACH PHONG DANG CO KHACH
    // =========================================================================
    Rectangle theTrai = {x, y, rongTrai, h};
    Widgets::veThe(theTrai, 16);

    float ty = theTrai.y + 16, tx = theTrai.x + 18;
    Theme::Chu("Khách đang lưu trú", tx, ty + 6, Theme::CO_VUA, Theme::CHU, true);

    // Tim kiem
    VeOTextNhap({theTrai.x + theTrai.width - 240, ty, 220, 36}, oTimKiem);

    // Tieu de cot
    float yBang = ty + 48;
    Rectangle dauBang = {theTrai.x + 14, yBang, theTrai.width - 28, 38};
    DrawRectangleRounded(dauBang, 0.25f, 6, Color{241, 245, 249, 255});

    float wB = dauBang.width;
    float c0 = dauBang.x + 10;
    float c1 = dauBang.x + wB * 0.18f;
    float c2 = dauBang.x + wB * 0.44f;
    float c3 = dauBang.x + wB * 0.72f;

    Theme::Chu("Phòng",     c0, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Khách hàng", c1, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Loại phòng", c2, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Đơn giá",    c3, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);

    std::string tuKhoa = vietHoa(oTimKiem.giaTri);

    float yDong = yBang + 44;
    int demHien = 0;
    for (int i = 0; i < (int)dsPhongDangO.size(); ++i) {
        const auto& p = dsPhongDangO[i];
        if (!tuKhoa.empty() &&
            vietHoa(p.ma).find(tuKhoa) == std::string::npos &&
            vietHoa(p.tenKhach).find(tuKhoa) == std::string::npos) {
            continue;
        }

        Rectangle dong = {dauBang.x, yDong, dauBang.width, 48};
        if (dong.y + dong.height > theTrai.y + theTrai.height - 40) break;

        bool hover = Widgets::diChuotVao(dong);
        bool chon = (p.ma == maPhongDangChon);
        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

        Color nen = chon ? Color{219, 234, 254, 255} : (hover ? Color{248, 250, 252, 255} : Theme::TRANG);
        DrawRectangleRounded(dong, 0.2f, 6, nen);
        if (chon) DrawRectangleRoundedLinesEx(dong, 0.2f, 6, 1.5f, Theme::CHINH);

        Theme::Chu(p.ma, c0, yDong + 14, Theme::CO_NHO + 1, Theme::LOI, true);
        Theme::Chu(Theme::CatChu(p.tenKhach, (c2 - c1 - 8), Theme::CO_NHO), c1, yDong + 14, Theme::CO_NHO, Theme::CHU, true);
        Theme::Chu(LuoiPhong::tenLoai(p.maLoai), c2, yDong + 14, Theme::CO_NHO - 1, Theme::CHU_PHU);
        Theme::Chu(dinhDangTien(p.donGia) + "/đêm", c3, yDong + 14, Theme::CO_NHO - 1, Theme::CHU_PHU);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
            chonPhongDangO(p.ma);
            tabThaoTac = 1; // Nhay sang tab dich vu & tra phong
        }
        yDong += 52;
        ++demHien;
    }

    if (dsPhongDangO.empty()) {
        Theme::Chu("Hiện tại không có phòng nào đang có khách.", theTrai.x + 24, yBang + 60, Theme::CO_THUONG, Theme::CHU_PHU);
    }

    // Dong thong ke chan trai
    Theme::Chu("Đang phục vụ: " + std::to_string(dsPhongDangO.size()) + " phòng có khách • Còn trống: " +
               std::to_string(dsPhongTrong.size()) + " phòng",
               theTrai.x + 18, theTrai.y + theTrai.height - 30, Theme::CO_NHO, Theme::CHU_PHU);

    // =========================================================================
    // 2. BEN PHAI: KHU VUC THAO TAC (CHECK-IN / CHECK-OUT / DICH VU)
    // =========================================================================
    Rectangle thePhai = {theTrai.x + theTrai.width + 20, y, rongPhai, h};
    Widgets::veThe(thePhai, 16);

    float px = thePhai.x + 20, py = thePhai.y + 16, pw = thePhai.width - 40;

    // Tabs chuyen doi thao tac
    float rTab = (pw - 8) / 2.0f;
    Rectangle rTab0 = {px, py, rTab, 38};
    Rectangle rTab1 = {px + rTab + 8, py, rTab, 38};

    if (VeNut(rTab0, "+ Nhận phòng (Check-in)", tabThaoTac == 0 ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO))
        tabThaoTac = 0;
    if (VeNut(rTab1, "Trả phòng & Dịch vụ", tabThaoTac == 1 ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO))
        tabThaoTac = 1;

    DrawLine((int)px, (int)py + 48, (int)(px + pw), (int)py + 48, Theme::VIEN);
    py += 58;

    // -------------------------------------------------------------------------
    // TAB 0: NHẬN PHÒNG MỚI (CHECK-IN)
    // -------------------------------------------------------------------------
    if (tabThaoTac == 0) {
        Theme::Chu("LẬP LƯỢT THUÊ PHÒNG MỚI", px, py, Theme::CO_THUONG, Theme::THANH_CONG, true);
        py += 30;

        // 1. Chon phong trong
        Theme::Chu("1. Chọn phòng còn trống:", px, py, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        py += 24;

        if (dsPhongTrong.empty()) {
            Rectangle rRong = {px, py, pw, 44};
            DrawRectangleRounded(rRong, 0.2f, 6, Fade(Theme::CANH_BAO, 0.15f));
            Theme::Chu("Tất cả các phòng hiện đã kín hoặc đang bảo trì!", px + 12, py + 14, Theme::CO_NHO, Theme::CANH_BAO, true);
            py += 54;
        } else {
            // Hien thi cac nut phong trong dang pill
            float rO = (pw - 8 * 3) / 4.0f;
            for (int i = 0; i < (int)dsPhongTrong.size() && i < 8; ++i) {
                Rectangle rP = {px + (i % 4) * (rO + 8), py + (i / 4) * 40, rO, 34};
                bool chon = (phongTrongChonIdx == i);
                if (VeNut(rP, dsPhongTrong[i].ma, chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO))
                    phongTrongChonIdx = i;
            }
            int soHangP = ((int)dsPhongTrong.size() > 4) ? 2 : 1;
            py += soHangP * 40 + 10;

            if (phongTrongChonIdx >= 0 && phongTrongChonIdx < (int)dsPhongTrong.size()) {
                const auto& pChon = dsPhongTrong[phongTrongChonIdx];
                Theme::Chu("→ " + pChon.ma + ": " + LuoiPhong::tenLoai(pChon.maLoai) + " • " + dinhDangTien(pChon.donGia) + "/đêm",
                           px, py, Theme::CO_NHO, Theme::CHINH, true);
            }
            py += 28;
        }

        // 2. Chon khach hang
        Theme::Chu("2. Khách hàng nhận phòng:", px, py, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        py += 24;

        Rectangle rKhach = {px, py, pw - 95, 38};
        DrawRectangleRounded(rKhach, 0.2f, 6, Color{241, 245, 249, 255});
        std::string tenKhachHien = tenKhachNhan.empty() ? "(Chưa chọn khách hàng)" : (maKhachNhan + " - " + tenKhachNhan);
        Theme::Chu(Theme::CatChu(tenKhachHien, rKhach.width - 20, Theme::CO_NHO + 1), rKhach.x + 10, rKhach.y + 10, Theme::CO_NHO + 1, Theme::CHU, true);

        Rectangle nutChonK = {rKhach.x + rKhach.width + 8, py, 87, 38};
        if (VeNut(nutChonK, "Tìm khách", KieuNut::PHU, true, Theme::CO_NHO)) {
            moPopupChonKhach = true;
            oTimKhach.giaTri.clear();
        }
        py += 50;

        // 3. Ngay nhan
        Theme::Chu("3. Ngày nhận phòng (dd/mm/yyyy):", px, py, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
        py += 24;
        VeOTextNhap({px, py, pw, 38}, oNgayNhan);
        py += 54;

        // Nut xac nhan check-in
        bool coTheCheckIn = (!dsPhongTrong.empty() && !maKhachNhan.empty());
        if (VeNut({px, py, pw, 46}, "Xác nhận Nhận phòng (Check-in)", KieuNut::CHINH, coTheCheckIn, Theme::CO_THUONG)) {
            const auto& pChon = dsPhongTrong[phongTrongChonIdx];
            KetQua kq = service.nhanPhong(pChon.ma, maKhachNhan, oNgayNhan.giaTri);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) {
                taiLai();
                chonPhongDangO(pChon.ma);
                tabThaoTac = 1; // Chuyen sang xem phong vua nhan
            }
        }
    }
    // -------------------------------------------------------------------------
    // TAB 1: TRẢ PHÒNG & GỌI DỊCH VỤ
    // -------------------------------------------------------------------------
    else {
        if (maPhongDangChon.empty() || dsPhongDangO.empty()) {
            Theme::Chu("Chọn một phòng ở danh sách bên trái để thao tác.", px, py + 20, Theme::CO_THUONG, Theme::CHU_PHU);
        } else {
            // The thong tin luot thue
            Rectangle rKhachInfo = {px, py, pw, 85};
            DrawRectangleRounded(rKhachInfo, 0.15f, 6, Color{248, 250, 252, 255});
            DrawRectangleRoundedLinesEx(rKhachInfo, 0.15f, 6, 1.0f, Theme::VIEN);

            Theme::Chu(chiTietDangO.ma + " • Khách: " + chiTietDangO.tenKhach, rKhachInfo.x + 12, rKhachInfo.y + 10, Theme::CO_THUONG, Theme::CHU, true);
            Theme::Chu("SĐT: " + (chiTietDangO.sdtKhach.empty() ? "—" : chiTietDangO.sdtKhach) + " • CCCD: " + chiTietDangO.cccdKhach,
                       rKhachInfo.x + 12, rKhachInfo.y + 34, Theme::CO_NHO - 1, Theme::CHU_PHU);
            Theme::Chu("Ngày nhận: " + chiTietDangO.ngayNhan.str() + " (" + std::to_string(chiTietDangO.soDemTamTinh) + " đêm)",
                       rKhachInfo.x + 12, rKhachInfo.y + 56, Theme::CO_NHO, Theme::CHINH, true);

            py += 96;

            // Form Goi dich vu
            Theme::Chu("GỌI DỊCH VỤ PHÒNG", px, py, Theme::CO_NHO + 1, Theme::CHU, true);
            py += 22;

            float rDV = (pw - 6 * 3) / 4.0f;
            for (int i = 0; i < (int)dsDichVuMenu.size() && i < 4; ++i) {
                Rectangle rBtn = {px + i * (rDV + 6), py, rDV, 32};
                bool chon = (dichVuChonIdx == i);
                if (VeNut(rBtn, dsDichVuMenu[i].ten, chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 2))
                    dichVuChonIdx = i;
            }
            py += 38;

            if (dichVuChonIdx >= 0 && dichVuChonIdx < (int)dsDichVuMenu.size()) {
                const auto& dv = dsDichVuMenu[dichVuChonIdx];
                Theme::Chu("Giá: " + dinhDangTien(dv.donGia), px, py + 6, Theme::CO_NHO - 1, Theme::CHU_PHU);
                Theme::Chu("SL:", px + pw - 170, py + 6, Theme::CO_NHO - 1, Theme::CHU_PHU, true);
                VeOTextNhap({px + pw - 142, py, 50, 32}, oSoLuongDV);

                if (VeNut({px + pw - 84, py, 84, 32}, "+ Thêm", KieuNut::CHINH, true, Theme::CO_NHO - 1)) {
                    double sl = std::atof(oSoLuongDV.giaTri.c_str());
                    if (sl <= 0) sl = 1.0;
                    KetQua kq = service.themDichVuChoPhong(chiTietDangO.ma, dv.ma, sl);
                    ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
                    if (kq.ok) {
                        oSoLuongDV.giaTri = "1";
                        taiLai();
                    }
                }
            }
            py += 44;

            // Danh sach dich vu da goi
            Theme::Chu("Dịch vụ đã dùng (" + std::to_string(chiTietDangO.dsDichVu.size()) + "):", px, py, Theme::CO_NHO, Theme::CHU_PHU, true);
            py += 18;

            Rectangle rVungDV = {px, py, pw, 74};
            DrawRectangleRounded(rVungDV, 0.12f, 6, Color{248, 250, 252, 255});
            DrawRectangleRoundedLinesEx(rVungDV, 0.12f, 6, 1.0f, Theme::VIEN);

            if (chiTietDangO.dsDichVu.empty()) {
                Theme::Chu("(Chưa dùng dịch vụ nào)", rVungDV.x + 12, rVungDV.y + 26, Theme::CO_NHO - 1, Theme::CHU_PHU);
            } else {
                float dy = rVungDV.y + 6;
                for (size_t i = 0; i < chiTietDangO.dsDichVu.size() && i < 3; ++i) {
                    const auto& d = chiTietDangO.dsDichVu[i];
                    std::string slStr = (d.soLuong == (long long)d.soLuong) ? std::to_string((long long)d.soLuong) : std::to_string(d.soLuong);
                    Theme::Chu("• " + d.tenDV + " (x" + slStr + ")", rVungDV.x + 8, dy, Theme::CO_NHO - 1, Theme::CHU);
                    Theme::Chu(dinhDangTien(d.thanhTien), rVungDV.x + rVungDV.width - 120, dy, Theme::CO_NHO - 1, Theme::CHU_PHU);
                    dy += 20;
                }
            }
            py += 86;

            // Chi phi tam tinh & Tra phong
            DrawLine((int)px, (int)py, (int)(px + pw), (int)py, Theme::VIEN);
            py += 8;
            Theme::Chu("Tạm tính: " + dinhDangTien(chiTietDangO.tongTienTamTinh), px, py, Theme::CO_THUONG, Theme::LOI, true);
            Theme::Chu("(Phòng: " + dinhDangTien(chiTietDangO.tienPhongTamTinh) + " + DV: " + dinhDangTien(chiTietDangO.tienDichVuTamTinh) + ")",
                       px, py + 22, Theme::CO_NHO - 1, Theme::CHU_PHU);

            py += 48;
            Theme::Chu("Ngày trả:", px, py + 8, Theme::CO_NHO, Theme::CHU_PHU, true);
            VeOTextNhap({px + 68, py, 120, 36}, oNgayTra);

            Rectangle nutTra = {px + 196, py, pw - 196, 36};
            if (VeNut(nutTra, "Trả phòng (Check-out)", KieuNut::NGUY_HIEM, true, Theme::CO_NHO)) {
                hopXacNhanTraPhong.moHop("Xác nhận trả phòng",
                    "Khách " + chiTietDangO.tenKhach + " trả phòng " + chiTietDangO.ma + "?\nHệ thống sẽ lập hóa đơn tự động.",
                    "Trả phòng", true);
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

    if (hopXacNhanTraPhong.ve() == 1) {
        std::string maHDMoi;
        KetQua kq = service.traPhong(chiTietDangO.ma, oNgayTra.giaTri, maHDMoi);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        taiLai();
    }
}
