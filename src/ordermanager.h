#pragma once
#include <QObject>
#include <QVector>
#include <QMap>
#include "menuitem.h"

// =============================================================================
//  OrderManager — Singleton state manager untuk sesi transaksi
// =============================================================================

struct OrderLine {
    MenuItem    item;
    int         quantity;

    int subtotal() const { return item.price * quantity; }

    QString formattedSubtotal() const {
        MenuItem dummy = item;
        dummy.price = subtotal();
        return dummy.formattedPrice();
    }
};

class OrderManager : public QObject {
    Q_OBJECT

public:
    explicit OrderManager(QObject* parent = nullptr);

    // Customer info
    void        setCustomerName(const QString& name);
    void        setTableNumber(const QString& table);
    QString     customerName() const { return m_customerName; }
    QString     tableNumber()  const { return m_tableNumber; }

    // Cart operations
    void        addItem(const MenuItem& item);
    void        removeItem(int itemId);
    void        setQuantity(int itemId, int qty);
    void        clearCart();

    // Queries
    QVector<OrderLine>  orderLines()    const { return m_lines; }
    int                 itemCount()     const;
    int                 subtotal()      const;
    int                 tax()           const;   // PB1 10%
    int                 total()         const;

    QString formattedSubtotal()  const;
    QString formattedTax()       const;
    QString formattedTotal()     const;

    // Menu data
    static QVector<MenuItem> allMenuItems();

signals:
    void cartChanged();
    void customerInfoChanged();

private:
    QString             m_customerName;
    QString             m_tableNumber;
    QVector<OrderLine>  m_lines;

    int findLine(int itemId) const;
};
