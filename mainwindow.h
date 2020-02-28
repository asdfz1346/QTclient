#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QStandardItemModel>
#include <QStandardItem>
#include <QMainWindow>
#include <QtNetwork/QtNetwork>
#include <windows.h>
#include <QTableView>
#include <memory>
#include <QString>
#include <memory.h>
#include<string>
#define LISTEN_NUM 5
#define THREAD_NUM 1
/*CtoS cmd*/
#define PUTIN 1
#define PUTOUT 2
#define ORDER 3
#define RESET 4
#define QUERY 5

/*StoC cmd*/
#define PUTINFAIL 0
#define PUTINSUCCESS 1
#define PUTOUTSUCCESS 2
#define PUTOUTFAIL 3
#define ORDERSUCCESS 4
#define ORDERFAIL 5
#define RESETSUCCESS   6
#define RESETFAIL     7
#define QUERYSUCCESS    8
#define QUERYFAIL       9
#define QUERYBACK       10

/*buf*/
#define SND_BUF_SIZE 32
#define RCV_BUF_SIZE 90

typedef struct CtoS{
    int cmd;
    int parkid;
    char carid[12];
    char tele[12];
}CtoS;
typedef struct StoC{
    int cmd;
    char carid[12];
    char tele[12];
    char intime[20];
    char outtime[20];
    char ordertime[20];
}StoC;





QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool createsocket(const QString ip,const QString port);
    int m_send(CtoS ctos);
    StoC m_rcv();
    QTcpSocket *tcpSocket;
    QStandardItemModel* model = new QStandardItemModel();
    int querybacknum=0;

private:
    char snd_buf[SND_BUF_SIZE];
    char rcv_buf[RCV_BUF_SIZE];
    Ui::MainWindow *ui;
    void worker(StoC stoc);

private slots:

  //  void newConnect(); //连接服务器
    void recvmassage();


    void on_pushButton_connect_clicked();
    void on_pushButton_putin_clicked();
    void on_pushButton_putout_clicked();
    void on_pushButton_reset_clicked();
    void on_pushButton_query_clicked();
};
#endif // MAINWINDOW_H
