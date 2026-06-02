# ☕ Brew n Bites 
### Aplikasi Kasir Kafe — Qt C++

---

## 📁 Struktur File

```
BrewNBites/
├── BrewNBites.pro      ← File project Qt
├── theme.h             ← ⭐ KONFIGURASI TEMA (ubah di sini!)
├── main.cpp            ← Entry point
├── mainwindow.h/.cpp   ← Window utama + navigasi antar halaman
├── menuitem.h/.cpp     ← Model data produk menu
├── ordermanager.h/.cpp ← State keranjang + data menu
├── registerpage.h/.cpp ← Halaman 1: Entri pelanggan
├── orderpage.h/.cpp    ← Halaman 2: Grid menu + order panel
└── paymentpage.h/.cpp  ← Halaman 3: Kasir numpad
```

---

## 🚀 Cara Kompilasi

### Menggunakan Qt Creator (Disarankan)
1. Buka Qt Creator
2. **File → Open File or Project**
3. Pilih file `BrewNBites.pro`
4. Klik **Configure Project**
5. Tekan **Ctrl+R** atau tombol ▶ hijau untuk Build & Run

### Menggunakan Command Line
```bash
cd BrewNBites/
qmake BrewNBites.pro
make          # Linux/macOS
# atau
mingw32-make  # Windows dengan MinGW
```

**Persyaratan:**
- Qt 5.12+ atau Qt 6.x
- Compiler: MSVC 2017+, GCC 8+, atau Clang 8+
- C++17 enabled (sudah dikonfigurasi di .pro file)

---

## 🎨 Cara Mengubah Tema

Semua pengaturan tampilan ada di **`theme.h`**. Cukup ubah nilai konstanta, lalu recompile.

### Mengubah Warna
```cpp
// Contoh: ganti ke tema gelap (dark mode)
constexpr const char* BG_APP        = "#1A1A2E";   // Background gelap
constexpr const char* BG_SIDEBAR    = "#16213E";   // Sidebar gelap
constexpr const char* BG_CARD       = "#0F3460";   // Kartu gelap
constexpr const char* ACCENT_PRIMARY= "#E94560";   // Aksen merah/pink
constexpr const char* TEXT_PRIMARY  = "#EAEAEA";   // Teks terang
```

### Mengubah Font
```cpp
// Ganti dengan font yang terinstall di sistem Anda
constexpr const char* FONT_DISPLAY  = "Playfair Display"; // Font judul
constexpr const char* FONT_HEADING  = "Raleway";          // Font heading
constexpr const char* FONT_BODY     = "Lato";             // Font body
```

### Mengubah Ukuran Layout
```cpp
constexpr int SIDEBAR_WIDTH   = 120;  // Lebar sidebar
constexpr int ORDER_PANEL_W   = 280;  // Lebar panel order kanan
constexpr int CARD_RADIUS     = 14;   // Sudut membulat kartu
constexpr int BUTTON_RADIUS   = 10;   // Sudut membulat tombol
```

---

## 🍽️ Cara Menambah/Ubah Menu

Edit fungsi `allMenuItems()` di **`ordermanager.cpp`**:

```cpp
// Format: {id, "Nama", "Deskripsi", harga, Kategori, "emoji", "warnaksen"}
{30, "Cold Brew",    "12-hour steep",     40000, MenuCategory::Coffee,    "🧊", "#7090C0"},
{31, "Açaí Bowl",    "Fresh tropical",    55000, MenuCategory::Food,      "🫐", "#8060C0"},
```

**Kategori yang tersedia:**
- `MenuCategory::Coffee`
- `MenuCategory::NonCoffee`
- `MenuCategory::Cake`
- `MenuCategory::Food`

---

## 💡 Fitur Aplikasi

| Halaman     | Fitur                                                     |
|-------------|-----------------------------------------------------------|
| Register    | Input nama pelanggan & nomor meja, validasi form          |
| Order       | Grid menu 4 kolom, filter kategori, search real-time      |
|             | Panel "Current Order" kanan, tambah/hapus item            |
|             | Kalkulasi subtotal + PB1 (10%) + total otomatis           |
| Payment     | Numpad kasir tunai, tab metode (Tunai/QRIS/Kartu)         |
|             | Kalkulasi kembalian real-time, konfirmasi transaksi        |

---

## 📝 Catatan

- Harga dalam **Rupiah (Rp)** dengan format `XX.XXX`
- PB1 dihitung otomatis **10%** dari subtotal
- Setelah transaksi selesai, aplikasi kembali ke halaman Register
- Font fallback ke sistem jika font khusus tidak terinstall

---

*Brew n Bites POS — Built with Qt C++*
