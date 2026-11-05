// LoginScreen.cpp: anh truong/khach san Bach Khoa giao thoa muot sang nen form dang nhap + dem nguoc tam khoa 30s.
#include "LoginScreen.h"
#include <algorithm>
#include "Theme.h"
#include "raygui.h"

LoginScreen::LoginScreen(AuthService& a) : auth(a) {
    oTen.goiY = "Nhập tên đăng nhập (vd: admin, letan)";
    oTen.toiDa = 32;
    oMatKhau.goiY = "Nhập mật khẩu";
    oMatKhau.anKyTu = true;
    oMatKhau.toiDa = 64;
}

void LoginScreen::napTaiNguyen() {
    if (FileExists("assets/images/hotel.png")) {
        anh = LoadTexture("assets/images/hotel.png");
        GenTextureMipmaps(&anh);
        SetTextureFilter(anh, TEXTURE_FILTER_TRILINEAR);
    }
}

void LoginScreen::giaiPhong() {
    if (anh.id) UnloadTexture(anh);
    anh = Texture2D{};
}

void LoginScreen::Enter() {
    oTen.giaTri.clear();
    oMatKhau.giaTri.clear();
    oTen.dangChon = true;
    oMatKhau.dangChon = false;
    hienMatKhau = false;
    loi.clear();
    vuaThanhCong = false;
}

void LoginScreen::thuDangNhap() {
    KetQua kq = auth.dangNhap(oTen.giaTri, oMatKhau.giaTri);
    if (kq.ok) {
        loi.clear();
        oMatKhau.giaTri.clear();
        vuaThanhCong = true;
        ThongBao::hien(kq.thongBao, ThongBao::Loai::THANH_CONG);
    } else {
        loi = kq.thongBao;
        oMatKhau.giaTri.clear();
        oTen.dangChon = oTen.giaTri.empty();
        oMatKhau.dangChon = !oTen.dangChon;
    }
}

bool LoginScreen::layVaXoaCoThanhCong() {
    bool c = vuaThanhCong;
    vuaThanhCong = false;
    return c;
}

void LoginScreen::Update() {
    if (IsKeyPressed(KEY_TAB)) {
        bool sangMatKhau = !oMatKhau.dangChon;
        oTen.dangChon = !sangMatKhau;
        oMatKhau.dangChon = sangMatKhau;
    }
    if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER)) {
        if (oTen.dangChon && oMatKhau.giaTri.empty() && !oTen.giaTri.empty()) {
            oTen.dangChon = false;
            oMatKhau.dangChon = true;
        } else {
            thuDangNhap();
        }
    }
}

void LoginScreen::Draw(Rectangle vung) {
    DrawRectangleRec(vung, Theme::NEN_SANG);

    // ---------- 1. Anh ben trai lan sang phai (62% chieu rong) + vung giao thoa muot ----------
    Rectangle vungAnh = {vung.x, vung.y, vung.width * 0.62f, vung.height};
    Widgets::veAnhPhuKin(anh, vungAnh);

    // Lop phu toi nhe o goc trai duoi de chu noi bat tren nen anh
    DrawRectangleGradientV((int)vungAnh.x, (int)(vungAnh.y + vungAnh.height * 0.38f),
                           (int)vungAnh.width, (int)(vungAnh.height * 0.62f) + 1,
                           Fade(BLACK, 0.0f), Fade(Color{15, 23, 42, 255}, 0.82f));

    // Dai giao thoa ngang: hoa tron muot tu anh sang mau nen form ben phai (khong co vach cat cung)
    float rongGiaoThoa = vung.width * 0.24f;
    float xGiaoThoa = vungAnh.x + vungAnh.width - rongGiaoThoa;
    DrawRectangleGradientH((int)xGiaoThoa, (int)vung.y, (int)rongGiaoThoa + 2, (int)vung.height,
                           Fade(Theme::NEN_SANG, 0.0f), Theme::NEN_SANG);
    // Them 1 lop nua o sat mep phai cua anh de chuyen mau cuc em
    DrawRectangleGradientH((int)(xGiaoThoa + rongGiaoThoa * 0.45f), (int)vung.y,
                           (int)(rongGiaoThoa * 0.55f) + 2, (int)vung.height,
                           Fade(Theme::NEN_SANG, 0.0f), Theme::NEN_SANG);

    // Thong tin Khach san Bach Khoa o goc trai duoi
    float xChu = vung.x + 48;
    float yChu = vung.y + vung.height - 170;
    DrawRectangleRounded({xChu, yChu - 36, 178, 28}, 1.0f, 8, Fade(Theme::CHINH, 0.90f));
    Theme::ChuGiua("PBL QUẢN LÝ KHÁCH SẠN", {xChu, yChu - 36, 178, 28}, Theme::CO_NHO - 1, WHITE, true);

    Theme::Chu("KHÁCH SẠN BÁCH KHOA", xChu, yChu, Theme::CO_TIEU_DE, WHITE, true);
    Theme::Chu("54 Nguyễn Lương Bằng, P. Hòa Khánh Bắc,", xChu + 2, yChu + 52,
               Theme::CO_THUONG, Color{241, 245, 249, 255}, true);
    Theme::Chu("Q. Liên Chiểu, TP. Đà Nẵng", xChu + 2, yChu + 80,
               Theme::CO_THUONG, Color{226, 232, 240, 255});
    DrawRectangle((int)xChu + 2, (int)(yChu + 116), 84, 4, Theme::CHINH);

    // ---------- 2. The dang nhap noi tren vung giao thoa ----------
    const float rongThe = 450, caoThe = 540;
    float xThe = vung.x + vung.width * 0.72f - rongThe * 0.5f;
    xThe = std::min(xThe, vung.x + vung.width - rongThe - 36.0f);
    Rectangle the = {xThe, vung.y + (vung.height - caoThe) / 2, rongThe, caoThe};

    // Vong sang mem phia sau the de tang cam giac giao thoa giua 2 nua
    Rectangle vongSang = {the.x - 18, the.y - 18, the.width + 36, the.height + 36};
    DrawRectangleRounded(vongSang, Widgets::boGoc(vongSang, 28), 12, Fade(WHITE, 0.55f));
    Widgets::veThe(the, 22);

    float x = the.x + 42, y = the.y + 36, rong = the.width - 84;
    Theme::Chu("Đăng nhập hệ thống", x, y, Theme::CO_LON + 2, Theme::CHU, true);
    Theme::Chu("Khách sạn Bách Khoa – Đà Nẵng", x, y + 40, Theme::CO_NHO + 2, Theme::CHU_PHU);

    y += 88;
    Theme::Chu("Tên đăng nhập", x, y, Theme::CO_NHO + 2, Theme::CHU, true);
    VeOTextNhap({x, y + 26, rong, 50}, oTen);

    y += 92;
    Theme::Chu("Mật khẩu", x, y, Theme::CO_NHO + 2, Theme::CHU, true);
    oMatKhau.anKyTu = !hienMatKhau;
    VeOTextNhap({x, y + 26, rong, 50}, oMatKhau);

    y += 88;
    GuiCheckBox({x, y, 20, 20}, "Hiện mật khẩu", &hienMatKhau);

    // Kiem tra dem nguoc tam khoa 30s theo thoi gian thuc
    int giayKhoa = std::max(auth.giayTamKhoaConLai(oTen.giaTri), auth.giayTamKhoaConLai("__chung__"));
    std::string dongLoi = giayKhoa > 0
        ? ("Đang tạm khóa đăng nhập: còn " + std::to_string(giayKhoa) + " giây...")
        : loi;

    y += 30;
    if (!dongLoi.empty()) {
        Rectangle hopLoi = {x, y - 4, rong, 32};
        DrawRectangleRounded(hopLoi, 0.35f, 6, Fade(Theme::LOI, 0.10f));
        Theme::Chu(Theme::CatChu(dongLoi, rong - 16, Theme::CO_NHO + 1), x + 8, y + 2,
                   Theme::CO_NHO + 1, Theme::LOI, true);
    }

    y += 38;
    if (VeNut({x, y, rong, 52}, giayKhoa > 0 ? ("Tạm khóa (" + std::to_string(giayKhoa) + "s)") : "Đăng nhập",
              KieuNut::CHINH, giayKhoa == 0, Theme::CO_THUONG + 1)) {
        thuDangNhap();
    }

    Theme::ChuGiua("Sai 3 lần tạm khóa 30s • Sai 5 lần khóa tài khoản",
                   {the.x, the.y + the.height - 42, the.width, 24}, Theme::CO_NHO - 1, Theme::CHU_PHU);
}
