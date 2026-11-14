// CustomerScreen.cpp: Bang danh sach khach hang, tim kiem, diem tich luy, hang thanh vien va giam gia.
#include "CustomerScreen.h"
#include <algorithm>
#include <cctype>
#include "Theme.h"

namespace {
std::string vietHoa(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::toupper(c); });
    return s;
}

Color mauHang(HangThanhVien h) {
    switch (h) {
        case HangThanhVien::THUONG:     return Theme::CHU_PHU;
        case HangThanhVien::BAC:        return Color{71, 85, 105, 255};
        case HangThanhVien::VANG:       return Color{217, 119, 6, 255};
        case HangThanhVien::KIM_CUONG: return Color{147, 51, 234, 255};
    }
    return Theme::CHU_PHU;
}
} // namespace

CustomerScreen::CustomerScreen(KhachSanService& s) : service(s) {
    oTimKiem.goiY = "Tìm theo tên, SĐT, CCCD, mã KH...";
    oTimKiem.toiDa = 32;

    oMa.goiY = "VD: KH03";       oMa.toiDa = 12;
    oHoTen.goiY = "Họ và tên";   oHoTen.toiDa = 48;
    oCCCD.goiY = "Số CCCD";      oCCCD.toiDa = 16;
    oSDT.goiY = "Số điện thoại"; oSDT.toiDa = 15;
    oEmail.goiY = "Email";       oEmail.toiDa = 48;
    oDiaChi.goiY = "Địa chỉ";    oDiaChi.toiDa = 64;
}

void CustomerScreen::taiLai() {
    service.danhSachKhachHang(dsKhach);
    apDungLoc();
    if (dangChon >= (int)dsKhachLoc.size()) dangChon = -1;
}

void CustomerScreen::apDungLoc() {
    dsKhachLoc.clear();
    std::string tuKhoa = vietHoa(oTimKiem.giaTri);
    for (const auto& k : dsKhach) {
        if (!tuKhoa.empty() &&
            vietHoa(k.hoTen).find(tuKhoa) == std::string::npos &&
            vietHoa(k.ma).find(tuKhoa) == std::string::npos &&
            k.sdt.find(tuKhoa) == std::string::npos &&
            k.cccd.find(tuKhoa) == std::string::npos) {
            continue;
        }
        dsKhachLoc.push_back(k);
    }
}

void CustomerScreen::datFormThemMoi() {
    dangChon = -1;
    oMa.giaTri.clear();
    oHoTen.giaTri.clear();
    oCCCD.giaTri.clear();
    oSDT.giaTri.clear();
    oEmail.giaTri.clear();
    oDiaChi.giaTri.clear();
}

void CustomerScreen::chonKhach(int idx) {
    if (idx < 0 || idx >= (int)dsKhachLoc.size()) { datFormThemMoi(); return; }
    dangChon = idx;
    const auto& k = dsKhachLoc[idx];
    oMa.giaTri = k.ma;
    oHoTen.giaTri = k.hoTen;
    oCCCD.giaTri = k.cccd;
    oSDT.giaTri = k.sdt;
    oEmail.giaTri = k.email;
    oDiaChi.giaTri = k.diaChi;
}

void CustomerScreen::Enter() {
    taiLai();
    if (!dsKhachLoc.empty()) chonKhach(0);
    else datFormThemMoi();
}

void CustomerScreen::Update() {
    apDungLoc();
}

void CustomerScreen::Draw(Rectangle vung) {
    if (hopXoa.mo) Widgets::batDauKhung(true);

    float x = vung.x + 24, y = vung.y + 16, w = vung.width - 48, h = vung.height - 32;
    float rongForm = 380;
    Rectangle theBang = {x, y, w - rongForm - 20, h};
    Rectangle theForm = {theBang.x + theBang.width + 20, y, rongForm, h};

    // =========================================================================
    // 1. BANG DANH SACH KHACH HANG
    // =========================================================================
    Widgets::veThe(theBang, 16);

    // Toolbar tim kiem + Nut them moi
    float ty = theBang.y + 16, tx = theBang.x + 20;
    VeOTextNhap({tx, ty, 300, 38}, oTimKiem);

    if (VeNut({theBang.x + theBang.width - 156, ty, 136, 38}, "+ Thêm khách", KieuNut::CHINH, true, Theme::CO_NHO + 1))
        datFormThemMoi();

    // Tieu de cac cot
    float yBang = ty + 50;
    Rectangle dauBang = {theBang.x + 16, yBang, theBang.width - 32, 38};
    DrawRectangleRounded(dauBang, 0.25f, 6, Color{241, 245, 249, 255});

    float wB = dauBang.width;
    float c0 = dauBang.x + 10;
    float c1 = dauBang.x + wB * 0.16f;
    float c2 = dauBang.x + wB * 0.46f;
    float c3 = dauBang.x + wB * 0.68f;
    float c4 = dauBang.x + wB * 0.82f;

    Theme::Chu("Mã KH",    c0, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Họ và tên", c1, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("SĐT",       c2, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Điểm",      c3, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Hạng TV",   c4, yBang + 9, Theme::CO_NHO, Theme::CHU_PHU, true);

    // Noi dung bang
    float yDong = yBang + 44;
    for (int i = 0; i < (int)dsKhachLoc.size(); ++i) {
        Rectangle dong = {dauBang.x, yDong, dauBang.width, 48};
        if (dong.y + dong.height > theBang.y + theBang.height - 16) break;

        bool hover = Widgets::diChuotVao(dong);
        bool chon = (i == dangChon);
        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

        Color nen = chon ? Color{219, 234, 254, 255} : (hover ? Color{248, 250, 252, 255} : Theme::TRANG);
        DrawRectangleRounded(dong, 0.2f, 6, nen);
        if (chon) DrawRectangleRoundedLinesEx(dong, 0.2f, 6, 1.5f, Theme::CHINH);

        const auto& k = dsKhachLoc[i];
        Theme::Chu(k.ma, c0, yDong + 14, Theme::CO_NHO + 1, Theme::CHU, true);
        Theme::Chu(Theme::CatChu(k.hoTen, c2 - c1 - 8, Theme::CO_NHO), c1, yDong + 14, Theme::CO_NHO, Theme::CHU);
        Theme::Chu(k.sdt.empty() ? "—" : k.sdt, c2, yDong + 14, Theme::CO_NHO - 1, Theme::CHU_PHU);
        Theme::Chu(std::to_string(k.diem) + " đ", c3, yDong + 14, Theme::CO_NHO - 1, Theme::CHINH, true);

        // Huy hieu hang thanh vien
        Color mH = mauHang(k.hang);
        std::string nhanHang = Widgets::nhanHienThi(k.tenHang);
        float rongH = std::min(dauBang.x + wB - c4 - 8, Widgets::rongBadge(nhanHang, Theme::CO_NHO - 2, 10));
        Rectangle rH = {c4, yDong + 11, rongH, 26};
        DrawBadge(nhanHang, rH, mH, Theme::CO_NHO - 2, 10);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) chonKhach(i);
        yDong += 52;
    }

    // =========================================================================
    // 2. FORM CHI TIET / THEM KHACH
    // =========================================================================
    Widgets::veThe(theForm, 16);
    float fx = theForm.x + 22, fy = theForm.y + 20, fr = theForm.width - 44;

    bool dangSua = (dangChon >= 0 && dangChon < (int)dsKhachLoc.size());
    Theme::Chu(dangSua ? ("Khách: " + dsKhachLoc[dangChon].ma) : "Thêm khách hàng mới",
               fx, fy, Theme::CO_VUA - 2, Theme::CHU, true);

    // Neu dang chon khach -> hien badge hang + giam gia
    if (dangSua) {
        const auto& k = dsKhachLoc[dangChon];
        fy += 34;
        Rectangle rHangThe = {fx, fy, fr, 56};
        DrawRectangleRounded(rHangThe, 0.15f, 6, Fade(mauHang(k.hang), 0.10f));
        DrawRectangleRoundedLinesEx(rHangThe, 0.15f, 6, 1.2f, mauHang(k.hang));

        Theme::Chu("Hạng: " + Widgets::nhanHienThi(k.tenHang) + " (" + std::to_string(k.diem) + " điểm tích lũy)",
                   rHangThe.x + 12, rHangThe.y + 10, Theme::CO_NHO + 1, mauHang(k.hang), true);
        Theme::Chu("Ưu đãi giảm giá tiền phòng: -" + std::to_string(k.phanTramGiam) + "%",
                   rHangThe.x + 12, rHangThe.y + 32, Theme::CO_NHO, Theme::CHU);
        fy += 34;
    }

    fy += 30;
    Theme::Chu("Mã khách hàng", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    if (dangSua) {
        Rectangle rM = {fx, fy + 18, fr, 36};
        DrawRectangleRounded(rM, 0.25f, 6, Color{241, 245, 249, 255});
        Theme::Chu(oMa.giaTri, rM.x + 12, rM.y + 9, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    } else {
        VeOTextNhap({fx, fy + 18, fr, 36}, oMa);
    }

    fy += 58;
    Theme::Chu("Họ và tên", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 18, fr, 36}, oHoTen);

    fy += 58;
    Theme::Chu("Số CCCD", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    if (dangSua) {
        Rectangle rC = {fx, fy + 18, fr, 36};
        DrawRectangleRounded(rC, 0.25f, 6, Color{241, 245, 249, 255});
        Theme::Chu(oCCCD.giaTri.empty() ? "(Chưa có)" : oCCCD.giaTri, rC.x + 12, rC.y + 9, Theme::CO_NHO + 1, Theme::CHU_PHU);
    } else {
        VeOTextNhap({fx, fy + 18, fr, 36}, oCCCD);
    }

    fy += 58;
    Theme::Chu("Số điện thoại", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 18, fr, 36}, oSDT);

    fy += 58;
    Theme::Chu("Email", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 18, fr, 36}, oEmail);

    fy += 58;
    Theme::Chu("Địa chỉ", fx, fy, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
    VeOTextNhap({fx, fy + 18, fr, 36}, oDiaChi);

    fy += 60;
    if (dangSua) {
        if (VeNut({fx, fy, fr - 90, 42}, "Lưu thay đổi", KieuNut::CHINH, true, Theme::CO_THUONG)) {
            KetQua kq = service.capNhatKhachHang(oMa.giaTri, oHoTen.giaTri, oSDT.giaTri, oEmail.giaTri, oDiaChi.giaTri);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { int c = dangChon; taiLai(); chonKhach(c); }
        }
        if (VeNut({fx + fr - 80, fy, 80, 42}, "Xóa", KieuNut::NGUY_HIEM, true, Theme::CO_THUONG)) {
            hopXoa.moHop("Xóa khách hàng", "Bạn có chắc muốn xóa khách hàng " + oMa.giaTri + "?", "Xóa", true);
        }
    } else {
        if (VeNut({fx, fy, fr, 42}, "Thêm khách hàng", KieuNut::CHINH, true, Theme::CO_THUONG)) {
            KetQua kq = service.themKhachHang(oMa.giaTri, oHoTen.giaTri, oCCCD.giaTri, oSDT.giaTri, oEmail.giaTri, oDiaChi.giaTri);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
            if (kq.ok) { taiLai(); chonKhach((int)dsKhachLoc.size() - 1); }
        }
    }

    if (hopXoa.ve() == 1 && dangSua) {
        KetQua kq = service.xoaKhachHang(oMa.giaTri);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        if (kq.ok) { taiLai(); datFormThemMoi(); }
    }
}
