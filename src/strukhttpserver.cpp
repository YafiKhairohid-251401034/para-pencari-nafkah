#include "strukhttpserver.h"
#include <QDateTime>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QTcpSocket>
#include <QTextStream>

//  StrukHttpServer — implementasi

StrukHttpServer::StrukHttpServer(QObject *parent)
    : QObject(parent)
    , m_server(new QTcpServer(this))
{
    connect(m_server, &QTcpServer::newConnection,
            this,     &StrukHttpServer::onNewConnection);
}

StrukHttpServer::~StrukHttpServer()
{
    stop();
}

//  start — simpan snapshot data + jalankan server di port bebas
bool StrukHttpServer::start(OrderManager *mgr,
                            const QString &paymentMethod,
                            long long cashPaid)
{
    stop(); // matikan server lama jika ada

    // Simpan snapshot semua data SEBELUM cart mungkin di-clear
    m_customerName  = mgr->customerName();
    m_tableNumber   = mgr->tableNumber();
    m_paymentMethod = paymentMethod;
    m_cashPaid      = cashPaid;
    m_subtotal      = static_cast<long long>(mgr->subtotal());
    m_tax           = static_cast<long long>(mgr->tax());
    m_total         = static_cast<long long>(mgr->total());
    m_datetime      = QDateTime::currentDateTime()
                     .toString("dd/MM/yyyy  HH:mm");

    m_lines.clear();
    for (const OrderLine &line : mgr->orderLines()) {
        m_lines.append({line.item.name,
                        line.quantity,
                        static_cast<long long>(line.subtotal())});
    }

    // Listen di semua interface, port 0 = OS pilih port bebas
    if (!m_server->listen(QHostAddress::AnyIPv4, 0))
        return false;

    quint16 port = m_server->serverPort();
    QString ip   = getLocalIp();
    m_url        = QString("http://%1:%2/struk").arg(ip).arg(port);
    return true;
}

void StrukHttpServer::stop()
{
    if (m_server && m_server->isListening())
        m_server->close();
    m_url.clear();
}

bool StrukHttpServer::isRunning() const
{
    return m_server && m_server->isListening();
}

//  onNewConnection — balas setiap request dengan HTML struk
void StrukHttpServer::onNewConnection()
{
    while (m_server->hasPendingConnections()) {
        QTcpSocket *socket = m_server->nextPendingConnection();

        // Tunggu request tiba (non-blocking, max 2 detik)
        socket->waitForReadyRead(2000);
        socket->readAll(); // buang request, kita tidak perlu parse-nya

        QByteArray html  = buildHtml().toUtf8();
        QByteArray response;
        response += "HTTP/1.1 200 OK\r\n";
        response += "Content-Type: text/html; charset=utf-8\r\n";
        response += "Content-Length: " + QByteArray::number(html.size()) + "\r\n";
        response += "Connection: close\r\n";
        response += "\r\n";
        response += html;

        socket->write(response);
        socket->flush();
        socket->disconnectFromHost();
        socket->deleteLater();
    }
}

//  buildHtml — render struk sebagai HTML yang mobile-friendly
QString StrukHttpServer::buildHtml() const
{
    bool isTunai = (m_paymentMethod == "Tunai");
    long long kembalian = isTunai ? (m_cashPaid - m_total) : 0LL;

    // item
    QString itemRows;
    for (const SnapLine &line : m_lines) {
        itemRows += QString(
                        "<tr>"
                        "<td>%1</td>"
                        "<td class='qty'>x%2</td>"
                        "<td class='price'>%3</td>"
                        "</tr>")
                        .arg(line.name.toHtmlEscaped())
                        .arg(line.quantity)
                        .arg(fmtRp(line.subtotalLine));
    }

    //bayar/kembalian (hanya Tunai)
    QString cashRows;
    if (isTunai) {
        cashRows = QString(
                       "<tr class='sep-row'><td colspan='3'><hr/></td></tr>"
                       "<tr><td colspan='2'>Bayar</td><td class='price'>%1</td></tr>"
                       "<tr class='bold'><td colspan='2'>Kembalian</td>"
                       "    <td class='price green'>%2</td></tr>")
                       .arg(fmtRp(m_cashPaid))
                       .arg(fmtRp(kembalian));
    }

    return QString(R"html(<!DOCTYPE html>
<html lang="id">
<head>
<meta charset="utf-8"/>
<meta name="viewport" content="width=device-width,initial-scale=1"/>
<title>Struk - Brew n Bites</title>
<style>
  * { box-sizing: border-box; margin: 0; padding: 0; }
  body {
    font-family: 'Courier New', monospace;
    background: #f5f0eb;
    display: flex;
    justify-content: center;
    padding: 24px 12px;
  }
  .receipt {
    background: #fff;
    width: 100%;
    max-width: 380px;
    border-radius: 12px;
    box-shadow: 0 4px 24px rgba(0,0,0,.12);
    overflow: hidden;
  }
  .header {
    background: #4A6741;
    color: white;
    text-align: center;
    padding: 20px 16px 16px;
  }
  .header h1 {
    font-size: 22px;
    letter-spacing: 3px;
    font-family: Georgia, serif;
  }
  .header .tagline {
    font-size: 11px;
    opacity: .8;
    margin-top: 2px;
    letter-spacing: 1px;
  }
  .header .datetime {
    font-size: 12px;
    margin-top: 8px;
    opacity: .9;
  }
  .body { padding: 16px; }
  .info-block {
    background: #faf8f5;
    border-radius: 8px;
    padding: 10px 12px;
    margin-bottom: 14px;
    font-size: 13px;
    line-height: 1.8;
    color: #555;
  }
  .info-block .val { color: #2c2c2c; font-weight: bold; }
  .sep {
    border: none;
    border-top: 1px dashed #ccc;
    margin: 12px 0;
  }
  table { width: 100%; border-collapse: collapse; font-size: 13px; }
  td { padding: 5px 2px; vertical-align: top; color: #333; }
  .qty  { text-align: center; width: 36px; color: #888; }
  .price { text-align: right; white-space: nowrap; }
  .bold td { font-weight: bold; color: #1a1a1a; }
  .total-row td { font-size: 15px; font-weight: bold; }
  .green { color: #4A6741 !important; }
  .sep-row td { padding: 6px 0; }
  .badge {
    display: inline-block;
    background: %1;
    color: white;
    border-radius: 20px;
    padding: 3px 14px;
    font-size: 12px;
    font-weight: bold;
    margin-top: 4px;
  }
  .footer {
    text-align: center;
    padding: 16px;
    background: #faf8f5;
    font-size: 12px;
    color: #888;
    border-top: 1px solid #eee;
    line-height: 1.7;
  }
  .footer .thanks {
    font-size: 14px;
    color: #4A6741;
    font-weight: bold;
    font-family: Georgia, serif;
  }
</style>
</head>
<body>
<div class="receipt">

  <div class="header">
    <h1>BREW N BITES</h1>
    <div class="tagline">COFFEE &amp; BITES</div>
    <div class="datetime">%2</div>
  </div>

  <div class="body">
    <div class="info-block">
      Pelanggan &nbsp;: <span class="val">%3</span><br/>
      Meja &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;: <span class="val">%4</span><br/>
      Metode &nbsp;&nbsp;&nbsp;&nbsp;: <span class="badge">%5</span>
    </div>

    <table>
      <tr style="color:#999;font-size:11px;border-bottom:1px solid #eee">
        <td>ITEM</td><td class="qty">QTY</td><td class="price">HARGA</td>
      </tr>
      <tr><td colspan="3" style="padding:4px 0"></td></tr>
      %6
      <tr class="sep-row"><td colspan="3"><hr class="sep"/></td></tr>
      <tr><td colspan="2">Subtotal</td><td class="price">%7</td></tr>
      <tr><td colspan="2">PB1 (10%%)</td><td class="price">%8</td></tr>
      <tr class="sep-row"><td colspan="3"><hr class="sep"/></td></tr>
      <tr class="total-row bold">
        <td colspan="2">TOTAL</td>
        <td class="price">%9</td>
      </tr>
      %10
    </table>
  </div>

  <div class="footer">
    <div class="thanks">Terima kasih!</div>
    Selamat menikmati hidangan Anda 🙏
  </div>

</div>
</body>
</html>)html")
        .arg(isTunai ? "#4A6741" : "#2563EB") // %1  warna badge metode
        .arg(m_datetime)                        // %2
        .arg(m_customerName.toHtmlEscaped())   // %3
        .arg(m_tableNumber.toHtmlEscaped())    // %4
        .arg(m_paymentMethod.toHtmlEscaped())  // %5
        .arg(itemRows)                          // %6
        .arg(fmtRp(m_subtotal))                // %7
        .arg(fmtRp(m_tax))                     // %8
        .arg(fmtRp(m_total))                   // %9
        .arg(cashRows);                         // %10
}

// -----------------------------------------------------------------------------
//  Helpers
// -----------------------------------------------------------------------------
QString StrukHttpServer::fmtRp(long long val)
{
    QString p = QString::number(val);
    int pos   = p.length() - 3;
    while (pos > 0) { p.insert(pos, '.'); pos -= 3; }
    return "Rp " + p;
}

QString StrukHttpServer::getLocalIp()
{
    // Cari IP WiFi/LAN (bukan loopback)
    for (const QNetworkInterface &iface : QNetworkInterface::allInterfaces()) {
        if (iface.flags() & QNetworkInterface::IsLoopBack) continue;
        if (!(iface.flags() & QNetworkInterface::IsUp))    continue;
        if (!(iface.flags() & QNetworkInterface::IsRunning)) continue;

        for (const QNetworkAddressEntry &entry : iface.addressEntries()) {
            QHostAddress addr = entry.ip();
            if (addr.protocol() == QAbstractSocket::IPv4Protocol
                && addr != QHostAddress::LocalHost) {
                return addr.toString();
            }
        }
    }
    return "127.0.0.1"; // fallback
}