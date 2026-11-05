#pragma once
#include <map>
#include <string>
#include <vector>
#include "LuotThue.h"
#include "NhanVien.h"
#include "DanhSach.h"

enum class TrangThaiHoaDon { CHUA_THANH_TOAN, THANH_TOAN_MOT_PHAN, DA_THANH_TOAN, DA_HUY };
enum class HinhThucThanhToan { TIEN_MAT, CHUYEN_KHOAN, THE, VI_DIEN_TU, QR };

std::string tenTrangThaiHD(TrangThaiHoaDon t);
std::string tenHinhThuc(HinhThucThanhToan h);

class DongHoaDon {
    std::string moTa;
    double soLuong;
    long long donGia, thanhTien;
public:
    DongHoaDon(std::string mt, double sl, long long dg, long long tt);
    const std::string& getMoTa() const { return moTa; }
    double getSoLuong() const { return soLuong; }
    long long getDonGia() const { return donGia; }
    long long getThanhTien() const { return thanhTien; }
};

class HoaDon;

class ThanhToanPhong {
    std::string maGD;
    HoaDon* hoaDon;
    long long soTien;
    HinhThucThanhToan hinhThuc;
    Date thoiDiem;
    NhanVien* nhanVienThu;
    std::string soThamChieu;
    bool daXuLy_ = false, daHoan_ = false;
public:
    ThanhToanPhong(std::string ma, HoaDon* hd, long long tien, HinhThucThanhToan ht, Date t,
                   NhanVien* nv, std::string tc = "");
    bool xuLy();
    bool hoanTien();
    std::string inBienLai() const;

    const std::string& getMa() const { return maGD; }
    HoaDon* getHoaDon() const { return hoaDon; }
    long long getSoTien() const { return soTien; }
    long long getSoTienThuc() const { return daHoan_ ? 0 : soTien; }
    HinhThucThanhToan getHinhThuc() const { return hinhThuc; }
    const Date& getThoiDiem() const { return thoiDiem; }
    NhanVien* getNhanVienThu() const { return nhanVienThu; }
    bool daHoan() const { return daHoan_; }
    const std::string& getSoThamChieu() const { return soThamChieu; }
    void khoiPhuc(bool daXuLy, bool daHoan) { daXuLy_ = daXuLy; daHoan_ = daHoan; } // khoi phuc tu file
};

class HoaDon {
    std::string maHD;
    Date ngayLap;
    LuotThue* luotThue;
    NhanVien* nhanVienLap;
    long long tienPhong = 0, tienDichVu = 0, giamGia = 0, thueVAT = 0, tongCong = 0;
    bool daHuy_ = false;
    std::vector<DongHoaDon> chiTiet;         // composition
    std::vector<ThanhToanPhong*> thanhToan;  // composition
public:
    HoaDon(std::string ma, LuotThue* lt, NhanVien* nv, Date ngay);
    // Khoi phuc tu file: giu nguyen cac so da chot, khong tinh lai, khong tang bo dem cua nhan vien
    HoaDon(std::string ma, LuotThue* lt, NhanVien* nv, Date ngay, long long phong, long long dv,
           long long giam, long long vat, long long tong, bool huy);
    HoaDon(const HoaDon&) = delete;
    HoaDon& operator=(const HoaDon&) = delete;
    ~HoaDon();

    void taoDongHoaDon();
    void themDongDaCo(DongHoaDon d) { chiTiet.push_back(std::move(d)); }
    void nhapThanhToan(ThanhToanPhong* tt) { thanhToan.push_back(tt); } // hoa don nhan quyen so huu
    void apDungGiamGia(HangThanhVien hang);
    long long tinhTongCong();
    // hoa don giu quyen so huu tt neu thanh cong; neu false, nguoi goi tu delete
    bool themThanhToan(ThanhToanPhong* tt);
    long long daThanhToanTien() const;
    long long conPhaiTra() const;
    bool daTatToan() const;
    bool huy();
    TrangThaiHoaDon getTrangThai() const;
    void hienThi() const;

    const std::string& getMa() const { return maHD; }
    const Date& getNgayLap() const { return ngayLap; }
    LuotThue* getLuotThue() const { return luotThue; }
    KhachHang* getKhach() const { return luotThue->getKhach(); }
    NhanVien* getNhanVienLap() const { return nhanVienLap; }
    long long getTienPhong() const { return tienPhong; }
    long long getTienDichVu() const { return tienDichVu; }
    long long getGiamGia() const { return giamGia; }
    long long getThueVAT() const { return thueVAT; }
    long long getTongCong() const { return tongCong; }
    // DOANH THU cua hoa don = tien phong + dich vu - giam gia (chua VAT); hoa don huy = 0.
    // Day la dinh nghia duy nhat cua "doanh thu" trong he thong.
    long long getDoanhThu() const { return daHuy_ ? 0 : tienPhong + tienDichVu - giamGia; }
    const std::vector<DongHoaDon>& getChiTiet() const { return chiTiet; }
    const std::vector<ThanhToanPhong*>& getThanhToan() const { return thanhToan; }
};

class QuanLyHoaDon {
    DanhSachLienKet<HoaDon*> dsHoaDon; // QuanLyHoaDon quan ly (so huu) HoaDon
    int demHD = 0, demGD = 0;
    static std::string soHieu(const char* tienTo, int n);
public:
    ~QuanLyHoaDon();
    HoaDon* timTheoMa(const std::string& maHD) const;
    HoaDon* timTheoLuotThue(const LuotThue* lt) const; // hoa don con hieu luc cua luot thue (null neu chua co)
    std::vector<HoaDon*> tatCa() const;
    void nhapHoaDon(HoaDon* hd) { dsHoaDon.themCuoi(hd); } // khoi phuc tu file
    int getDemHD() const { return demHD; }
    int getDemGD() const { return demGD; }
    void datBoDem(int hd, int gd) { demHD = hd; demGD = gd; }
    // Phan quyen: thanhToan chi cho THU_NGAN/QUAN_LY; huyHoaDon chi cho QUAN_LY
    // Moi hoa don do dung 1 nhan vien lap; 1 luot thue chi co 1 hoa don con hieu luc
    HoaDon* taoHoaDon(LuotThue* lt, NhanVien* nv, Date ngay);
    bool thanhToan(const std::string& maHD, long long tien, HinhThucThanhToan ht, NhanVien* thuNgan,
                   Date ngay, const std::string& soThamChieu = "");
    bool huyHoaDon(const std::string& maHD, const NhanVien* nguoiHuy);
    std::vector<HoaDon*> timTheoKhach(const std::string& maKH) const;
    std::vector<HoaDon*> timTheoNhanVienLap(const std::string& maNV) const;
    // DOANH THU: tong HoaDon::getDoanhThu() cua cac hoa don chua huy
    long long tongDoanhThu() const;
    std::map<std::string, long long> doanhThuTheoLoai() const; // theo loai phong cua luot thue
    // THUC THU (dong tien): tien da thu trong ngay, gom VAT, tru phan da hoan
    long long thucThuNgay(const Date& ngay) const;
    void hienThi() const;
};

class TrinhInHoaDon {
    std::string tenKhachSan, diaChi, maSoThue, khoGiay;
public:
    TrinhInHoaDon(std::string ten, std::string dc, std::string mst, std::string kg = "A5");
    std::string inHoaDon(const HoaDon* hd) const;
    // Hien ghi ra file van ban; thay bang thu vien PDF (vd. libharu) neu can PDF that
    bool xuatPDF(const HoaDon* hd, const std::string& duongDan) const;
    std::string inBienLai(const ThanhToanPhong* tt) const;
    static std::string dinhDangTien(long long v);
};
