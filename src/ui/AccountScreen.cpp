// AccountScreen.cpp: Quan ly mo khoa tai khoan bi khoa do nhap sai 5 lan, sua mat khau/ho ten/vai tro.
#include "AccountScreen.h"
#include "PhanQuyen.h"
#include "Theme.h"

AccountScreen::AccountScreen(AuthService& a) : auth(a) {
    oTenDN.goiY = "Tên đăng nhập (vd: letan2)"; oTenDN.toiDa = 24;
    oMatKhau.goiY = "Mật khẩu";                 oMatKhau.toiDa = 32;
    oHoTen.goiY = "Họ tên hiển thị";            oHoTen.toiDa = 48;
    oMaNV.goiY = "Mã NV liên kết (vd: NV01)";   oMaNV.toiDa = 12;
}

void AccountScreen::taiLai() {
    auth.danhSachTaiKhoan(ds);
    if (dangChon >= (int)ds.size()) dangChon = -1;
}

void AccountScreen::datFormThemMoi() {
    dangChon = -1;
    oTenDN.giaTri.clear();
    oMatKhau.giaTri.clear();
    oHoTen.giaTri.clear();
    oMaNV.giaTri.clear();
    vaiTroChon = VaiTro::LE_TAN;
}

void AccountScreen::chonTaiKhoan(int idx) {
    if (idx < 0 || idx >= (int)ds.size()) { datFormThemMoi(); return; }
    dangChon = idx;
    const auto& t = ds[idx];
    oTenDN.giaTri = t.tenDangNhap;
    oMatKhau.giaTri = t.matKhau; // mat khau luu truc tiep nen hien luon de Quan ly xem/sua
    oHoTen.giaTri = t.hoTen;
    oMaNV.giaTri = t.maNV;
    vaiTroChon = t.vaiTro;
}

void AccountScreen::Enter() {
    taiLai();
    if (!ds.empty()) chonTaiKhoan(0);
    else datFormThemMoi();
}

void AccountScreen::Draw(Rectangle vung) {
    if (hopXoa.mo) Widgets::batDauKhung(true);

    float x = vung.x + 24, y = vung.y + 20, rong = vung.width - 48, cao = vung.height - 40;
    float rongForm = 390;
    Rectangle theBang = {x, y, rong - rongForm - 20, cao};
    Rectangle theForm = {theBang.x + theBang.width + 20, y, rongForm, cao};

    // ---------- 1. Bang danh sach tai khoan ----------
    Widgets::veThe(theBang, 16);
    Theme::Chu("Tài khoản nhân viên (Quản lý mở khóa khi sai 5 lần)", theBang.x + 22, theBang.y + 18,
               Theme::CO_VUA - 2, Theme::CHU, true);
    if (VeNut({theBang.x + theBang.width - 156, theBang.y + 12, 134, 38}, "+ Tạo mới", KieuNut::CHINH, true, Theme::CO_NHO + 2))
        datFormThemMoi();

    float yBang = theBang.y + 64;
    Rectangle dauBang = {theBang.x + 16, yBang, theBang.width - 32, 38};
    DrawRectangleRounded(dauBang, 0.25f, 6, Color{241, 245, 249, 255});
    float wB = dauBang.width;
    float c0 = dauBang.x + 10;
    float c1 = dauBang.x + wB * 0.18f;
    float c2 = dauBang.x + wB * 0.44f;
    float c3 = dauBang.x + wB * 0.62f;
    float c4 = dauBang.x + wB * 0.80f;

    Theme::Chu("Tài khoản", c0, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Họ tên",    c1, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Vai trò",   c2, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Mật khẩu",  c3, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Trạng thái", c4, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);

    float yDong = yBang + 44;
    for (int i = 0; i < (int)ds.size(); ++i) {
        Rectangle dong = {dauBang.x, yDong, dauBang.width, 48};
        if (dong.y + dong.height > theBang.y + theBang.height - 16) break;
        bool hover = Widgets::diChuotVao(dong);
        bool chon = (i == dangChon);
        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

        Color nen = chon ? Color{219, 234, 254, 255} : (hover ? Color{248, 250, 252, 255} : Theme::TRANG);
        DrawRectangleRounded(dong, 0.2f, 6, nen);
        if (chon) DrawRectangleRoundedLinesEx(dong, 0.2f, 6, 1.5f, Theme::CHINH);

        const auto& t = ds[i];
        Theme::Chu(t.tenDangNhap, c0, yDong + 14, Theme::CO_NHO + 1, Theme::CHU, true);
        Theme::Chu(Theme::CatChu(t.hoTen, c2 - c1 - 8, Theme::CO_NHO), c1, yDong + 14, Theme::CO_NHO, Theme::CHU);
        Theme::Chu(PhanQuyen::tenVaiTro(t.vaiTro), c2, yDong + 14, Theme::CO_NHO - 1, Theme::CHINH, true);
        Theme::Chu(Theme::CatChu(t.matKhau, c4 - c3 - 8, Theme::CO_NHO - 1), c3, yDong + 14, Theme::CO_NHO - 1, Theme::CHU_PHU);

        // Nhan trang thai + nut Mo khoa nhanh ngay tren dong neu bi khoa
        std::string tt = t.biKhoa ? "ĐÃ KHÓA" : (t.soLanSai > 0 ? ("Sai " + std::to_string(t.soLanSai) + "/5") : "Hoạt động");
        Color mauTT = t.biKhoa ? Theme::LOI : (t.soLanSai > 0 ? Theme::CANH_BAO : Theme::THANH_CONG);
        Theme::Chu(tt, c4, yDong + 14, Theme::CO_NHO - 1, mauTT, true);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) chonTaiKhoan(i);
        yDong += 52;
    }

    // ---------- 2. Form chinh sua / mo khoa ----------
    Widgets::veThe(theForm, 16);
    float fx = theForm.x + 24, fy = theForm.y + 20, fr = theForm.width - 48;
    bool dangSua = (dangChon >= 0 && dangChon < (int)ds.size());
    Theme::Chu(dangSua ? ("Tài khoản: " + ds[dangChon].tenDangNhap) : "Tạo tài khoản mới",
               fx, fy, Theme::CO_VUA - 2, Theme::CHU, true);

    fy += 32;
    Theme::Chu("Tên đăng nhập", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    if (dangSua) {
        oTenDN.dangChon = false;
        Rectangle hop = {fx, fy + 20, fr, 36};
        DrawRectangleRounded(hop, 0.25f, 6, Color{241, 245, 249, 255});
        Theme::Chu(oTenDN.giaTri, fx + 12, fy + 29, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    } else {
        VeOTextNhap({fx, fy + 20, fr, 36}, oTenDN);
    }

    fy += 60;
    Theme::Chu("Mật khẩu (lưu trực tiếp)", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oMatKhau);

    fy += 60;
    Theme::Chu("Họ tên nhân viên hiển thị", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oHoTen);

    fy += 60;
    Theme::Chu("Mã nhân viên liên kết (tùy chọn)", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oMaNV);

    fy += 60;
    Theme::Chu("Vai trò phân quyền", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    float rongVT = (fr - 16) / 3;
    const VaiTro dsVT[3] = {VaiTro::QUAN_LY, VaiTro::LE_TAN, VaiTro::BUONG_PHONG};
    for (int i = 0; i < 3; ++i) {
        Rectangle rvt = {fx + i * (rongVT + 8), fy + 22, rongVT, 34};
        bool chon = (vaiTroChon == dsVT[i]);
        if (VeNut(rvt, PhanQuyen::tenVaiTro(dsVT[i]), chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO))
            vaiTroChon = dsVT[i];
    }

    fy += 70;
    if (dangSua) {
        const auto& tk = ds[dangChon];
        if (VeNut({fx, fy, fr, 40}, "Lưu thông tin & mật khẩu", KieuNut::CHINH, true, Theme::CO_THUONG)) {
            KetQua kq = auth.capNhatTaiKhoan(tk.tenDangNhap, oMatKhau.giaTri, oHoTen.giaTri, oMaNV.giaTri, vaiTroChon);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { int c = dangChon; taiLai(); chonTaiKhoan(c); }
        }
        fy += 48;
        // Nut Mo khoa (khi tai khoan bi khoa do sai 5 lan) hoac Khoa tai khoan
        std::string nhanKhoa = tk.biKhoa ? "Mở khóa tài khoản" : "Khóa tài khoản";
        KieuNut kieuKhoa = tk.biKhoa ? KieuNut::CHINH : KieuNut::PHU;
        if (VeNut({fx, fy, fr - 100, 40}, nhanKhoa, kieuKhoa, true, Theme::CO_NHO + 1)) {
            KetQua kq = auth.datKhoaTaiKhoan(tk.tenDangNhap, !tk.biKhoa);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { int c = dangChon; taiLai(); chonTaiKhoan(c); }
        }
        if (VeNut({fx + fr - 90, fy, 90, 40}, "Xóa", KieuNut::NGUY_HIEM, true, Theme::CO_NHO + 1)) {
            hopXoa.moHop("Xóa tài khoản", "Bạn có chắc muốn xóa tài khoản " + tk.tenDangNhap + "?", "Xóa", true);
        }
    } else {
        if (VeNut({fx, fy, fr, 42}, "Tạo tài khoản", KieuNut::CHINH, true, Theme::CO_THUONG)) {
            KetQua kq = auth.themTaiKhoan(oTenDN.giaTri, oMatKhau.giaTri, oHoTen.giaTri, oMaNV.giaTri, vaiTroChon);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { taiLai(); chonTaiKhoan((int)ds.size() - 1); }
        }
    }

    if (hopXoa.ve() == 1 && dangSua) {
        KetQua kq = auth.xoaTaiKhoan(ds[dangChon].tenDangNhap);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        if (kq.ok) { taiLai(); datFormThemMoi(); }
    }
}
