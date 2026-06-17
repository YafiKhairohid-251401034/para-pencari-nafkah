#include "ordermanager.h"

OrderManager::OrderManager(QObject *parent)
    : QObject(parent)
{}

void OrderManager::setCustomerName(const QString &name)
{
    m_customerName = name;
    emit customerInfoChanged();
}

void OrderManager::setTableNumber(const QString &table)
{
    m_tableNumber = table;
    emit customerInfoChanged();
}

void OrderManager::addItem(const MenuItem &item)
{
    int idx = findLine(item.id);
    if (idx >= 0) {
        m_lines[idx].quantity++;
    } else {
        m_lines.append({item, 1});
    }
    emit cartChanged();
}

void OrderManager::removeItem(int itemId)
{
    int idx = findLine(itemId);
    if (idx >= 0) {
        if (m_lines[idx].quantity > 1) {
            m_lines[idx].quantity--;
        } else {
            m_lines.removeAt(idx);
        }
        emit cartChanged();
    }
}

void OrderManager::setQuantity(int itemId, int qty)
{
    int idx = findLine(itemId);
    if (idx >= 0) {
        if (qty <= 0)
            m_lines.removeAt(idx);
        else
            m_lines[idx].quantity = qty;
        emit cartChanged();
    }
}

void OrderManager::clearCart()
{
    m_lines.clear();
    m_customerName.clear();
    m_tableNumber.clear();
    emit cartChanged();
    emit customerInfoChanged();
}

int OrderManager::itemCount() const
{
    int n = 0;
    for (const auto &l : m_lines)
        n += l.quantity;
    return n;
}

int OrderManager::subtotal() const
{
    int s = 0;
    for (const auto &l : m_lines)
        s += l.subtotal();
    return s;
}

int OrderManager::tax() const
{
    return subtotal() / 10; // PB1 10%
}

int OrderManager::total() const
{
    return subtotal() + tax();
}

// Helper lambda to format rupiah
static QString formatRp(int amount)
{
    QString p = QString::number(amount);
    int insertPos = p.length() - 3;
    while (insertPos > 0) {
        p.insert(insertPos, '.');
        insertPos -= 3;
    }
    return "Rp " + p;
}

QString OrderManager::formattedSubtotal() const
{
    return formatRp(subtotal());
}
QString OrderManager::formattedTax() const
{
    return formatRp(tax());
}
QString OrderManager::formattedTotal() const
{
    return formatRp(total());
}

int OrderManager::findLine(int itemId) const
{
    for (int i = 0; i < m_lines.size(); ++i)
        if (m_lines[i].item.id == itemId)
            return i;
    return -1;
}

//  MENU DATA — Tambah / ubah item menu di sini
QVector<MenuItem> OrderManager::allMenuItems()
{
    return {
        // --- COFFEE ---
        {1, "Caffe Latte", "Double shot, steamed milk", 38000, MenuCategory::Coffee, "☕", "#C8A882"},
        {2,
         "Iced Americano",
         "Espresso blend, served cold",
         32000,
         MenuCategory::Coffee,
         "🧊",
         "#8BB5C8"},
        {3,
         "Kopi Susu Gula Aren",
         "Gula aren, full cream",
         45000,
         MenuCategory::Coffee,
         "🍯",
         "#C8A050"},
        {4, "Matcha Latte", "Premium grade, oat milk", 42000, MenuCategory::Coffee, "🍵", "#8BB880"},
        {5, "Dark Mocha", "Handcrafted, dark cocoa", 40000, MenuCategory::Coffee, "🍫", "#8B6040"},
        {6, "Double Espresso", "Intense & bold", 30000, MenuCategory::Coffee, "⚡", "#7A5A40"},
        {7, "Butterscotch Latte", "Smooth, creamy", 35000, MenuCategory::Coffee, "🧡", "#D4A843"},
        {8, "Vanilla Latte", "Vanilla syrup, silky", 40000, MenuCategory::Coffee, "🌿", "#C8C080"},
        {9, "Hazelnut Latte", "Hazelnut syrup, creamy", 40000, MenuCategory::Coffee, "🌰", "#B08040"},
        {10, "Cappuccino", "Classic Italian style", 35000, MenuCategory::Coffee, "☕", "#C09060"},

        // --- NON-COFFEE ---
        {11,
         "Matcha Milk",
         "Premium green tea, full cream",
         38000,
         MenuCategory::NonCoffee,
         "🍵",
         "#7AAF70"},
        {12, "Taro Latte", "Purple taro, creamy", 40000, MenuCategory::NonCoffee, "💜", "#A080B8"},
        {13, "Red Velvet Latte", "Beetroot, vanilla", 38000, MenuCategory::NonCoffee, "❤️", "#C04040"},
        {14, "Chamomile Tea", "Calming, floral", 25000, MenuCategory::NonCoffee, "🌼", "#D4C060"},
        {15, "Lychee Soda", "Fresh, sparkling", 30000, MenuCategory::NonCoffee, "🫧", "#E8A0B0"},
        {16,
         "Mango Smoothie",
         "Fresh tropical blend",
         35000,
         MenuCategory::NonCoffee,
         "🥭",
         "#E8A830"},

        // --- CAKE ---
        {17,
         "Butter Croissant",
         "Flaky, 100% French butter",
         28000,
         MenuCategory::Cake,
         "🥐",
         "#E8C870"},
        {18, "Roti Susu Manis", "Clean, soft", 16000, MenuCategory::Cake, "🍞", "#E8D0A0"},
        {19, "Chocolate Lava Cake", "Warm, gooey center", 45000, MenuCategory::Cake, "🎂", "#806040"},
        {20, "Cheesecake Slice", "New York style", 42000, MenuCategory::Cake, "🧁", "#F0E0B8"},
        {21, "Banana Bread", "Moist, homemade", 32000, MenuCategory::Cake, "🍌", "#E8C040"},
        {22, "Tiramisu", "Classic Italian", 48000, MenuCategory::Cake, "✨", "#C09060"},

        // --- FOOD ---
        {23,
         "Nasi Goreng Kimchi",
         "Spicy fusion, egg, spice",
         55000,
         MenuCategory::Food,
         "🍳",
         "#E87840"},
        {24, "Avocado Toast", "Sourdough, fresh avocado", 48000, MenuCategory::Food, "🥑", "#80A860"},
        {25, "Club Sandwich", "Triple layer, chicken", 55000, MenuCategory::Food, "🥪", "#D4A843"},
        {26, "Chicken Waffle", "Crispy fried chicken", 65000, MenuCategory::Food, "🧇", "#E8B840"},
        {27, "Truffle Fries", "Crispy, truffle oil", 45000, MenuCategory::Food, "🍟", "#D4C060"},
        {28,
         "Mushroom Bruschetta",
         "Sauteed, fresh herbs",
         42000,
         MenuCategory::Food,
         "🍄",
         "#A08060"},
    };
}
