#pragma once

// Luu / doc toan bo du lieu chuong trinh ra cac file van ban (dinh dang '|').
// Cac file nam canh file chay (thu muc lam viec hien tai):
//   dulieu_dichvu.txt, dulieu_khach.txt, dulieu_nhanvien.txt, dulieu_phong.txt,
//   dulieu_luotthue.txt, dulieu_hoadon.txt, dulieu_hethong.txt
// Co the mo bang Notepad de xem hoac sua tay (nho dong chuong trinh truoc khi sua).
class UngDung;

class LuuTru {
public:
    // Doc du lieu vao ung dung. Tra false neu chua co file (lan chay dau tien).
    static bool tai(UngDung& app);
    // Ghi lai toan bo du lieu. Tra false neu khong ghi duoc file.
    static bool luu(const UngDung& app);
};
