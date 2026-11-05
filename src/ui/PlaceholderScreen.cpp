// PlaceholderScreen.cpp: hien so do phong (neu la tab So do phong) hoac thong bao buoc tiep theo.
#include "PlaceholderScreen.h"
#include "LuoiPhong.h"
#include "Theme.h"
#include "Widgets.h"

void PlaceholderScreen::Enter() {
    dangChon = -1;
    if (hienLuoiPhong) service.danhSachPhong(dsPhong);
}

void PlaceholderScreen::Draw(Rectangle vung) {
    float x = vung.x + 28, y = vung.y + 24, rong = vung.width - 56;
    if (hienLuoiPhong) {
        Theme::Chu("Danh sách phòng (bấm vào phòng để chọn)", x, y, Theme::CO_VUA, Theme::CHU, true);
        LuoiPhong::veChuGiai(x + 470, y + 3);
        int bam = LuoiPhong::ve({x, y + 44, rong, vung.height - 90}, dsPhong, dangChon);
        if (bam >= 0) {
            dangChon = bam;
            ThongBao::hien("Đã chọn phòng " + dsPhong[bam].ma + " (chi tiết sẽ làm ở Bước 2)", ThongBao::Loai::THONG_TIN);
        }
        return;
    }
    Rectangle the = {x, y, rong, 180};
    Widgets::veThe(the, 16);
    Theme::Chu(tieuDe, the.x + 28, the.y + 28, Theme::CO_VUA, Theme::CHU, true);
    Theme::Chu(moTa, the.x + 28, the.y + 72, Theme::CO_THUONG, Theme::CHU_PHU);
    Theme::Chu("Màn hình này sẽ được hoàn thiện ở các bước tiếp theo.", the.x + 28, the.y + 112,
               Theme::CO_NHO + 2, Theme::CHINH);
}
