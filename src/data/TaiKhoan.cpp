// TaiKhoan.cpp: doc/ghi file tai khoan (mat khau luu truc tiep, kem soLanSai va biKhoa).
#include "TaiKhoan.h"
#include <cstdlib>
#include <fstream>

namespace {

const char* F_TAIKHOAN = "dulieu_taikhoan.txt";

std::string sach(std::string s) {
    for (char& c : s) if (c == '|' || c == '\n' || c == '\r') c = ' ';
    return s;
}

std::vector<std::string> tach(const std::string& line) {
    std::vector<std::string> r;
    std::string cur;
    for (char c : line) {
        if (c == '|') { r.push_back(cur); cur.clear(); }
        else cur += c;
    }
    r.push_back(cur);
    return r;
}

} // namespace

std::string maVaiTro(VaiTro v) {
    switch (v) {
        case VaiTro::QUAN_LY: return "QUAN_LY";
        case VaiTro::LE_TAN: return "LE_TAN";
        default: return "BUONG_PHONG";
    }
}

bool docVaiTro(const std::string& s, VaiTro& v) {
    if (s == "QUAN_LY") { v = VaiTro::QUAN_LY; return true; }
    if (s == "LE_TAN") { v = VaiTro::LE_TAN; return true; }
    if (s == "BUONG_PHONG") { v = VaiTro::BUONG_PHONG; return true; }
    return false;
}

bool KhoTaiKhoan::tai(std::vector<TaiKhoan>& ds) {
    std::ifstream in(F_TAIKHOAN);
    if (!in) return false;
    ds.clear();
    std::string line;
    bool dau = true;
    while (std::getline(in, line)) {
        if (dau) { dau = false; if (line.compare(0, 3, "\xEF\xBB\xBF") == 0) line.erase(0, 3); } // bo BOM
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty() || line[0] == '#') continue;
        auto p = tach(line);
        TaiKhoan tk;
        // Dinh dang moi: tenDangNhap|hoTen|maNV|vaiTro|matKhau|biKhoa(0/1)|soLanSai
        if (p.size() < 6 || p[0].empty() || !docVaiTro(p[3], tk.vaiTro)) continue;
        // Bo qua dong cu cua phien ban bam SHA-256 (co cot bam 64 ky tu o p[5])
        if (p.size() >= 7 && p[5].size() == 64) continue;
        tk.tenDangNhap = p[0];
        tk.hoTen = p[1];
        tk.maNV = p[2];
        tk.matKhau = p[4];
        tk.biKhoa = (p[5] == "1");
        tk.soLanSai = (p.size() >= 7) ? std::atoi(p[6].c_str()) : 0;
        ds.push_back(tk);
    }
    return !ds.empty();
}

bool KhoTaiKhoan::luu(const std::vector<TaiKhoan>& ds) {
    std::ofstream out(F_TAIKHOAN, std::ios::trunc);
    if (!out) return false;
    out << "# tenDangNhap|hoTen|maNV|vaiTro(QUAN_LY|LE_TAN|BUONG_PHONG)|matKhau|biKhoa(0/1)|soLanSai\n";
    for (auto& t : ds)
        out << sach(t.tenDangNhap) << '|' << sach(t.hoTen) << '|' << sach(t.maNV) << '|' << maVaiTro(t.vaiTro)
            << '|' << sach(t.matKhau) << '|' << (t.biKhoa ? "1" : "0") << '|' << t.soLanSai << '\n';
    return (bool)out;
}

std::vector<TaiKhoan> KhoTaiKhoan::macDinh() {
    struct Mau { const char* ten; const char* hoTen; const char* maNV; VaiTro vt; const char* mk; };
    const Mau mau[] = {
        {"admin",      "Phạm Quang Lý",      "NV03", VaiTro::QUAN_LY,     "admin123"},
        {"letan",      "Nguyễn Hoàng Nghĩa", "NV01", VaiTro::LE_TAN,      "letan123"},
        {"thungan",    "Trần Thu Hà",        "NV02", VaiTro::LE_TAN,      "thungan123"},
        {"buongphong", "Võ Thị Lan",         "",     VaiTro::BUONG_PHONG, "buongphong123"},
    };
    std::vector<TaiKhoan> ds;
    for (auto& m : mau) {
        TaiKhoan t;
        t.tenDangNhap = m.ten; t.hoTen = m.hoTen; t.maNV = m.maNV; t.vaiTro = m.vt; t.matKhau = m.mk;
        ds.push_back(t);
    }
    return ds;
}
