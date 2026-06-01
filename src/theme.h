#pragma once
// =============================================================================
//  BREW N BITES — THEME CONFIGURATION
//  ============================================================
//  Ubah nilai-nilai di sini untuk mengustomisasi tampilan aplikasi.
//  Semua warna, font, dan ukuran dikontrol dari file ini.
// =============================================================================

namespace Theme {

// -----------------------------------------------------------------------------
//  COLOR PALETTE — Warm Cafe Neutral
// -----------------------------------------------------------------------------
//  Ganti nilai hex di bawah untuk mengubah skema warna keseluruhan.

// Background colors
constexpr const char* BG_APP           = "#F5F0EB";   // Latar belakang utama aplikasi
constexpr const char* BG_SIDEBAR       = "#FDFAF7";   // Sidebar navigasi
constexpr const char* BG_CARD          = "#FFFFFF";   // Kartu produk / panel
constexpr const char* BG_CARD_HOVER    = "#FFF8F2";   // Kartu saat hover
constexpr const char* BG_INPUT         = "#F9F5F1";   // Field input
constexpr const char* BG_PANEL         = "#FDFAF7";   // Panel kanan (order)
constexpr const char* BG_NUMPAD_KEY    = "#FFFFFF";   // Tombol numpad
constexpr const char* BG_NUMPAD_HOVER  = "#F0EBE5";   // Tombol numpad hover
constexpr const char* BG_BADGE         = "#F0EBE5";   // Badge kategori

// Primary accent — Sage Green (warna utama tombol & aksen)
constexpr const char* ACCENT_PRIMARY   = "#4A6741";   // Tombol utama, ikon aktif
constexpr const char* ACCENT_HOVER     = "#3D5635";   // Tombol saat hover
constexpr const char* ACCENT_LIGHT     = "#E8F0E6";   // Latar badge aktif
constexpr const char* ACCENT_TEXT      = "#FFFFFF";   // Teks di atas tombol hijau

// Secondary accent — Warm Brown
constexpr const char* ACCENT_SECONDARY = "#8B6F47";   // Aksen sekunder coklat
constexpr const char* ACCENT_SEC_LIGHT = "#F5EDE0";   // Latar aksen sekunder

// Text colors
constexpr const char* TEXT_PRIMARY     = "#2C2420";   // Teks utama (gelap)
constexpr const char* TEXT_SECONDARY   = "#7A6A5A";   // Teks sekunder (abu-coklat)
constexpr const char* TEXT_MUTED       = "#B0A090";   // Teks muted / placeholder
constexpr const char* TEXT_PRICE       = "#4A6741";   // Warna harga

// Border & divider
constexpr const char* BORDER_LIGHT     = "#EDE8E2";   // Border halus
constexpr const char* BORDER_MEDIUM    = "#DDD5CA";   // Border sedang
constexpr const char* BORDER_INPUT     = "#D5CAC0";   // Border input field

// Status colors
constexpr const char* COLOR_SUCCESS    = "#4A6741";   // Sukses (hijau)
constexpr const char* COLOR_WARNING    = "#D4A843";   // Peringatan (kuning)
constexpr const char* COLOR_ERROR      = "#C0392B";   // Error (merah)

// Special
constexpr const char* COLOR_TOTAL_BG   = "#4A6741";   // Latar total harga
constexpr const char* COLOR_SHADOW     = "rgba(60,40,20,0.08)"; // Bayangan kartu

// -----------------------------------------------------------------------------
//  TYPOGRAPHY
// -----------------------------------------------------------------------------
//  Ganti nama font di sini. Pastikan font terinstall di sistem Anda.

constexpr const char* FONT_DISPLAY    = "Georgia";          // Font judul besar / brand
constexpr const char* FONT_HEADING    = "Trebuchet MS";     // Font heading seksi
constexpr const char* FONT_BODY       = "Segoe UI";         // Font teks biasa
constexpr const char* FONT_MONO       = "Consolas";         // Font angka / harga
constexpr const char* FONT_FALLBACK   = "Arial";            // Fallback universal

// Font sizes (px)
constexpr int FONT_SIZE_XS     = 10;
constexpr int FONT_SIZE_SM     = 11;
constexpr int FONT_SIZE_BASE   = 12;
constexpr int FONT_SIZE_MD     = 13;
constexpr int FONT_SIZE_LG     = 15;
constexpr int FONT_SIZE_XL     = 18;
constexpr int FONT_SIZE_2XL    = 22;
constexpr int FONT_SIZE_3XL    = 28;
constexpr int FONT_SIZE_4XL    = 36;

// -----------------------------------------------------------------------------
//  SPACING & LAYOUT
// -----------------------------------------------------------------------------

constexpr int SIDEBAR_WIDTH    = 120;   // Lebar sidebar navigasi
constexpr int ORDER_PANEL_W    = 280;   // Lebar panel "Current Order"
constexpr int CARD_RADIUS      = 14;    // Border-radius kartu produk
constexpr int BUTTON_RADIUS    = 10;    // Border-radius tombol
constexpr int INPUT_RADIUS     = 8;     // Border-radius input field
constexpr int BADGE_RADIUS     = 20;    // Border-radius badge kategori
constexpr int SPACING_XS       = 4;
constexpr int SPACING_SM       = 8;
constexpr int SPACING_MD       = 12;
constexpr int SPACING_LG       = 16;
constexpr int SPACING_XL       = 24;
constexpr int SPACING_2XL      = 32;

// -----------------------------------------------------------------------------
//  ANIMATION
// -----------------------------------------------------------------------------

constexpr int ANIM_FAST        = 150;   // ms — hover cepat
constexpr int ANIM_MEDIUM      = 250;   // ms — transisi sedang
constexpr int ANIM_SLOW        = 400;   // ms — transisi lambat

// -----------------------------------------------------------------------------
//  WINDOW
// -----------------------------------------------------------------------------

constexpr int WINDOW_MIN_W     = 1200;
constexpr int WINDOW_MIN_H     = 700;
constexpr int WINDOW_DEFAULT_W = 1280;
constexpr int WINDOW_DEFAULT_H = 780;

} // namespace Theme
