#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QString>
#include <QDebug>
#include <QDir>
#include <QObject>
#include <QListWidget>
#include <QTableWidget>
#include <QVector>
#include <QPair>
#include <QThread>
#include <QTime>
#include <QPainter>
#include <QPicture>
#include <QColor>
#include <QMessageBox>
class CTableWidgetItem: public QTableWidgetItem
{
    public:
        CTableWidgetItem();
        CTableWidgetItem(const QString &text,int type= Type):QTableWidgetItem( text,type){};
        virtual bool operator<(const QTableWidgetItem &item) const;
};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSettings *calypso;
    QString fileName;
    QStringList u_field_list;
    QStringList u_value_list;
    int init_flag;
    QStringList all_list;
    bool do_save;
    QStringList sorted_list;
    bool do_display;

private slots:
    void on_checkBox3_stateChanged(int arg1);

    void on_AddButton1_clicked();

    void on_tableWidget_2_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_DelButton1_clicked();

    void on_AddButton2_clicked();

    void on_DelButton2_clicked();

    void on_pushButton_clicked();


    void on_tableWidget_2_cellChanged(int row, int column);

    void on_line_filter_textChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;
    void paintEvent(QPaintEvent *);
};


#endif // MAINWINDOW_H
