#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{


    ui->setupUi(this);

    ui->pushButton_connect->setEnabled(true);
    ui->pushButton_putin->setEnabled(false);
    ui->pushButton_putout->setEnabled(false);
    ui->pushButton_reset->setEnabled(false);
    ui->pushButton_query->setEnabled(false);

    ui->tableView->setShowGrid(true);//显示网格线
    ui->tableView->setSortingEnabled(true);//排序

    QStringList labels = QObject::trUtf8("停车场id,车牌,电话,预定时间,入库时间").simplified().split(",");
    model->setHorizontalHeaderLabels(labels);
    ui->tableView->setModel(model);
    //设置表格的各列的宽度值
    ui->tableView->setColumnWidth(0,50);
    ui->tableView->setColumnWidth(1,100);
    ui->tableView->setColumnWidth(2,100);
    ui->tableView->setColumnWidth(3,100);
    ui->tableView->setColumnWidth(4,100);
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置选中行
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers); //设置表格的单元为只读属性，即不能编辑
    ui->tableView->sortByColumn(3,Qt::AscendingOrder);


}

MainWindow::~MainWindow()
{
    delete ui;
}
/*connect*/
bool MainWindow::createsocket(const QString ip,const QString port){
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket,SIGNAL(readyRead()),this,SLOT(recvmassage()));
    connect(tcpSocket,SIGNAL(error(QAbstractSocket::SocketError)),
             this,SLOT(displayError(QAbstractSocket::SocketError)));
    tcpSocket->connectToHost(ip, port.toInt());
    if (tcpSocket->waitForConnected(1000))  // 连接成功则进入if{}
          return true;
    return false;
}
/*发送*/
int  MainWindow::m_send(CtoS ctos){
     memset(snd_buf,0,sizeof snd_buf);
     memcpy(snd_buf,&ctos,sizeof ctos);
     tcpSocket->write(snd_buf,sizeof snd_buf);
     return 0;
}
/*接收*/
StoC MainWindow::m_rcv(){
    StoC tempstoc;
    memset(&tempstoc,0,sizeof tempstoc);
    memset(rcv_buf,0,sizeof rcv_buf);
    while(tcpSocket->bytesAvailable()){
        tcpSocket->read(rcv_buf,sizeof rcv_buf);
        memcpy(&tempstoc,rcv_buf,sizeof tempstoc);
        worker(tempstoc);
    }
    return tempstoc;
}

void MainWindow::worker(StoC stoc){
    QTime time;
    time = QTime::currentTime();
    QString str;
    str = time.toString();
    if(stoc.cmd==PUTINSUCCESS)          str = str +"  入库成功;";
    else if(stoc.cmd ==PUTINFAIL)       str = str +"  入库失败;";
    else if(stoc.cmd ==PUTOUTFAIL)      str = str +"  出库失败;";
    else if(stoc.cmd ==ORDERSUCCESS)    str = str +"  预定成功;";
    else if(stoc.cmd ==ORDERFAIL)       str = str +"  预定失败;";
    else if(stoc.cmd ==QUERYFAIL)       str = str +"  查询失败;";
    else if(stoc.cmd ==RESETFAIL)       str = str +"  重置失败;";
    else if(stoc.cmd ==RESETSUCCESS){
        str = str+"  重置成功；";
    }
    else if(stoc.cmd ==QUERYSUCCESS){
        str=str+"  查询成功;";
    }
    else if(stoc.cmd ==QUERYBACK){
        model->setItem(querybacknum, 0, new QStandardItem());
        model->setItem(querybacknum, 1, new QStandardItem(stoc.carid));
        model->setItem(querybacknum, 2, new QStandardItem(stoc.tele));
        model->setItem(querybacknum, 3, new QStandardItem(stoc.ordertime));
        model->setItem(querybacknum, 4, new QStandardItem(stoc.intime));
        querybacknum++;
        Sleep(9);/*      jojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojjojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojojoj*/
    }
    else if(stoc.cmd ==PUTOUTSUCCESS)   {
        str=str+"  出库成功;";
        QString timestr;
        if(stoc.ordertime[0]!=0){
            timestr ="预定时间:";
            timestr += stoc.ordertime;
            ui->textEdit->append(timestr);
        }
        if(stoc.intime[0]!=0){
            timestr ="入库时间:";
            timestr += stoc.intime;
            ui->textEdit->append(timestr);
        }
        if(stoc.outtime[0]!=0){
            timestr ="出库时间:";
            timestr += stoc.outtime;
            ui->textEdit->append(timestr);
        }
    }


    ui->textEdit->append(str);
    return;
}


void MainWindow::recvmassage(){
    StoC stoc;
    stoc = m_rcv();
    //worker(stoc);
    return;
}

void MainWindow::on_pushButton_connect_clicked()
{
    if(createsocket(ui->lineEdit_ip->text(),ui->lineEdit_port->text())){
        ui->pushButton_connect->setEnabled(false);
        ui->pushButton_putin->setEnabled(true);
        ui->pushButton_putout->setEnabled(true);
        ui->pushButton_reset->setEnabled(true);
        ui->pushButton_query->setEnabled(true);
    }
}

void MainWindow::on_pushButton_putin_clicked()
{
    CtoS ctos;
     memset(&ctos,0,sizeof ctos);
     ctos.cmd    = PUTIN;
     ctos.parkid = ui->spinBox_parkid->value();
     memcpy(&ctos.carid, ui->lineEdit_carid->text().toStdString().c_str(),sizeof(ctos.carid));
     memcpy(&ctos.tele, ui->lineEdit_tele->text().toStdString().c_str(),sizeof(ctos.tele));
     m_send(ctos);
}



void MainWindow::on_pushButton_putout_clicked()
{
    CtoS ctos;
    memset(&ctos,0,sizeof ctos);
    ctos.cmd = PUTOUT;
    ctos.parkid = ui->spinBox_parkid->value();
    memcpy(&ctos.carid, ui->lineEdit_carid->text().toStdString().c_str(),sizeof(ctos.carid));
    m_send(ctos);
}

void MainWindow::on_pushButton_reset_clicked()
{
    model->removeRows(0,model->rowCount());
    CtoS ctos;
    memset(&ctos,0,sizeof ctos);
    ctos.cmd = RESET;
    ctos.parkid = ui->spinBox_parkid->value();
    m_send(ctos);
}

void MainWindow::on_pushButton_query_clicked()
{
    model->removeRows(0,model->rowCount());
    querybacknum=0;
    CtoS ctos;
    memset(&ctos,0,sizeof ctos);
    ctos.cmd  = QUERY;
    ctos.parkid = ui->spinBox_parkid->value();
    m_send(ctos);
}


