#pragma once
#include <QDate>
#include <QObject>
#include <QString>
#include <QVector>
#include "ordermanager.h"

// =============================================================================
//  HistoryManager — Menyimpan & memuat riwayat transaksi secara persisten
//  menggunakan QSettings (file INI lokal, tidak perlu modul Qt::Sql tambahan)
// =============================================================================

struct ReceiptItem
{
    QString name;
    int quantity;
    int unitPrice;
    int subtotal;
};

struct TransactionRecord
{
    int id;                 // ID unik (urutan penyimpanan)
    QString dateTime;       // "dd/MM/yyyy hh:mm"
    QString customerName;
    QString tableNumber;
    QString paymentMethod;  // "Tunai" / "QRIS"
    int subtotal;
    int tax;
    int total;
    long long cashPaid;     // 0 jika QRIS
    long long change;       // 0 jika QRIS
    QVector<ReceiptItem> items;

    QString formattedTotal() const;

    // Format tampilan yang lebih jelas & menarik, mis. "Rabu, 17 Juni 2026, 14:30"
    // (dateTime mentah tetap disimpan dalam format "dd/MM/yyyy hh:mm" untuk parsing/filter)
    QString formattedDateTime() const;

    // Ambil bagian tanggal saja (tanpa jam) untuk keperluan filter per hari
    QDate parsedDate() const
    {
        return QDate::fromString(dateTime.left(10), "dd/MM/yyyy");
    }
};

class HistoryManager : public QObject
{
    Q_OBJECT

public:
    explicit HistoryManager(QObject *parent = nullptr);

    // Simpan transaksi baru (dipanggil saat transaksi selesai)
    void saveTransaction(OrderManager *mgr, const QString &paymentMethod, long long cashPaid);

    // Ambil semua riwayat, terbaru di urutan pertama
    QVector<TransactionRecord> allTransactions() const;

    // Hapus seluruh riwayat (opsional, untuk reset)
    void clearHistory();

signals:
    void historyChanged();

private:
    static QString settingsFilePath();
};