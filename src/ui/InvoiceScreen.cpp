// InvoiceScreen.cpp: Xem chi tiet hoa don nhu to bien lai, thu tien nhieu hinh thuc, huy va xuat file.
#include "InvoiceScreen.h"
#include <algorithm>
#include <cctype>
#include <sstream>
#include "Common.h"
#include "PhanQuyen.h"
#include "Theme.h"
#include "Widgets.h"

namespace {
std::string vietHoa(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::toupper(c); });
    return s;
}

Color mauTrangThaiHD(TrangThaiHoaDon t) {
    switch (t) {
        case TrangThaiHoaDon::CHUA_THANH_TOAN:       return Theme::LOI;
        case TrangThaiHoaDon::THANH_TOAN_MOT_PHAN: return Theme::CANH_BAO;
        case TrangThaiHoaDon::DA_THANH_TOAN:         return Theme::THANH_CONG;
        case TrangThaiHoaDon::DA_HUY:                return Theme::CHU_PHU;
    }
    return Theme::CHU_PHU;
}
} // namespace

InvoiceScreen::InvoiceScreen(AuthService& a, KhachSanService& s) : auth(a), service(s) {
    oTimKiem.goiY = "Tìm mã HĐ, mã phòng, tên khách...";
    oTimKiem.toiDa = 32;

    oSoTienThu.goiY = "Số tiền thu";
    oSoTienThu.toiDa = 14;
    oSoTienThu.chiNhapTien = true;

    oSoThamChieu.goiY = "Mã GD ngân hàng / tham chiếu";
    oSoThamChieu.toiDa = 32;

    oNgayThu.toiDa = 12;
}

void InvoiceScreen::taiLai() {
    service.danhSachHoaDon(dsHoaDon);
    apDungLoc();
    if (!maHDChon.empty()) chonHoaDon(maHDChon);
}

void InvoiceScreen::apDungLoc() {
    dsHoaDonLoc.clear();
    std::string tuKhoa = vietHoa(oTimKiem.giaTri);

    for (const auto& h : dsHoaDon) {
        if (!tuKhoa.empty() &&
            vietHoa(h.maHD).find(tuKhoa) == std::string::npos &&
            vietHoa(h.maPhong).find(tuKhoa) == std::string::npos &&
            vietHoa(h.tenKhach).find(tuKhoa) == std::string::npos) {
            continue;
        }

        if (boLocTrangThai == 1 && h.trangThai != TrangThaiHoaDon::CHUA_THANH_TOAN && h.trangThai != TrangThaiHoaDon::THANH_TOAN_MOT_PHAN) continue;
        if (boLocTrangThai == 2 && h.trangThai != TrangThaiHoaDon::DA_THANH_TOAN) continue;
        if (boLocTrangThai == 3 && h.trangThai != TrangThaiHoaDon::DA_HUY) continue;

        dsHoaDonLoc.push_back(h);
    }
}

void InvoiceScreen::chonHoaDon(const std::string& maHD) {
    maHDChon = maHD;
    KetQua kq = service.chiTietHoaDon(maHD, chiTiet);
    if (!kq.ok) { maHDChon.clear(); return; }

    oSoTienThu.giaTri = Widgets::dinhDangTienNhap(std::to_string(chiTiet.conPhaiTra));
    oSoThamChieu.giaTri.clear();
    oNgayThu.giaTri = Date::homNay().str();
    hinhThucChonIdx = 0;
}

void InvoiceScreen::Enter() {
    maHDChon.clear();
    boLocTrangThai = 0;
    oTimKiem.giaTri.clear();
    taiLai();
    if (!dsHoaDonLoc.empty()) chonHoaDon(dsHoaDonLoc.front().maHD);
}

void InvoiceScreen::Update() {
    apDungLoc();
}

void InvoiceScreen::Draw(Rectangle vung) {
    if (hopHuyHD.mo || moModalXemIn) Widgets::batDauKhung(true);

    float x = vung.x + 24, y = vung.y + 16, w = vung.width - 48, h = vung.height - 32;
    float rongToIn = std::min(420.0f, std::max(370.0f, w * 0.40f));
    float rongBangTrai = w - rongToIn - 20.0f;

    // =========================================================================
    // 1. BANG DANH SACH HOA DON
    // =========================================================================
    Rectangle theBang = {x, y, rongBangTrai, h};
    Widgets::veThe(theBang, 16);

    // Toolbar 2 hang ro rang, khong the chen chuc
    float ty = theBang.y + 14, tx = theBang.x + 16, tw = theBang.width - 32;
    float rongTK = std::min(240.0f, tw * 0.55f);
    VeOTextNhap({tx, ty, rongTK, 36}, oTimKiem);

    // Hang 2: Tabs bo loc trang thai
    float gy = ty + 44;
    const char* dsTab[] = {"Tất cả", "Chưa xong", "Đã thu đủ", "Đã hủy"};
    float tabX = tx;
    for (int i = 0; i < 4; ++i) {
        Vector2 kt = Theme::DoChu(dsTab[i], Theme::CO_NHO - 1, true);
        Rectangle rTab = {tabX, gy, kt.x + 20, 32};
        bool chon = (boLocTrangThai == i);
        if (VeNut(rTab, dsTab[i], chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 1)) {
            boLocTrangThai = i;
            apDungLoc();
        }
        tabX += rTab.width + 6;
    }

    DrawLine((int)theBang.x + 14, (int)gy + 40, (int)(theBang.x + theBang.width - 14), (int)gy + 40, Theme::VIEN);

    // Tieu de cot (tinh ty le % de khong bao gio bi tran/de nhau)
    float yBang = gy + 48;
    Rectangle dauBang = {theBang.x + 14, yBang, theBang.width - 28, 38};
    DrawRectangleRounded(dauBang, 0.25f, 6, Color{241, 245, 249, 255});

    float wB = dauBang.width;
    float c0 = dauBang.x + 8;
    // Cot "Trang thai" rong hon (24%) de chua nhan tieng Viet "Chưa thanh toán"
    float c1 = dauBang.x + wB * 0.12f;
    float c2 = dauBang.x + wB * 0.22f;
    float c3 = dauBang.x + wB * 0.42f;
    float c4 = dauBang.x + wB * 0.56f;
    float c5 = dauBang.x + wB * 0.76f;
    float rongCotTT = dauBang.x + wB - c5 - 8;

    Theme::Chu("Mã HĐ",    c0, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Phòng",    c1, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Khách hàng", c2, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Ngày lập", c3, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Tổng cộng", c4, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);
    Theme::Chu("Trạng thái", c5, yBang + 10, Theme::CO_NHO, Theme::CHU_PHU, true);

    // Noi dung bang
    float yDong = yBang + 44;
    for (int i = 0; i < (int)dsHoaDonLoc.size(); ++i) {
        Rectangle dong = {dauBang.x, yDong, dauBang.width, 48};
        if (dong.y + dong.height > theBang.y + theBang.height - 16) break;

        bool hover = Widgets::diChuotVao(dong);
        bool chon = (dsHoaDonLoc[i].maHD == maHDChon);
        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);

        Color nen = chon ? Color{219, 234, 254, 255} : (hover ? Color{248, 250, 252, 255} : Theme::TRANG);
        DrawRectangleRounded(dong, 0.2f, 6, nen);
        if (chon) DrawRectangleRoundedLinesEx(dong, 0.2f, 6, 1.5f, Theme::CHINH);

        const auto& hd = dsHoaDonLoc[i];
        Theme::Chu(hd.maHD, c0, yDong + 14, Theme::CO_NHO + 1, Theme::CHU, true);
        Theme::Chu(hd.maPhong, c1, yDong + 14, Theme::CO_NHO + 1, Theme::CHINH, true);
        Theme::Chu(Theme::CatChu(hd.tenKhach, (c3 - c2 - 8), Theme::CO_NHO), c2, yDong + 14, Theme::CO_NHO, Theme::CHU);
        Theme::Chu(hd.ngayLap.str(), c3, yDong + 14, Theme::CO_NHO - 1, Theme::CHU_PHU);
        Theme::Chu(Theme::CatChu(dinhDangTien(hd.tongCong), c5 - c4 - 6, Theme::CO_NHO - 1, true), c4, yDong + 15,
                   Theme::CO_NHO - 1, Theme::CHU, true);

        // Huy hieu trang thai: nhan tieng Viet, rong theo do chu + dem 2 ben, can giua
        Color mTT = mauTrangThaiHD(hd.trangThai);
        std::string nhanTT = Widgets::nhanHienThi(hd.tenTrangThai);
        float rTTW = std::min(rongCotTT, Widgets::rongBadge(nhanTT, Theme::CO_NHO - 2, 10));
        Rectangle rTT = {c5, yDong + 11, rTTW, 26};
        DrawBadge(nhanTT, rTT, mTT, Theme::CO_NHO - 2, 10);

        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) chonHoaDon(hd.maHD);
        yDong += 52;
    }

    // =========================================================================
    // 2. TO PHIEU HOA DON CHI TIET (RIGHT SIDE)
    // =========================================================================
    Rectangle theToIn = {theBang.x + theBang.width + 20, y, rongToIn, h};
    Widgets::veThe(theToIn, 16);

    float ix = theToIn.x + 22, iy = theToIn.y + 18, iw = theToIn.width - 44;

    if (maHDChon.empty()) {
        Theme::Chu("Chi tiết hóa đơn", ix, iy, Theme::CO_VUA, Theme::CHU, true);
        Theme::Chu("Bấm chọn 1 hóa đơn bên trái để xem phiếu và thu tiền.", ix, iy + 36, Theme::CO_THUONG, Theme::CHU_PHU);
    } else {
        // Tieu de to bien lai: thu nho co chu neu khong vua chieu rong panel
        const std::string tieuDe = "HÓA ĐƠN THANH TOÁN";
        int coTD = Theme::CO_LON;
        while (coTD > Theme::CO_NHO + 4 && Theme::DoChu(tieuDe, coTD, true).x > iw) --coTD;
        float rongTD = Theme::DoChu(tieuDe, coTD, true).x;
        Theme::Chu(tieuDe, ix, iy, coTD, Theme::CHU, true);

        // Huy hieu trang thai: cung dong tieu de neu du cho, neu khong thi xuong dong 2 (can phai)
        Color mTT = mauTrangThaiHD(chiTiet.trangThai);
        std::string nhanTT = Widgets::nhanHienThi(chiTiet.tenTrangThai);
        float rongH = std::min(iw, Widgets::rongBadge(nhanTT, Theme::CO_NHO, 12));
        bool cungDong = (rongTD + 16 + rongH <= iw);
        Rectangle rH = {ix + iw - rongH, cungDong ? iy + 1 : iy + 33, rongH, 28};
        DrawBadge(nhanTT, rH, mTT, Theme::CO_NHO, 12);

        std::string dong2 = chiTiet.maHD + " • Phòng " + chiTiet.maPhong;
        float rongD2 = cungDong ? iw : (iw - rongH - 12);
        Theme::Chu(Theme::CatChu(dong2, rongD2, Theme::CO_NHO + 2, true), ix, iy + 36, Theme::CO_NHO + 2, Theme::CHINH, true);

        DrawLine((int)ix, (int)iy + 68, (int)(ix + iw), (int)iy + 68, Theme::VIEN);
        float py = iy + 76;

        // Thong tin khach & phong
        Theme::Chu(Theme::CatChu("Khách hàng: " + chiTiet.tenKhach + " (" + Widgets::nhanHienThi(chiTiet.tenHangKhach) + ")",
                                 iw, Theme::CO_NHO + 1, true), ix, py, Theme::CO_NHO + 1, Theme::CHU, true);
        Theme::Chu("Thời gian: " + chiTiet.ngayNhan.str() + " → " + chiTiet.ngayTra.str() + " (" + std::to_string(chiTiet.soDem) + " đêm)",
                   ix, py + 22, Theme::CO_NHO, Theme::CHU_PHU);
        Theme::Chu("Nhân viên lập: " + chiTiet.tenNVLap + " • Ngày: " + chiTiet.ngayLap.str(), ix, py + 42, Theme::CO_NHO - 1, Theme::CHU_PHU);

        py += 68;

        // Bang chi tiet dong tien
        Rectangle rBangTien = {ix, py, iw, 140};
        DrawRectangleRounded(rBangTien, 0.12f, 6, Color{248, 250, 252, 255});
        DrawRectangleRoundedLinesEx(rBangTien, 0.12f, 6, 1.0f, Theme::VIEN);

        float ry = rBangTien.y + 10;
        Theme::Chu("Tiền phòng:", rBangTien.x + 12, ry, Theme::CO_NHO + 1, Theme::CHU_PHU);
        Theme::Chu(dinhDangTien(chiTiet.tienPhong), rBangTien.x + rBangTien.width - 150, ry, Theme::CO_NHO + 1, Theme::CHU, true);

        ry += 24;
        Theme::Chu("Tiền dịch vụ:", rBangTien.x + 12, ry, Theme::CO_NHO + 1, Theme::CHU_PHU);
        Theme::Chu(dinhDangTien(chiTiet.tienDichVu), rBangTien.x + rBangTien.width - 150, ry, Theme::CO_NHO + 1, Theme::CHU, true);

        ry += 24;
        Theme::Chu("Giảm giá thành viên:", rBangTien.x + 12, ry, Theme::CO_NHO + 1, Theme::CHU_PHU);
        // Chi them dau "-" khi thuc su co giam gia
        std::string chuGiam = chiTiet.giamGia > 0 ? "-" + dinhDangTien(chiTiet.giamGia) : dinhDangTien(0);
        Theme::Chu(chuGiam, rBangTien.x + rBangTien.width - 150, ry, Theme::CO_NHO + 1, Theme::THANH_CONG, true);

        ry += 24;
        Theme::Chu("Thuế VAT (10%):", rBangTien.x + 12, ry, Theme::CO_NHO + 1, Theme::CHU_PHU);
        Theme::Chu("+" + dinhDangTien(chiTiet.thueVAT), rBangTien.x + rBangTien.width - 150, ry, Theme::CO_NHO + 1, Theme::CHU, true);

        ry += 26;
        DrawLine((int)rBangTien.x + 8, (int)ry, (int)(rBangTien.x + rBangTien.width - 8), (int)ry, Theme::VIEN);
        ry += 6;
        Theme::Chu("TỔNG CỘNG:", rBangTien.x + 12, ry, Theme::CO_THUONG, Theme::LOI, true);
        Theme::Chu(dinhDangTien(chiTiet.tongCong), rBangTien.x + rBangTien.width - 160, ry, Theme::CO_THUONG, Theme::LOI, true);

        py += 152;

        // Da thu & Con lai: 2 dong rieng, nhan ben trai - so tien can phai (khong bao gio de nhau)
        {
            Color mauCon = chiTiet.conPhaiTra > 0 ? Theme::LOI : Theme::THANH_CONG;
            std::string tienDa = dinhDangTien(chiTiet.daThanhToan), tienCon = dinhDangTien(chiTiet.conPhaiTra);
            Theme::Chu("Đã thanh toán:", ix + 12, py, Theme::CO_NHO + 1, Theme::THANH_CONG, true);
            Theme::Chu(tienDa, ix + iw - 12 - Theme::DoChu(tienDa, Theme::CO_NHO + 1, true).x, py, Theme::CO_NHO + 1, Theme::THANH_CONG, true);
            Theme::Chu("CÒN PHẢI TRẢ:", ix + 12, py + 24, Theme::CO_NHO + 1, mauCon, true);
            Theme::Chu(tienCon, ix + iw - 12 - Theme::DoChu(tienCon, Theme::CO_NHO + 1, true).x, py + 24, Theme::CO_NHO + 1, mauCon, true);
        }

        py += 54;

        // ---------------------------------------------------------------------
        // FORM THU TIEN HOAC THONG TIN DA HOAN TAT
        // ---------------------------------------------------------------------
        if (chiTiet.trangThai != TrangThaiHoaDon::DA_HUY && chiTiet.conPhaiTra > 0) {
            DrawLine((int)ix, (int)py, (int)(ix + iw), (int)py, Theme::VIEN);
            py += 10;
            Theme::Chu("THU TIỀN HÓA ĐƠN", ix, py, Theme::CO_NHO + 2, Theme::CHU, true);

            py += 24;
            // Cac nut chon hinh thuc thanh toan
            const char* dsHT[] = {"Tiền mặt", "Chuyển khoản", "Thẻ", "Ví điện tử", "QR"};
            float rHT = (iw - 8 * 4) / 5.0f;
            for (int i = 0; i < 5; ++i) {
                Rectangle rBtn = {ix + i * (rHT + 8), py, rHT, 32};
                bool chon = (hinhThucChonIdx == i);
                if (VeNut(rBtn, dsHT[i], chon ? KieuNut::CHINH : KieuNut::PHU, true, Theme::CO_NHO - 2))
                    hinhThucChonIdx = i;
            }

            py += 40;
            Theme::Chu("Số tiền thu:", ix, py + 6, Theme::CO_NHO + 1, Theme::CHU_PHU, true);
            VeOTextNhap({ix + 90, py, 140, 36}, oSoTienThu);

            if (VeNut({ix + 240, py, 85, 36}, "Thu hết", KieuNut::PHU, true, Theme::CO_NHO)) {
                oSoTienThu.giaTri = Widgets::dinhDangTienNhap(std::to_string(chiTiet.conPhaiTra));
            }

            py += 44;
            Theme::Chu("Số tham chiếu (với CK/Thẻ/QR):", ix, py, Theme::CO_NHO, Theme::CHU_PHU);
            VeOTextNhap({ix, py + 20, iw, 36}, oSoThamChieu);

            py += 64;
            if (VeNut({ix, py, iw, 44}, "Xác nhận thu tiền", KieuNut::CHINH, true, Theme::CO_THUONG)) {
                long long tien = Widgets::layGiaTriTien(oSoTienThu.giaTri);
                KetQua kq = service.thanhToanHoaDon(chiTiet.maHD, tien, static_cast<HinhThucThanhToan>(hinhThucChonIdx),
                                                    oSoThamChieu.giaTri, oNgayThu.giaTri);
                ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
                if (kq.ok) taiLai();
            }

        } else if (chiTiet.trangThai == TrangThaiHoaDon::DA_THANH_TOAN) {
            DrawLine((int)ix, (int)py, (int)(ix + iw), (int)py, Theme::VIEN);
            py += 14;
            Rectangle rXong = {ix, py, iw, 70};
            DrawRectangleRounded(rXong, 0.15f, 6, Fade(Theme::THANH_CONG, 0.10f));
            DrawRectangleRoundedLinesEx(rXong, 0.15f, 6, 1.2f, Theme::THANH_CONG);
            Theme::Chu("HÓA ĐƠN ĐÃ ĐƯỢC TẤT TOÁN ĐỦ", rXong.x + 16, rXong.y + 14, Theme::CO_THUONG, Theme::THANH_CONG, true);
            Theme::Chu("Khách đã được tích lũy điểm thành viên theo quy định.", rXong.x + 16, rXong.y + 38, Theme::CO_NHO + 1, Theme::CHU);
        }

        // Nut Xem phieu in, Xuat file & Huy hoa don o day panel
        float bottomY = theToIn.y + theToIn.height - 50;
        float rongHuy = auth.coQuyen(Quyen::HUY_HOA_DON) ? 80.0f : 0.0f;
        float rongCon = iw - (rongHuy > 0 ? (rongHuy + 8) : 0);
        float rongBtn = (rongCon - 8) / 2.0f;

        Rectangle rXemIn = {ix, bottomY, rongBtn, 38};
        Rectangle rXuat = {ix + rongBtn + 8, bottomY, rongBtn, 38};

        if (VeNut(rXemIn, "Xem phiếu in", KieuNut::CHINH, true, Theme::CO_NHO)) {
            service.xuatHoaDonTxt(chiTiet.maHD, noiDungXemIn);
            moModalXemIn = true;
        }

        if (VeNut(rXuat, "Lưu file .txt", KieuNut::PHU, true, Theme::CO_NHO)) {
            std::string nd;
            KetQua kq = service.xuatHoaDonTxt(chiTiet.maHD, nd);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        }

        if (auth.coQuyen(Quyen::HUY_HOA_DON)) {
            Rectangle rHuy = {ix + iw - rongHuy, bottomY, rongHuy, 38};
            bool coTheHuy = (chiTiet.trangThai != TrangThaiHoaDon::DA_HUY && chiTiet.dsThanhToan.empty());
            if (VeNut(rHuy, "Hủy HĐ", KieuNut::NGUY_HIEM, coTheHuy, Theme::CO_NHO - 1)) {
                hopHuyHD.moHop("Hủy hóa đơn", "Bạn có chắc muốn hủy hóa đơn " + chiTiet.maHD + "?", "Hủy hóa đơn", true);
            }
        }
    }

    if (hopHuyHD.ve() == 1) {
        KetQua kq = service.huyHoaDon(chiTiet.maHD);
        ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        if (kq.ok) taiLai();
    }

    // Modal Xem truoc to phieu hoa don ngay tren App
    if (moModalXemIn) {
        Widgets::batDauHopThoai();
        float sw = (float)GetScreenWidth(), sh = (float)GetScreenHeight();
        DrawRectangle(0, 0, (int)sw, (int)sh, Fade(BLACK, 0.50f));

        Rectangle hop = {sw / 2 - 270, sh / 2 - 280, 540, 560};
        Widgets::veThe(hop, 16);
        Theme::Chu("Phiếu hóa đơn: " + chiTiet.maHD, hop.x + 24, hop.y + 18, Theme::CO_VUA, Theme::CHU, true);

        // Khung giay in hoa don (paper bill)
        Rectangle rGiay = {hop.x + 20, hop.y + 54, hop.width - 40, hop.height - 116};
        DrawRectangleRounded(rGiay, 0.08f, 6, Color{248, 250, 252, 255});
        DrawRectangleRoundedLinesEx(rGiay, 0.08f, 6, 1.0f, Theme::VIEN);

        std::istringstream iss(noiDungXemIn);
        std::string dong;
        float dy = rGiay.y + 12;
        while (std::getline(iss, dong)) {
            if (dy + 18 > rGiay.y + rGiay.height - 8) break;
            Theme::Chu(dong, rGiay.x + 14, dy, Theme::CO_NHO - 1, Theme::CHU);
            dy += 19;
        }

        // Nut bam o chan hop thoai
        Rectangle rDong = {hop.x + hop.width - 20 - 110, hop.y + hop.height - 48, 110, 38};
        Rectangle rLuu = {rDong.x - 10 - 130, rDong.y, 130, 38};

        if (VeNut(rDong, "Đóng", KieuNut::PHU, true, Theme::CO_NHO + 1)) moModalXemIn = false;
        if (VeNut(rLuu, "Lưu file .txt", KieuNut::CHINH, true, Theme::CO_NHO + 1)) {
            std::string nd;
            KetQua kq = service.xuatHoaDonTxt(chiTiet.maHD, nd);
            ThongBao::hien(kq.thongBao, kq.ok ? ThongBao::Loai::THANH_CONG : ThongBao::Loai::LOI);
        }
        Widgets::ketThucHopThoai();
    }
}
