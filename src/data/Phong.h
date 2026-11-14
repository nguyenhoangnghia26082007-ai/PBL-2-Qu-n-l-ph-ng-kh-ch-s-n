#pragma once
#include <string>
#include <vector>
#include <map>
#include <functional>
#include "LuotThue.h"

enum class TinhTrangPhong { TRONG, DANG_SU_DUNG, BAO_TRI };

std::string tenTinhTrang(TinhTrangPhong t);

class Phong {
protected:
    std::string maPhong;
    int sucChua;
    long long giaCoBan;
    TinhTrangPhong tinhTrang = TinhTrangPhong::TRONG;
    Date ngayDuaVaoSuDung;
    std::vector<LuotThue*> lichSu; // composition
public:
    Phong(std::string ma, int sc, long long gia, Date ngay = Date());
    Phong(const Phong&) = delete;
    Phong& operator=(const Phong&) = delete;
    virtual ~Phong();

    virtual long long tinhDonGia() const = 0;
    virtual std::string layTenLoaiPhong() const = 0;
    virtual std::string maLoai() const = 0; // DON / DOI / SUITE / VIP (khop khoa PhongFactory, dung khi luu file)

    bool nhanPhong(KhachHang* kh, Date ngay);
    long long traPhong(Date ngay);
    bool baoTri(bool bat);
    bool themDichVu(const DichVu& dv, double soLuong);

    int getSoLuotThue() const { return (int)lichSu.size(); }
    LuotThue* getLuotThueHienTai() const;
    LuotThue* getLuotThueCuoi() const { return lichSu.empty() ? nullptr : lichSu.back(); }
    const std::vector<LuotThue*>& getLichSu() const { return lichSu; }
    const Date& getNgayDua() const { return ngayDuaVaoSuDung; }
    void datNgayDua(const Date& d) { ngayDuaVaoSuDung = d; }
    // Khoi phuc tu file: phong nhan quyen so huu luot thue
    void nhapLuotThue(LuotThue* l) { lichSu.push_back(l); }
    void khoiPhuc(TinhTrangPhong t, Date ngayDua);
    const std::string& getMa() const { return maPhong; }
    long long getGiaCoBan() const { return giaCoBan; }
    TinhTrangPhong getTinhTrang() const { return tinhTrang; }
    void hienThi() const;
};

class PhongDon : public Phong {
    bool coBanCong;
public:
    PhongDon(std::string ma, long long gia, bool bc = false);
    long long tinhDonGia() const override;
    std::string layTenLoaiPhong() const override;
    std::string maLoai() const override { return "DON"; }
};

class PhongDoi : public Phong {
    std::string kieuGiuong;
public:
    PhongDoi(std::string ma, long long gia, std::string kg = "Queen");
    long long tinhDonGia() const override;
    std::string layTenLoaiPhong() const override;
    std::string maLoai() const override { return "DOI"; }
};

class PhongSuite : public Phong {
    int soPhongNho;
    double heSoSuite;
public:
    PhongSuite(std::string ma, long long gia, int spn = 2, double he = 1.5);
    long long tinhDonGia() const override;
    std::string layTenLoaiPhong() const override;
    std::string maLoai() const override { return "SUITE"; }
};

class PhongVIP : public Phong {
    long long phuPhiVIP;
public:
    PhongVIP(std::string ma, long long gia, long long pp = 500000);
    long long tinhDonGia() const override;
    std::string layTenLoaiPhong() const override;
    std::string maLoai() const override { return "VIP"; }
};

class PhongFactory {
public:
    using Creator = std::function<Phong*(const std::string&, long long)>;
    static void dangKy(const std::string& loai, Creator c);
    static Phong* taoPhong(const std::string& loai, const std::string& ma, long long gia, Date ngayDua = Date());
    // Gia co ban co dinh theo loai phong (0 neu loai khong hop le)
    static long long giaCoBanMacDinh(const std::string& loai);
private:
    static std::map<std::string, Creator>& registry();
    static void macDinh();
};
