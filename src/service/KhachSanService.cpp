// KhachSanService.cpp: doc/ghi du lieu + thong ke + danh sach phong + quan ly nhan vien.
#include "KhachSanService.h"
#include <algorithm>
#include <cctype>
#include <cstdio>
#include "LuuTru.h"

namespace {
std::string catKhoangTrang(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n"), b = s.find_last_not_of(" \t\r\n");
    return (a == std::string::npos) ? "" : s.substr(a, b - a + 1);
}
std::string chuanMa(std::string s) {
    s = catKhoangTrang(s);
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return (char)std::toupper(c); });
    return s;
}
bool tachNgay(const std::string& s, Date& d) {
    std::string t = catKhoangTrang(s);
    if (t.empty()) { d = Date::homNay(); return true; }
    int ng = 0, th = 0, nam = 0;
    if (std::sscanf(t.c_str(), "%d/%d/%d", &ng, &th, &nam) == 3 && Date(ng, th, nam).hopLe()) {
        d = Date(ng, th, nam);
        return true;
    }
    return false;
}
} // namespace

std::string KhachSanService::khoiDong() {
    if (LuuTru::tai(data)) return "Đã đọc dữ liệu từ file.";
    data.duLieuMau();
    luu();
    return "Chưa có file dữ liệu: đã tạo dữ liệu mẫu.";
}

bool KhachSanService::luu() const { return LuuTru::luu(data); }

KetQua KhachSanService::thongKeNhanh(ThongKeNhanh& kq) const {
    KetQua q = auth.yeuCau(Quyen::XEM_TONG_QUAN);
    if (!q) return q;
    kq = ThongKeNhanh{};
    for (Phong* p : data.dsPhong.tatCa()) {
        ++kq.tongPhong;
        switch (p->getTinhTrang()) {
            case TinhTrangPhong::TRONG: ++kq.phongTrong; break;
            case TinhTrangPhong::DANG_SU_DUNG: ++kq.phongCoKhach; break;
            default: ++kq.phongBaoTri; break;
        }
    }
    kq.soKhach = (int)data.dsKhach.size();
    if (auth.coQuyen(Quyen::XEM_DOANH_THU)) {
        kq.coDoanhThu = true;
        kq.doanhThu = data.qlHoaDon.tongDoanhThu();
    }
    return KetQua::Ok();
}

KetQua KhachSanService::danhSachPhong(std::vector<PhongHienThi>& kq) const {
    KetQua q = auth.yeuCau(Quyen::XEM_SO_DO_PHONG);
    if (!q) return q;
    kq.clear();
    for (Phong* p : data.dsPhong.tatCa()) {
        PhongHienThi v;
        v.ma = p->getMa();
        v.maLoai = p->maLoai();
        v.tinhTrang = p->getTinhTrang();
        v.donGia = p->tinhDonGia();
        if (LuotThue* lt = p->getLuotThueHienTai()) v.tenKhach = lt->getKhach()->getHoTen();
        kq.push_back(v);
    }
    return KetQua::Ok();
}

// ---------------- Chi tiet phong & Nhan / Tra phong (Buoc 2) ----------------
KetQua KhachSanService::chiTietPhong(const std::string& maPhong, ChiTietPhongHienThi& kq) const {
    Phong* p = data.dsPhong.timTheoMa(chuanMa(maPhong));
    if (!p) return KetQua::Loi("Không tìm thấy phòng " + maPhong);

    kq = ChiTietPhongHienThi{};
    kq.ma = p->getMa();
    kq.maLoai = p->maLoai();
    kq.tenLoai = p->layTenLoaiPhong();
    kq.giaCoBan = p->getGiaCoBan();
    kq.donGiaHienTai = p->tinhDonGia();
    kq.tinhTrang = p->getTinhTrang();
    kq.ngayDuaVaoSuDung = p->getNgayDua();

    LuotThue* lt = p->getLuotThueHienTai();
    if (lt && !lt->daTra()) {
        kq.coKhach = true;
        kq.maLT = lt->getMa();
        kq.maKH = lt->getKhach()->getMa();
        kq.tenKhach = lt->getKhach()->getHoTen();
        kq.sdtKhach = lt->getKhach()->getSDT();
        kq.cccdKhach = lt->getKhach()->getCCCD();
        kq.ngayNhan = lt->getNgayNhan();

        Date homNay = Date::homNay();
        long long dem = homNay - lt->getNgayNhan();
        if (dem <= 0) dem = 1;
        kq.soDemTamTinh = (int)dem;
        kq.tienPhongTamTinh = dem * lt->getDonGiaChot();
        kq.tienDichVuTamTinh = lt->tienDichVu();
        kq.tongTienTamTinh = kq.tienPhongTamTinh + kq.tienDichVuTamTinh;

        for (const auto& d : lt->getDichVu()) {
            kq.dsDichVu.push_back({
                d.getDichVu()->getTen(),
                d.getSoLuong(),
                d.getDichVu()->getDonGia(),
                d.getThanhTien(),
                d.getThoiDiem()
            });
        }
    }
    return KetQua::Ok();
}

KetQua KhachSanService::nhanPhong(const std::string& maPhong, const std::string& maKH, const std::string& ngayNhanStr) {
    KetQua q = auth.yeuCau(Quyen::NHAN_TRA_PHONG);
    if (!q) return q;

    Phong* p = data.dsPhong.timTheoMa(chuanMa(maPhong));
    if (!p) return KetQua::Loi("Không tìm thấy phòng " + maPhong);
    if (p->getTinhTrang() != TinhTrangPhong::TRONG)
        return KetQua::Loi("Phòng " + maPhong + " hiện không ở trạng thái TRỐNG.");

    KhachHang* kh = data.timKhach(chuanMa(maKH));
    if (!kh) return KetQua::Loi("Không tìm thấy khách hàng mã " + maKH);

    Date ngayNhan;
    if (!tachNgay(ngayNhanStr, ngayNhan)) return KetQua::Loi("Ngày nhận phòng không hợp lệ (dd/mm/yyyy).");

    if (!p->nhanPhong(kh, ngayNhan))
        return KetQua::Loi("Không thể nhận phòng lúc này.");

    luu();
    return KetQua::Ok("Phòng " + p->getMa() + " đã được nhận thành công cho khách " + kh->getHoTen() + ".");
}

KetQua KhachSanService::themDichVuChoPhong(const std::string& maPhong, const std::string& maDV, double soLuong) {
    KetQua q = auth.yeuCau(Quyen::NHAN_TRA_PHONG);
    if (!q) return q;

    Phong* p = data.dsPhong.timTheoMa(chuanMa(maPhong));
    if (!p || !p->getLuotThueHienTai())
        return KetQua::Loi("Phòng không tồn tại hoặc hiện không có khách đang ở.");

    DichVu* dv = data.timDichVu(chuanMa(maDV));
    if (!dv) return KetQua::Loi("Không tìm thấy dịch vụ " + maDV);

    if (soLuong <= 0) return KetQua::Loi("Số lượng dịch vụ phải lớn hơn 0.");

    if (!p->themDichVu(*dv, soLuong))
        return KetQua::Loi("Không thể thêm dịch vụ cho phòng này.");

    luu();
    return KetQua::Ok("Đã thêm dịch vụ " + dv->getTen() + " vào phòng " + p->getMa() + ".");
}

KetQua KhachSanService::traPhong(const std::string& maPhong, const std::string& ngayTraStr, std::string& maHDMoi) {
    KetQua q = auth.yeuCau(Quyen::NHAN_TRA_PHONG);
    if (!q) return q;

    Phong* p = data.dsPhong.timTheoMa(chuanMa(maPhong));
    if (!p || !p->getLuotThueHienTai())
        return KetQua::Loi("Phòng không tồn tại hoặc chưa có khách.");

    Date ngayTra;
    if (!tachNgay(ngayTraStr, ngayTra)) return KetQua::Loi("Ngày trả phòng không hợp lệ (dd/mm/yyyy).");

    LuotThue* lt = p->getLuotThueHienTai();
    long long tienPhong = p->traPhong(ngayTra);

    // Tu dong lap hoa don cho luot thue vua tra de khong sot cong no
    std::string maNV = auth.phienHienTai().maNV;
    NhanVien* nv = data.timNhanVien(maNV);
    if (!nv && !data.dsNhanVien.empty()) nv = data.dsNhanVien.front().get();

    if (nv) {
        HoaDon* hd = data.qlHoaDon.taoHoaDon(lt, nv, ngayTra);
        if (hd) maHDMoi = hd->getMa();
    }

    luu();
    std::string tb = "Đã trả phòng " + p->getMa() + ". Tiền phòng: " + dinhDangTien(tienPhong) +
                     ", Dịch vụ: " + dinhDangTien(lt->tienDichVu()) + ".";
    if (!maHDMoi.empty()) tb += " Đã tạo hóa đơn: " + maHDMoi + " (vào mục Hóa đơn để thu tiền).";
    return KetQua::Ok(tb);
}

KetQua KhachSanService::doiBaoTri(const std::string& maPhong, bool bat) {
    KetQua q = auth.yeuCau(Quyen::CAP_NHAT_TRANG_THAI_PHONG);
    if (!q) return q;

    Phong* p = data.dsPhong.timTheoMa(chuanMa(maPhong));
    if (!p) return KetQua::Loi("Không tìm thấy phòng " + maPhong);

    if (!p->baoTri(bat)) {
        return KetQua::Loi(bat ? "Chỉ phòng TRỐNG mới có thể chuyển sang BẢO TRÌ."
                               : "Phòng hiện không ở trạng thái BẢO TRÌ.");
    }

    luu();
    return KetQua::Ok(bat ? ("Phòng " + p->getMa() + " đã chuyển sang BẢO TRÌ / ĐANG DỌN.")
                          : ("Phòng " + p->getMa() + " đã hoàn tất bảo trì -> TRỐNG (sẵn sàng đón khách)."));
}

KetQua KhachSanService::themPhong(const std::string& loai, const std::string& maPhong, long long giaCoBan, const std::string& ngayDuaStr) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_PHONG);
    if (!q) return q;

    std::string m = chuanMa(maPhong);
    std::string l = chuanMa(loai);
    if (m.empty()) return KetQua::Loi("Mã phòng không được rỗng.");
    if (data.dsPhong.timTheoMa(m)) return KetQua::Loi("Mã phòng " + m + " đã tồn tại.");
    if (giaCoBan <= 0) return KetQua::Loi("Giá phòng cơ bản phải lớn hơn 0.");

    Date ngayDua;
    if (!tachNgay(ngayDuaStr, ngayDua)) return KetQua::Loi("Ngày đưa vào sử dụng không hợp lệ.");

    Phong* p = PhongFactory::taoPhong(l, m, giaCoBan, ngayDua);
    if (!p) return KetQua::Loi("Loại phòng không hợp lệ (DON / DOI / SUITE / VIP).");

    if (!data.dsPhong.themCuoi(p)) {
        delete p;
        return KetQua::Loi("Không thể thêm phòng vào danh sách.");
    }

    luu();
    return KetQua::Ok("Đã thêm thành công phòng " + m + " (" + l + ").");
}

KetQua KhachSanService::xoaPhong(const std::string& maPhong) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_PHONG);
    if (!q) return q;

    std::string m = chuanMa(maPhong);
    Phong* p = data.dsPhong.timTheoMa(m);
    if (!p) return KetQua::Loi("Không tìm thấy phòng " + m);

    // Kiem tra da co hoa don chua
    for (HoaDon* h : data.qlHoaDon.tatCa()) {
        if (h->getLuotThue()->getMaPhong() == m)
            return KetQua::Loi("Không thể xóa phòng " + m + " vì đã có lịch sử hóa đơn thanh toán.");
    }

    if (!data.dsPhong.xoaTheoMa(m))
        return KetQua::Loi("Không thể xóa phòng (phòng đang có khách hoặc không tồn tại).");

    luu();
    return KetQua::Ok("Đã xóa phòng " + m + ".");
}

KetQua KhachSanService::danhSachDichVu(std::vector<DichVuMenuHienThi>& kq) const {
    kq.clear();
    for (const auto& d : data.dsDichVu) {
        kq.push_back({d->getMa(), d->getTen(), d->loaiDV(), d->getDonGia()});
    }
    return KetQua::Ok();
}

KetQua KhachSanService::danhSachKhachHangNhanh(std::vector<KhachHangNhanh>& kq) const {
    kq.clear();
    for (const auto& k : data.dsKhach) {
        kq.push_back({k->getMa(), k->getHoTen(), k->getSDT(), k->getCCCD()});
    }
    return KetQua::Ok();
}

// ---------------- Khach hang & Hang thanh vien (Buoc 3) ----------------
KetQua KhachSanService::danhSachKhachHang(std::vector<KhachHangHienThi>& kq) const {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_KHACH);
    if (!q) return q;

    kq.clear();
    for (const auto& k : data.dsKhach) {
        KhachHangHienThi item;
        item.ma = k->getMa();
        item.hoTen = k->getHoTen();
        item.cccd = k->getCCCD();
        item.sdt = k->getSDT();
        item.email = k->getEmail();
        item.diaChi = k->getDiaChi();
        item.diem = k->getDiem();
        item.hang = k->getHang();
        item.tenHang = tenHang(k->getHang());
        item.phanTramGiam = tyLeGiamGia(k->getHang());
        kq.push_back(item);
    }
    return KetQua::Ok();
}

KetQua KhachSanService::themKhachHang(const std::string& ma, const std::string& hoTen, const std::string& cccd,
                                      const std::string& sdt, const std::string& email, const std::string& diaChi) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_KHACH);
    if (!q) return q;

    std::string m = chuanMa(ma);
    std::string ht = catKhoangTrang(hoTen);
    if (m.empty()) return KetQua::Loi("Mã khách hàng không được rỗng.");
    if (data.timKhach(m)) return KetQua::Loi("Mã khách hàng " + m + " đã tồn tại.");
    if (ht.empty()) return KetQua::Loi("Họ tên khách hàng không được rỗng.");

    auto kh = std::make_unique<KhachHang>(m, ht, catKhoangTrang(cccd), catKhoangTrang(sdt), catKhoangTrang(email));
    kh->datDiaChi(catKhoangTrang(diaChi));
    data.dsKhach.push_back(std::move(kh));
    luu();
    return KetQua::Ok("Đã thêm khách hàng " + m + " - " + ht + ".");
}

KetQua KhachSanService::capNhatKhachHang(const std::string& ma, const std::string& hoTen, const std::string& sdt,
                                         const std::string& email, const std::string& diaChi) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_KHACH);
    if (!q) return q;

    std::string m = chuanMa(ma);
    std::string ht = catKhoangTrang(hoTen);
    KhachHang* kh = data.timKhach(m);
    if (!kh) return KetQua::Loi("Không tìm thấy khách hàng " + m + ".");
    if (ht.empty()) return KetQua::Loi("Họ tên khách hàng không được rỗng.");

    kh->capNhatThongTin(ht, catKhoangTrang(sdt), catKhoangTrang(email));
    kh->datDiaChi(catKhoangTrang(diaChi));
    luu();
    return KetQua::Ok("Đã cập nhật thông tin khách hàng " + m + ".");
}

KetQua KhachSanService::xoaKhachHang(const std::string& ma) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_KHACH);
    if (!q) return q;

    std::string m = chuanMa(ma);
    KhachHang* kh = data.timKhach(m);
    if (!kh) return KetQua::Loi("Không tìm thấy khách hàng " + m + ".");

    // Kiem tra xem khach da co phong dang o hoac lich su hoa don khong
    for (Phong* p : data.dsPhong.tatCa()) {
        for (LuotThue* lt : p->getLichSu()) {
            if (lt->getKhach()->getMa() == m)
                return KetQua::Loi("Không thể xóa: Khách hàng " + m + " đã có lịch sử thuê phòng.");
        }
    }

    data.dsKhach.erase(std::remove_if(data.dsKhach.begin(), data.dsKhach.end(),
                                      [&](const std::unique_ptr<KhachHang>& k) { return k->getMa() == m; }),
                       data.dsKhach.end());
    luu();
    return KetQua::Ok("Đã xóa khách hàng " + m + ".");
}

// ---------------- Hoa don, Thanh toan & Doanh thu (Buoc 4) ----------------
KetQua KhachSanService::danhSachHoaDon(std::vector<HoaDonHienThi>& kq) const {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_HOA_DON);
    if (!q) return q;

    kq.clear();
    for (HoaDon* h : data.qlHoaDon.tatCa()) {
        HoaDonHienThi item;
        item.maHD = h->getMa();
        item.maPhong = h->getLuotThue() ? h->getLuotThue()->getMaPhong() : "";
        item.tenKhach = h->getKhach() ? h->getKhach()->getHoTen() : "";
        item.sdtKhach = h->getKhach() ? h->getKhach()->getSDT() : "";
        item.tenNVLap = h->getNhanVienLap() ? h->getNhanVienLap()->getHoTen() : "";
        item.ngayLap = h->getNgayLap();
        item.tongCong = h->getTongCong();
        item.daThanhToan = h->daThanhToanTien();
        item.conPhaiTra = h->conPhaiTra();
        item.trangThai = h->getTrangThai();
        item.tenTrangThai = tenTrangThaiHD(h->getTrangThai());
        kq.push_back(item);
    }
    std::reverse(kq.begin(), kq.end()); // Hoa don moi nhat len tren
    return KetQua::Ok();
}

KetQua KhachSanService::chiTietHoaDon(const std::string& maHD, ChiTietHoaDonHienThi& kq) const {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_HOA_DON);
    if (!q) return q;

    HoaDon* h = data.qlHoaDon.timTheoMa(chuanMa(maHD));
    if (!h) return KetQua::Loi("Không tìm thấy hóa đơn " + maHD);

    kq = ChiTietHoaDonHienThi{};
    kq.maHD = h->getMa();
    kq.ngayLap = h->getNgayLap();
    kq.trangThai = h->getTrangThai();
    kq.tenTrangThai = tenTrangThaiHD(h->getTrangThai());

    if (h->getLuotThue()) {
        kq.maLT = h->getLuotThue()->getMa();
        kq.maPhong = h->getLuotThue()->getMaPhong();
        kq.ngayNhan = h->getLuotThue()->getNgayNhan();
        kq.ngayTra = h->getLuotThue()->getNgayTra();
        kq.soDem = h->getLuotThue()->soDem();
    }
    if (h->getKhach()) {
        kq.tenKhach = h->getKhach()->getHoTen();
        kq.sdtKhach = h->getKhach()->getSDT();
        kq.cccdKhach = h->getKhach()->getCCCD();
        kq.diaChiKhach = h->getKhach()->getDiaChi();
        kq.tenHangKhach = tenHang(h->getKhach()->getHang());
    }
    if (h->getNhanVienLap()) {
        kq.tenNVLap = h->getNhanVienLap()->getHoTen();
    }

    kq.tienPhong = h->getTienPhong();
    kq.tienDichVu = h->getTienDichVu();
    kq.giamGia = h->getGiamGia();
    kq.thueVAT = h->getThueVAT();
    kq.tongCong = h->getTongCong();
    kq.daThanhToan = h->daThanhToanTien();
    kq.conPhaiTra = h->conPhaiTra();

    for (const auto& d : h->getChiTiet()) {
        kq.dsDong.push_back({d.getMoTa(), d.getSoLuong(), d.getDonGia(), d.getThanhTien()});
    }

    for (const auto* t : h->getThanhToan()) {
        kq.dsThanhToan.push_back({
            t->getMa(),
            t->getSoTien(),
            tenHinhThuc(t->getHinhThuc()),
            t->getThoiDiem(),
            t->getNhanVienThu() ? t->getNhanVienThu()->getHoTen() : "",
            t->getSoThamChieu()
        });
    }
    return KetQua::Ok();
}

KetQua KhachSanService::thanhToanHoaDon(const std::string& maHD, long long soTien, HinhThucThanhToan ht,
                                        const std::string& soThamChieu, const std::string& ngayStr) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_HOA_DON);
    if (!q) return q;

    HoaDon* hd = data.qlHoaDon.timTheoMa(chuanMa(maHD));
    if (!hd) return KetQua::Loi("Không tìm thấy hóa đơn " + maHD);

    if (hd->getTrangThai() == TrangThaiHoaDon::DA_HUY)
        return KetQua::Loi("Hóa đơn này đã bị hủy, không thể thanh toán.");

    if (hd->daTatToan() || hd->conPhaiTra() <= 0)
        return KetQua::Loi("Hóa đơn đã được thanh toán đủ.");

    if (soTien <= 0 || soTien > hd->conPhaiTra())
        return KetQua::Loi("Số tiền thanh toán không hợp lệ (tối đa: " + dinhDangTien(hd->conPhaiTra()) + ").");

    // Tim nhan vien thu tien
    std::string maNV = auth.phienHienTai().maNV;
    NhanVien* nv = data.timNhanVien(maNV);
    if (!nv && !data.dsNhanVien.empty()) nv = data.dsNhanVien.front().get();
    if (!nv) return KetQua::Loi("Không tìm thấy thông tin nhân viên thu tiền.");

    Date ngay;
    if (!tachNgay(ngayStr, ngay)) return KetQua::Loi("Ngày thanh toán không hợp lệ.");

    if (!data.qlHoaDon.thanhToan(hd->getMa(), soTien, ht, nv, ngay, catKhoangTrang(soThamChieu)))
        return KetQua::Loi("Thanh toán thất bại (vui lòng kiểm tra số tham chiếu với giao dịch chuyển khoản/thẻ).");

    // Cong diem tich luy cho khach neu tat toan
    if (hd->daTatToan() && data.daCongDiem.insert(hd->getMa()).second) {
        int diem = (int)(hd->getTongCong() / 100000);
        hd->getKhach()->congDiem(diem);
    }

    luu();
    return KetQua::Ok("Thanh toán thành công " + dinhDangTien(soTien) + " cho hóa đơn " + hd->getMa() + ".");
}

KetQua KhachSanService::huyHoaDon(const std::string& maHD) {
    KetQua q = auth.yeuCau(Quyen::HUY_HOA_DON);
    if (!q) return q;

    HoaDon* hd = data.qlHoaDon.timTheoMa(chuanMa(maHD));
    if (!hd) return KetQua::Loi("Không tìm thấy hóa đơn " + maHD);

    std::string maNV = auth.phienHienTai().maNV;
    NhanVien* nv = data.timNhanVien(maNV);
    if (!nv && !data.dsNhanVien.empty()) nv = data.dsNhanVien.front().get();

    if (!data.qlHoaDon.huyHoaDon(hd->getMa(), nv))
        return KetQua::Loi("Không thể hủy hóa đơn (hóa đơn không tồn tại hoặc đã có giao dịch thanh toán).");

    luu();
    return KetQua::Ok("Đã hủy thành công hóa đơn " + hd->getMa() + ".");
}

KetQua KhachSanService::xuatHoaDonTxt(const std::string& maHD, std::string& noiDungFile) const {
    HoaDon* hd = data.qlHoaDon.timTheoMa(chuanMa(maHD));
    if (!hd) return KetQua::Loi("Không tìm thấy hóa đơn " + maHD);

    noiDungFile = data.trinhIn.inHoaDon(hd);
    std::string tenFile = "hoadon_" + hd->getMa() + ".txt";
    data.trinhIn.xuatPDF(hd, tenFile);
    return KetQua::Ok("Đã lưu hóa đơn ra file " + tenFile);
}

KetQua KhachSanService::thongKeBaoCaoDoanhThu(ThongKeDoanhThuChiTiet& kq) const {
    KetQua q = auth.yeuCau(Quyen::XEM_DOANH_THU);
    if (!q) return q;

    kq = ThongKeDoanhThuChiTiet{};
    kq.tongDoanhThu = data.qlHoaDon.tongDoanhThu();
    kq.thucThuHomNay = data.qlHoaDon.thucThuNgay(Date::homNay());

    for (HoaDon* h : data.qlHoaDon.tatCa()) {
        ++kq.tongSoHoaDon;
        if (h->getTrangThai() == TrangThaiHoaDon::DA_HUY) ++kq.hoaDonDaHuy;
        else if (h->daTatToan()) ++kq.hoaDonDaTatToan;
        else ++kq.hoaDonChuaThanhToan;
    }

    auto mapDT = data.qlHoaDon.doanhThuTheoLoai();
    long long tongTheoLoai = 0;
    for (const auto& [k, v] : mapDT) tongTheoLoai += v;

    const std::vector<std::pair<std::string, std::string>> dsLoai = {
        {"DON", "Phòng đơn"}, {"DOI", "Phòng đôi"}, {"SUITE", "Phòng Suite"}, {"VIP", "Phòng VIP"}
    };

    for (const auto& [maL, tenL] : dsLoai) {
        long long dt = 0;
        if (mapDT.count(maL)) dt += mapDT[maL];
        if (mapDT.count(tenL)) dt += mapDT[tenL];
        float pt = (tongTheoLoai > 0) ? (float)dt / (float)tongTheoLoai * 100.0f : 0.0f;
        kq.doanhThuTheoLoai.push_back({maL, tenL, dt, pt});
    }

    return KetQua::Ok();
}

// ---------------- Nhan vien (chi QUAN_LY) ----------------
KetQua KhachSanService::danhSachNhanVien(std::vector<NhanVienHienThi>& kq) const {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_NHAN_VIEN);
    if (!q) return q;
    kq.clear();
    for (auto& n : data.dsNhanVien) {
        kq.push_back({n->getMa(), n->getHoTen(), n->getCCCD(), n->getSDT(),
                      n->getChucVu(), n->getNgayVaoLam(), n->getSoHoaDonDaLap()});
    }
    return KetQua::Ok();
}

KetQua KhachSanService::themNhanVien(const std::string& ma, const std::string& hoTen, const std::string& cccd,
                                     const std::string& sdt, ChucVu chucVu, const std::string& ngayStr) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_NHAN_VIEN);
    if (!q) return q;
    std::string m = chuanMa(ma), ht = catKhoangTrang(hoTen);
    if (m.empty()) return KetQua::Loi("Mã nhân viên không được rỗng.");
    if (data.timNhanVien(m)) return KetQua::Loi("Mã nhân viên đã tồn tại.");
    if (ht.empty()) return KetQua::Loi("Họ tên nhân viên không được rỗng.");
    Date ngay;
    if (!tachNgay(ngayStr, ngay)) return KetQua::Loi("Ngày vào làm không hợp lệ (dd/mm/yyyy).");

    auto nv = std::make_unique<NhanVien>(m, ht, chucVu, ngay);
    nv->khoiPhuc(catKhoangTrang(cccd), catKhoangTrang(sdt), 0);
    data.dsNhanVien.push_back(std::move(nv));
    luu();
    return KetQua::Ok("Đã thêm nhân viên " + m + " - " + ht + ".");
}

KetQua KhachSanService::capNhatNhanVien(const std::string& ma, const std::string& hoTen, const std::string& cccd,
                                        const std::string& sdt, ChucVu chucVu, const std::string& ngayStr) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_NHAN_VIEN);
    if (!q) return q;
    std::string m = chuanMa(ma), ht = catKhoangTrang(hoTen);
    NhanVien* nv = data.timNhanVien(m);
    if (!nv) return KetQua::Loi("Không tìm thấy nhân viên " + m + ".");
    if (ht.empty()) return KetQua::Loi("Họ tên nhân viên không được rỗng.");
    Date ngay;
    if (!tachNgay(ngayStr, ngay)) return KetQua::Loi("Ngày vào làm không hợp lệ (dd/mm/yyyy).");

    nv->capNhatThongTin(ht, catKhoangTrang(cccd), catKhoangTrang(sdt), chucVu, ngay);
    auth.dongBoHoTenTheoMaNV(m, ht); // cap nhat luon ten hien thi tren tai khoan lien ket
    luu();
    return KetQua::Ok("Đã cập nhật thông tin nhân viên " + m + ".");
}

KetQua KhachSanService::xoaNhanVien(const std::string& ma) {
    KetQua q = auth.yeuCau(Quyen::QUAN_LY_NHAN_VIEN);
    if (!q) return q;
    std::string m = chuanMa(ma);
    NhanVien* nv = data.timNhanVien(m);
    if (!nv) return KetQua::Loi("Không tìm thấy nhân viên " + m + ".");
    for (HoaDon* h : data.qlHoaDon.tatCa()) {
        if (h->getNhanVienLap() == nv) return KetQua::Loi("Không xóa được: nhân viên đã lập hóa đơn.");
        for (auto* tt : h->getThanhToan())
            if (tt->getNhanVienThu() == nv) return KetQua::Loi("Không xóa được: nhân viên đã thu tiền hóa đơn.");
    }
    data.dsNhanVien.erase(std::remove_if(data.dsNhanVien.begin(), data.dsNhanVien.end(),
                                         [&](const std::unique_ptr<NhanVien>& p) { return p->getMa() == m; }),
                          data.dsNhanVien.end());
    luu();
    return KetQua::Ok("Đã xóa nhân viên " + m + ".");
}
