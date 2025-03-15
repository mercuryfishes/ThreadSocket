#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include "mytcpserver.h"
#include "sendfile.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

signals:
    void start(QString name); // 通知子线程中的对象可以开始工作了

private slots:
    void on_start_clicked();

    void on_selectFile_clicked();

    void on_sendFile_clicked();

private:
    Ui::MainWindow *ui;
    MyTcpServer* m_server;
};

#endif // MAINWINDOW_H
