#include "orderpage.h"
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QStyle>
#include <QVBoxLayout>
#include "theme.h"

// =============================================================================
//  MenuCard — widget kartu produk di grid
// =============================================================================
class MenuCard : public QFrame
{
    Q_OBJECT
public:
    explicit MenuCard(const MenuItem &item, QWidget *parent = nullptr)
        : QFrame(parent)
        , m_item(item)
    {
        setObjectName("menuCard");
        setFixedSize(190, 220);
        setCursor(Qt::PointingHandCursor);

        QVBoxLayout *lay = new QVBoxLayout(this);
        lay->setContentsMargins(14, 14, 14, 12);
        lay->setSpacing(0);

        // Emoji / image placeholder
        m_emojiLabel = new QLabel(item.emoji);
        m_emojiLabel->setObjectName("cardEmoji");
        m_emojiLabel->setAlignment(Qt::AlignCenter);
        m_emojiLabel->setFixedHeight(80);

        // Name
        m_nameLabel = new QLabel(item.name);
        m_nameLabel->setObjectName("cardName");
        m_nameLabel->setWordWrap(true);

        // Description
        m_descLabel = new QLabel(item.description);
        m_descLabel->setObjectName("cardDesc");
        m_descLabel->setWordWrap(true);

        // Price row
        QHBoxLayout *priceRow = new QHBoxLayout();
        priceRow->setContentsMargins(0, 0, 0, 0);
        m_priceLabel = new QLabel(item.formattedPrice());
        m_priceLabel->setObjectName("cardPrice");

        m_addBtn = new QPushButton("+");
        m_addBtn->setObjectName("cardAddBtn");
        m_addBtn->setFixedSize(28, 28);
        m_addBtn->setCursor(Qt::PointingHandCursor);

        priceRow->addWidget(m_priceLabel);
        priceRow->addStretch();
        priceRow->addWidget(m_addBtn);

        lay->addWidget(m_emojiLabel);
        lay->addSpacing(4);
        lay->addWidget(m_nameLabel);
        lay->addSpacing(2);
        lay->addWidget(m_descLabel);
        lay->addStretch();
        lay->addLayout(priceRow);

        connect(m_addBtn, &QPushButton::clicked, this, &MenuCard::onAdd);

        applyStyle();
    }

    const MenuItem &item() const { return m_item; }

signals:
    void itemAdded(const MenuItem &item);

private slots:
    void onAdd() { emit itemAdded(m_item); }

private:
    void applyStyle()
    {
        setStyleSheet(QString(R"(
            #menuCard {
                background-color: %1;
                border-radius: %2px;
                border: 1px solid %3;
            }
            #menuCard:hover {
                background-color: %4;
                border: 1px solid %5;
            }
            #cardEmoji {
                font-size: 40px;
            }
            #cardName {
                font-family: '%6';
                font-size: 13px;
                font-weight: bold;
                color: %7;
            }
            #cardDesc {
                font-family: '%8';
                font-size: 10px;
                color: %9;
            }
            #cardPrice {
                font-family: '%10';
                font-size: 12px;
                font-weight: bold;
                color: %11;
            }
            #cardAddBtn {
                background-color: %12;
                color: %13;
                border: none;
                border-radius: 14px;
                font-size: 18px;
                font-weight: bold;
            }
            #cardAddBtn:hover {
                background-color: %14;
            }
        )")
                          .arg(Theme::BG_CARD)
                          .arg(Theme::CARD_RADIUS)
                          .arg(Theme::BORDER_LIGHT)
                          .arg(Theme::BG_CARD_HOVER)
                          .arg(Theme::BORDER_MEDIUM)
                          .arg(Theme::FONT_HEADING)
                          .arg(Theme::TEXT_PRIMARY)
                          .arg(Theme::FONT_BODY)
                          .arg(Theme::TEXT_MUTED)
                          .arg(Theme::FONT_MONO)
                          .arg(Theme::TEXT_PRICE)
                          .arg(Theme::ACCENT_PRIMARY)
                          .arg(Theme::ACCENT_TEXT)
                          .arg(Theme::ACCENT_HOVER));
    }

    MenuItem m_item;
    QLabel *m_emojiLabel;
    QLabel *m_nameLabel;
    QLabel *m_descLabel;
    QLabel *m_priceLabel;
    QPushButton *m_addBtn;
};

// =============================================================================
//  OrderLineWidget — satu baris di panel current order
// =============================================================================
class OrderLineWidget : public QFrame
{
    Q_OBJECT
public:
    explicit OrderLineWidget(const OrderLine &line, QWidget *parent = nullptr)
        : QFrame(parent)
        , m_itemId(line.item.id)
    {
        setObjectName("orderLine");

        QHBoxLayout *lay = new QHBoxLayout(this);
        lay->setContentsMargins(0, 8, 0, 8);
        lay->setSpacing(8);

        // Qty badge
        m_qtyLabel = new QLabel(QString("%1×").arg(line.quantity));
        m_qtyLabel->setObjectName("orderQty");
        m_qtyLabel->setFixedWidth(28);
        m_qtyLabel->setAlignment(Qt::AlignCenter);

        // Name
        QLabel *name = new QLabel(line.item.name);
        name->setObjectName("orderName");
        name->setWordWrap(false);

        // Price
        m_priceLabel = new QLabel(line.formattedSubtotal());
        m_priceLabel->setObjectName("orderPrice");
        m_priceLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

        // Remove btn
        m_removeBtn = new QPushButton("−");
        m_removeBtn->setObjectName("removeBtn");
        m_removeBtn->setFixedSize(22, 22);
        m_removeBtn->setCursor(Qt::PointingHandCursor);

        lay->addWidget(m_qtyLabel);
        lay->addWidget(name, 1);
        lay->addWidget(m_priceLabel);
        lay->addWidget(m_removeBtn);

        connect(m_removeBtn, &QPushButton::clicked, [this] { emit removeItem(m_itemId); });

        setStyleSheet(QString(R"(
            #orderLine {
                border-bottom: 1px solid %1;
                background: transparent;
            }
            #orderQty {
                font-family: '%2';
                font-size: 11px;
                font-weight: bold;
                background: %3;
                color: %4;
                border-radius: 5px;
                padding: 2px;
            }
            #orderName {
                font-family: '%5';
                font-size: 12px;
                color: %6;
            }
            #orderPrice {
                font-family: '%2';
                font-size: 12px;
                font-weight: bold;
                color: %6;
            }
            #removeBtn {
                background: %7;
                color: %8;
                border: none;
                border-radius: 11px;
                font-size: 14px;
                font-weight: bold;
            }
            #removeBtn:hover {
                background: %9;
                color: white;
            }
        )")
                          .arg(Theme::BORDER_LIGHT)
                          .arg(Theme::FONT_MONO)
                          .arg(Theme::ACCENT_LIGHT)
                          .arg(Theme::ACCENT_PRIMARY)
                          .arg(Theme::FONT_BODY)
                          .arg(Theme::TEXT_PRIMARY)
                          .arg(Theme::BG_BADGE)
                          .arg(Theme::TEXT_SECONDARY)
                          .arg(Theme::COLOR_ERROR));
    }

signals:
    void removeItem(int itemId);

private:
    int m_itemId;
    QLabel *m_qtyLabel;
    QLabel *m_priceLabel;
    QPushButton *m_removeBtn;
};

// =============================================================================
//  OrderPage
// =============================================================================
OrderPage::OrderPage(OrderManager *mgr, QWidget *parent)
    : QWidget(parent)
    , m_mgr(mgr)
    , m_currentCategory(MenuCategory::All)
{
    setupUi();
    setupStyle();
    connect(m_mgr, &OrderManager::cartChanged, this, &OrderPage::onCartChanged);
}

void OrderPage::refreshPage()
{
    // Update customer badge
    if (m_customerBadge) {
        m_customerBadge->setText(
            QString(" %1  |   Meja %2").arg(m_mgr->customerName()).arg(m_mgr->tableNumber()));
    }
    refreshOrderPanel();
}

void OrderPage::setupUi()
{
    QHBoxLayout *root = new QHBoxLayout(this);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    buildSidebar();
    buildMenuGrid();
    buildOrderPanel();

    root->addWidget(m_sidebar);
    // Main area (header + content)
    QWidget *mainArea = new QWidget();
    mainArea->setObjectName("mainArea");
    QVBoxLayout *mainLay = new QVBoxLayout(mainArea);
    mainLay->setContentsMargins(0, 0, 0, 0);
    mainLay->setSpacing(0);

    buildHeader();
    mainLay->addWidget(m_header);
    mainLay->addWidget(m_menuScroll, 1);

    root->addWidget(mainArea, 1);
    root->addWidget(m_orderPanel);
}

void OrderPage::buildSidebar()
{
    m_sidebar = new QFrame();
    m_sidebar->setObjectName("sidebar");
    m_sidebar->setFixedWidth(Theme::SIDEBAR_WIDTH);

    QVBoxLayout *lay = new QVBoxLayout(m_sidebar);
    lay->setContentsMargins(0, 20, 0, 20);
    lay->setSpacing(0);
    lay->setAlignment(Qt::AlignTop);

    // Brand
    QLabel *brandName = new QLabel("brew n bites");
    brandName->setObjectName("sidebarBrand");
    brandName->setAlignment(Qt::AlignCenter);
    brandName->setWordWrap(true);

    lay->addWidget(brandName);
    lay->addSpacing(28);

    // Nav buttons
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
    m_navOrder->setObjectName("navBtnActive");

    lay->addWidget(m_navRegister);
    lay->addSpacing(4);
    lay->addWidget(m_navOrder);
    lay->addSpacing(4);
    lay->addWidget(m_navPayment);
    lay->addStretch();

    connect(m_navRegister, &QPushButton::clicked, this, &OrderPage::navigateToRegister);
    connect(m_navPayment, &QPushButton::clicked, this, &OrderPage::proceedToPayment);
}

void OrderPage::buildHeader()
{
    m_header = new QFrame();
    m_header->setObjectName("pageHeader");
    m_header->setFixedHeight(72);

    QHBoxLayout *lay = new QHBoxLayout(m_header);
    lay->setContentsMargins(24, 0, 24, 0);

    // Search
    m_searchEdit = new QLineEdit();
    m_searchEdit->setObjectName("searchEdit");
    m_searchEdit->setPlaceholderText(" Search menu items...");
    m_searchEdit->setFixedWidth(240);
    m_searchEdit->setFixedHeight(36);

    // Customer badge
    m_customerBadge = new QLabel();
    m_customerBadge->setObjectName("customerBadge");

    // Category filter buttons
    QHBoxLayout *catLay = new QHBoxLayout();
    catLay->setSpacing(6);

    struct CatDef
    {
        QString label;
        MenuCategory cat;
    };
    QVector<CatDef> cats = {
        {"All Items", MenuCategory::All},
        {"Coffee", MenuCategory::Coffee},
        {"Non-Coffee", MenuCategory::NonCoffee},
        {"Cake", MenuCategory::Cake},
        {"Food", MenuCategory::Food},
    };

    for (const auto &c : cats) {
        QPushButton *btn = new QPushButton(c.label);
        btn->setObjectName(c.cat == MenuCategory::All ? "catBtnActive" : "catBtn");
        btn->setFixedHeight(32);
        btn->setCursor(Qt::PointingHandCursor);
        MenuCategory cat = c.cat;
        connect(btn, &QPushButton::clicked, [this, cat]() { onCategoryFilter(cat); });
        m_catBtns.append(btn);
        catLay->addWidget(btn);
    }

    lay->addWidget(m_searchEdit);
    lay->addSpacing(12);
    lay->addLayout(catLay);
    lay->addStretch();
    lay->addWidget(m_customerBadge);

    // Search filter connection
    connect(m_searchEdit, &QLineEdit::textChanged, [this](const QString &text) {
        // Filter by search text
        for (int i = 0; i < m_menuGrid->count(); ++i) {
            QLayoutItem *it = m_menuGrid->itemAt(i);
            if (!it)
                continue;
            if (MenuCard *card = qobject_cast<MenuCard *>(it->widget())) {
                bool match = text.isEmpty() || card->item().name.contains(text, Qt::CaseInsensitive)
                             || card->item().description.contains(text, Qt::CaseInsensitive);
                card->setVisible(match);
            }
        }
    });
}

void OrderPage::buildMenuGrid()
{
    m_menuScroll = new QScrollArea();
    m_menuScroll->setObjectName("menuScroll");
    m_menuScroll->setWidgetResizable(true);
    m_menuScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_menuScroll->setFrameShape(QFrame::NoFrame);

    m_menuContainer = new QWidget();
    m_menuContainer->setObjectName("menuContainer");

    QVBoxLayout *outer = new QVBoxLayout(m_menuContainer);
    outer->setContentsMargins(20, 16, 20, 20);
    outer->setSpacing(8);

    // Category label
    QLabel *menuLabel = new QLabel("All Items");
    menuLabel->setObjectName("menuSectionLabel");
    outer->addWidget(menuLabel);

    QWidget *gridHost = new QWidget();
    m_menuGrid = new QGridLayout(gridHost);
    m_menuGrid->setSpacing(14);
    m_menuGrid->setContentsMargins(0, 0, 0, 0);
    outer->addWidget(gridHost);
    outer->addStretch();

    m_menuScroll->setWidget(m_menuContainer);

    populateMenu(MenuCategory::All);
}

void OrderPage::buildOrderPanel()
{
    m_orderPanel = new QFrame();
    m_orderPanel->setObjectName("orderPanel");
    m_orderPanel->setFixedWidth(Theme::ORDER_PANEL_W);

    QVBoxLayout *lay = new QVBoxLayout(m_orderPanel);
    lay->setContentsMargins(18, 20, 18, 18);
    lay->setSpacing(0);

    // Header row
    QHBoxLayout *panelHeader = new QHBoxLayout();
    m_orderTitle = new QLabel("Current Order");
    m_orderTitle->setObjectName("orderPanelTitle");

    QLabel *dot = new QLabel("•••");
    dot->setObjectName("orderDots");

    panelHeader->addWidget(m_orderTitle);
    panelHeader->addStretch();
    panelHeader->addWidget(dot);
    lay->addLayout(panelHeader);

    // Customer sub-label
    m_orderCustomer = new QLabel();
    m_orderCustomer->setObjectName("orderCustomerLabel");
    lay->addSpacing(2);
    lay->addWidget(m_orderCustomer);

    // Divider
    QFrame *div1 = new QFrame();
    div1->setObjectName("divider");
    div1->setFixedHeight(1);
    lay->addSpacing(12);
    lay->addWidget(div1);
    lay->addSpacing(4);

    // Order lines scroll
    m_orderScroll = new QScrollArea();
    m_orderScroll->setObjectName("orderScroll");
    m_orderScroll->setWidgetResizable(true);
    m_orderScroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_orderScroll->setFrameShape(QFrame::NoFrame);

    m_orderLinesContainer = new QWidget();
    m_orderLinesLayout = new QVBoxLayout(m_orderLinesContainer);
    m_orderLinesLayout->setContentsMargins(0, 0, 0, 0);
    m_orderLinesLayout->setSpacing(0);
    m_orderLinesLayout->setAlignment(Qt::AlignTop);

    m_emptyCart = new QLabel("\nBelum ada item.\nSilakan pilih menu.");
    m_emptyCart->setObjectName("emptyCart");
    m_emptyCart->setAlignment(Qt::AlignCenter);
    m_orderLinesLayout->addWidget(m_emptyCart);

    m_orderScroll->setWidget(m_orderLinesContainer);
    lay->addWidget(m_orderScroll, 1);

    // Divider
    QFrame *div2 = new QFrame();
    div2->setObjectName("divider");
    div2->setFixedHeight(1);
    lay->addWidget(div2);
    lay->addSpacing(12);

    // Subtotal
    auto makeRow = [&](QLabel *&lbl, QLabel *&val, const QString &text) {
        QHBoxLayout *row = new QHBoxLayout();
        lbl = new QLabel(text);
        lbl->setObjectName("summaryLabel");
        val = new QLabel("Rp 0");
        val->setObjectName("summaryValue");
        row->addWidget(lbl);
        row->addStretch();
        row->addWidget(val);
        lay->addLayout(row);
        lay->addSpacing(4);
    };
    makeRow(m_subtotalLabel, m_subtotalValue, "Subtotal");
    makeRow(m_taxLabel, m_taxValue, "PB1 (10%)");

    lay->addSpacing(6);

    // Total row
    QFrame *totalFrame = new QFrame();
    totalFrame->setObjectName("totalFrame");
    QHBoxLayout *totalRow = new QHBoxLayout(totalFrame);
    totalRow->setContentsMargins(12, 8, 12, 8);
    m_totalLabel = new QLabel("Total Amount");
    m_totalLabel->setObjectName("totalLabel");
    m_totalValue = new QLabel("Rp 0");
    m_totalValue->setObjectName("totalValue");
    totalRow->addWidget(m_totalLabel);
    totalRow->addStretch();
    totalRow->addWidget(m_totalValue);
    lay->addWidget(totalFrame);
    lay->addSpacing(12);

    // Proceed button
    m_payBtn = new QPushButton("Proceed to Payment  →");
    m_payBtn->setObjectName("payBtn");
    m_payBtn->setMinimumHeight(46);
    m_payBtn->setCursor(Qt::PointingHandCursor);
    connect(m_payBtn, &QPushButton::clicked, this, &OrderPage::proceedToPayment);
    lay->addWidget(m_payBtn);
}

void OrderPage::populateMenu(MenuCategory cat)
{
    // Clear existing cards
    QLayoutItem *item;
    while ((item = m_menuGrid->takeAt(0)) != nullptr) {
        if (item->widget())
            item->widget()->deleteLater();
        delete item;
    }

    QVector<MenuItem> all = OrderManager::allMenuItems();
    int col = 0, row = 0;
    const int COLS = 4;

    for (const MenuItem &mi : all) {
        if (cat != MenuCategory::All && mi.category != cat)
            continue;
        MenuCard *card = new MenuCard(mi);
        connect(card, &MenuCard::itemAdded, this, &OrderPage::onItemAdded);
        m_menuGrid->addWidget(card, row, col);
        col++;
        if (col >= COLS) {
            col = 0;
            row++;
        }
    }
}

void OrderPage::onCategoryFilter(MenuCategory cat)
{
    m_currentCategory = cat;
    populateMenu(cat);

    // Update active button style
    for (int i = 0; i < m_catBtns.size(); ++i) {
        MenuCategory thisCat = static_cast<MenuCategory>(i);
        m_catBtns[i]->setObjectName(thisCat == cat ? "catBtnActive" : "catBtn");
        m_catBtns[i]->style()->unpolish(m_catBtns[i]);
        m_catBtns[i]->style()->polish(m_catBtns[i]);
    }
}

void OrderPage::onItemAdded(const MenuItem &item)
{
    m_mgr->addItem(item);
}

void OrderPage::onCartChanged()
{
    refreshOrderPanel();
}

void OrderPage::refreshOrderPanel()
{
    // Update customer label
    m_orderCustomer->setText(
        QString("%1  ·  Meja %2").arg(m_mgr->customerName()).arg(m_mgr->tableNumber()));

    // Clear order lines
    QLayoutItem *it;
    while ((it = m_orderLinesLayout->takeAt(0)) != nullptr) {
        if (it->widget())
            it->widget()->deleteLater();
        delete it;
    }

    QVector<OrderLine> lines = m_mgr->orderLines();

    if (lines.isEmpty()) {
        m_emptyCart = new QLabel("\nBelum ada item.\nSilakan pilih menu.");
        m_emptyCart->setObjectName("emptyCart");
        m_emptyCart->setAlignment(Qt::AlignCenter);
        m_orderLinesLayout->addWidget(m_emptyCart);
    } else {
        for (const OrderLine &line : lines) {
            OrderLineWidget *w = new OrderLineWidget(line);
            connect(w, &OrderLineWidget::removeItem, m_mgr, &OrderManager::removeItem);
            m_orderLinesLayout->addWidget(w);
        }
    }
    m_orderLinesLayout->addStretch();

    // Update totals
    m_subtotalValue->setText(m_mgr->formattedSubtotal());
    m_taxValue->setText(m_mgr->formattedTax());
    m_totalValue->setText(m_mgr->formattedTotal());
}

void OrderPage::setupStyle()
{
    setStyleSheet(QString(R"(
        OrderPage, #mainArea {
            background-color: %1;
        }

        /* ── Sidebar ── */
        #sidebar {
            background-color: %2;
            border-right: 1px solid %3;
        }
        #sidebarIcon {
            font-size: 24px;
        }
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
            color: %7;
            text-align: center;
            padding: 6px 8px;
            margin: 0 10px;
        }
        #navBtn:hover {
            background: %8;
            color: %9;
        }
        #navBtnActive {
            background: %8;
            border: none;
            border-radius: 6px;
            font-family: '%6';
            font-size: 12px;
            font-weight: bold;
            color: %9;
            text-align: center;
            padding: 6px 8px;
            margin: 0 10px;
        }

        /* ── Header ── */
        #pageHeader {
            background-color: %2;
            border-bottom: 1px solid %3;
        }
        #searchEdit {
            background: %10;
            border: 1.5px solid %3;
            border-radius: 18px;
            padding: 0 14px;
            font-family: '%6';
            font-size: 12px;
            color: %11;
        }
        #searchEdit:focus {
            border: 1.5px solid %12;
        }
        #customerBadge {
            font-family: '%6';
            font-size: 11px;
            color: %7;
            background: %8;
            border-radius: 12px;
            padding: 4px 12px;
        }
        #catBtn {
            background: %13;
            border: 1px solid %3;
            border-radius: 16px;
            font-family: '%6';
            font-size: 11px;
            color: %7;
            padding: 0 14px;
        }
        #catBtn:hover {
            background: %10;
            border-color: %12;
        }
        #catBtnActive {
            background: %12;
            border: 1px solid %12;
            border-radius: 16px;
            font-family: '%6';
            font-size: 11px;
            font-weight: bold;
            color: white;
            padding: 0 14px;
        }

        /* ── Menu area ── */
        #menuScroll, #menuContainer {
            background-color: %1;
        }
        QScrollBar:vertical {
            background: transparent;
            width: 6px;
        }
        QScrollBar::handle:vertical {
            background: %3;
            border-radius: 3px;
        }
        QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
            height: 0;
        }
        #menuSectionLabel {
            font-family: '%15';
            font-size: 16px;
            font-weight: bold;
            color: %11;
        }

        /* ── Order panel ── */
        #orderPanel {
            background-color: %2;
            border-left: 1px solid %3;
        }
        #orderPanelTitle {
            font-family: '%15';
            font-size: 16px;
            font-weight: bold;
            color: %11;
        }
        #orderDots {
            font-size: 14px;
            color: %7;
        }
        #orderCustomerLabel {
            font-family: '%6';
            font-size: 11px;
            color: %7;
        }
        #divider {
            background: %3;
        }
        #orderScroll, #orderLinesContainer {
            background: transparent;
        }
        #emptyCart {
            font-family: '%6';
            font-size: 12px;
            color: %14;
            padding: 30px 20px;
            line-height: 1.8;
        }
        #summaryLabel {
            font-family: '%6';
            font-size: 12px;
            color: %7;
        }
        #summaryValue {
            font-family: '%16';
            font-size: 12px;
            color: %11;
        }
        #totalFrame {
            background: %8;
            border-radius: 10px;
        }
        #totalLabel {
            font-family: '%6';
            font-size: 13px;
            font-weight: bold;
            color: %11;
        }
        #totalValue {
            font-family: '%16';
            font-size: 14px;
            font-weight: bold;
            color: %12;
        }
        #payBtn {
            background-color: %12;
            color: white;
            font-family: '%15';
            font-size: 13px;
            font-weight: bold;
            border: none;
            border-radius: %17px;
        }
        #payBtn:hover {
            background-color: %18;
        }
    )")
                      .arg(Theme::BG_APP)         // %1
                      .arg(Theme::BG_SIDEBAR)     // %2
                      .arg(Theme::BORDER_LIGHT)   // %3
                      .arg(Theme::FONT_DISPLAY)   // %4
                      .arg(Theme::TEXT_SECONDARY) // %5
                      .arg(Theme::FONT_BODY)      // %6
                      .arg(Theme::TEXT_SECONDARY) // %7
                      .arg(Theme::ACCENT_LIGHT)   // %8
                      .arg(Theme::ACCENT_PRIMARY) // %9
                      .arg(Theme::BG_INPUT)       // %10
                      .arg(Theme::TEXT_PRIMARY)   // %11
                      .arg(Theme::ACCENT_PRIMARY) // %12
                      .arg(Theme::BG_BADGE)       // %13
                      .arg(Theme::TEXT_MUTED)     // %14
                      .arg(Theme::FONT_HEADING)   // %15
                      .arg(Theme::FONT_MONO)      // %16
                      .arg(Theme::BUTTON_RADIUS)  // %17
                      .arg(Theme::ACCENT_HOVER)   // %18
    );
}

#include "orderpage.moc"
