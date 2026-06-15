#pragma once
#include <QFrame>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>
#include "historymanager.h"
#include "ordermanager.h"

// =============================================================================
//  HistoryPage — Daftar riwayat transaksi + panel detail (mirip struk)
// =============================================================================

class HistoryPage : public QWidget
{
    Q_OBJECT

public:
    explicit HistoryPage(HistoryManager *historyMgr, QWidget *parent = nullptr);

    // Dipanggil setiap kali halaman ditampilkan agar data selalu up-to-date
    void refreshPage();

signals:
    void navigateToRegister();
    void navigateToOrder();
    void navigateToPayment();

private slots:
    void onTransactionSelected(int row);

private:
    void setupUi();
    void setupStyle();
    void buildSidebar();
    void populateList();
    void showDetail(const TransactionRecord &rec);
    void clearDetail();

    HistoryManager *m_historyMgr;
    QVector<TransactionRecord> m_records;

    // Sidebar
    QFrame *m_sidebar;
    QPushButton *m_navRegister;
    QPushButton *m_navOrder;
    QPushButton *m_navPayment;
    QPushButton *m_navHistory;

    // List
    QListWidget *m_listWidget;
    QLabel *m_emptyLabel;

    // Detail panel
    QFrame *m_detailPanel;
    QVBoxLayout *m_detailLay;
    QWidget *m_detailContent;
};
