#include "LuotThue.h"

LuotThue::LuotThue(std::string ma, Phong* p, std::string maP, KhachHang* kh, Date nhan, long long gia)
    : maLT(std::move(ma)), phong(p), maPhong(std::move(maP)), ngayNhan(nhan),
      donGiaChot(gia), khach(kh) {}

bool LuotThue::themDichVu(const DichVu& dv, double soLuong, Date ngay) {
    if (daTra_ || soLuong <= 0) return false;
    if (ngay.laMacDinh()) ngay = ngayNhan;
    dichVu.emplace_back(dv, soLuong, ngay);
    return true;
}

long long LuotThue::dong(Date ngay) {
    if (daTra_) return tienPhong();
    ngayTra = (ngay - ngayNhan < 0) ? ngayNhan : ngay;
    daTra_ = true;
    return tienPhong();
}

void LuotThue::khoiPhucTra(Date ngay, bool tra) {
    ngayTra = ngay;
    daTra_ = tra;
}

void LuotThue::themDichVuDaCo(const DichVu& dv, double soLuong, Date ngay, long long thanhTien) {
    dichVu.emplace_back(dv, soLuong, ngay, thanhTien);
}

int LuotThue::soDem() const {
    if (!daTra_) return 0;
    long long n = ngayTra - ngayNhan;
    return n < 1 ? 1 : (int)n;
}

long long LuotThue::tienPhong() const { return (long long)soDem() * donGiaChot; }

long long LuotThue::tienDichVu() const {
    long long s = 0;
    for (auto& x : dichVu) s += x.getThanhTien();
    return s;
}
