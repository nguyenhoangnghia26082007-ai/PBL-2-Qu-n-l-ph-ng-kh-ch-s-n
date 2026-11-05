// AuthService.cpp: sai 3 lan tam khoa 30 giay, sai 5 lan khoa vinh vien (luu file) de Quan ly mo lai.
#include "AuthService.h"
#include <algorithm>
#include <cctype>

namespace {
const int LAN_TAM_KHOA  = 3;
const int LAN_KHOA_LUON = 5;
const int GIAY_TAM_KHOA = 30;

std::string catKhoangTrang(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}

std::string chuanHoaTen(std::string s) {
    s = catKhoangTrang(s);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::tolower(c); });
    return s;
}
} // namespace

TaiKhoan* AuthService::tim(const std::string& tenDangNhap) {
    std::string t = chuanHoaTen(tenDangNhap);
    for (auto& tk : dsTaiKhoan) if (chuanHoaTen(tk.tenDangNhap) == t) return &tk;
    return nullptr;
}

const TaiKhoan* AuthService::tim(const std::string& tenDangNhap) const {
    std::string t = chuanHoaTen(tenDangNhap);
    for (auto& tk : dsTaiKhoan) if (chuanHoaTen(tk.tenDangNhap) == t) return &tk;
    return nullptr;
}

int AuthService::demQuanLyHoatDong(const std::string& boQuaTen) const {
    std::string boQua = chuanHoaTen(boQuaTen);
    int dem = 0;
    for (auto& t : dsTaiKhoan)
        if (chuanHoaTen(t.tenDangNhap) != boQua && t.vaiTro == VaiTro::QUAN_LY && !t.biKhoa) ++dem;
    return dem;
}

std::string AuthService::khoiDong() {
    if (KhoTaiKhoan::tai(dsTaiKhoan) && !dsTaiKhoan.empty()) return "";
    dsTaiKhoan = KhoTaiKhoan::macDinh();
    KhoTaiKhoan::luu(dsTaiKhoan);
    return "Đã tạo tài khoản mặc định (admin / letan / thungan / buongphong).";
}

bool AuthService::luu() const { return KhoTaiKhoan::luu(dsTaiKhoan); }

int AuthService::giayTamKhoaConLai(const std::string& tenDangNhap) const {
    std::string t = chuanHoaTen(tenDangNhap);
    auto it = tamKhoaDen.find(t);
    if (it == tamKhoaDen.end()) return 0;
    auto bayGio = std::chrono::steady_clock::now();
    if (bayGio >= it->second) return 0;
    return (int)std::chrono::duration_cast<std::chrono::seconds>(it->second - bayGio).count() + 1;
}

KetQua AuthService::dangNhap(const std::string& tenDangNhap, const std::string& matKhau) {
    std::string ten = chuanHoaTen(tenDangNhap);
    if (ten.empty() || matKhau.empty()) return KetQua::Loi("Vui lòng nhập tên đăng nhập và mật khẩu.");

    // Kiem tra tam khoa 30s
    if (int con = giayTamKhoaConLai(ten); con > 0)
        return KetQua::Loi("Tài khoản tạm khóa. Vui lòng thử lại sau " + std::to_string(con) + " giây.");
    if (int conChung = giayTamKhoaConLai("__chung__"); conChung > 0)
        return KetQua::Loi("Nhập sai quá nhiều lần. Thử lại sau " + std::to_string(conChung) + " giây.");

    TaiKhoan* tk = tim(ten);
    if (!tk) {
        if (++saiKhongTonTai >= LAN_TAM_KHOA) {
            saiKhongTonTai = 0;
            tamKhoaDen["__chung__"] = std::chrono::steady_clock::now() + std::chrono::seconds(GIAY_TAM_KHOA);
            return KetQua::Loi("Nhập sai 3 lần. Tạm khóa " + std::to_string(GIAY_TAM_KHOA) + " giây.");
        }
        return KetQua::Loi("Tài khoản không tồn tại.");
    }

    if (tk->biKhoa)
        return KetQua::Loi("Tài khoản đã bị khóa vĩnh viễn. Vui lòng liên hệ Quản lý để mở lại.");

    if (!tk->kiemTraMatKhau(matKhau)) {
        ++tk->soLanSai;
        // Rieng tai khoan Quan ly duy nhat: chi tam khoa 30s, khong khoa vinh vien de tranh mat quyen vao he thong
        bool laQuanLyDuyNhat = (tk->vaiTro == VaiTro::QUAN_LY && demQuanLyHoatDong(tk->tenDangNhap) == 0);

        if (tk->soLanSai >= LAN_KHOA_LUON && !laQuanLyDuyNhat) {
            tk->biKhoa = true;
            tamKhoaDen.erase(ten);
            luu();
            return KetQua::Loi("Nhập sai 5 lần! Tài khoản đã bị khóa. Liên hệ Quản lý để mở lại.");
        }
        luu();
        if (tk->soLanSai == LAN_TAM_KHOA || (laQuanLyDuyNhat && tk->soLanSai >= LAN_KHOA_LUON)) {
            tamKhoaDen[ten] = std::chrono::steady_clock::now() + std::chrono::seconds(GIAY_TAM_KHOA);
            std::string canhBao = laQuanLyDuyNhat
                ? "Nhập sai nhiều lần! Tạm khóa 30 giây."
                : "Nhập sai 3 lần! Tạm khóa 30 giây (sai đủ 5 lần sẽ khóa vĩnh viễn).";
            return KetQua::Loi(canhBao);
        }
        int conLai = LAN_KHOA_LUON - tk->soLanSai;
        return KetQua::Loi("Sai mật khẩu (lần " + std::to_string(tk->soLanSai) + "/5, còn " +
                           std::to_string(std::max(1, conLai)) + " lần trước khi khóa).");
    }

    // Dang nhap thanh cong -> xoa bo dem sai
    saiKhongTonTai = 0;
    if (tk->soLanSai != 0) { tk->soLanSai = 0; luu(); }
    tamKhoaDen.erase(ten);

    phien.daDangNhap = true;
    phien.tenDangNhap = tk->tenDangNhap;
    phien.hoTen = tk->hoTen.empty() ? tk->tenDangNhap : tk->hoTen;
    phien.maNV = tk->maNV;
    phien.vaiTro = tk->vaiTro;
    return KetQua::Ok("Xin chào, " + phien.hoTen + "!");
}

void AuthService::dangXuat() { phien = Phien{}; }

bool AuthService::coQuyen(Quyen q) const {
    return phien.daDangNhap && PhanQuyen::coQuyen(phien.vaiTro, q);
}

KetQua AuthService::yeuCau(Quyen q) const {
    if (!phien.daDangNhap) return KetQua::Loi("Bạn chưa đăng nhập.");
    if (!PhanQuyen::coQuyen(phien.vaiTro, q))
        return KetQua::Loi("Vai trò " + PhanQuyen::tenVaiTro(phien.vaiTro) + " không có quyền thực hiện thao tác này.");
    return KetQua::Ok();
}

// ---------------- Quan ly tai khoan ----------------
KetQua AuthService::danhSachTaiKhoan(std::vector<TaiKhoan>& kq) const {
    KetQua q = yeuCau(Quyen::QUAN_LY_TAI_KHOAN);
    if (!q) return q;
    kq = dsTaiKhoan;
    return KetQua::Ok();
}

KetQua AuthService::themTaiKhoan(const std::string& tenDangNhap, const std::string& matKhau,
                                 const std::string& hoTen, const std::string& maNV, VaiTro vaiTro) {
    KetQua q = yeuCau(Quyen::QUAN_LY_TAI_KHOAN);
    if (!q) return q;
    std::string ten = chuanHoaTen(tenDangNhap);
    std::string ht = catKhoangTrang(hoTen);
    if (ten.empty()) return KetQua::Loi("Tên đăng nhập không được rỗng.");
    if (matKhau.empty()) return KetQua::Loi("Mật khẩu không được rỗng.");
    if (ht.empty()) return KetQua::Loi("Họ tên hiển thị không được rỗng.");
    if (tim(ten)) return KetQua::Loi("Tên đăng nhập đã tồn tại.");

    TaiKhoan tk;
    tk.tenDangNhap = ten;
    tk.matKhau = matKhau;
    tk.hoTen = ht;
    tk.maNV = catKhoangTrang(maNV);
    tk.vaiTro = vaiTro;
    dsTaiKhoan.push_back(tk);
    luu();
    return KetQua::Ok("Đã thêm tài khoản " + ten + ".");
}

KetQua AuthService::capNhatTaiKhoan(const std::string& tenDangNhap, const std::string& matKhauMoi,
                                    const std::string& hoTen, const std::string& maNV, VaiTro vaiTro) {
    KetQua q = yeuCau(Quyen::QUAN_LY_TAI_KHOAN);
    if (!q) return q;
    TaiKhoan* tk = tim(tenDangNhap);
    if (!tk) return KetQua::Loi("Không tìm thấy tài khoản.");
    std::string ht = catKhoangTrang(hoTen);
    if (ht.empty()) return KetQua::Loi("Họ tên hiển thị không được rỗng.");
    if (tk->vaiTro == VaiTro::QUAN_LY && vaiTro != VaiTro::QUAN_LY && demQuanLyHoatDong(tk->tenDangNhap) == 0)
        return KetQua::Loi("Phải giữ ít nhất 1 tài khoản Quản lý đang hoạt động.");

    tk->hoTen = ht;
    tk->maNV = catKhoangTrang(maNV);
    tk->vaiTro = vaiTro;
    if (!matKhauMoi.empty()) tk->matKhau = matKhauMoi;
    if (chuanHoaTen(phien.tenDangNhap) == chuanHoaTen(tk->tenDangNhap)) {
        phien.hoTen = tk->hoTen;
        phien.maNV = tk->maNV;
        phien.vaiTro = tk->vaiTro;
    }
    luu();
    return KetQua::Ok("Đã cập nhật tài khoản " + tk->tenDangNhap + ".");
}

KetQua AuthService::datKhoaTaiKhoan(const std::string& tenDangNhap, bool khoa) {
    KetQua q = yeuCau(Quyen::QUAN_LY_TAI_KHOAN);
    if (!q) return q;
    TaiKhoan* tk = tim(tenDangNhap);
    if (!tk) return KetQua::Loi("Không tìm thấy tài khoản.");
    if (khoa && chuanHoaTen(tk->tenDangNhap) == chuanHoaTen(phien.tenDangNhap))
        return KetQua::Loi("Không thể tự khóa tài khoản đang đăng nhập.");
    if (khoa && tk->vaiTro == VaiTro::QUAN_LY && demQuanLyHoatDong(tk->tenDangNhap) == 0)
        return KetQua::Loi("Không thể khóa tài khoản Quản lý duy nhất.");

    tk->biKhoa = khoa;
    tk->soLanSai = 0; // mo khoa hoac khoa deu reset bo dem sai
    tamKhoaDen.erase(chuanHoaTen(tk->tenDangNhap));
    luu();
    return KetQua::Ok(khoa ? "Đã khóa tài khoản " + tk->tenDangNhap + "."
                           : "Đã mở khóa tài khoản " + tk->tenDangNhap + " (đặt lại số lần sai = 0).");
}

KetQua AuthService::xoaTaiKhoan(const std::string& tenDangNhap) {
    KetQua q = yeuCau(Quyen::QUAN_LY_TAI_KHOAN);
    if (!q) return q;
    std::string ten = chuanHoaTen(tenDangNhap);
    if (ten == chuanHoaTen(phien.tenDangNhap))
        return KetQua::Loi("Không thể xóa tài khoản đang đăng nhập.");
    const TaiKhoan* tk = tim(ten);
    if (!tk) return KetQua::Loi("Không tìm thấy tài khoản.");
    if (tk->vaiTro == VaiTro::QUAN_LY && demQuanLyHoatDong(ten) == 0)
        return KetQua::Loi("Không thể xóa tài khoản Quản lý duy nhất.");

    dsTaiKhoan.erase(std::remove_if(dsTaiKhoan.begin(), dsTaiKhoan.end(),
                                    [&](const TaiKhoan& t) { return chuanHoaTen(t.tenDangNhap) == ten; }),
                     dsTaiKhoan.end());
    luu();
    return KetQua::Ok("Đã xóa tài khoản " + ten + ".");
}

void AuthService::dongBoHoTenTheoMaNV(const std::string& maNV, const std::string& hoTenMoi) {
    if (maNV.empty() || hoTenMoi.empty()) return;
    bool doi = false;
    for (auto& t : dsTaiKhoan) {
        if (t.maNV == maNV) {
            t.hoTen = hoTenMoi;
            if (chuanHoaTen(phien.tenDangNhap) == chuanHoaTen(t.tenDangNhap)) phien.hoTen = hoTenMoi;
            doi = true;
        }
    }
    if (doi) luu();
}
