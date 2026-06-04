#pragma once
#include <QString>

// =============================================================================
//  MenuItem — Model data untuk satu produk menu
// =============================================================================

enum class MenuCategory { All, Coffee, NonCoffee, Cake, Food };

struct MenuItem
{
    int id;
    QString name;
    QString description;
    int price; // dalam Rupiah
    MenuCategory category;
    QString emoji;       // emoji representasi produk (fallback gambar)
    QString colorAccent; // warna aksen kartu produk

    // Helper: format harga ke "Rp XX.XXX"
    QString formattedPrice() const
    {
        QString p = QString::number(price);
        // Insert dots every 3 digits from right
        int insertPos = p.length() - 3;
        while (insertPos > 0) {
            p.insert(insertPos, '.');
            insertPos -= 3;
        }
        return "Rp " + p;
    }

    QString categoryString() const
    {
        switch (category) {
        case MenuCategory::Coffee:
            return "Coffee";
        case MenuCategory::NonCoffee:
            return "Non-Coffee";
        case MenuCategory::Cake:
            return "Cake";
        case MenuCategory::Food:
            return "Food";
        default:
            return "All";
        }
    }
};
