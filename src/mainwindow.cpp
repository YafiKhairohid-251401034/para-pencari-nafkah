#include "mainwindow.h"
#include "theme.h"
#include <QScreen>
#include <QApplication>
#include <QPushButton>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent)
{
    setWindowTitle("Brew n Bites — POS System");
    setMinimumSize(Theme::WINDOW_MIN_W, Theme::WINDOW_MIN_H);
    resize(Theme::WINDOW_DEFAULT_W, Theme::WINDOW_DEFAULT_H);

    // Center window on screen
    QScreen* screen = QApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->availableGeometry();
        move((geo.width()  - width())  / 2,
             (geo.height() - height()) / 2);
    }

    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    m_orderMgr    = new OrderManager(this);

    m_registerPage = new RegisterPage(m_orderMgr, this);
    m_orderPage    = new OrderPage(m_orderMgr, this);
    m_paymentPage  = new PaymentPage(m_orderMgr, this);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_registerPage);   // index 0
    m_stack->addWidget(m_orderPage);       // index 1
    m_stack->addWidget(m_paymentPage);     // index 2

    setCentralWidget(m_stack);

    // ── Wiring navigation ────────────────────────────────────────────────────
    connect(m_registerPage, &RegisterPage::proceedToOrder, this, &MainWindow::showOrderPage);
    connect(m_orderPage,    &OrderPage::proceedToPayment,  this, &MainWindow::showPaymentPage);
    connect(m_paymentPage,  &PaymentPage::transactionComplete, this, &MainWindow::onTransactionComplete);

    // Sidebar nav wiring
    connect(m_orderPage,   &OrderPage::navigateToRegister,  this, &MainWindow::showRegisterPage);
    connect(m_paymentPage, &PaymentPage::navigateToOrder,   this, &MainWindow::showOrderPage);

    showRegisterPage();
}

void MainWindow::showRegisterPage()
{
    m_stack->setCurrentIndex(0);
}

void MainWindow::showOrderPage()
{
    m_orderPage->refreshPage();
    m_stack->setCurrentIndex(1);
}

void MainWindow::showPaymentPage()
{
    m_paymentPage->refreshPage();
    m_stack->setCurrentIndex(2);
}

void MainWindow::onTransactionComplete()
{
    showRegisterPage();
}
