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
    explicit ReceiptDialog(OrderManager *mgr, long long cashPaid, QWidget *parent = nullptr);
};