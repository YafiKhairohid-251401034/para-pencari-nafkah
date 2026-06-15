#pragma once
#include <QFrame>
#include <QLabel>
#include <QPushButton>
#include <QVector>
#include <QWidget>
#include "historymanager.h"
#include "ordermanager.h"

// =============================================================================
//  PaymentPage — Layar kasir: numpad tunai + ringkasan
// =============================================================================

class PaymentPage : public QWidget
{
    Q_OBJECT

public:
    explicit PaymentPage(OrderManager *mgr, QWidget *parent = nullptr);

    void refreshPage();
    void setHistoryManager(HistoryManager *historyMgr) { m_historyMgr = historyMgr; }

signals:
    void navigateToOrder();
    void navigateToHistory();
    void transactionComplete();

private slots:
    void onNumpadPressed(const QString &val);
    void onCompleteTransaction();

private:
    void setupUi();
    void setupStyle();
    void buildSidebar();
    void buildNumpad();
    void buildSummaryPanel();
    void updateCashDisplay();
    void updateChange();

    OrderManager *m_mgr;
    HistoryManager *m_historyMgr = nullptr;

    // Sidebar
    QFrame *m_sidebar;
    QPushButton *m_navRegister;
    QPushButton *m_navOrder;
    QPushButton *m_navPayment;
    QPushButton *m_navHistory;

    // Header info
    QLabel *m_totalDueLabel;
    QLabel *m_totalDueValue;
    QLabel *m_itemsLabel;
    QLabel *m_nameLabel;
    QLabel *m_tableLabel;

    // Payment tabs (Tunai / QRIS)
    QVector<QPushButton *> m_methodBtns;
    QString m_activeMethod;

    // Numpad
    QFrame *m_numpadFrame;
    QLabel *m_cashEnteredLabel;
    QWidget *m_numpadWidget;
    QLabel  *m_qrisImageLabel;
    QString m_cashString;

    // Summary
    QLabel *m_sumTotalValue;
    QLabel *m_sumCashValue;
    QLabel *m_sumChangeValue;
    QLabel *m_changeLabel;

    QLabel *m_sumCashLabel;   // label "Tunai"
    QLabel *m_sumChangeLabel; // label "Kembalian" (sudah ada m_changeLabel, pakai itu)

    QPushButton *m_completeBtn;
    QLabel *m_thankYouLabel;

    QWidget *m_sumCashRow;
    QWidget *m_sumChangeRow;
};
