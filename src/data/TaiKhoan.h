#pragma once
// TaiKhoan: tai khoan dang nhap cua nhan vien + doc/ghi file dulieu_taikhoan.txt.
// Mat khau luu truc tiep (khong bam); luu kem soLanSai va trang thai biKhoa de Quan ly mo khoa.
#include <string>
#include <vector>

// 3 vai tro trong giao dien (thu tu quyen: QUAN_LY > LE_TAN > BUONG_PHONG)
enum class VaiTro { QUAN_LY, LE_TAN, BUONG_PHONG };

std::string maVaiTro(VaiTro v);                    // "QUAN_LY"... (dung khi luu file)
bool docVaiTro(const std::string& s, VaiTro& v);   // nguoc lai

struct TaiKhoan {
    std::string tenDangNhap;
    std::string hoTen;      // ten hien thi (UTF-8, co dau)
    std::string maNV;       // lien ket NhanVien cua ban console (co the rong, vd buong phong)
    VaiTro vaiTro = VaiTro::LE_TAN;
    std::string matKhau;    // mat khau luu truc tiep (khong bam)
    bool biKhoa = false;    // bi khoa vinh vien (sai 5 lan hoac do Quan ly khoa) -> Quan ly mo lai
    int soLanSai = 0;       // dem so lan nhap sai: 3 lan khoa 30s, 5 lan khoa luon

    void datMatKhau(const std::string& mk) { matKhau = mk; }
    bool kiemTraMatKhau(const std::string& mk) const { return matKhau == mk; }
};

class KhoTaiKhoan {
public:
    // Doc file; tra false neu chua co file
    static bool tai(std::vector<TaiKhoan>& ds);
    static bool luu(const std::vector<TaiKhoan>& ds);
    // Tai khoan mac dinh khi chay lan dau
    static std::vector<TaiKhoan> macDinh();
};
