#pragma once
// KetQua: ket qua tra ve tu tang Service cho UI (thanh cong/that bai + thong bao tieng Viet).
#include <string>

struct KetQua {
    bool ok = true;
    std::string thongBao;

    static KetQua Ok(const std::string& tb = "") { return {true, tb}; }
    static KetQua Loi(const std::string& tb) { return {false, tb}; }
    explicit operator bool() const { return ok; }
};
