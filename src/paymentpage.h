#pragma once
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QVector>
#include <QPixmap>
#include "ordermanager.h"
#include "historymanager.h"

class StrukHttpServer;

//  PaymentPage — Layar kasir: numpad tunai + QRIS + ringkasan

class PaymentPage : public QWidget {
    Q_OBJECT

public:
    explicit PaymentPage(OrderManager* mgr, QWidget* parent = nullptr);

    void refreshPage();

    // Diberikan oleh MainWindow agar transaksi bisa disimpan ke riwayat
    void setHistoryManager(HistoryManager *historyMgr) { m_historyMgr = historyMgr; }

signals:
    void navigateToOrder();
    void navigateToHistory();
    void transactionComplete();

private slots:
    void onNumpadPressed(const QString& val);
    void onCompleteTransaction();

private:
    void setupUi();
    void setupStyle();
    void buildSidebar();
    void buildNumpad();
    void buildSummaryPanel();
    void updateCashDisplay();
    void updateChange();
    void updateQrisCodeImage();      // nyalakan StrukHttpServer + render ulang QR dinamis
    static QPixmap renderQrPixmap(const QString &text, int moduleSize = 6, int border = 3);

    OrderManager*       m_mgr;
    HistoryManager*     m_historyMgr = nullptr;

    // Sidebar
    QFrame*             m_sidebar;
    QPushButton*        m_navRegister;
    QPushButton*        m_navOrder;
    QPushButton*        m_navPayment;
    QPushButton*        m_navHistory;

    // Header info
    QLabel*             m_totalDueLabel;
    QLabel*             m_totalDueValue;
    QLabel*             m_itemsLabel;
    QLabel*             m_nameLabel;
    QLabel*             m_tableLabel;

    // Payment tabs (Tunai / QRIS)
    QVector<QPushButton*> m_methodBtns;
    QString             m_activeMethod;

    // Numpad
    QFrame*             m_numpadFrame;
    QLabel*             m_cashEnteredLabel;
    QWidget*            m_numpadWidget;
    QString             m_cashString;

    // QRIS — kontainer berisi gambar QR + struk ringkas
    QWidget*            m_qrisImageLabel;   // nama dipertahankan agar kompatibel, sekarang berupa container
    QLabel*             m_qrImageLbl = nullptr;     // label yang menampilkan pixmap QR (dinamis)
    QLabel*             m_qrUrlHintLbl = nullptr;   // teks alamat/struk di bawah QR (fallback manual)
    StrukHttpServer*    m_strukServer = nullptr;    // mini HTTP server yang serve halaman struk ke HP

    // Summary
    QLabel*             m_sumTotalValue;
    QLabel*             m_sumCashValue;
    QLabel*             m_sumChangeValue;
    QLabel*             m_changeLabel;

    QWidget*            m_sumCashRow;
    QWidget*            m_sumChangeRow;

    QPushButton*        m_completeBtn;
    QLabel*             m_thankYouLabel;
};