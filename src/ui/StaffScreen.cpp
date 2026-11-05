// StaffScreen.cpp: Quan ly xem danh sach nhan vien, bam vao dong de sua ho ten/CCCD/SDT/chuc vu/ngay vao lam.
#include "StaffScreen.h"
#include "Theme.h"

namespace {
const char* tenChucVuTV(ChucVu cv) {
    switch (cv) {
        case ChucVu::LE_TAN: return "Lễ tân";
        case ChucVu::THU_NGAN: return "Thu ngân";
        case ChucVu::QUAN_LY: return "Quản lý";
        default: return "Kế toán";
    }
}
} // namespace

StaffScreen::StaffScreen(KhachSanService& s) : service(s) {
    oMa.goiY = "VD: NV04";       oMa.toiDa = 12;
    oHoTen.goiY = "Nhập họ tên"; oHoTen.toiDa = 48;
    oCCCD.goiY = "Số CCCD (12 số)"; oCCCD.toiDa = 16;
    oSDT.goiY = "Số điện thoại"; oSDT.toiDa = 15;
    oNgay.goiY = "dd/mm/yyyy (trống = hôm nay)"; oNgay.toiDa = 12;
}

void StaffScreen::taiLai() {
    service.danhSachNhanVien(ds);
    if (dangChon >= (int)ds.size()) dangChon = -1;
}

void StaffScreen::datFormThemMoi() {
    dangChon = -1;
    oMa.giaTri.clear();
    oHoTen.giaTri.clear();
    oCCCD.giaTri.clear();
    oSDT.giaTri.clear();
    oNgay.giaTri = Date::homNay().str();
    chucVuChon = ChucVu::LE_TAN;
}

void StaffScreen::chonNhanVien(int idx) {
    if (idx < 0 || idx >= (int)ds.size()) { datFormThemMoi(); return; }
    dangChon = idx;
    const auto& n = ds[idx];
    oMa.giaTri = n.ma;
    oHoTen.giaTri = n.hoTen;
    oCCCD.giaTri = n.cccd;
    oSDT.giaTri = n.sdt;
    oNgay.giaTri = n.ngayVaoLam.str();
    chucVuChon = n.chucVu;
}

void StaffScreen::Enter() {
    taiLai();
    if (!ds.empty()) chonNhanVien(0);
    else datFormThemMoi();
}

void StaffScreen::Draw(Rectangle vung) {
    if (hopXoa.mo) Widgets::batDauKhung(true);

    float x = vung.x + 24, y = vung.y + 20, rong = vung.width - 48, cao = vung.height - 40;
    float rongForm = 390;
    Rectangle theBang = {x, y, rong - rongForm - 20, cao};
    Rectangle theForm = {theBang.x + theBang.width + 20, y, rongForm, cao};

    // ---------- 1. Bang danh sach nhan vien ----------
    Widgets::veThe(theBang, 16);
    Theme::Chu("Danh sách nhân viên (bấm vào dòng để chỉnh sửa)", theBang.x + 22, theBang.y + 18,
               Theme::CO_VUA - 2, Theme::CHU, true);
    if (VeNut({theBang.x + theBang.width - 156, theBang.y + 12, 134, 38}, "+ Thêm mới", KieuNut::CHINH, true, Theme::CO_NHO + 2))
        datFormThemMoi();

    // Tieu de cot
    float yBang = theBang.y + 64;
    Rectangle dauBang = {theBang.x + 16, yBang, theBang.width - 32, 38};
    DrawRectangleRounded(dauBang, 0.25f, 6, Color{241, 245, 249, 255});
    float wB = dauBang.width;
    float c0 = dauBang.x + 10;
    float c1 = dauBang.x + wB * 0.16f;
    float c2 = dauBang.x + wB * 0.46f;
    float c3 = dauBang.x + wB * 0.65f;
    float c4 = dauBang.x + wB * 0.82f;

    Theme::Chu("Mã NV",    c0, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Họ và tên", c1, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Chức vụ",   c2, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("SĐT",       c3, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Vào làm",   c4, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);

    float yDong = yBang + 44;
    for (int i = 0; i < (int)ds.size(); ++i) {
        Rectangle dong = {dauBang.x, yDong, dauBang.width, 46};
        if (dong.y + dong.height > theBang.y + theBang.height - 16) break;
        bool hover = Widgets::diChuotVao(dong);
        bool chon = (i == dangChon);
        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

        Color nen = chon ? Color{219, 234, 254, 255} : (hover ? Color{248, 250, 252, 255} : Theme::TRANG);
        DrawRectangleRounded(dong, 0.2f, 6, nen);
        if (chon) DrawRectangleRoundedLinesEx(dong, 0.2f, 6, 1.5f, Theme::CHINH);

        const auto& n = ds[i];
        Theme::Chu(n.ma, c0, yDong + 13, Theme::CO_NHO + 1, Theme::CHU, true);
        Theme::Chu(Theme::CatChu(n.hoTen, c2 - c1 - 8, Theme::CO_NHO), c1, yDong + 13, Theme::CO_NHO, Theme::CHU);
        Theme::Chu(tenChucVuTV(n.chucVu), c2, yDong + 13, Theme::CO_NHO - 1, Theme::CHINH, true);
        Theme::Chu(n.sdt.empty() ? "—" : n.sdt, c3, yDong + 13, Theme::CO_NHO - 1, Theme::CHU_PHU);
        Theme::Chu(n.ngayVaoLam.str(), c4, yDong + 13, Theme::CO_NHO - 1, Theme::CHU_PHU);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) chonNhanVien(i);
        yDong += 50;
    }

    // ---------- 2. Form them / cap nhat thong tin nhan vien ----------
    Widgets::veThe(theForm, 16);
    float fx = theForm.x + 24, fy = theForm.y + 20, fr = theForm.width - 48;
    bool dangSua = (dangChon >= 0 && dangChon < (int)ds.size());
    Theme::Chu(dangSua ? ("Sửa nhân viên: " + ds[dangChon].ma) : "Thêm nhân viên mới",
               fx, fy, Theme::CO_VUA - 2, Theme::CHU, true);

    fy += 32;
    Theme::Chu("Mã nhân viên", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    if (dangSua) {
        oMa.dangChon = false;
        Rectangle hopMa = {fx, fy + 20, fr, 36};
        DrawRectangleRounded(hopMa, 0.25f, 6, Color{241, 245, 249, 255});
        Theme::Chu(oMa.giaTri + " (không đổi mã)", fx + 12, fy + 29, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    } else {
        VeOTextNhap({fx, fy + 20, fr, 36}, oMa);
    }

    fy += 60;
    Theme::Chu("Họ và tên", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oHoTen);

    fy += 60;
    Theme::Chu("Số CCCD", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oCCCD);

    fy += 60;
    Theme::Chu("Số điện thoại", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oSDT);

    fy += 60;
    Theme::Chu("Ngày vào làm (dd/mm/yyyy)", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 20, fr, 36}, oNgay);

    fy += 60;
    Theme::Chu("Chức vụ", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    float rongNutCV = (fr - 12) / 2;
    const ChucVu dsCV[4] = {ChucVu::LE_TAN, ChucVu::THU_NGAN, ChucVu::QUAN_LY, ChucVu::KE_TOAN};
    for (int i = 0; i < 4; ++i) {
        Rectangle rcv = {fx + (i % 2) * (rongNutCV + 12), fy + 22 + (i / 2) * 38, rongNutCV, 34};
        bool chon = (chucVuChon == dsCV[i]);
        if (VeNut(rcv, tenChucVuTV(dsCV[i]), chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO))
            chucVuChon = dsCV[i];
    }

    fy += 106;
    if (dangSua) {
        if (VeNut({fx, fy, fr - 100, 42}, "Lưu thay đổi", KieuNut::CHINH, true, Theme::CO_THUONG)) {
            KetQua kq = service.capNhatNhanVien(oMa.giaTri, oHoTen.giaTri, oCCCD.giaTri,
                                                oSDT.giaTri, chucVuChon, oNgay.giaTri);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { int c = dangChon; taiLai(); chonNhanVien(c); }
        }
        if (VeNut({fx + fr - 90, fy, 90, 42}, "Xóa", KieuNut::NGUY_HIEM, true, Theme::CO_THUONG)) {
            hopXoa.moHop("Xóa nhân viên", "Bạn có chắc muốn xóa nhân viên " + oMa.giaTri + "?", "Xóa", true);
        }
    } else {
        if (VeNut({fx, fy, fr, 42}, "Thêm nhân viên", KieuNut::CHINH, true, Theme::CO_THUONG)) {
            KetQua kq = service.themNhanVien(oMa.giaTri, oHoTen.giaTri, oCCCD.giaTri,
                                             oSDT.giaTri, chucVuChon, oNgay.giaTri);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { taiLai(); chonNhanVien((int)ds.size() - 1); }
        }
    }

    if (hopXoa.ve() == 1 && dangSua) {
        KetQua kq = service.xoaNhanVien(oMa.giaTri);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        if (kq.ok) { taiLai(); datFormThemMoi(); }
    }
}
