#include "historymanager.h"
#include <QDateTime>
#include <QSettings>
#include <QStandardPaths>
#include <QDir>

// Helper format rupiah (sama seperti di OrderManager / ReceiptDialog)
static QString formatRp(long long amount)
{
    QString p = QString::number(amount);
    int pos = p.length() - 3;
    while (pos > 0) {
        p.insert(pos, '.');
        pos -= 3;
    }
    return "Rp " + p;
}

QString TransactionRecord::formattedTotal() const
{
    return formatRp(total);
}

//  formattedDateTime — ubah "dd/MM/yyyy hh:mm"
static const char *const HARI_ID[8] =
    {"", "Senin", "Selasa", "Rabu", "Kamis", "Jumat", "Sabtu", "Minggu"};
static const char *const BULAN_ID[13] =
    {"",       "Januari", "Februari", "Maret",     "April",   "Mei",
     "Juni",   "Juli",    "Agustus",  "September", "Oktober", "November",
     "Desember"};

QString TransactionRecord::formattedDateTime() const
{
    QDateTime dt = QDateTime::fromString(dateTime, "dd/MM/yyyy hh:mm");
    if (!dt.isValid())
        return dateTime; // fallback — data lama/tidak terduga, tampilkan apa adanya

    const QDate d = dt.date();
    return QString("%1, %2 %3 %4, %5")
        .arg(HARI_ID[d.dayOfWeek()])
        .arg(d.day())
        .arg(BULAN_ID[d.month()])
        .arg(d.year())
        .arg(dt.toString("HH:mm"));
}

//  Lokasi file penyimpanan, Disimpan di folder AppData/AppConfig pengguna sehingga riwayat tetap ada
QString HistoryManager::settingsFilePath()
{
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dir);
    return dir + "/transaction_history.ini";
}

HistoryManager::HistoryManager(QObject *parent)
    : QObject(parent)
{}

void HistoryManager::saveTransaction(OrderManager *mgr, const QString &paymentMethod, long long cashPaid)
{
    QSettings settings(settingsFilePath(), QSettings::IniFormat);

    int count = settings.value("history/count", 0).toInt();
    int newId = count + 1;

    long long total = static_cast<long long>(mgr->total());
    long long change = (paymentMethod == "Tunai") ? (cashPaid - total) : 0;

    QString group = QString("transaction_%1").arg(newId);
    settings.beginGroup(group);
    settings.setValue("id", newId);
    settings.setValue("dateTime", QDateTime::currentDateTime().toString("dd/MM/yyyy hh:mm"));
    settings.setValue("customerName", mgr->customerName());
    settings.setValue("tableNumber", mgr->tableNumber());
    settings.setValue("paymentMethod", paymentMethod);
    settings.setValue("subtotal", mgr->subtotal());
    settings.setValue("tax", mgr->tax());
    settings.setValue("total", mgr->total());
    settings.setValue("cashPaid", static_cast<qlonglong>(paymentMethod == "Tunai" ? cashPaid : 0));
    settings.setValue("change", static_cast<qlonglong>(paymentMethod == "Tunai" ? change : 0));

    // Simpan item-item pesanan
    const auto lines = mgr->orderLines();
    settings.setValue("itemCount", lines.size());
    for (int i = 0; i < lines.size(); ++i) {
        const OrderLine &line = lines[i];
        settings.setValue(QString("item_%1_name").arg(i), line.item.name);
        settings.setValue(QString("item_%1_qty").arg(i), line.quantity);
        settings.setValue(QString("item_%1_price").arg(i), line.item.price);
        settings.setValue(QString("item_%1_subtotal").arg(i), line.subtotal());
    }
    settings.endGroup();

    settings.setValue("history/count", newId);
    settings.sync();

    emit historyChanged();
}

QVector<TransactionRecord> HistoryManager::allTransactions() const
{
    QSettings settings(settingsFilePath(), QSettings::IniFormat);
    int count = settings.value("history/count", 0).toInt();

    QVector<TransactionRecord> result;
    result.reserve(count);

    // Iterasi mundur agar transaksi terbaru muncul pertama
    for (int id = count; id >= 1; --id) {
        QString group = QString("transaction_%1").arg(id);
        if (!settings.childGroups().contains(group))
            continue;

        settings.beginGroup(group);

        TransactionRecord rec;
        rec.id = settings.value("id", id).toInt();
        rec.dateTime = settings.value("dateTime").toString();
        rec.customerName = settings.value("customerName").toString();
        rec.tableNumber = settings.value("tableNumber").toString();
        rec.paymentMethod = settings.value("paymentMethod").toString();
        rec.subtotal = settings.value("subtotal", 0).toInt();
        rec.tax = settings.value("tax", 0).toInt();
        rec.total = settings.value("total", 0).toInt();
        rec.cashPaid = settings.value("cashPaid", 0).toLongLong();
        rec.change = settings.value("change", 0).toLongLong();

        int itemCount = settings.value("itemCount", 0).toInt();
        for (int i = 0; i < itemCount; ++i) {
            ReceiptItem item;
            item.name = settings.value(QString("item_%1_name").arg(i)).toString();
            item.quantity = settings.value(QString("item_%1_qty").arg(i), 0).toInt();
            item.unitPrice = settings.value(QString("item_%1_price").arg(i), 0).toInt();
            item.subtotal = settings.value(QString("item_%1_subtotal").arg(i), 0).toInt();
            rec.items.append(item);
        }

        settings.endGroup();
        result.append(rec);
    }

    return result;
}

void HistoryManager::clearHistory()
{
    QSettings settings(settingsFilePath(), QSettings::IniFormat);
    settings.clear();
    settings.sync();
    emit historyChanged();
}