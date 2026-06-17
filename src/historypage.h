#pragma once
#include <QDate>
#include <QDateEdit>
#include <QFrame>
#include <QHBoxLayout>
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
//  + Filter penjualan per tanggal
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
    void onFilterDateChanged(const QDate &date);
    void onShowToday();
    void onShowAll();

private:
    void setupUi();
    void setupStyle();
    void buildSidebar();
    QHBoxLayout *buildFilterBar();
    void populateList();
    void applyDateFilter();
    void showDetail(const TransactionRecord &rec);
    void clearDetail();
    void updateSummaryBar();

    HistoryManager *m_historyMgr;
    QVector<TransactionRecord> m_allRecords;     // seluruh riwayat (belum difilter)
    QVector<TransactionRecord> m_records;        // hasil setelah filter tanggal aktif

    // Sidebar
    QFrame *m_sidebar;
    QPushButton *m_navRegister;
    QPushButton *m_navOrder;
    QPushButton *m_navPayment;
    QPushButton *m_navHistory;

    // Filter bar
    QDateEdit   *m_dateFilterEdit;
    QPushButton *m_btnToday;
    QPushButton *m_btnAllDates;
    bool         m_filterActive = false; // true jika filter tanggal spesifik aktif

    // Ringkasan hasil filter
    QLabel *m_summaryLabel;

    // List
    QListWidget *m_listWidget;
    QLabel *m_emptyLabel;

    // Detail panel
    QFrame *m_detailPanel;
    QVBoxLayout *m_detailLay;
    QWidget *m_detailContent;
};