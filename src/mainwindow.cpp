#include "mainwindow.h"
#include <QApplication>
#include <QPushButton>
#include <QScreen>
#include "theme.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Brew n Bites");
    setMinimumSize(Theme::WINDOW_MIN_W, Theme::WINDOW_MIN_H);
    resize(Theme::WINDOW_DEFAULT_W, Theme::WINDOW_DEFAULT_H);

    // Center window on screen
    QScreen *screen = QApplication::primaryScreen();
    if (screen) {
        QRect geo = screen->availableGeometry();
        move((geo.width() - width()) / 2, (geo.height() - height()) / 2);
    }

    setupUi();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUi()
{
    m_orderMgr = new OrderManager(this);
    m_historyMgr = new HistoryManager(this);

    m_registerPage = new RegisterPage(m_orderMgr, this);
    m_orderPage = new OrderPage(m_orderMgr, this);
    m_paymentPage = new PaymentPage(m_orderMgr, this);
    m_historyPage = new HistoryPage(m_historyMgr, this);

    m_stack = new QStackedWidget(this);
    m_stack->addWidget(m_registerPage);
    m_stack->addWidget(m_orderPage);
    m_stack->addWidget(m_paymentPage);
    m_stack->addWidget(m_historyPage);

    setCentralWidget(m_stack);

    // ── Wiring navigation ────────────────────────────────────────────────────
    connect(m_registerPage, &RegisterPage::proceedToOrder, this, &MainWindow::showOrderPage);
    connect(m_orderPage, &OrderPage::proceedToPayment, this, &MainWindow::showPaymentPage);
    connect(m_paymentPage,
            &PaymentPage::transactionComplete,
            this,
            &MainWindow::onTransactionComplete);

    // Pass HistoryManager to PaymentPage so it can save completed transactions
    m_paymentPage->setHistoryManager(m_historyMgr);

    // Sidebar nav wiring
    connect(m_orderPage, &OrderPage::navigateToRegister, this, &MainWindow::showRegisterPage);
    connect(m_paymentPage, &PaymentPage::navigateToOrder, this, &MainWindow::showOrderPage);

    // Register page sidebar nav wiring
    connect(m_registerPage, &RegisterPage::navigateToOrder,   this, &MainWindow::showOrderPage);
    connect(m_registerPage, &RegisterPage::navigateToPayment, this, &MainWindow::showPaymentPage);
    connect(m_registerPage, &RegisterPage::navigateToHistory, this, &MainWindow::showHistoryPage);

    // History page sidebar nav wiring
    connect(m_historyPage, &HistoryPage::navigateToRegister, this, &MainWindow::showRegisterPage);
    connect(m_historyPage, &HistoryPage::navigateToOrder, this, &MainWindow::showOrderPage);
    connect(m_historyPage, &HistoryPage::navigateToPayment, this, &MainWindow::showPaymentPage);

    // History nav button from other pages
    connect(m_orderPage, &OrderPage::navigateToHistory, this, &MainWindow::showHistoryPage);
    connect(m_paymentPage, &PaymentPage::navigateToHistory, this, &MainWindow::showHistoryPage);

    showRegisterPage();
}

void MainWindow::showRegisterPage()
{
    m_registerPage->refreshPage(); // bersihkan form & keranjang sebelum tampil
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

void MainWindow::showHistoryPage()
{
    m_historyPage->refreshPage();
    m_stack->setCurrentIndex(3);
}

void MainWindow::onTransactionComplete()
{
    showRegisterPage();
}