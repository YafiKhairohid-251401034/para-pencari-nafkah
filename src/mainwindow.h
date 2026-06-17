#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include "historymanager.h"
#include "historypage.h"
#include "ordermanager.h"
#include "orderpage.h"
#include "paymentpage.h"
#include "registerpage.h"

//  MainWindow — Kontainer utama dengan QStackedWidget untuk navigasi antar layar

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void showRegisterPage();
    void showOrderPage();
    void showPaymentPage();
    void showHistoryPage();
    void onTransactionComplete();

private:
    void setupUi();

    QStackedWidget *m_stack;
    OrderManager *m_orderMgr;
    HistoryManager *m_historyMgr;

    RegisterPage *m_registerPage;
    OrderPage *m_orderPage;
    PaymentPage *m_paymentPage;
    HistoryPage *m_historyPage;
};
