#include "paymentpage.h"
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStringList>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include "theme.h"
#include "receiptdialog.h"

PaymentPage::PaymentPage(OrderManager *mgr, QWidget *parent)
    : QWidget(parent)
    , m_mgr(mgr)
    , m_activeMethod("Tunai")
    , m_numpadFrame(nullptr)
    , m_cashEnteredLabel(nullptr)
    , m_sumTotalValue(nullptr)
    , m_sumCashValue(nullptr)
    , m_sumChangeValue(nullptr)
    , m_changeLabel(nullptr)
    , m_completeBtn(nullptr)
    , m_thankYouLabel(nullptr)
{
    setupUi();
    setupStyle();
}

void PaymentPage::refreshPage()
{
    m_cashString.clear();
    updateCashDisplay();

    m_activeMethod = "Tunai";
    m_numpadWidget->setVisible(true);
    m_qrisImageLabel->setVisible(false);
    m_sumCashRow->setVisible(true);
    m_sumChangeRow->setVisible(true);
    for (QPushButton *b : m_methodBtns) {
        b->setObjectName(b->text() == "Tunai" ? "methodBtnActive" : "methodBtn");
        b->style()->unpolish(b);
        b->style()->polish(b);
    }

    m_totalDueValue->setText(m_mgr->formattedTotal());
    m_itemsLabel->setText(QString("%1 item").arg(m_mgr->itemCount()));
    m_nameLabel->setText(m_mgr->customerName());
    m_tableLabel->setText(QString("Meja %1").arg(m_mgr->tableNumber()));
    m_sumTotalValue->setText(m_mgr->formattedTotal());
    m_sumCashValue->setText("Rp 0");
    m_sumChangeValue->setText("Rp 0");
    if (m_thankYouLabel)
        m_thankYouLabel->hide();
    if (m_completeBtn) {
        m_completeBtn->setEnabled(true);
        m_completeBtn->setText("Complete Transaction  ✓");
    }
}

void PaymentPage::setupUi()
{
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // ── Sidebar ──────────────────────────────────────────────────────────────
    buildSidebar();
    mainLayout->addWidget(m_sidebar);

    // ── Center (topbar + numpad) ──────────────────────────────────────────
    QWidget *centerWidget = new QWidget();
    centerWidget->setObjectName("payMain");
    QVBoxLayout *centerLay = new QVBoxLayout(centerWidget);
    centerLay->setContentsMargins(0, 0, 0, 0);
    centerLay->setSpacing(0);

    // Top bar
    QFrame *topBar = new QFrame();
    topBar->setObjectName("payTopBar");
    topBar->setFixedHeight(72);
    QHBoxLayout *topLay = new QHBoxLayout(topBar);
    topLay->setContentsMargins(28, 0, 28, 0);

    QVBoxLayout *totalDueLay = new QVBoxLayout();
    QLabel *totalDueStatic = new QLabel("TOTAL DUE");
    totalDueStatic->setObjectName("totalDueStaticLabel");
    m_totalDueValue = new QLabel("Rp 0");
    m_totalDueValue->setObjectName("totalDueBig");
    totalDueLay->setSpacing(0);
    totalDueLay->addWidget(totalDueStatic);
    totalDueLay->addWidget(m_totalDueValue);

    m_itemsLabel = new QLabel("0 item");
    m_itemsLabel->setObjectName("payBadge");
    m_nameLabel = new QLabel("—");
    m_nameLabel->setObjectName("payBadge");
    m_tableLabel = new QLabel("Meja —");
    m_tableLabel->setObjectName("payBadge");

    topLay->addLayout(totalDueLay);
    topLay->addStretch();
    topLay->addWidget(m_itemsLabel);
    topLay->addSpacing(8);
    topLay->addWidget(m_nameLabel);
    topLay->addSpacing(8);
    topLay->addWidget(m_tableLabel);

    centerLay->addWidget(topBar);

    // Numpad area
    QWidget *numpadArea = new QWidget();
    numpadArea->setObjectName("numpadArea");
    QVBoxLayout *numpadAreaLay = new QVBoxLayout(numpadArea);
    numpadAreaLay->setContentsMargins(32, 24, 32, 24);
    numpadAreaLay->setSpacing(12);

    // Payment method tabs
    QHBoxLayout *tabsLay = new QHBoxLayout();
    tabsLay->setSpacing(8);
    QStringList methods = {"Tunai", "QRIS"};
    for (const QString &method : methods) {
        QPushButton *btn = new QPushButton(method);
        btn->setObjectName(method == "Tunai" ? "methodBtnActive" : "methodBtn");
        btn->setFixedHeight(34);
        btn->setMinimumWidth(80);
        btn->setCursor(Qt::PointingHandCursor);
        m_methodBtns.append(btn);
        tabsLay->addWidget(btn);
        connect(btn, &QPushButton::clicked, [this, method]() {
            m_activeMethod = method;
            m_cashString.clear();
            updateCashDisplay();
            for (QPushButton *b : m_methodBtns) {
                b->setObjectName(b->text() == method ? "methodBtnActive" : "methodBtn");
                b->style()->unpolish(b);
                b->style()->polish(b);
            }
            m_numpadWidget->setVisible(method == "Tunai");
            m_qrisImageLabel->setVisible(method == "QRIS");
            m_sumCashRow->setVisible(method == "Tunai");
            m_sumChangeRow->setVisible(method == "Tunai");
        });
    }
    tabsLay->addStretch();

    // Widget pembungkus numpad
    m_numpadWidget = new QWidget();
    QVBoxLayout *numpadInnerLay = new QVBoxLayout(m_numpadWidget);
    numpadInnerLay->setContentsMargins(0, 0, 0, 0);
    numpadInnerLay->setSpacing(12);

    // Widget gambar QRIS
    m_qrisImageLabel = new QLabel();
    QPixmap qrisPixmap(":/images/qr_brewnbites.png");
    if (qrisPixmap.isNull()) {
        m_qrisImageLabel->setText("❌ Gambar tidak ditemukan");
        m_qrisImageLabel->setStyleSheet("color: red; font-size: 14px;");
    } else {
        m_qrisImageLabel->setPixmap(
            qrisPixmap.scaled(280, 280, Qt::KeepAspectRatio, Qt::SmoothTransformation)
            );
    }
    m_qrisImageLabel->setAlignment(Qt::AlignCenter);
    m_qrisImageLabel->hide();

    // Cash label
    QLabel *cashSectionLabel = new QLabel("Tunai");
    cashSectionLabel->setObjectName("cashSectionLabel");

    m_cashEnteredLabel = new QLabel("Rp 0");
    m_cashEnteredLabel->setObjectName("cashDisplay");

    // Numpad grid
    QWidget *gridHost = new QWidget();
    QGridLayout *gridLay = new QGridLayout(gridHost);
    gridLay->setSpacing(10);
    gridLay->setContentsMargins(0, 0, 0, 0);

    struct KeyDef
    {
        QString label;
        int r;
        int c;
    };
    QVector<KeyDef> keys = {
        {"1", 0, 0},
        {"2", 0, 1},
        {"3", 0, 2},
        {"4", 1, 0},
        {"5", 1, 1},
        {"6", 1, 2},
        {"7", 2, 0},
        {"8", 2, 1},
        {"9", 2, 2},
        {"00", 3, 0},
        {"0", 3, 1},
        {"⌫", 3, 2},
    };
    for (const KeyDef &k : keys) {
        QPushButton *btn = new QPushButton(k.label);
        btn->setObjectName(k.label == "⌫" ? "numBtnBack" : "numBtn");
        btn->setMinimumSize(90, 76);
        btn->setCursor(Qt::PointingHandCursor);
        gridLay->addWidget(btn, k.r, k.c);
        QString val = k.label;
        connect(btn, &QPushButton::clicked, [this, val]() { onNumpadPressed(val); });
    }

    numpadInnerLay->addWidget(cashSectionLabel);
    numpadInnerLay->addWidget(m_cashEnteredLabel);
    numpadInnerLay->addWidget(gridHost);

    numpadAreaLay->addLayout(tabsLay);
    numpadAreaLay->addWidget(m_numpadWidget);
    numpadAreaLay->addWidget(m_qrisImageLabel);
    numpadAreaLay->addStretch();

    centerLay->addWidget(numpadArea, 1);
    mainLayout->addWidget(centerWidget, 1);

    // ── Summary panel ────────────────────────────────────────────────────────
    QFrame *summaryPanel = new QFrame();
    summaryPanel->setObjectName("summaryPanel");
    summaryPanel->setFixedWidth(260);
    QVBoxLayout *sumLay = new QVBoxLayout(summaryPanel);
    sumLay->setContentsMargins(20, 28, 20, 20);
    sumLay->setSpacing(0);

    QLabel *sumTitle = new QLabel("Ringkasan");
    sumTitle->setObjectName("sumPanelTitle");
    sumLay->addWidget(sumTitle);
    sumLay->addSpacing(20);

    auto addRow = [&](const QString &text, QLabel *&valOut, const QString &valId) {
        QHBoxLayout *row = new QHBoxLayout();
        QLabel *lbl = new QLabel(text);
        lbl->setObjectName("sumRowLbl");
        valOut = new QLabel("Rp 0");
        valOut->setObjectName(valId);
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(valOut);
        sumLay->addLayout(row);
        sumLay->addSpacing(8);
    };

    addRow("Total", m_sumTotalValue, "sumRowVal");

    // Row Tunai — simpan sebagai widget agar bisa hide/show
    m_sumCashRow = new QWidget();
    QHBoxLayout *cashRowLay = new QHBoxLayout(m_sumCashRow);
    cashRowLay->setContentsMargins(0, 0, 0, 0);
    QLabel *cashLbl = new QLabel("Tunai");
    cashLbl->setObjectName("sumRowLbl");
    m_sumCashValue = new QLabel("Rp 0");
    m_sumCashValue->setObjectName("sumRowVal");
    cashRowLay->addWidget(cashLbl);
    cashRowLay->addStretch();
    cashRowLay->addWidget(m_sumCashValue);
    sumLay->addWidget(m_sumCashRow);
    sumLay->addSpacing(8);

    QFrame *div = new QFrame();
    div->setObjectName("divider");
    div->setFixedHeight(1);
    sumLay->addWidget(div);
    sumLay->addSpacing(8);

    // Kembalian row
    m_sumChangeRow = new QWidget();
    QHBoxLayout *changeRow = new QHBoxLayout(m_sumChangeRow);
    changeRow->setContentsMargins(0, 0, 0, 0);
    m_changeLabel = new QLabel("Kembalian");
    m_changeLabel->setObjectName("sumChangeLabel");
    m_sumChangeValue = new QLabel("Rp 0");
    m_sumChangeValue->setObjectName("sumChangeValue");
    changeRow->addWidget(m_changeLabel);
    changeRow->addStretch();
    changeRow->addWidget(m_sumChangeValue);
    sumLay->addWidget(m_sumChangeRow);
    sumLay->addSpacing(20);

    m_thankYouLabel = new QLabel("Terimakasih!");
    m_thankYouLabel->setObjectName("thankYou");
    m_thankYouLabel->setAlignment(Qt::AlignCenter);
    m_thankYouLabel->hide();
    sumLay->addWidget(m_thankYouLabel);

    sumLay->addStretch();

    m_completeBtn = new QPushButton("Complete Transaction ");
    m_completeBtn->setObjectName("completeBtn");
    m_completeBtn->setMinimumHeight(50);
    m_completeBtn->setCursor(Qt::PointingHandCursor);
    connect(m_completeBtn, &QPushButton::clicked, this, &PaymentPage::onCompleteTransaction);
    sumLay->addWidget(m_completeBtn);

    mainLayout->addWidget(summaryPanel);
}

void PaymentPage::buildSidebar()
{
    m_sidebar = new QFrame();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(Theme::SIDEBAR_WIDTH);

    QVBoxLayout *lay = new QVBoxLayout(m_sidebar);
    lay->setContentsMargins(0, 20, 0, 20);
    lay->setSpacing(0);
    lay->setAlignment(Qt::AlignTop);

    QLabel *brand = new QLabel("brew n bites");
    brand->setObjectName("sidebarBrand");
    brand->setAlignment(Qt::AlignCenter);
    brand->setWordWrap(true);

    lay->addWidget(brand);
    lay->addSpacing(28);

    auto makeNavBtn = [](const QString &text) -> QPushButton * {
        QPushButton *btn = new QPushButton(text);
        btn->setObjectName("navBtn");
        btn->setFixedHeight(36);
        btn->setCursor(Qt::PointingHandCursor);
        return btn;
    };

    m_navRegister = makeNavBtn("Register");
    m_navOrder = makeNavBtn("Order");
    m_navPayment = makeNavBtn("Payment");
    m_navPayment->setObjectName("navBtnActive");

    lay->addWidget(m_navRegister);
    lay->addSpacing(4);
    lay->addWidget(m_navOrder);
    lay->addSpacing(4);
    lay->addWidget(m_navPayment);
    lay->addStretch();

    connect(m_navRegister, &QPushButton::clicked, this, &PaymentPage::transactionComplete);
    connect(m_navOrder, &QPushButton::clicked, this, &PaymentPage::navigateToOrder);
}

void PaymentPage::buildNumpad() {}
void PaymentPage::buildSummaryPanel() {}

void PaymentPage::updateCashDisplay()
{
    if (!m_cashEnteredLabel)
        return;
    if (m_cashString.isEmpty()) {
        m_cashEnteredLabel->setText("Rp 0");
    } else {
        bool ok;
        long long val = m_cashString.toLongLong(&ok);
        if (ok) {
            QString p = QString::number(val);
            int pos = p.length() - 3;
            while (pos > 0) {
                p.insert(pos, '.');
                pos -= 3;
            }
            m_cashEnteredLabel->setText("Rp " + p);
        }
    }
    updateChange();
}

void PaymentPage::updateChange()
{
    if (!m_sumCashValue || !m_sumChangeValue)
        return;

    long long cash = m_cashString.isEmpty() ? 0LL : m_cashString.toLongLong();
    long long total = static_cast<long long>(m_mgr->total());
    long long change = cash - total;

    if (cash > 0) {
        QString p = QString::number(cash);
        int pos = p.length() - 3;
        while (pos > 0) {
            p.insert(pos, '.');
            pos -= 3;
        }
        m_sumCashValue->setText("Rp " + p);
    } else {
        m_sumCashValue->setText("Rp 0");
    }

    if (change >= 0) {
        QString p = QString::number(change);
        int pos = p.length() - 3;
        while (pos > 0) {
            p.insert(pos, '.');
            pos -= 3;
        }
        m_sumChangeValue->setText("Rp " + p);
        m_sumChangeValue->setStyleSheet(
            QString("color: %1; font-weight: bold; font-family: '%2'; font-size: 13px;")
                .arg(Theme::ACCENT_PRIMARY)
                .arg(Theme::FONT_MONO));
    } else {
        m_sumChangeValue->setText("Kurang");
        m_sumChangeValue->setStyleSheet(QString("color: %1; font-family: '%2'; font-size: 13px;")
                                            .arg(Theme::COLOR_ERROR)
                                            .arg(Theme::FONT_MONO));
    }
}

void PaymentPage::onNumpadPressed(const QString &val)
{
    if (val == "⌫") {
        if (!m_cashString.isEmpty())
            m_cashString.chop(1);
    } else if (val == "00") {
        if (!m_cashString.isEmpty())
            m_cashString += "00";
    } else {
        if (m_cashString.length() < 12)
            m_cashString += val;
    }
    updateCashDisplay();
}

void PaymentPage::onCompleteTransaction()
{
    long long cash = m_cashString.isEmpty() ? 0LL : m_cashString.toLongLong();
    long long total = static_cast<long long>(m_mgr->total());

    if (cash < total && m_activeMethod == "Tunai") {
        if (m_sumChangeValue)
            m_sumChangeValue->setText("Nominal kurang!");
        return;
    }

    if (m_activeMethod == "Tunai") {
        ReceiptDialog *receipt = new ReceiptDialog(m_mgr, cash, this);
        receipt->exec();
        receipt->deleteLater();
    }

    if (m_thankYouLabel)
        m_thankYouLabel->show();
    if (m_completeBtn) {
        m_completeBtn->setEnabled(false);
        m_completeBtn->setText("✓  Transaksi Selesai");
    }

    QTimer *t = new QTimer(this);
    t->setSingleShot(true);
    connect(t, &QTimer::timeout, [this, t]() {
        t->deleteLater();
        m_mgr->clearCart();
        emit transactionComplete();
    });
    t->start(1800);
}

void PaymentPage::setupStyle()
{
    setStyleSheet(QString(R"(
        PaymentPage, #payMain, #numpadArea {
            background-color: %1;
        }
        #sidebar {
            background-color: %2;
            border-right: 1px solid %3;
        }
        #sidebarIcon { font-size: 24px; }
        #sidebarBrand {
            font-family: '%4';
            font-size: 10px;
            color: %5;
            letter-spacing: 1px;
        }
        #navBtn {
            background: transparent;
            border: none;
            border-radius: 6px;
            font-family: '%6';
            font-size: 12px;
            color: %5;
            text-align: center;
            padding: 6px 8px;
            margin: 0 10px;
        }
        #navBtn:hover { background: %7; color: %8; }
        #navBtnActive {
            background: %7;
            border: none;
            border-radius: 6px;
            font-family: '%6';
            font-size: 12px;
            font-weight: bold;
            color: %8;
            text-align: center;
            padding: 6px 8px;
            margin: 0 10px;
        }
        #payTopBar {
            background: %2;
            border-bottom: 1px solid %3;
        }
        #totalDueStaticLabel {
            font-family: '%6';
            font-size: 10px;
            color: %5;
            letter-spacing: 1px;
        }
        #totalDueBig {
            font-family: '%4';
            font-size: 28px;
            font-weight: bold;
            color: %9;
        }
        #payBadge {
            font-family: '%6';
            font-size: 11px;
            color: %5;
            background: %7;
            border-radius: 12px;
            padding: 4px 12px;
        }
        #methodBtn {
            background: %10;
            border: 1.5px solid %3;
            border-radius: 8px;
            font-family: '%6';
            font-size: 12px;
            color: %5;
        }
        #methodBtn:hover { background: %11; border-color: %8; }
        #methodBtnActive {
            background: %8;
            border: 1.5px solid %8;
            border-radius: 8px;
            font-family: '%6';
            font-size: 12px;
            font-weight: bold;
            color: white;
        }
        #cashSectionLabel {
            font-family: '%6';
            font-size: 12px;
            color: %5;
            font-weight: 600;
        }
        #cashDisplay {
            font-family: '%4';
            font-size: 32px;
            font-weight: bold;
            color: %9;
        }
        #numBtn {
            background-color: %10;
            border: 1px solid %3;
            border-radius: 10px;
            font-family: '%12';
            font-size: 22px;
            color: %9;
        }
        #numBtn:hover { background-color: %11; border-color: %5; }
        #numBtn:pressed { background-color: %3; }
        #numBtnBack {
            background-color: %13;
            border: 1px solid %3;
            border-radius: 10px;
            font-size: 20px;
            color: %5;
        }
        #numBtnBack:hover { background-color: #E08070; color: white; }
        #summaryPanel {
            background-color: %2;
            border-left: 1px solid %3;
        }
        #sumPanelTitle {
            font-family: '%14';
            font-size: 16px;
            font-weight: bold;
            color: %9;
        }
        #sumRowLbl { font-family: '%6'; font-size: 12px; color: %5; }
        #sumRowVal { font-family: '%12'; font-size: 12px; color: %9; }
        #divider { background: %3; }
        #sumChangeLabel {
            font-family: '%6';
            font-size: 13px;
            font-weight: bold;
            color: %9;
        }
        #thankYou {
            font-family: '%4';
            font-size: 16px;
            color: %8;
            padding: 10px;
            background: %7;
            border-radius: 10px;
        }
        #completeBtn {
            background-color: %8;
            color: white;
            font-family: '%14';
            font-size: 13px;
            font-weight: bold;
            border: none;
            border-radius: %15px;
        }
        #completeBtn:hover { background-color: %16; }
        #completeBtn:disabled { background-color: %17; }
    )")
                      .arg(Theme::BG_APP)          // %1
                      .arg(Theme::BG_SIDEBAR)      // %2
                      .arg(Theme::BORDER_LIGHT)    // %3
                      .arg(Theme::FONT_DISPLAY)    // %4
                      .arg(Theme::TEXT_SECONDARY)  // %5
                      .arg(Theme::FONT_BODY)       // %6
                      .arg(Theme::ACCENT_LIGHT)    // %7
                      .arg(Theme::ACCENT_PRIMARY)  // %8
                      .arg(Theme::TEXT_PRIMARY)    // %9
                      .arg(Theme::BG_NUMPAD_KEY)   // %10
                      .arg(Theme::BG_NUMPAD_HOVER) // %11
                      .arg(Theme::FONT_BODY)       // %12 (placeholder, mono)
                      .arg(Theme::BG_BADGE)        // %13
                      .arg(Theme::FONT_HEADING)    // %14
                      .arg(Theme::BUTTON_RADIUS)   // %15
                      .arg(Theme::ACCENT_HOVER)    // %16
                      .arg(Theme::TEXT_MUTED)      // %17
    );
}
