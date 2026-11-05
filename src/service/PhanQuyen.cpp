// PhanQuyen.cpp: Quan ly toan quyen > Le tan > Buong phong.
#include "PhanQuyen.h"

bool PhanQuyen::coQuyen(VaiTro vt, Quyen q) {
    switch (vt) {
        case VaiTro::QUAN_LY:
            return true; // toan quyen
        case VaiTro::LE_TAN:
            switch (q) {
                case Quyen::XEM_TONG_QUAN:
                case Quyen::XEM_SO_DO_PHONG:
                case Quyen::CAP_NHAT_TRANG_THAI_PHONG:
                case Quyen::NHAN_TRA_PHONG:
                case Quyen::QUAN_LY_KHACH:
                case Quyen::QUAN_LY_HOA_DON:
                    return true;
                default:
                    return false;
            }
        case VaiTro::BUONG_PHONG:
            return q == Quyen::NHAN_TRA_PHONG || q == Quyen::CAP_NHAT_TRANG_THAI_PHONG || q == Quyen::XEM_SO_DO_PHONG;
    }
    return false;
}

std::string PhanQuyen::tenVaiTro(VaiTro vt) {
    switch (vt) {
        case VaiTro::QUAN_LY: return "Quản lý";
        case VaiTro::LE_TAN: return "Lễ tân";
        default: return "Buồng phòng";
    }
}
