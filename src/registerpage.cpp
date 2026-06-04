#include "registerpage.h"
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QPropertyAnimation>
#include <QVBoxLayout>
#include "theme.h"

RegisterPage::RegisterPage(OrderManager *mgr, QWidget *parent)
    : QWidget(parent)
    , m_mgr(mgr)
{
    setupUi();
    setupStyle();
}

void RegisterPage::setupUi()
{
    setMinimumSize(Theme::WINDOW_MIN_W, Theme::WINDOW_MIN_H);

    // ── Root layout — center card di tengah layar ────────────────────────
    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setAlignment(Qt::AlignCenter);

    // ── Card ─────────────────────────────────────────────────────────────
    QFrame *card = new QFrame();
    card->setObjectName("registerCard");
    card->setFixedWidth(440);

    QVBoxLayout *cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(44, 48, 44, 48);
    cardLayout->setSpacing(0);

    m_titleLabel = new QLabel("Entri Pelanggan Baru");
    m_titleLabel->setObjectName("cardTitle");
    m_titleLabel->setAlignment(Qt::AlignCenter);

    m_subtitleLabel = new QLabel("Silakan masukkan detail pesanan untuk memulai transaksi.");
    m_subtitleLabel->setObjectName("cardSubtitle");
    m_subtitleLabel->setAlignment(Qt::AlignCenter);
    m_subtitleLabel->setWordWrap(true);

    // ── Name field ───────────────────────────────────────────────────────
    QLabel *nameLabel = new QLabel("Nama Pelanggan");
    nameLabel->setObjectName("fieldLabel");

    m_nameEdit = new QLineEdit();
    m_nameEdit->setObjectName("fieldInput");
    m_nameEdit->setPlaceholderText("Joko");
    m_nameEdit->setMinimumHeight(46);

    // ── Table field ──────────────────────────────────────────────────────
    QLabel *tableLabel = new QLabel("Nomor Meja");
    tableLabel->setObjectName("fieldLabel");

    m_tableEdit = new QLineEdit();
    m_tableEdit->setObjectName("fieldInput");
    m_tableEdit->setPlaceholderText("11");
    m_tableEdit->setMinimumHeight(46);

    // ── Error label ──────────────────────────────────────────────────────
    m_errorLabel = new QLabel("");
    m_errorLabel->setObjectName("errorLabel");
    m_errorLabel->setAlignment(Qt::AlignCenter);
    m_errorLabel->hide();

    // ── Start button ─────────────────────────────────────────────────────
    m_startBtn = new QPushButton("Mulai Pesanan  →");
    m_startBtn->setObjectName("startBtn");
    m_startBtn->setMinimumHeight(50);
    m_startBtn->setCursor(Qt::PointingHandCursor);

    connect(m_startBtn, &QPushButton::clicked, this, &RegisterPage::onStartOrder);
    connect(m_nameEdit, &QLineEdit::returnPressed, this, &RegisterPage::onStartOrder);
    connect(m_tableEdit, &QLineEdit::returnPressed, this, &RegisterPage::onStartOrder);

    cardLayout->addWidget(m_titleLabel);
    cardLayout->addSpacing(8);
    cardLayout->addWidget(m_subtitleLabel);
    cardLayout->addSpacing(36);
    cardLayout->addWidget(nameLabel);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_nameEdit);
    cardLayout->addSpacing(20);
    cardLayout->addWidget(tableLabel);
    cardLayout->addSpacing(6);
    cardLayout->addWidget(m_tableEdit);
    cardLayout->addSpacing(10);
    cardLayout->addWidget(m_errorLabel);
    cardLayout->addSpacing(24);
    cardLayout->addWidget(m_startBtn);

    // Drop shadow
    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(40);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(60, 40, 20, 30));
    card->setGraphicsEffect(shadow);

    rootLayout->addWidget(card, 0, Qt::AlignCenter);
}

void RegisterPage::setupStyle()
{
    setStyleSheet(QString(R"(
        RegisterPage {
            background-color: %1;
        }

        #registerCard {
            background-color: %2;
            border-radius: 18px;
            border: 1px solid %3;
        }

        #cardTitle {
            font-family: '%4';
            font-size: 22px;
            font-weight: bold;
            color: %5;
        }
        #cardSubtitle {
            font-family: '%6';
            font-size: 12px;
            color: %7;
            line-height: 1.5;
        }
        #fieldLabel {
            font-family: '%6';
            font-size: 12px;
            font-weight: 600;
            color: %5;
        }

        #fieldInput {
            background-color: %8;
            border: 1.5px solid %9;
            border-radius: %10px;
            padding: 0 14px;
            font-family: '%6';
            font-size: 13px;
            color: %5;
        }
        #fieldInput:focus {
            border: 1.5px solid %11;
            background-color: #FFFFFF;
        }
        #fieldInput::placeholder {
            color: %12;
        }

        #errorLabel {
            font-family: '%6';
            font-size: 11px;
            color: %13;
        }

        #startBtn {
            background-color: %11;
            color: #FFFFFF;
            font-family: '%4';
            font-size: 14px;
            font-weight: bold;
            border: none;
            border-radius: %14px;
            letter-spacing: 0.5px;
        }
        #startBtn:hover {
            background-color: %15;
        }
        #startBtn:pressed {
            background-color: %15;
        }
    )")
                      .arg("#FAF8F5")             // %1 — putih hangat
                      .arg(Theme::BG_CARD)        // %2
                      .arg(Theme::BORDER_LIGHT)   // %3
                      .arg(Theme::FONT_DISPLAY)   // %4
                      .arg(Theme::TEXT_PRIMARY)   // %5
                      .arg(Theme::FONT_BODY)      // %6
                      .arg(Theme::TEXT_SECONDARY) // %7
                      .arg(Theme::BG_INPUT)       // %8
                      .arg(Theme::BORDER_INPUT)   // %9
                      .arg(Theme::INPUT_RADIUS)   // %10
                      .arg(Theme::ACCENT_PRIMARY) // %11
                      .arg(Theme::TEXT_MUTED)     // %12
                      .arg(Theme::COLOR_ERROR)    // %13
                      .arg(Theme::BUTTON_RADIUS)  // %14
                      .arg(Theme::ACCENT_HOVER)   // %15
                  );
}

void RegisterPage::onStartOrder()
{
    if (!validate())
        return;
    m_mgr->setCustomerName(m_nameEdit->text().trimmed());
    m_mgr->setTableNumber(m_tableEdit->text().trimmed());
    emit proceedToOrder();
}

bool RegisterPage::validate()
{
    if (m_nameEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("Nama pelanggan tidak boleh kosong.");
        m_errorLabel->show();
        m_nameEdit->setFocus();
        return false;
    }
    if (m_tableEdit->text().trimmed().isEmpty()) {
        m_errorLabel->setText("Nomor meja tidak boleh kosong.");
        m_errorLabel->show();
        m_tableEdit->setFocus();
        return false;
    }
    m_errorLabel->hide();
    return true;
}