#pragma once
#include <QDialog>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>
#include "ordermanager.h"

class ReceiptDialog : public QDialog
{
    Q_OBJECT
public:
    // cashPaid diabaikan jika paymentMethod bukan "Tunai"
    explicit ReceiptDialog(OrderManager *mgr,
                           long long cashPaid,
                           const QString &paymentMethod,
                           QWidget *parent = nullptr);
};