#pragma once
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include "ordermanager.h"

// =============================================================================
//  RegisterPage — Layar pertama: entri nama pelanggan & nomor meja
// =============================================================================

class RegisterPage : public QWidget {
    Q_OBJECT

public:
    explicit RegisterPage(OrderManager* mgr, QWidget* parent = nullptr);

signals:
    void proceedToOrder();

private slots:
    void onStartOrder();

private:
    void setupUi();
    void setupStyle();
    bool validate();

    OrderManager*   m_mgr;

    QLabel*         m_brandLabel;
    QLabel*         m_brandSub;
    QLabel*         m_titleLabel;
    QLabel*         m_subtitleLabel;
    QLineEdit*      m_nameEdit;
    QLineEdit*      m_tableEdit;
    QPushButton*    m_startBtn;
    QLabel*         m_errorLabel;
};
