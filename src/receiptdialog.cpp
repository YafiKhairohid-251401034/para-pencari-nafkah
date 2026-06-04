#include "receiptdialog.h"
#include <QDateTime>
#include <QHBoxLayout>
#include <QFont>

static QString fmtRp(long long val) {
    QString p = QString::number(val);
    int pos = p.length() - 3;
    while (pos > 0) { p.insert(pos, '.'); pos -= 3; }
    return "Rp " + p;
}

ReceiptDialog::ReceiptDialog(OrderManager *mgr, long long cashPaid, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle("Struk Pembayaran");
    setFixedWidth(300);

    QVBoxLayout *lay = new QVBoxLayout(this);
    lay->setSpacing(4);
    lay->setContentsMargins(16, 16, 16, 16);
    setStyleSheet("background-color: white; color: black;");

    auto addLabel = [&](const QString &text, bool bold = false, Qt::Alignment align = Qt::AlignCenter) {
        QLabel *lbl = new QLabel(text);
        lbl->setAlignment(align);
        if (bold) {
            QFont f = lbl->font();
            f.setBold(true);
            lbl->setFont(f);
        }
        lay->addWidget(lbl);
    };

    auto addSep = [&]() {
        QLabel *sep = new QLabel("- - - - - - - - - - - - - - - - - - -");
        sep->setAlignment(Qt::AlignCenter);
        lay->addWidget(sep);
    };

    auto addRow = [&](const QString &label, const QString &value, bool bold = false) {
        QHBoxLayout *rowLay = new QHBoxLayout();
        QLabel *lbl = new QLabel(label);
        QLabel *val = new QLabel(value);
        val->setAlignment(Qt::AlignRight);
        if (bold) {
            QFont f; f.setBold(true);
            lbl->setFont(f);
            val->setFont(f);
        }
        rowLay->addWidget(lbl);
        rowLay->addStretch();
        rowLay->addWidget(val);
        lay->addLayout(rowLay);
    };

    // Header
    addLabel("BREW N BITES", true);
    addLabel(QDateTime::currentDateTime().toString("dd/MM/yyyy   hh:mm"));
    addSep();

    // Info pelanggan
    addLabel("Pelanggan : " + mgr->customerName(), false, Qt::AlignLeft);
    addLabel("Meja      : " + mgr->tableNumber(),  false, Qt::AlignLeft);
    addLabel("Metode    : Tunai",                   false, Qt::AlignLeft);
    addSep();

    // Item pesanan
    for (const OrderLine &line : mgr->orderLines()) {
        QHBoxLayout *rowLay = new QHBoxLayout();
        QLabel *nameLabel = new QLabel(
            QString("%1 x%2").arg(line.item.name).arg(line.quantity)
        );
        QLabel *priceLabel = new QLabel(fmtRp(line.subtotal()));
        priceLabel->setAlignment(Qt::AlignRight);
        rowLay->addWidget(nameLabel);
        rowLay->addStretch();
        rowLay->addWidget(priceLabel);
        lay->addLayout(rowLay);
    }
    addSep();

    // Subtotal, pajak, total
    addRow("Subtotal",  mgr->formattedSubtotal());
    addRow("PB1 (10%)", mgr->formattedTax());
    addRow("TOTAL",     mgr->formattedTotal(), true);
    addSep();

    // Info cash
    long long change = cashPaid - static_cast<long long>(mgr->total());
    addRow("Bayar",     fmtRp(cashPaid));
    addRow("Kembalian", fmtRp(change), true);
    addSep();

    addLabel("Terima kasih atas kunjungan Anda!");
    addLabel("Selamat menikmati!");

    lay->addSpacing(8);
    QPushButton *btnClose = new QPushButton("Tutup");
    connect(btnClose, &QPushButton::clicked, this, &QDialog::accept);
    lay->addWidget(btnClose);
}