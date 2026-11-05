#pragma once
#include <functional>
#include <map>
#include <string>
#include <algorithm>
#include <vector>
#include "Phong.h"

template <class T>
struct Node {
    T data;
    Node* prev = nullptr;
    Node* next = nullptr;
    explicit Node(T v) : data(std::move(v)) {}
};

// Danh sach lien ket doi (template nen phai nam trong header).
// Khong so huu du lieu ben trong: chu so huu tu delete.
template <class T>
class DanhSachLienKet {
    Node<T>* dau = nullptr;
    Node<T>* cuoi = nullptr;
    int soLuong = 0;

    void goNode(Node<T>* n) {
        if (n->prev) n->prev->next = n->next; else dau = n->next;
        if (n->next) n->next->prev = n->prev; else cuoi = n->prev;
        delete n;
        --soLuong;
    }
public:
    DanhSachLienKet() = default;
    DanhSachLienKet(const DanhSachLienKet&) = delete;
    DanhSachLienKet& operator=(const DanhSachLienKet&) = delete;
    ~DanhSachLienKet() {
        while (dau) { Node<T>* t = dau; dau = dau->next; delete t; }
    }
    void themDau(T v) {
        auto* n = new Node<T>(std::move(v));
        n->next = dau;
        if (dau) dau->prev = n; else cuoi = n;
        dau = n; ++soLuong;
    }
    void themCuoi(T v) {
        auto* n = new Node<T>(std::move(v));
        n->prev = cuoi;
        if (cuoi) cuoi->next = n; else dau = n;
        cuoi = n; ++soLuong;
    }
    bool chenTaiViTri(int i, T v) {
        if (i < 0 || i > soLuong) return false;
        if (i == 0) { themDau(std::move(v)); return true; }
        if (i == soLuong) { themCuoi(std::move(v)); return true; }
        Node<T>* p = dau;
        for (int k = 0; k < i; ++k) p = p->next;
        auto* n = new Node<T>(std::move(v));
        n->prev = p->prev; n->next = p;
        p->prev->next = n; p->prev = n;
        ++soLuong;
        return true;
    }
    bool xoaTaiViTri(int i) {
        if (i < 0 || i >= soLuong) return false;
        Node<T>* p = dau;
        for (int k = 0; k < i; ++k) p = p->next;
        goNode(p);
        return true;
    }
    bool xoaNeu(std::function<bool(const T&)> dk) {
        for (Node<T>* p = dau; p; p = p->next)
            if (dk(p->data)) { goNode(p); return true; }
        return false;
    }
    T timNeu(std::function<bool(const T&)> dk) const {
        for (Node<T>* p = dau; p; p = p->next)
            if (dk(p->data)) return p->data;
        return T();
    }
    void sapXep(std::function<bool(const T&, const T&)> ss) {
        for (Node<T>* a = dau; a; a = a->next)
            for (Node<T>* b = a->next; b; b = b->next)
                if (ss(b->data, a->data)) std::swap(a->data, b->data);
    }
    void duyet(std::function<void(const T&)> f) const {
        for (Node<T>* p = dau; p; p = p->next) f(p->data);
    }
    int size() const { return soLuong; }
};

class DanhSachPhong {
    DanhSachLienKet<Phong*> dsPhong; // DanhSachPhong so huu cac Phong*
public:
    ~DanhSachPhong();
    Phong* timTheoMa(const std::string& ma) const;
    // false neu p null hoac trung ma (khi do nguoi goi tu giai phong p)
    bool themDau(Phong* p);
    bool themCuoi(Phong* p);
    bool chenTaiViTri(int viTri, Phong* p);
    bool xoaTheoMa(const std::string& ma);
    void sapXepTheoDonGia(bool tangDan);
    void hienThi() const;
    int getSoLuong() const { return dsPhong.size(); }
    std::vector<Phong*> tatCa() const; // theo thu tu hien tai (dung khi luu file)
};
