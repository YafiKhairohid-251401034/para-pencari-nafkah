#include "registerpage.h"
#include "theme.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QPropertyAnimation>
#include <QKeyEvent>

RegisterPage::RegisterPage(OrderManager* mgr, QWidget* parent)
    : QWidget(parent), m_mgr(mgr)
{
    setupUi();
    setupStyle();
}

void RegisterPage::setupUi()
{
    setMinimumSize(Theme::WINDOW_MIN_W, Theme::WINDOW_MIN_H);

    // ── Root layout ──────────────────────────────────────────────────────────
    QHBoxLayout* rootLayout = new QHBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    // ── Left decorative panel ─────────────────────────────────────────────
    QFrame* leftPanel = new QFrame();
    leftPanel->setObjectName("leftPanel");
    leftPanel->setFixedWidth(420);

    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->setContentsMargins(40, 50, 40, 40);
    leftLayout->setSpacing(0);

    // Brand mark on left panel
    QLabel* brandMark = new QLabel("☕");
    brandMark->setObjectName("brandMark");
    brandMark->setAlignment(Qt::AlignCenter);

    m_brandLabel = new QLabel("brew n bites");
    m_brandLabel->setObjectName("brandLabel");
    m_brandLabel->setAlignment(Qt::AlignCenter);

    m_brandSub = new QLabel("Cafe & Bites Experience");
    m_brandSub->setObjectName("brandSub");
    m_brandSub->setAlignment(Qt::AlignCenter);

    // Decorative items list
    QStringList features = {
        "☕  Premium Coffee Blends",
        "🥐  Freshly Baked Pastries",
        "🍽️  Savory Food Selections",
        "📱  Easy Order & Payment"
    };

    QVBoxLayout* featureLayout = new QVBoxLayout();
    featureLayout->setSpacing(12);
    for (const QString& f : features) {
        QLabel* lbl = new QLabel(f);
        lbl->setObjectName("featureItem");
        featureLayout->addWidget(lbl);
    }

    leftLayout->addStretch(1);
    leftLayout->addWidget(brandMark);
    leftLayout->addSpacing(8);
    leftLayout->addWidget(m_brandLabel);
    leftLayout->addSpacing(4);
    leftLayout->addWidget(m_brandSub);
    leftLayout->addSpacing(48);
    leftLayout->addLayout(featureLayout);
    leftLayout->addStretch(2);

    // ── Right form area ───────────────────────────────────────────────────
    QWidget* rightArea = new QWidget();
    rightArea->setObjectName("rightArea");

    QVBoxLayout* rightLayout = new QVBoxLayout(rightArea);
    rightLayout->setAlignment(Qt::AlignCenter);
    rightLayout->setContentsMargins(60, 40, 60, 40);

    // The card
    QFrame* card = new QFrame();
    card->setObjectName("registerCard");
    card->setFixedWidth(420);

    QVBoxLayout* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(40, 44, 40, 44);
    cardLayout->setSpacing(0);

    m_titleLabel = new QLabel("Entri Pelanggan Baru");
    m_titleLabel->setObjectName("cardTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_subtitleLabel = new QLabel("Silakan masukkan detail pesanan untuk memulai transaksi.");
    m_subtitleLabel->setObjectName("cardSubtitle");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);
    m_subtitleLabel->setWordWrap(true);

    // ── Name field ──────────────────────────────────────────────
    QLabel* nameLabel = new QLabel("Nama Pelanggan");
    nameLabel->setObjectName("fieldLabel");

    m_nameEdit = new QLineEdit();
    m_nameEdit->setObjectName("fieldInput");
    m_nameEdit->setPlaceholderText("David Sinclair");
    m_nameEdit->setMinimumHeight(46);

    // ── Table field ─────────────────────────────────────────────
    QLabel* tableLabel = new QLabel("Nomor Meja");
    tableLabel->setObjectName("fieldLabel");

    m_tableEdit = new QLineEdit();
    m_tableEdit->setObjectName("fieldInput");
    m_tableEdit->setPlaceholderText("11");
    m_tableEdit->setMinimumHeight(46);

    // ── Error label ─────────────────────────────────────────────
    m_errorLabel = new QLabel("");
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();

    // ── Start button ─────────────────────────────────────────────
    m_startBtn = new QPushButton("Mulai Pesanan  →");
    m_startBtn->setObjectName("startBtn");
    m_startBtn->setMinimumHeight(50);
    m_startBtn->setCursor(Qt::PointingHandCursor);

    connect(m_startBtn, &QPushButton::clicked, this, &RegisterPage::onStartOrder);
    connect(m_nameEdit,  &QLineEdit::returnPressed, this, &RegisterPage::onStartOrder);
    connect(m_tableEdit, &QLineEdit::returnPressed, this, &RegisterPage::onStartOrder);

    cardLayout->addWidget(m_titleLabel);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(m_subtitleLabel);
    cardLayout->addSpacing(32);
    cardLayout->addWidget(nameLabel);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_nameEdit);
    cardLayout->addSpacing(18);
    cardLayout->addWidget(tableLabel);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_tableEdit);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(m_errorLabel);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(m_startBtn);

    // Drop shadow on card
    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(40);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(60, 40, 20, 30));
    card->setGraphicsEffect(shadow);

    rightLayout->addWidget(card, 0, Qt::AlignCenter);

    rootLayout->addWidget(leftPanel);
    rootLayout->addWidget(rightArea, 1);
}

void RegisterPage::setupStyle()
{
    setStyleSheet(QString(R"(
        RegisterPage {
            background-color: %1;
        }

        /* ── Left panel ── */
        #leftPanel {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 %2, stop:0.5 %3, stop:1 %4);
        }
        #brandMark {
            font-size: 52px;
        }
        #brandLabel {
            font-family: '%5';
            font-size: 26px;
            font-weight: bold;
            color: #FFFFFF;
            letter-spacing: 2px;
        }
        #brandSub {
            font-family: '%6';
            font-size: 12px;
            color: rgba(255,255,255,0.65);
            letter-spacing: 1px;
        }
        #featureItem {
            font-family: '%6';
            font-size: 13px;
            color: rgba(255,255,255,0.85);
            padding: 10px 16px;
            background: rgba(255,255,255,0.10);
            border-radius: 10px;
        }

        /* ── Right area ── */
        #rightArea {
            background-color: %1;
        }

        /* ── Card ── */
        #registerCard {
            background-color: %7;
            border-radius: 18px;
            border: 1px solid %8;
        }

        /* ── Typography ── */
        #cardTitle {
            font-family: '%5';
            font-size: 22px;
            font-weight: bold;
            color: %9;
        }
        #cardSubtitle {
            font-family: '%6';
            font-size: 12px;
            color: %10;
            line-height: 1.5;
        }
        #fieldLabel {
            font-family: '%6';
            font-size: 12px;
            font-weight: 600;
            color: %9;
        }

        /* ── Input ── */
        #fieldInput {
            background-color: %11;
            border: 1.5px solid %12;
            border-radius: %13px;
            padding: 0 14px;
            font-family: '%6';
            font-size: 13px;
            color: %9;
        }
        #fieldInput:focus {
            border: 1.5px solid %14;
            background-color: #FFFFFF;
        }
        #fieldInput::placeholder {
            color: %15;
        }

        /* ── Error ── */
        #errorLabel {
            font-family: '%6';
            font-size: 11px;
            color: %16;
        }

        /* ── Start button ── */
        #startBtn {
            background-color: %17;
            color: #FFFFFF;
            font-family: '%5';
            font-size: 14px;
            font-weight: bold;
            border: none;
            border-radius: %18px;
            letter-spacing: 0.5px;
        }
        #startBtn:hover {
            background-color: %19;
        }
        #startBtn:pressed {
            background-color: %19;
        }
    )")
    .arg(Theme::BG_APP)           // %1
    .arg("#3D5635")                // %2 gradient stop 0 (dark sage)
    .arg(Theme::ACCENT_PRIMARY)   // %3 gradient stop 0.5
    .arg("#8B6F47")                // %4 gradient stop 1 (brown)
    .arg(Theme::FONT_DISPLAY)     // %5
    .arg(Theme::FONT_BODY)        // %6
    .arg(Theme::BG_CARD)          // %7
    .arg(Theme::BORDER_LIGHT)     // %8
    .arg(Theme::TEXT_PRIMARY)     // %9
    .arg(Theme::TEXT_SECONDARY)   // %10
    .arg(Theme::BG_INPUT)         // %11
    .arg(Theme::BORDER_INPUT)     // %12
    .arg(Theme::INPUT_RADIUS)     // %13
    .arg(Theme::ACCENT_PRIMARY)   // %14 focus border
    .arg(Theme::TEXT_MUTED)       // %15
    .arg(Theme::COLOR_ERROR)      // %16
    .arg(Theme::ACCENT_PRIMARY)   // %17 button bg
    .arg(Theme::BUTTON_RADIUS)    // %18
    .arg(Theme::ACCENT_HOVER)     // %19
    );
}

void RegisterPage::onStartOrder()
{
    if (!validate()) return;
    m_mgr->setCustomerName(m_nameEdit->text().trimmed());
    m_mgr->setTableNumber(m_tableEdit->text().trimmed());
    emit proceedToOrder();
}

bool RegisterPage::validate()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠ Nama pelanggan tidak boleh kosong.");
        m_errorLabel->show();
        m_nameEdit->setFocus();
        return false;
    }
    if (m_tableEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("⚠ Nomor meja tidak boleh kosong.");
        m_errorLabel->show();
        m_tableEdit->setFocus();
        return false;
    }
    m_errorLabel->hide();
    return true;
}
