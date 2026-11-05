#pragma once
// Theme: bang mau, co chu va font tieng Viet dung chung cho toan bo giao dien.
#include <string>
#include "raylib.h"

namespace Theme {
    // ---- Bang mau ----
    const Color NEN          = {241, 245, 249, 255}; // nen vung noi dung
    const Color NEN_SANG     = {246, 248, 252, 255}; // nen trang dang nhap
    const Color TRANG        = {255, 255, 255, 255};
    const Color SIDEBAR      = { 15,  23,  42, 255};
    const Color SIDEBAR_HOVER= { 30,  41,  59, 255};
    const Color SIDEBAR_CHON = { 37,  99, 235, 255};
    const Color CHU          = { 15,  23,  42, 255}; // chu chinh
    const Color CHU_PHU      = {100, 116, 139, 255}; // chu mo
    const Color CHU_SIDEBAR  = {203, 213, 225, 255};
    const Color VIEN         = {203, 213, 225, 255};
    const Color CHINH        = { 37,  99, 235, 255}; // xanh duong chu dao
    const Color CHINH_DAM    = { 29,  78, 216, 255};
    const Color THANH_CONG   = { 22, 163,  74, 255};
    const Color LOI          = {220,  38,  38, 255};
    const Color CANH_BAO     = {217, 119,   6, 255};
    const Color XAM          = {100, 116, 139, 255};

    // ---- Co chu ----
    const int CO_NHO = 16, CO_THUONG = 20, CO_VUA = 24, CO_LON = 30, CO_TIEU_DE = 40;

    // Nap font (goi sau InitWindow). Tu tim font trong assets/fonts, khong co thi dung font Windows.
    void napFont();
    void giaiPhongFont();
    // Font theo co chu (duoc nap 1 lan roi luu lai)
    Font F(int co, bool dam = false);
    // Ap mau + font cho raygui
    void apDungRaygui();

    // Ve chu UTF-8
    void Chu(const std::string& s, float x, float y, int co, Color mau, bool dam = false);
    Vector2 DoChu(const std::string& s, int co, bool dam = false);
    void ChuGiua(const std::string& s, Rectangle r, int co, Color mau, bool dam = false);
    // Cat bot chu cho vua chieu rong (them "…")
    std::string CatChu(const std::string& s, float rongToiDa, int co, bool dam = false);
}
