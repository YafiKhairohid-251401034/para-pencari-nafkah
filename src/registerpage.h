#pragma once

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

signals:
    void proceedToOrder();

private slots:
    void onStartOrder();

private:
    void setupUi();
    void setupStyle();
    bool validate();

    OrderManager *m_mgr;

    QLabel *m_titleLabel;
    QLabel *m_subtitleLabel;
    QLineEdit *m_nameEdit;
    QLineEdit *m_tableEdit;
    QLabel *m_errorLabel;
    QPushButton *m_startBtn;
};