#pragma once
// Widgets: cac thanh phan giao dien tu ve (o nhap, nut bo goc, the, thong bao, hop xac nhan).
// Ve theo kieu "immediate mode": goi moi khung hinh, ham tra ve true khi nguoi dung tac dong.
#include <string>
#include "raylib.h"

namespace Widgets {
    // Goi dau / cuoi moi khung hinh (quan ly con tro chuot, khoa nhap khi co hop thoai)
    void batDauKhung(bool coHopThoai);
    void ketThucKhung();
    // Dung khi ve ben trong hop thoai: cho phep nhan chuot du dang co hop thoai
    void batDauHopThoai();
    void ketThucHopThoai();
    bool nhanChuot();                 // false neu dang bi hop thoai che
    bool diChuotVao(Rectangle r);
    void datConTro(int conTro);       // vd MOUSE_CURSOR_POINTING_HAND (ap dung o cuoi khung)

    float boGoc(Rectangle r, float px); // doi ban kinh bo goc (pixel) sang ty le cua raylib
    void veBong(Rectangle r, float px, int lop = 6);
    void veThe(Rectangle r, float px = 14);   // the trang bo goc co bong
    void veAnhPhuKin(Texture2D tex, Rectangle dich); // anh phu kin vung, cat phan thua (kieu "cover")
}

// ---- O nhap chu (ho tro tieng Viet UTF-8, an ky tu cho mat khau) ----
struct OTextNhap {
    std::string giaTri;
    std::string goiY;          // chu mo khi o rong
    bool anKyTu = false;       // true: hien "•" thay cho ky tu
    bool dangChon = false;     // dang nhan ban phim
    size_t toiDa = 64;         // so ky tu toi da
};
// Tra ve true khi nhan Enter trong luc o dang duoc chon
bool VeOTextNhap(Rectangle r, OTextNhap& o);

// ---- Nut bam bo goc ----
enum class KieuNut { CHINH, PHU, NGUY_HIEM, MO };
bool VeNut(Rectangle r, const std::string& nhan, KieuNut kieu = KieuNut::CHINH, bool batDuoc = true, int co = 20);

// ---- Thong bao goc phai (tu tat sau vai giay) ----
namespace ThongBao {
    enum class Loai { THANH_CONG, LOI, THONG_TIN };
    void hien(const std::string& noiDung, Loai loai = Loai::THONG_TIN);
    void ve(float lechTren);
}

// ---- Hop xac nhan (dung khi xoa, dang xuat...) ----
struct HopXacNhan {
    bool mo = false;
    std::string tieuDe, noiDung, nhanDongY = "Đồng ý";
    bool nguyHiem = false;

    void moHop(const std::string& td, const std::string& nd, const std::string& nhan = "Đồng ý", bool nguy = false);
    // -1: van dang mo / khong mo, 0: Huy, 1: Dong y
    int ve();
};
