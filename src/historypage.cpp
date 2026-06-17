#include "historypage.h"
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QListWidgetItem>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include "theme.h"

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

HistoryPage::HistoryPage(HistoryManager *historyMgr, QWidget *parent)
    : QWidget(parent)
    , m_historyMgr(historyMgr)
    , m_detailContent(nullptr)
{
    setupUi();
    setupStyle();
}

void HistoryPage::refreshPage()
{
    // Tampilkan "Hari Ini" sebagai default setiap kali halaman dibuka
    m_dateFilterEdit->setDate(QDate::currentDate());
    m_filterActive = true;
    populateList();
    clearDetail();
}

void HistoryPage::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    //  Sidebar
    buildSidebar();
    mainLayout->addWidget(m_sidebar);

    // Center: list of transactions
    QWidget *centerWidget = new QWidget();
    centerWidget->setObjectName("histMain");
    QVBoxLayout *centerLay = new QVBoxLayout(centerWidget);
    centerLay->setContentsMargins(28, 24, 28, 24);
    centerLay->setSpacing(12);

    QLabel *titleLabel = new QLabel("Riwayat Transaksi");
    titleLabel->setObjectName("histTitle");
    centerLay->addWidget(titleLabel);

    QLabel *subtitleLabel = new QLabel("Daftar transaksi yang telah selesai, urut dari yang terbaru.");
    subtitleLabel->setObjectName("histSubtitle");
    centerLay->addWidget(subtitleLabel);

    centerLay->addSpacing(12);

    // Filter tanggal
    centerLay->addLayout(buildFilterBar());

    centerLay->addSpacing(8);

    m_summaryLabel = new QLabel();
    m_summaryLabel->setObjectName("histSummary");
    centerLay->addWidget(m_summaryLabel);

    centerLay->addSpacing(8);

    m_listWidget = new QListWidget();
    m_listWidget->setObjectName("histList");
    m_listWidget->setSpacing(8);
    m_listWidget->setFrameShape(QFrame::NoFrame);
    connect(m_listWidget, &QListWidget::currentRowChanged, this, &HistoryPage::onTransactionSelected);
    centerLay->addWidget(m_listWidget, 1);

    m_emptyLabel = new QLabel("Belum ada riwayat transaksi.\nTransaksi yang berhasil akan muncul di sini.");
    m_emptyLabel->setObjectName("histEmpty");
    m_emptyLabel->setAlignment(Qt::AlignCenter);
    m_emptyLabel->setWordWrap(true);
    m_emptyLabel->hide();
    centerLay->addWidget(m_emptyLabel, 1);

    mainLayout->addWidget(centerWidget, 1);

    //  Detail panel (mirip struk)
    m_detailPanel = new QFrame();
    m_detailPanel->setObjectName("histDetailPanel");
    m_detailPanel->setFixedWidth(300);

    m_detailLay = new QVBoxLayout(m_detailPanel);
    m_detailLay->setContentsMargins(20, 24, 20, 20);
    m_detailLay->setSpacing(0);

    mainLayout->addWidget(m_detailPanel);

    clearDetail();
}

void HistoryPage::buildSidebar()
{
    m_sidebar = new QFrame();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(Theme::SIDEBAR_WIDTH);

    QVBoxLayout *lay = new QVBoxLayout(m_sidebar);
    lay->setContentsMargins(0, 20, 0, 20);
    lay->setSpacing(0);
    lay->setAlignment(Qt::AlignTop);

    // Logo
    QLabel *logoLabel = new QLabel();
    QPixmap logoPix(":/images/logobrew.png");
    logoLabel->setPixmap(logoPix.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    logoLabel->setAlignment(Qt::AlignCenter);
    lay->addWidget(logoLabel);
    lay->addSpacing(6);

    auto makeNavBtn = [](const QString &text) {
        QPushButton *btn = new QPushButton(text);
        btn->setObjectName("navBtn");
        btn->setFixedHeight(36);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    m_navRegister = makeNavBtn("Register");
    m_navOrder = makeNavBtn("Order");
    m_navPayment = makeNavBtn("Payment");
    m_navHistory = makeNavBtn("History");
    m_navHistory->setObjectName("navBtnActive");

    lay->addWidget(m_navRegister);
    lay->addSpacing(4);
    lay->addWidget(m_navOrder);
    lay->addSpacing(4);
    lay->addWidget(m_navPayment);
    lay->addSpacing(4);
    lay->addWidget(m_navHistory);
    lay->addStretch();

    connect(m_navRegister, &QPushButton::clicked, this, &HistoryPage::navigateToRegister);
    connect(m_navOrder, &QPushButton::clicked, this, &HistoryPage::navigateToOrder);
    connect(m_navPayment, &QPushButton::clicked, this, &HistoryPage::navigateToPayment);
}

QHBoxLayout *HistoryPage::buildFilterBar()
{
    QHBoxLayout *filterLay = new QHBoxLayout();
    filterLay->setSpacing(8);

    QLabel *filterLbl = new QLabel("Tampilkan tanggal:");
    filterLbl->setObjectName("histFilterLbl");

    // Pilih tanggal — default ke hari ini
    m_dateFilterEdit = new QDateEdit(QDate::currentDate());
    m_dateFilterEdit->setObjectName("histDateEdit");
    m_dateFilterEdit->setCalendarPopup(true);
    m_dateFilterEdit->setDisplayFormat("dd/MM/yyyy");
    m_dateFilterEdit->setFixedHeight(32);
    connect(m_dateFilterEdit, &QDateEdit::dateChanged, this, &HistoryPage::onFilterDateChanged);

    m_btnToday = new QPushButton("Hari Ini");
    m_btnToday->setObjectName("histFilterBtn");
    m_btnToday->setFixedHeight(32);
    m_btnToday->setCursor(Qt::PointingHandCursor);
    connect(m_btnToday, &QPushButton::clicked, this, &HistoryPage::onShowToday);

    m_btnAllDates = new QPushButton("Semua Tanggal");
    m_btnAllDates->setObjectName("histFilterBtn");
    m_btnAllDates->setFixedHeight(32);
    m_btnAllDates->setCursor(Qt::PointingHandCursor);
    connect(m_btnAllDates, &QPushButton::clicked, this, &HistoryPage::onShowAll);

    filterLay->addWidget(filterLbl);
    filterLay->addWidget(m_dateFilterEdit);
    filterLay->addWidget(m_btnToday);
    filterLay->addWidget(m_btnAllDates);
    filterLay->addStretch();

    return filterLay;
}

void HistoryPage::onFilterDateChanged(const QDate &date)
{
    Q_UNUSED(date);
    m_filterActive = true; // tanggal spesifik dipilih dari kalender
    applyDateFilter();
}

void HistoryPage::onShowToday()
{
    m_dateFilterEdit->blockSignals(true);
    m_dateFilterEdit->setDate(QDate::currentDate());
    m_dateFilterEdit->blockSignals(false);
    m_filterActive = true;
    applyDateFilter();
}

void HistoryPage::onShowAll()
{
    m_filterActive = false; // matikan filter — tampilkan semua riwayat
    applyDateFilter();
}

void HistoryPage::applyDateFilter()
{
    if (!m_filterActive) {
        m_records = m_allRecords;
    } else {
        QDate target = m_dateFilterEdit->date();
        m_records.clear();
        for (const TransactionRecord &rec : m_allRecords) {
            if (rec.parsedDate() == target)
                m_records.append(rec);
        }
    }

    updateSummaryBar();

    m_listWidget->clear();
    if (m_records.isEmpty()) {
        m_listWidget->hide();
        m_emptyLabel->show();
        clearDetail();
        return;
    }

    m_emptyLabel->hide();
    m_listWidget->show();

    for (const TransactionRecord &rec : m_records) {
        QListWidgetItem *listItem = new QListWidgetItem();
        listItem->setSizeHint(QSize(0, 64));

        QWidget *card = new QWidget();
        card->setObjectName("histCard");
        QHBoxLayout *cardLay = new QHBoxLayout(card);
        cardLay->setContentsMargins(14, 8, 14, 8);

        QVBoxLayout *leftCol = new QVBoxLayout();
        leftCol->setSpacing(2);
        QLabel *nameLbl = new QLabel(rec.customerName.isEmpty() ? "—" : rec.customerName);
        nameLbl->setObjectName("histCardName");
        QLabel *dateLbl = new QLabel(QString("%1  ·  Meja %2").arg(rec.formattedDateTime(), rec.tableNumber));
        dateLbl->setObjectName("histCardSub");
        leftCol->addWidget(nameLbl);
        leftCol->addWidget(dateLbl);

        QVBoxLayout *rightCol = new QVBoxLayout();
        rightCol->setSpacing(2);
        rightCol->setAlignment(Qt::AlignRight);
        QLabel *totalLbl = new QLabel(rec.formattedTotal());
        totalLbl->setObjectName("histCardTotal");
        totalLbl->setAlignment(Qt::AlignRight);
        QLabel *methodLbl = new QLabel(rec.paymentMethod);
        methodLbl->setObjectName("histCardMethod");
        methodLbl->setAlignment(Qt::AlignRight);
        rightCol->addWidget(totalLbl);
        rightCol->addWidget(methodLbl);

        cardLay->addLayout(leftCol);
        cardLay->addStretch();
        cardLay->addLayout(rightCol);

        m_listWidget->addItem(listItem);
        m_listWidget->setItemWidget(listItem, card);
    }
}

void HistoryPage::updateSummaryBar()
{
    long long totalOmzet = 0;
    for (const TransactionRecord &rec : m_records)
        totalOmzet += rec.total;

    QString periodText = m_filterActive
                             ? QString("tanggal %1").arg(m_dateFilterEdit->date().toString("dd/MM/yyyy"))
                             : "semua tanggal";

    m_summaryLabel->setText(QString("%1 transaksi  ·  Total penjualan %2  (%3)")
                                .arg(m_records.size())
                                .arg(formatRp(totalOmzet))
                                .arg(periodText));
}

void HistoryPage::populateList()
{
    m_allRecords = m_historyMgr->allTransactions();
    applyDateFilter();
}

void HistoryPage::onTransactionSelected(int row)
{
    if (row < 0 || row >= m_records.size()) {
        clearDetail();
        return;
    }
    showDetail(m_records[row]);
}

void HistoryPage::clearDetail()
{
    if (m_detailContent) {
        m_detailLay->removeWidget(m_detailContent);
        m_detailContent->deleteLater();
        m_detailContent = nullptr;
    }

    QLabel *placeholder = new QLabel("Pilih salah satu transaksi\nuntuk melihat detail struk.");
    placeholder->setObjectName("histDetailPlaceholder");
    placeholder->setAlignment(Qt::AlignCenter);
    placeholder->setWordWrap(true);

    m_detailContent = new QWidget();
    QVBoxLayout *wrapLay = new QVBoxLayout(m_detailContent);
    wrapLay->addStretch();
    wrapLay->addWidget(placeholder);
    wrapLay->addStretch();

    m_detailLay->addWidget(m_detailContent);
}

void HistoryPage::showDetail(const TransactionRecord &rec)
{
    if (m_detailContent) {
        m_detailLay->removeWidget(m_detailContent);
        m_detailContent->deleteLater();
        m_detailContent = nullptr;
    }

    m_detailContent = new QWidget();
    QVBoxLayout *lay = new QVBoxLayout(m_detailContent);
    lay->setContentsMargins(0, 0, 0, 0);
    lay->setSpacing(0);

    QLabel *title = new QLabel("Detail Transaksi");
    title->setObjectName("histDetailTitle");
    lay->addWidget(title);
    lay->addSpacing(4);

    QLabel *dateLbl = new QLabel(rec.formattedDateTime());
    dateLbl->setObjectName("histDetailMeta");
    lay->addWidget(dateLbl);

    QLabel *custLbl = new QLabel(QString("%1  ·  Meja %2")
                                     .arg(rec.customerName.isEmpty() ? "—" : rec.customerName,
                                          rec.tableNumber));
    custLbl->setObjectName("histDetailMeta");
    lay->addWidget(custLbl);

    lay->addSpacing(14);

    QFrame *div1 = new QFrame();
    div1->setObjectName("histDivider");
    div1->setFixedHeight(1);
    lay->addWidget(div1);
    lay->addSpacing(10);

    // Item list (scrollable jika banyak)
    QScrollArea *scroll = new QScrollArea();
    scroll->setObjectName("histItemScroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);
    scroll->setMaximumHeight(180);

    QWidget *itemsWidget = new QWidget();
    QVBoxLayout *itemsLay = new QVBoxLayout(itemsWidget);
    itemsLay->setContentsMargins(0, 0, 0, 0);
    itemsLay->setSpacing(6);

    for (const ReceiptItem &item : rec.items) {
        QHBoxLayout *rowLay = new QHBoxLayout();
        QLabel *nameLbl = new QLabel(QString("%1 x%2").arg(item.name).arg(item.quantity));
        nameLbl->setObjectName("histItemName");
        QLabel *priceLbl = new QLabel(formatRp(item.subtotal));
        priceLbl->setObjectName("histItemPrice");
        priceLbl->setAlignment(Qt::AlignRight);
        rowLay->addWidget(nameLbl);
        rowLay->addStretch();
        rowLay->addWidget(priceLbl);
        itemsLay->addLayout(rowLay);
    }
    itemsLay->addStretch();

    scroll->setWidget(itemsWidget);
    lay->addWidget(scroll);

    lay->addSpacing(10);
    QFrame *div2 = new QFrame();
    div2->setObjectName("histDivider");
    div2->setFixedHeight(1);
    lay->addWidget(div2);
    lay->addSpacing(10);

    auto addRow = [&](const QString &label, const QString &value, bool bold = false) {
        QHBoxLayout *rowLay = new QHBoxLayout();
        QLabel *lbl = new QLabel(label);
        lbl->setObjectName(bold ? "histRowLblBold" : "histRowLbl");
        QLabel *val = new QLabel(value);
        val->setObjectName(bold ? "histRowValBold" : "histRowVal");
        val->setAlignment(Qt::AlignRight);
        rowLay->addWidget(lbl);
        rowLay->addStretch();
        rowLay->addWidget(val);
        lay->addLayout(rowLay);
        lay->addSpacing(6);
    };

    addRow("Subtotal", formatRp(rec.subtotal));
    addRow("PB1 (10%)", formatRp(rec.tax));
    addRow("Total", rec.formattedTotal(), true);

    lay->addSpacing(4);
    QFrame *div3 = new QFrame();
    div3->setObjectName("histDivider");
    div3->setFixedHeight(1);
    lay->addWidget(div3);
    lay->addSpacing(10);

    addRow("Metode", rec.paymentMethod);
    if (rec.paymentMethod == "Tunai") {
        addRow("Tunai Diterima", formatRp(rec.cashPaid));
        addRow("Kembalian", formatRp(rec.change), true);
    }

    lay->addStretch();

    m_detailLay->addWidget(m_detailContent);
}

void HistoryPage::setupStyle()
{
    setStyleSheet(QString(R"(
        HistoryPage, #histMain {
            background-color: %1;
        }
        #sidebar {
            background-color: %2;
            border-right: 1px solid %3;
        }
        #navBtn {
            background: transparent;
            border: none;
            border-radius: 6px;
            font-family: '%4';
            font-size: 12px;
            color: %5;
            text-align: center;
            padding: 6px 8px;
            margin: 0 10px;
        }
        #navBtn:hover { background: %6; color: %7; }
        #navBtnActive {
            background: %6;
            border: none;
            border-radius: 6px;
            font-family: '%4';
            font-size: 12px;
            font-weight: bold;
            color: %7;
            text-align: center;
            padding: 6px 8px;
            margin: 0 10px;
        }
        #histTitle {
            font-family: '%8';
            font-size: 22px;
            font-weight: bold;
            color: %9;
        }
        #histSubtitle {
            font-family: '%4';
            font-size: 12px;
            color: %5;
        }
        #histFilterLbl {
            font-family: '%4';
            font-size: 12px;
            color: %5;
        }
        #histDateEdit {
            background-color: %10;
            border: 1.5px solid %3;
            border-radius: 6px;
            padding: 0 8px;
            font-family: '%11';
            font-size: 12px;
            color: #000000;
        }
        #histDateEdit QAbstractItemView {
            color: #000000;
        }
        #histDateEdit QCalendarWidget QWidget {
            color: #000000;
        }
        #histFilterBtn {
            background: %10;
            border: 1.5px solid %3;
            border-radius: 6px;
            font-family: '%4';
            font-size: 12px;
            color: %5;
            padding: 0 14px;
        }
        #histFilterBtn:hover {
            background: %6;
            border-color: %7;
            color: %7;
        }
        #histSummary {
            font-family: '%11';
            font-size: 12px;
            font-weight: bold;
            color: %7;
            background: %6;
            border-radius: 8px;
            padding: 8px 14px;
        }
        #histList {
            background: transparent;
            border: none;
        }
        #histList::item {
            background: transparent;
            border: none;
        }
        #histList::item:selected {
            background: transparent;
        }
        #histCard {
            background-color: %10;
            border: 1px solid %3;
            border-radius: 10px;
        }
        #histCardName {
            font-family: '%4';
            font-size: 13px;
            font-weight: bold;
            color: %9;
        }
        #histCardSub {
            font-family: '%4';
            font-size: 11px;
            color: %5;
        }
        #histCardTotal {
            font-family: '%11';
            font-size: 14px;
            font-weight: bold;
            color: %7;
        }
        #histCardMethod {
            font-family: '%4';
            font-size: 11px;
            color: %5;
            background: %6;
            border-radius: 8px;
            padding: 1px 8px;
        }
        #histEmpty {
            font-family: '%4';
            font-size: 13px;
            color: %5;
        }
        #histDetailPanel {
            background-color: %2;
            border-left: 1px solid %3;
        }
        #histDetailPlaceholder {
            font-family: '%4';
            font-size: 12px;
            color: %5;
        }
        #histDetailTitle {
            font-family: '%8';
            font-size: 16px;
            font-weight: bold;
            color: %9;
        }
        #histDetailMeta {
            font-family: '%4';
            font-size: 11px;
            color: %5;
        }
        #histDivider { background: %3; }
        #histItemScroll { background: transparent; border: none; }
        #histItemName {
            font-family: '%4';
            font-size: 11px;
            color: %9;
        }
        #histItemPrice {
            font-family: '%11';
            font-size: 11px;
            color: %9;
        }
        #histRowLbl {
            font-family: '%4';
            font-size: 12px;
            color: %5;
        }
        #histRowVal {
            font-family: '%11';
            font-size: 12px;
            color: %9;
        }
        #histRowLblBold {
            font-family: '%4';
            font-size: 13px;
            font-weight: bold;
            color: %9;
        }
        #histRowValBold {
            font-family: '%11';
            font-size: 13px;
            font-weight: bold;
            color: %7;
        }
    )
    )")
                      .arg(Theme::BG_APP)         // %1
                      .arg(Theme::BG_SIDEBAR)     // %2
                      .arg(Theme::BORDER_LIGHT)   // %3
                      .arg(Theme::FONT_BODY)      // %4
                      .arg(Theme::TEXT_SECONDARY) // %5
                      .arg(Theme::ACCENT_LIGHT)   // %6
                      .arg(Theme::ACCENT_PRIMARY) // %7
                      .arg(Theme::FONT_DISPLAY)   // %8
                      .arg(Theme::TEXT_PRIMARY)   // %9
                      .arg(Theme::BG_CARD)        // %10
                      .arg(Theme::FONT_MONO)      // %11
                  );
}