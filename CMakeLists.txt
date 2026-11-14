cmake_minimum_required(VERSION 3.16)
project(QuanLyKhachSanGUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(MSVC)
    add_compile_options(/utf-8 /W3 /wd4244 /wd4267 /wd4996)
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)
endif()

include(FetchContent)

# 1. Tu dong tai raylib 5.5 neu may chua cai san
find_package(raylib 5.0 QUIET)
if(NOT raylib_FOUND)
    message(STATUS "Dang tai raylib 5.5...")
    set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(BUILD_GAMES    OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG        5.5
        GIT_SHALLOW    TRUE
    )
    FetchContent_MakeAvailable(raylib)
endif()

# 2. Tu dong tai raygui 4.0 (single-header)
FetchContent_Declare(
    raygui
    GIT_REPOSITORY https://github.com/raysan5/raygui.git
    GIT_TAG        4.0
    GIT_SHALLOW    TRUE
)
FetchContent_MakeAvailable(raygui)

# 3. Cac file nguon theo 3 tang: data / service / ui (khong dung bam mat khau)
set(SOURCES
    src/main.cpp
    # ---- data (tai su dung nguyen ban console + tai khoan luu mat khau truc tiep) ----
    src/data/Common.cpp
    src/data/DanhSach.cpp
    src/data/DichVu.cpp
    src/data/HoaDon.cpp
    src/data/KhachHang.cpp
    src/data/LuotThue.cpp
    src/data/NhanVien.cpp
    src/data/Phong.cpp
    src/data/KhachSanData.cpp
    src/data/LuuTru.cpp
    src/data/TaiKhoan.cpp
    # ---- service (nghiep vu + phan quyen) ----
    src/service/PhanQuyen.cpp
    src/service/AuthService.cpp
    src/service/KhachSanService.cpp
    # ---- ui (raylib + raygui) ----
    src/ui/Theme.cpp
    src/ui/Widgets.cpp
    src/ui/LuoiPhong.cpp
    src/ui/Layout.cpp
    src/ui/LoginScreen.cpp
    src/ui/DashboardScreen.cpp
    src/ui/RoomScreen.cpp
    src/ui/CustomerScreen.cpp
    src/ui/InvoiceScreen.cpp
    src/ui/RevenueScreen.cpp
    src/ui/StaffScreen.cpp
    src/ui/AccountScreen.cpp
    src/ui/App.cpp
)

add_executable(QuanLyKhachSanGUI ${SOURCES})

target_include_directories(QuanLyKhachSanGUI PRIVATE
    src
    src/data
    src/service
    src/ui
    ${raygui_SOURCE_DIR}/src
)

target_link_libraries(QuanLyKhachSanGUI PRIVATE raylib)
if(WIN32)
    target_link_libraries(QuanLyKhachSanGUI PRIVATE winmm gdi32 opengl32)
endif()

# Copy thu muc assets canh file chay de tim thay anh + font
add_custom_command(TARGET QuanLyKhachSanGUI POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:QuanLyKhachSanGUI>/assets"
)

# 4. Target chay test case kiem thu toan bo he thong
set(TEST_SOURCES
    test.cpp
    src/data/Common.cpp
    src/data/DanhSach.cpp
    src/data/DichVu.cpp
    src/data/HoaDon.cpp
    src/data/KhachHang.cpp
    src/data/LuotThue.cpp
    src/data/NhanVien.cpp
    src/data/Phong.cpp
    src/data/KhachSanData.cpp
    src/data/LuuTru.cpp
    src/data/TaiKhoan.cpp
    src/service/PhanQuyen.cpp
    src/service/AuthService.cpp
    src/service/KhachSanService.cpp
)
add_executable(KiemThu ${TEST_SOURCES})
target_include_directories(KiemThu PRIVATE
    src
    src/data
    src/service
)
