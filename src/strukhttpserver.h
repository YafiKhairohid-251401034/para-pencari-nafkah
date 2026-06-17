#pragma once
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QString>
#include "ordermanager.h"

//  StrukHttpServer — Server HTTP mini yang serve halaman struk ke HP pelanggan.
//  Cara kerja:
//    1. start(mgr, paymentMethod, cashPaid) → pilih port bebas, simpan data struk
//    2. serverUrl() → kembalikan "http://[IP WiFi lokal]:PORT/struk"
//    3. HP scan QR → buka URL → terima HTML struk yang cantik
//    4. stop() → matikan server (dipanggil setelah transaksi selesai)

class StrukHttpServer : public QObject
{
    Q_OBJECT

public:
    explicit StrukHttpServer(QObject *parent = nullptr);
    ~StrukHttpServer();

    // Mulai server, simpan snapshot data struk dari OrderManager
    // Harus dipanggil SEBELUM cart dibersihkan
    bool start(OrderManager *mgr,
               const QString &paymentMethod,
               long long cashPaid);

    void stop();

    QString serverUrl() const { return m_url; }
    bool isRunning() const;

private slots:
    void onNewConnection();

private:
    QString buildHtml() const;
    static QString fmtRp(long long val);
    static QString getLocalIp();

    QTcpServer *m_server = nullptr;
    QString     m_url;

    // Snapshot data transaksi (disimpan saat start, sebelum cart di-clear)
    QString  m_customerName;
    QString  m_tableNumber;
    QString  m_paymentMethod;
    long long m_cashPaid   = 0;
    long long m_subtotal   = 0;
    long long m_tax        = 0;
    long long m_total      = 0;
    QString  m_datetime;

    struct SnapLine {
        QString name;
        int     quantity;
        long long subtotalLine;
    };
    QVector<SnapLine> m_lines;
};