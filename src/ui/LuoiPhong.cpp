// LuoiPhong.cpp: moi phong la 1 o bo goc, vien trai to mau theo trang thai.
#include "LuoiPhong.h"
#include <algorithm>
#include "Common.h"
#include "Theme.h"
#include "Widgets.h"

std::string LuoiPhong::tenLoai(const std::string& maLoai) {
    if (maLoai == "DON") return "Phòng đơn";
    if (maLoai == "DOI") return "Phòng đôi";
    if (maLoai == "SUITE") return "Phòng Suite";
    if (maLoai == "VIP") return "Phòng VIP";
    return maLoai;
}

std::string LuoiPhong::tenTrangThai(TinhTrangPhong t) {
    switch (t) {
        case TinhTrangPhong::TRONG: return "Trống";
        case TinhTrangPhong::DANG_SU_DUNG: return "Có khách";
        default: return "Bảo trì";
    }
}

Color LuoiPhong::mauTrangThai(TinhTrangPhong t) {
    switch (t) {
        case TinhTrangPhong::TRONG: return Theme::THANH_CONG;
        case TinhTrangPhong::DANG_SU_DUNG: return Theme::LOI;
        default: return Theme::CANH_BAO;
    }
}

int LuoiPhong::ve(Rectangle vung, const std::vector<PhongHienThi>& ds, int dangChon, float* caoDaDung) {
    const float khe = 16.0f, caoO = 118.0f;
    int soCot = std::max(1, (int)((vung.width + khe) / (190.0f + khe)));
    float rongO = (vung.width - khe * (soCot - 1)) / (float)soCot;
    if (rongO > 240.0f) rongO = 240.0f;
    float tongRong = soCot * rongO + (soCot - 1) * khe;
    float lechX = (vung.width - tongRong) / 2.0f;
    if (lechX < 0) lechX = 0;

    int bam = -1;

    for (int i = 0; i < (int)ds.size(); ++i) {
        const PhongHienThi& p = ds[i];
        Rectangle o = {vung.x + lechX + (i % soCot) * (rongO + khe), vung.y + (i / soCot) * (caoO + khe), rongO, caoO};
        Color mau = mauTrangThai(p.tinhTrang);
        bool hover = Widgets::diChuotVao(o);

        Widgets::veBong(o, 10, hover ? 6 : 3);
        DrawRectangleRounded(o, Widgets::boGoc(o, 12), 10, hover ? Color{248, 250, 252, 255} : Theme::TRANG);
        DrawRectangleRounded({o.x, o.y, 7, o.height}, 1.0f, 6, mau);          // vach mau ben trai
        if (i == dangChon) DrawRectangleRoundedLinesEx(o, Widgets::boGoc(o, 12), 10, 2.5f, Theme::CHINH);

        float x = o.x + 18;
        Theme::Chu(p.ma, x, o.y + 12, Theme::CO_VUA, Theme::CHU, true);
        // nhan trang thai dang "vien thuoc"
        std::string tt = tenTrangThai(p.tinhTrang);
        float rongNhan = Widgets::rongBadge(tt, Theme::CO_NHO - 1, 8);
        Rectangle nhan = {o.x + o.width - rongNhan - 6, o.y + 14, rongNhan, 24};
        DrawBadge(tt, nhan, mau, Theme::CO_NHO - 1, 8);

        Theme::Chu(tenLoai(p.maLoai), x, o.y + 48, Theme::CO_NHO, Theme::CHU_PHU);
        std::string dong3 = p.tenKhach.empty() ? dinhDangTien(p.donGia) + "/đêm" : p.tenKhach;
        Theme::Chu(Theme::CatChu(dong3, o.width - 30, Theme::CO_NHO), x, o.y + 76, Theme::CO_NHO,
                   p.tenKhach.empty() ? Theme::CHU : Theme::LOI, !p.tenKhach.empty());

        if (hover) Widgets::datConTro(MOUSE_CURSOR_POINTING_HAND);
        if (hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) bam = i;
    }
    if (caoDaDung) {
        int soHang = ((int)ds.size() + soCot - 1) / soCot;
        *caoDaDung = soHang * (caoO + khe);
    }
    return bam;
}

void LuoiPhong::veChuGiai(float x, float y) {
    for (TinhTrangPhong t : {TinhTrangPhong::TRONG, TinhTrangPhong::DANG_SU_DUNG, TinhTrangPhong::BAO_TRI}) {
        DrawCircle((int)x + 7, (int)y + 11, 7, mauTrangThai(t));
        std::string s = tenTrangThai(t);
        Theme::Chu(s, x + 20, y, Theme::CO_NHO + 2, Theme::CHU_PHU);
        x += 20 + Theme::DoChu(s, Theme::CO_NHO + 2).x + 26;
    }
}
