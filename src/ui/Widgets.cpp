// Widgets.cpp: cai dat cac thanh phan giao dien dung chung.
#include "Widgets.h"
#include <algorithm>
#include <cmath>
#include <vector>
#include "Theme.h"

namespace {

bool g_coHopThoai = false;   // dang co hop thoai mo -> khoa cac nut ben duoi
bool g_trongHopThoai = false; // dang ve ben trong hop thoai
int g_conTro = MOUSE_CURSOR_DEFAULT, g_conTroCu = -1;

size_t demKyTu(const std::string& s) { // dem so ky tu UTF-8 (khong phai so byte)
    size_t n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n;
    return n;
}

void xoaKyTuCuoi(std::string& s) {
    while (!s.empty() && ((unsigned char)s.back() & 0xC0) == 0x80) s.pop_back();
    if (!s.empty()) s.pop_back();
}

} // namespace

// ===================== Widgets chung =====================
void Widgets::batDauKhung(bool coHopThoai) {
    g_coHopThoai = coHopThoai;
    g_trongHopThoai = false;
    g_conTro = MOUSE_CURSOR_DEFAULT;
}

void Widgets::ketThucKhung() {
    if (g_conTro != g_conTroCu) { SetMouseCursor(g_conTro); g_conTroCu = g_conTro; }
}

void Widgets::batDauHopThoai() { g_trongHopThoai = true; g_conTro = MOUSE_CURSOR_DEFAULT; }
void Widgets::ketThucHopThoai() { g_trongHopThoai = false; }
bool Widgets::nhanChuot() { return !g_coHopThoai || g_trongHopThoai; }
void Widgets::datConTro(int conTro) { g_conTro = conTro; }

bool Widgets::diChuotVao(Rectangle r) {
    return nhanChuot() && CheckCollisionPointRec(GetMousePosition(), r);
}

float Widgets::boGoc(Rectangle r, float px) {
    float m = std::min(r.width, r.height);
    if (m <= 0) return 0;
    return std::clamp(2 * px / m, 0.0f, 1.0f);
}

void Widgets::veBong(Rectangle r, float px, int lop) {
    for (int i = lop; i >= 1; --i) {
        Rectangle e = {r.x - i, r.y - i + 4, r.width + 2.0f * i, r.height + 2.0f * i};
        DrawRectangleRounded(e, boGoc(e, px + i), 10, Fade(BLACK, 0.018f));
    }
}

void Widgets::veThe(Rectangle r, float px) {
    veBong(r, px);
    DrawRectangleRounded(r, boGoc(r, px), 10, Theme::TRANG);
}

void Widgets::veAnhPhuKin(Texture2D tex, Rectangle dich) {
    if (tex.id == 0) { DrawRectangleRec(dich, Theme::SIDEBAR); return; }
    float tyLe = std::max(dich.width / tex.width, dich.height / tex.height);
    float rongNguon = dich.width / tyLe, caoNguon = dich.height / tyLe;
    Rectangle nguon = {(tex.width - rongNguon) / 2, (tex.height - caoNguon) / 2, rongNguon, caoNguon};
    DrawTexturePro(tex, nguon, dich, {0, 0}, 0, WHITE);
}

std::string Widgets::dinhDangTienNhap(const std::string& chuoi) {
    std::string raw;
    for (char c : chuoi) {
        if (c >= '0' && c <= '9') {
            raw += c;
        }
    }
    while (raw.size() > 1 && raw[0] == '0') {
        raw.erase(raw.begin());
    }
    if (raw.empty()) return "";

    std::string ketQua;
    int dem = 0;
    for (int i = (int)raw.size() - 1; i >= 0; --i) {
        ketQua.insert(ketQua.begin(), raw[i]);
        if (++dem % 3 == 0 && i > 0) {
            ketQua.insert(ketQua.begin(), ' ');
        }
    }
    return ketQua;
}

long long Widgets::layGiaTriTien(const std::string& chuoi) {
    std::string raw;
    for (char c : chuoi) {
        if (c >= '0' && c <= '9') {
            raw += c;
        }
    }
    if (raw.empty()) return 0LL;
    try {
        return std::stoll(raw);
    } catch (...) {
        return 0LL;
    }
}

// ===================== O nhap chu =====================
bool VeOTextNhap(Rectangle r, OTextNhap& o) {
    bool hover = Widgets::diChuotVao(r);
    if (hover) g_conTro = MOUSE_CURSOR_IBEAM;
    if (Widgets::nhanChuot() && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) o.dangChon = hover;

    if (o.chiNhapTien && !o.giaTri.empty()) {
        std::string dd = Widgets::dinhDangTienNhap(o.giaTri);
        if (dd != o.giaTri) o.giaTri = dd;
    }

    bool enter = false;
    if (o.dangChon && Widgets::nhanChuot()) {
        if (o.chiNhapTien) {
            for (int cp = GetCharPressed(); cp > 0; cp = GetCharPressed()) {
                if (cp >= '0' && cp <= '9') {
                    std::string raw;
                    for (char c : o.giaTri) if (c >= '0' && c <= '9') raw += c;
                    if (raw.size() < o.toiDa) {
                        if (raw == "0") raw.clear();
                        raw += (char)cp;
                        o.giaTri = Widgets::dinhDangTienNhap(raw);
                    }
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) {
                std::string raw;
                for (char c : o.giaTri) if (c >= '0' && c <= '9') raw += c;
                if (!raw.empty()) {
                    raw.pop_back();
                    o.giaTri = Widgets::dinhDangTienNhap(raw);
                }
            }
            bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
            if (ctrl && IsKeyPressed(KEY_V)) {
                if (const char* c = GetClipboardText()) {
                    std::string raw;
                    for (char ch : o.giaTri) if (ch >= '0' && ch <= '9') raw += ch;
                    for (const char* p = c; *p; ++p) {
                        if (*p >= '0' && *p <= '9' && raw.size() < o.toiDa) {
                            if (raw == "0") raw.clear();
                            raw += *p;
                        }
                    }
                    o.giaTri = Widgets::dinhDangTienNhap(raw);
                }
            }
            enter = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
        } else {
            // Nhan ky tu Unicode (go tieng Viet bang Unikey/EVKey van hoat dong)
            for (int cp = GetCharPressed(); cp > 0; cp = GetCharPressed()) {
                if (cp >= 32 && demKyTu(o.giaTri) < o.toiDa) {
                    int n = 0;
                    const char* u = CodepointToUTF8(cp, &n);
                    o.giaTri.append(u, n);
                }
            }
            if (IsKeyPressed(KEY_BACKSPACE) || IsKeyPressedRepeat(KEY_BACKSPACE)) xoaKyTuCuoi(o.giaTri);
            bool ctrl = IsKeyDown(KEY_LEFT_CONTROL) || IsKeyDown(KEY_RIGHT_CONTROL);
            if (ctrl && IsKeyPressed(KEY_V) && !o.anKyTu) { // dan tu clipboard (khong ap dung cho mat khau)
                if (const char* c = GetClipboardText())
                    for (const char* p = c; *p && demKyTu(o.giaTri) < o.toiDa; ++p)
                        if (*p != '\n' && *p != '\r' && *p != '\t') o.giaTri += *p;
            }
            enter = IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_KP_ENTER);
        }
    }

    // ---- Ve ----
    const float goc = 10, dem = 14;
    const int co = Theme::CO_THUONG;
    Color vien = o.dangChon ? Theme::CHINH : (hover ? Color{148, 163, 184, 255} : Theme::VIEN);
    if (o.dangChon) {
        Rectangle vong = {r.x - 3, r.y - 3, r.width + 6, r.height + 6};
        DrawRectangleRounded(vong, Widgets::boGoc(vong, goc + 3), 10, Fade(Theme::CHINH, 0.15f));
    }
    DrawRectangleRounded(r, Widgets::boGoc(r, goc), 10, Theme::TRANG);
    DrawRectangleRoundedLinesEx(r, Widgets::boGoc(r, goc), 10, o.dangChon ? 2.0f : 1.2f, vien);

    std::string hien;
    if (o.anKyTu) for (size_t i = 0, n = demKyTu(o.giaTri); i < n; ++i) hien += "•";
    else hien = o.giaTri;

    float rongChu = Theme::DoChu(hien, co).x;
    float rongVung = r.width - 2 * dem;
    float x = r.x + dem;
    if (rongChu > rongVung) x -= rongChu - rongVung; // chu dai thi day sang trai, luon thay con tro
    float y = r.y + (r.height - co) / 2 - 1;

    BeginScissorMode((int)(r.x + dem - 2), (int)r.y, (int)(rongVung + 4), (int)r.height);
    if (hien.empty()) Theme::Chu(o.goiY, r.x + dem, y, co, Color{148, 163, 184, 255});
    else Theme::Chu(hien, x, y, co, Theme::CHU);
    if (o.dangChon && std::fmod(GetTime(), 1.0) < 0.55)
        DrawRectangle((int)(x + rongChu + 1), (int)(r.y + 12), 2, (int)(r.height - 24), Theme::CHINH);
    EndScissorMode();
    return enter;
}

// ===================== Nut bam =====================
bool VeNut(Rectangle r, const std::string& nhan, KieuNut kieu, bool batDuoc, int co) {
    bool hover = batDuoc && Widgets::diChuotVao(r);
    bool dangNhan = hover && IsMouseButtonDown(MOUSE_BUTTON_LEFT);
    if (hover) g_conTro = MOUSE_CURSOR_POINTING_HAND;

    Color nen = Theme::CHINH, chu = WHITE, vien = BLANK;
    switch (kieu) {
        case KieuNut::CHINH:     nen = hover ? Theme::CHINH_DAM : Theme::CHINH; break;
        case KieuNut::PHU:       nen = hover ? Color{241, 245, 249, 255} : Theme::TRANG; chu = Theme::CHU; vien = Theme::VIEN; break;
        case KieuNut::NGUY_HIEM: nen = hover ? Color{185, 28, 28, 255} : Theme::LOI; break;
        case KieuNut::MO:        nen = hover ? Fade(Theme::CHINH, 0.10f) : BLANK; chu = Theme::CHINH; break;
    }
    if (!batDuoc) { nen = {226, 232, 240, 255}; chu = {148, 163, 184, 255}; vien = BLANK; }

    Rectangle rv = r;
    if (dangNhan) rv.y += 1; // hieu ung lun xuong khi nhan
    DrawRectangleRounded(rv, Widgets::boGoc(rv, 10), 10, nen);
    if (vien.a) DrawRectangleRoundedLinesEx(rv, Widgets::boGoc(rv, 10), 10, 1.2f, vien);
    Theme::ChuGiua(nhan, rv, co, chu, true);
    return hover && IsMouseButtonReleased(MOUSE_BUTTON_LEFT);
}

// ===================== Nhan hien thi & huy hieu =====================
std::string Widgets::nhanHienThi(const std::string& ma) {
    // Trang thai hoa don
    if (ma == "CHUA_THANH_TOAN")     return "Chưa thanh toán";
    if (ma == "THANH_TOAN_MOT_PHAN") return "Trả một phần";
    if (ma == "DA_THANH_TOAN")       return "Đã thanh toán";
    if (ma == "DA_HUY")              return "Đã hủy";
    // Hang thanh vien
    if (ma == "THUONG")    return "Thường";
    if (ma == "BAC")       return "Bạc";
    if (ma == "VANG")      return "Vàng";
    if (ma == "KIM_CUONG") return "Kim cương";
    return ma;
}

float Widgets::rongBadge(const std::string& nhan, int co, float dem) {
    return Theme::DoChu(nhan, co, true).x + 2 * dem;
}

void DrawBadge(const std::string& nhan, Rectangle r, Color mau, int co, float dem) {
    DrawRectangleRounded(r, 1.0f, 8, Fade(mau, 0.12f));
    float rongChu = r.width - 2 * dem;
    // Giam co chu toi da 3 bac neu chu khong vua, sau do moi cat chu bang "…"
    int c = co;
    while (c > co - 3 && c > 10 && Theme::DoChu(nhan, c, true).x > rongChu) --c;
    std::string hien = Theme::CatChu(nhan, rongChu, c, true);
    Theme::ChuGiua(hien, r, c, mau, true);
}

// ===================== Thong bao =====================
namespace {
struct MucThongBao { std::string noiDung; ThongBao::Loai loai; double batDau; };
std::vector<MucThongBao> g_thongBao;
const double TB_THOI_GIAN = 3.5, TB_MO_DAN = 0.5;
} // namespace

void ThongBao::hien(const std::string& noiDung, Loai loai) {
    if (noiDung.empty()) return;
    g_thongBao.push_back({noiDung, loai, GetTime()});
    if (g_thongBao.size() > 4) g_thongBao.erase(g_thongBao.begin());
}

void ThongBao::ve(float lechTren) {
    (void)lechTren;
    double bayGio = GetTime();
    g_thongBao.erase(std::remove_if(g_thongBao.begin(), g_thongBao.end(),
                                    [&](const MucThongBao& m) { return bayGio - m.batDau > TB_THOI_GIAN; }),
                     g_thongBao.end());
    const float rong = 380, cao = 60;
    // Hien o goc duoi ben phai (khong che nut bam hay tieu de tren giao dien)
    float y = (float)GetScreenHeight() - 20.0f - cao;
    for (int i = (int)g_thongBao.size() - 1; i >= 0; --i) {
        auto& m = g_thongBao[i];
        double tuoi = bayGio - m.batDau;
        float alpha = tuoi > TB_THOI_GIAN - TB_MO_DAN ? (float)((TB_THOI_GIAN - tuoi) / TB_MO_DAN) : 1.0f;
        float truot = tuoi < 0.2 ? (float)(1 - tuoi / 0.2) * 40 : 0;
        Rectangle r = {GetScreenWidth() - rong - 20 + truot, y, rong, cao};
        Color mau = m.loai == Loai::THANH_CONG ? Theme::THANH_CONG : m.loai == Loai::LOI ? Theme::LOI : Theme::CHINH;

        DrawRectangleRounded({r.x, r.y + 3, r.width, r.height}, Widgets::boGoc(r, 12), 10, Fade(BLACK, 0.12f * alpha));
        DrawRectangleRounded(r, Widgets::boGoc(r, 12), 10, Fade(Theme::TRANG, alpha));
        DrawRectangleRounded({r.x, r.y, 6, r.height}, 1.0f, 6, Fade(mau, alpha));
        
        Vector2 tam = {r.x + 30, r.y + cao / 2};
        DrawCircleV(tam, 13, Fade(mau, alpha));
        if (m.loai == Loai::THANH_CONG) {
            DrawLineEx({tam.x - 5, tam.y}, {tam.x - 2, tam.y + 4}, 2.5f, Fade(WHITE, alpha));
            DrawLineEx({tam.x - 2, tam.y + 4}, {tam.x + 6, tam.y - 4}, 2.5f, Fade(WHITE, alpha));
        } else {
            Theme::ChuGiua(m.loai == Loai::LOI ? "!" : "i", {tam.x - 13, tam.y - 13, 26, 26}, 18, Fade(WHITE, alpha), true);
        }
        std::string nd = Theme::CatChu(m.noiDung, rong - 76, Theme::CO_NHO + 1);
        Theme::Chu(nd, r.x + 54, r.y + (cao - 18) / 2 - 1, Theme::CO_NHO + 1, Fade(Theme::CHU, alpha));
        y -= (cao + 10);
    }
}

// ===================== Hop xac nhan =====================
void HopXacNhan::moHop(const std::string& td, const std::string& nd, const std::string& nhan, bool nguy) {
    mo = true; tieuDe = td; noiDung = nd; nhanDongY = nhan; nguyHiem = nguy;
}

int HopXacNhan::ve() {
    if (!mo) return -1;
    Widgets::batDauHopThoai();
    float w = (float)GetScreenWidth(), h = (float)GetScreenHeight();
    DrawRectangle(0, 0, (int)w, (int)h, Fade(BLACK, 0.45f));

    Rectangle hop = {w / 2 - 240, h / 2 - 115, 480, 230};
    Widgets::veThe(hop, 16);
    Theme::Chu(tieuDe, hop.x + 30, hop.y + 28, Theme::CO_VUA, Theme::CHU, true);
    Theme::Chu(Theme::CatChu(noiDung, hop.width - 60, Theme::CO_THUONG), hop.x + 30, hop.y + 80,
               Theme::CO_THUONG, Theme::CHU_PHU);

    Rectangle nutOk = {hop.x + hop.width - 30 - 150, hop.y + hop.height - 30 - 48, 150, 48};
    Rectangle nutHuy = {nutOk.x - 12 - 120, nutOk.y, 120, 48};
    int kq = -1;
    if (VeNut(nutHuy, "Hủy", KieuNut::PHU) || IsKeyPressed(KEY_ESCAPE)) kq = 0;
    if (VeNut(nutOk, nhanDongY, nguyHiem ? KieuNut::NGUY_HIEM : KieuNut::CHINH) || IsKeyPressed(KEY_ENTER)) kq = 1;
    Widgets::ketThucHopThoai();
    if (kq >= 0) mo = false;
    return kq;
}
