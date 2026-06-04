#pragma once
#include <QFrame>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QVector>
#include <QWidget>
#include "menuitem.h"
#include "ordermanager.h"

// =============================================================================
//  OrderPage — Layar utama: grid menu + panel current order
// =============================================================================

class MenuCard;
class OrderLineWidget;

class OrderPage : public QWidget
{
    Q_OBJECT

public:
    explicit OrderPage(OrderManager *mgr, QWidget *parent = nullptr);

    void refreshPage();

signals:
    void navigateToRegister();
    void proceedToPayment();

private slots:
    void onCategoryFilter(MenuCategory cat);
    void onCartChanged();
    void onItemAdded(const MenuItem &item);

private:
    void setupUi();
    void setupStyle();
    void buildSidebar();
    void buildHeader();
    void buildMenuGrid();
    void buildOrderPanel();

    void populateMenu(MenuCategory cat = MenuCategory::All);
    void refreshOrderPanel();

    OrderManager *m_mgr;
    MenuCategory m_currentCategory;

    // Sidebar
    QFrame *m_sidebar;
    QPushButton *m_navRegister;
    QPushButton *m_navOrder;
    QPushButton *m_navPayment;

    // Header
    QFrame *m_header;
    QLineEdit *m_searchEdit;
    QLabel *m_customerBadge;

    // Category filters
    QVector<QPushButton *> m_catBtns;

    // Menu grid
    QScrollArea *m_menuScroll;
    QWidget *m_menuContainer;
    QGridLayout *m_menuGrid;

    // Order panel
    QFrame *m_orderPanel;
    QLabel *m_orderTitle;
    QLabel *m_orderCustomer;
    QScrollArea *m_orderScroll;
    QWidget *m_orderLinesContainer;
    QVBoxLayout *m_orderLinesLayout;
    QLabel *m_emptyCart;
    QLabel *m_subtotalLabel;
    QLabel *m_subtotalValue;
    QLabel *m_taxLabel;
    QLabel *m_taxValue;
    QLabel *m_totalLabel;
    QLabel *m_totalValue;
    QPushButton *m_payBtn;
};
