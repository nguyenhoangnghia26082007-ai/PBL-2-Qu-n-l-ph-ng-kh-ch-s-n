// KhachSanData.cpp: ham tim kiem + du lieu mau (chep nguyen tu UngDung ban console).
#include "KhachSanData.h"

KhachHang* KhachSanData::timKhach(const std::string& ma) const {
    for (auto& k : dsKhach) if (k->getMa() == ma) return k.get();
    return nullptr;
}
NhanVien* KhachSanData::timNhanVien(const std::string& ma) const {
    for (auto& n : dsNhanVien) if (n->getMa() == ma) return n.get();
    return nullptr;
}
DichVu* KhachSanData::timDichVu(const std::string& ma) const {
    for (auto& d : dsDichVu) if (d->getMa() == ma) return d.get();
    return nullptr;
}

void KhachSanData::duLieuMau() {
    Date homNay = Date::homNay();
    dsPhong.themCuoi(PhongFactory::taoPhong("DON", "P101", 400000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("DON", "P102", 400000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("DOI", "P103", 600000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("DOI", "P104", 650000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("SUITE", "P201", 1200000, homNay));
    dsPhong.themCuoi(PhongFactory::taoPhong("VIP", "P301", 2000000, homNay));

    dsKhach.push_back(std::make_unique<KhachHang>("KH01", "Do Thanh Nhan", "012345678901", "0905123456", "nhan@mail.com"));
    dsKhach.push_back(std::make_unique<KhachHang>("KH02", "Le Minh Anh", "098765432109", "0912345678", "anh@mail.com"));

    auto nv1 = std::make_unique<NhanVien>("NV01", "Nguyen Hoang Nghia", ChucVu::LE_TAN, Date(1, 6, 2025));
    nv1->khoiPhuc("048205001234", "0905112233", 0);
    dsNhanVien.push_back(std::move(nv1));
    auto nv2 = std::make_unique<NhanVien>("NV02", "Tran Thu Ha", ChucVu::THU_NGAN, Date(1, 7, 2025));
    nv2->khoiPhuc("048204005678", "0867467964", 0);
    dsNhanVien.push_back(std::move(nv2));
    auto nv3 = std::make_unique<NhanVien>("NV03", "Pham Quang Ly", ChucVu::QUAN_LY, Date(1, 1, 2025));
    nv3->khoiPhuc("048195009012", "0913445566", 0);
    dsNhanVien.push_back(std::move(nv3));

    dsDichVu.push_back(std::make_unique<DichVuAnUong>("DV01", "Buffet sang", 150000));
    dsDichVu.push_back(std::make_unique<DichVuGiatUi>("DV02", "Giat ui", 40000));
    dsDichVu.push_back(std::make_unique<DichVuSpa>("DV03", "Massage", 300000, 1.2));
    dsDichVu.push_back(std::make_unique<DichVuDuaDon>("DV04", "Don san bay", 15000));
}
