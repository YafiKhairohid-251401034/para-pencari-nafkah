#pragma once

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>
#include "ordermanager.h"

class RegisterPage : public QWidget
{
    Q_OBJECT
public:
    explicit RegisterPage(OrderManager *mgr, QWidget *parent = nullptr);

    // Dipanggil MainWindow tiap kali halaman ini muncul —
    // bersihkan form & reset sesi OrderManager sebelumnya
    void refreshPage();

signals:
    void proceedToOrder();
    void navigateToOrder();
    void navigateToPayment();
    void navigateToHistory();

private slots:
    void onStartOrder();

private:
    void setupUi();
    void buildSidebar();
    void setupStyle();
    bool validate();

    OrderManager *m_mgr;

    // Sidebar
    QFrame      *m_sidebar;
    QPushButton *m_navRegister;
    QPushButton *m_navOrder;
    QPushButton *m_navPayment;
    QPushButton *m_navHistory;

    // Form
    QLabel      *m_titleLabel;
    QLabel      *m_subtitleLabel;
    QLineEdit   *m_nameEdit;
    QLineEdit   *m_tableEdit;
    QLabel      *m_errorLabel;
    QPushButton *m_startBtn;
};