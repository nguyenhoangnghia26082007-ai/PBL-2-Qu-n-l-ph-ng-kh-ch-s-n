// Theme.cpp: nap font day du ky tu tieng Viet bang LoadFontEx + bang ma ky tu.
#include "Theme.h"
#include <map>
#include <vector>
#include "raygui.h"

namespace {

std::string g_fileThuong, g_fileDam;
std::map<int, Font> g_font; // khoa = co*2 + dam
std::vector<int> g_maKyTu;  // danh sach codepoint can nap

// Tao danh sach ky tu: ASCII + Latin-1 + Latin Extended-A (Ă Đ Ĩ Ũ) + Ơ Ư + khoi tieng Viet U+1EA0..U+1EF9
void taoBangKyTu() {
    if (!g_maKyTu.empty()) return;
    for (int c = 0x20; c <= 0x7E; ++c) g_maKyTu.push_back(c);
    for (int c = 0xA0; c <= 0xFF; ++c) g_maKyTu.push_back(c);
    for (int c = 0x100; c <= 0x17F; ++c) g_maKyTu.push_back(c);
    for (int c : {0x1A0, 0x1A1, 0x1AF, 0x1B0}) g_maKyTu.push_back(c);
    for (int c = 0x300; c <= 0x323; ++c) g_maKyTu.push_back(c);          // dau to hop (go kieu to hop)
    for (int c = 0x1EA0; c <= 0x1EF9; ++c) g_maKyTu.push_back(c);
    for (int c : {0x2013, 0x2014, 0x2018, 0x2019, 0x201C, 0x201D, 0x2022, 0x2026, 0x20AB, 0x2190, 0x2192})
        g_maKyTu.push_back(c);                                           // – — ' ' " " • … ₫ ← →
}

std::string timFile(std::initializer_list<const char*> ds) {
    for (const char* f : ds) if (FileExists(f)) return f;
    return "";
}

} // namespace

void Theme::napFont() {
    taoBangKyTu();
    // Uu tien font dat trong assets/fonts (vd. Be Vietnam Pro, Roboto), sau do font co san cua Windows
    g_fileThuong = timFile({"assets/fonts/Regular.ttf", "C:/Windows/Fonts/segoeui.ttf", "C:/Windows/Fonts/arial.ttf",
                            "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"});
    g_fileDam = timFile({"assets/fonts/Bold.ttf", "C:/Windows/Fonts/segoeuib.ttf", "C:/Windows/Fonts/arialbd.ttf",
                         "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf"});
    if (g_fileDam.empty()) g_fileDam = g_fileThuong;
    if (g_fileThuong.empty()) TraceLog(LOG_WARNING, "Khong tim thay font TTF, chu co dau se hien sai!");
}

void Theme::giaiPhongFont() {
    for (auto& [k, f] : g_font) UnloadFont(f);
    g_font.clear();
}

Font Theme::F(int co, bool dam) {
    int khoa = co * 2 + (dam ? 1 : 0);
    auto it = g_font.find(khoa);
    if (it != g_font.end()) return it->second;

    const std::string& file = dam ? g_fileDam : g_fileThuong;
    Font f = file.empty() ? GetFontDefault()
                          : LoadFontEx(file.c_str(), co, g_maKyTu.data(), (int)g_maKyTu.size());
    if (f.texture.id == 0) f = GetFontDefault();
    else SetTextureFilter(f.texture, TEXTURE_FILTER_BILINEAR);
    g_font[khoa] = f;
    return f;
}

void Theme::apDungRaygui() {
    GuiSetFont(F(CO_NHO + 2));
    GuiSetStyle(DEFAULT, TEXT_SIZE, CO_NHO + 2);
    GuiSetStyle(DEFAULT, TEXT_SPACING, 0);
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, ColorToInt(VIEN));
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, ColorToInt(TRANG));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, ColorToInt(CHU_PHU));
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, ColorToInt(CHINH));
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, ColorToInt({219, 234, 254, 255}));
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, ColorToInt(CHU));
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, ColorToInt(CHINH_DAM));
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, ColorToInt(CHINH));
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, ColorToInt(CHU));
    GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(VIEN));
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(TRANG));
    GuiSetStyle(CHECKBOX, TEXT_PADDING, 10);
}

void Theme::Chu(const std::string& s, float x, float y, int co, Color mau, bool dam) {
    DrawTextEx(F(co, dam), s.c_str(), {x, y}, (float)co, 0, mau);
}

Vector2 Theme::DoChu(const std::string& s, int co, bool dam) {
    return MeasureTextEx(F(co, dam), s.c_str(), (float)co, 0);
}

void Theme::ChuGiua(const std::string& s, Rectangle r, int co, Color mau, bool dam) {
    Vector2 kt = DoChu(s, co, dam);
    Chu(s, r.x + (r.width - kt.x) / 2, r.y + (r.height - kt.y) / 2, co, mau, dam);
}

std::string Theme::CatChu(const std::string& s, float rongToiDa, int co, bool dam) {
    if (DoChu(s, co, dam).x <= rongToiDa) return s;
    std::string r = s;
    while (!r.empty()) {
        // bo 1 ky tu UTF-8 o cuoi
        while (!r.empty() && ((unsigned char)r.back() & 0xC0) == 0x80) r.pop_back();
        if (!r.empty()) r.pop_back();
        if (DoChu(r + "…", co, dam).x <= rongToiDa) return r + "…";
    }
    return "";
}
