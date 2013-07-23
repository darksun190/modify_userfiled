#include "mainwindow.h"
#include "ui_mainwindow.h"


bool intmorethan(const QString &s1, const QString &s2)
{
    if (s1.length()==s2.length())
    {
        return s1<s2;
    }
    else
    {
        return s1.length()<s2.length();
    }

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("Userfields Interface");
    init_flag=0;
    //ui->tableWidget->hideColumn(0);
    QSettings setting(".\\config.ini",QSettings::IniFormat);

    fileName = setting.value("filename").toString();

    QFile origin_name(fileName);
    if (!origin_name.exists())
    {
        if (origin_name.copy(":/userfields.ini",fileName))
        {
            QMessageBox msgbox(QMessageBox::Warning, tr("Warning"),
                               tr("as your file didn't exist.\nI use a default one to instead."),0,this);
            msgbox.addButton(tr("Continue"), QMessageBox::AcceptRole);
            msgbox.exec();
        }

    }
    origin_name.setPermissions(QFile::Permission(0x7777));
    origin_name.remove("./qt_settings_format.ini");
    origin_name.copy("./qt_settings_format.ini");

    QFile temp_file("./qt_settings_format.ini");
    temp_file.setPermissions(QFile::Permission(0x7777));
    calypso = new QSettings("./qt_settings_format.ini",QSettings::IniFormat);
    u_field_list;
    u_value_list;

    all_list;
    all_list = calypso->childGroups();
    QString str;
    foreach(str,all_list)
    {
        calypso->beginGroup(str);
        QStringList sub_list = calypso->childKeys();
        calypso->endGroup();
        if (sub_list.contains("name"))
            u_field_list.push_back(str);
        else
            u_value_list.push_back(str);
    }

    int i=0;
    QString str_u;
    ui->tableWidget_2->blockSignals(true);
    foreach(str_u,u_field_list)
    {
        ui->tableWidget_2->insertRow(i);
        QTableWidgetItem *u_item;
        u_item = new QTableWidgetItem (str_u);
        ui->tableWidget_2->setItem(i,0,u_item);
        i++;
    }

    ui->tableWidget_2->setCurrentCell(0,0);
    ui->tableWidget_2->blockSignals(false);
    {
        //display the first u_field
        QString str;
        bool setlist;
        QString list_name;
        {
            int size = ui->tableWidget->rowCount();

            for(int i;i<size;i++)
            {
                ui->tableWidget->removeRow(0);
            }
        }
        ui->lineEdit->clear();
        ui->lineEdit_2->clear();
        ui->lineEdit_3->clear();
        str = ui->tableWidget_2->currentItem()->text();

        if (!calypso->childGroups().contains(str))
            return;
        calypso->beginGroup(str);
        ui->lineEdit->setText(calypso->value("name").toString());
        if (calypso->value("editMode").toBool())
            ui->checkBox1->setChecked(true);
        else
            ui->checkBox1->setChecked(false);
        if (calypso->value("runMode").toBool())
            ui->checkBox2->setChecked(true);
        else
            ui->checkBox2->setChecked(false);
        if (calypso->value("selectiveList").toBool())
        {
            ui->checkBox3->setChecked(true);
            setlist = true;
            list_name = calypso->value("selectiveListValues").toString();
            ui->lineEdit_2->setText(list_name);
        }
        else
        {
            ui->checkBox3->setChecked(false);
            setlist = false;
        }
        if (calypso->value("editable").toBool())
        {
            ui->checkBox4->setChecked(true);
        }
        else
            ui->checkBox4->setChecked(false);

        ui->lineEdit_3->setText(calypso->value("defaultValue").toString());
        calypso->endGroup();


        if (setlist)
        {
            ui->label_6->show();
            ui->lineEdit_3->show();
            calypso->beginGroup(list_name);

            int i=0;
            QString temp;
            sorted_list = calypso->childKeys();

            qSort(sorted_list.begin(),sorted_list.end(),intmorethan);
            foreach(temp,sorted_list)
            {
                ui->tableWidget->insertRow(i);
                QTableWidgetItem *keyitem = new QTableWidgetItem(temp);
                QTableWidgetItem *valueitem = new QTableWidgetItem(calypso->value(temp).toString());
                ui->tableWidget->setItem(i,0,keyitem);
                ui->tableWidget->setItem(i,1,valueitem);

                i++;
            }
            calypso->endGroup();
        }
        else
        {
            //ui->lineEdit_3->hide();
        }

    }
    do_save = true;
    do_display = true;
    QPixmap pixpic(":/logo.bmp");
    ui->label_13->setPixmap(pixpic);
}

MainWindow::~MainWindow()
{
    delete ui;
}



void MainWindow::on_checkBox3_stateChanged(int arg1)
{
    if (ui->checkBox3->isChecked())
    {
        ui->lineEdit_2->show();
        ui->label_6->show();

    }
    else
    {
        ui->lineEdit_2->hide();
    }
}

void MainWindow::on_AddButton1_clicked()
{
    do_display = false;
    //ui->tableWidget_2->blockSignals(true);
    int j = ui->tableWidget_2->rowCount();
    ui->tableWidget_2->insertRow(j);
    QString str("u_newfield");
    str.append(QString::number(j+1));

    QTableWidgetItem *new_item = new QTableWidgetItem(str);
    ui->tableWidget_2->setItem(j,0,new_item);
    ui->tableWidget_2->setCurrentCell(j,0);
    calypso->beginGroup(str);
    calypso->setValue("name",str);
    ui->lineEdit->setText(str);
    calypso->endGroup();

    //ui->tableWidget_2->blockSignals(false);
    ui->checkBox1->setChecked(false);
    ui->checkBox2->setChecked(false);
    ui->checkBox3->setChecked(false);
    ui->checkBox4->setChecked(false);
}

void MainWindow::on_tableWidget_2_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    //save the old u_field
    bool list_changed = false;
    if (do_save)
    {
        calypso->beginGroup(previous->text());
        calypso->setValue("name",ui->lineEdit->text());
        calypso->setValue("editMode",ui->checkBox1->isChecked());
        calypso->setValue("runMode",ui->checkBox2->isChecked());
        calypso->setValue("selectiveList",ui->checkBox3->isChecked());
        if (ui->checkBox3->isChecked())
        {
            if (calypso->value("selectiveListValues").toString().compare(ui->lineEdit_2->text())!=0)
            {
                list_changed = true;
            }

        }
        calypso->setValue("editable",ui->checkBox4->isChecked());
        calypso->setValue("defaultValue",ui->lineEdit_3->text());
        calypso->endGroup();
    }
    else
    {
        do_save = true;
    }

    if (list_changed)
    {
        calypso->remove(calypso->value(QString("%1/selectiveListValues").arg(previous->text())).toString());
        calypso->setValue(QString("%1/selectiveListValues").arg(previous->text()),ui->lineEdit_2->text());
        calypso->beginGroup(ui->lineEdit_2->text());
        for(int i=0;i<ui->tableWidget->rowCount();++i)
        {
            calypso->setValue(ui->tableWidget->item(i,0)->text(),ui->tableWidget->item(i,1)->text());
        }
        calypso->endGroup();

    }

    //display the new u_field

    ui->tableWidget_2->setCurrentItem(current);
    QString str;
    bool setlist;
    QString list_name;
    {
        int size = ui->tableWidget->rowCount();

        for(int i;i<size;i++)
        {
            ui->tableWidget->removeRow(0);
        }
    }
    ui->lineEdit->clear();
    ui->lineEdit_2->clear();
    ui->lineEdit_3->clear();
    if (!do_display)
    {
        do_display = true;
        return;
    }

    str = current->text();
    if (!calypso->childGroups().contains(str))
        return;
    calypso->beginGroup(str);

    ui->lineEdit->setText(calypso->value("name").toString());
    if (calypso->value("editMode").toBool())
        ui->checkBox1->setChecked(true);
    else
        ui->checkBox1->setChecked(false);
    if (calypso->value("runMode").toBool())
        ui->checkBox2->setChecked(true);
    else
        ui->checkBox2->setChecked(false);
    if (calypso->value("selectiveList").toBool())
    {
        ui->checkBox3->setChecked(true);
        setlist = true;
        list_name = calypso->value("selectiveListValues").toString();
        ui->lineEdit_2->setText(list_name);
    }
    else
    {
        ui->checkBox3->setChecked(false);
        setlist = false;
    }
    if (calypso->value("editable").toBool())
    {
        ui->checkBox4->setChecked(true);
    }
    else
        ui->checkBox4->setChecked(false);

    ui->lineEdit_3->setText(calypso->value("defaultValue").toString());
    calypso->endGroup();


    if (setlist)
    {
        ui->label_6->show();
        ui->lineEdit_3->show();
        calypso->beginGroup(list_name);

        int i=0;
        QString temp;
        QStringList sorted_list = calypso->childKeys();

        qSort(sorted_list.begin(),sorted_list.end(),intmorethan);
        foreach(temp,sorted_list)
        {
            ui->tableWidget->insertRow(i);
            QTableWidgetItem *keyitem = new QTableWidgetItem(temp);
            QTableWidgetItem *valueitem = new QTableWidgetItem(calypso->value(temp).toString());
            ui->tableWidget->setItem(i,0,keyitem);
            ui->tableWidget->setItem(i,1,valueitem);

            i++;
        }
        calypso->endGroup();
    }


}

void MainWindow::on_DelButton1_clicked()
{
    if (ui->tableWidget_2->rowCount() ==1 )
        return;
    do_save = false;
    calypso->remove(ui->tableWidget_2->currentItem()->text());
    ui->tableWidget_2->removeRow(ui->tableWidget_2->currentRow());

}

void MainWindow::on_AddButton2_clicked()
{
    ui->tableWidget->insertRow(ui->tableWidget->rowCount());
    QString str("new_value");
    QTableWidgetItem *new_item = new QTableWidgetItem(str);
    QTableWidgetItem *key_item = new QTableWidgetItem(QString::number(ui->tableWidget->rowCount()));
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,1,new_item);
    ui->tableWidget->setItem(ui->tableWidget->rowCount()-1,0,key_item);
    calypso->beginGroup(ui->lineEdit_2->text());
    calypso->setValue(key_item->text(),new_item->text());
    calypso->endGroup();
}

void MainWindow::on_DelButton2_clicked()
{
    calypso->beginGroup(ui->lineEdit_2->text());
    calypso->remove(ui->tableWidget->item(ui->tableWidget->currentRow(),0)->text());
    calypso->endGroup();
    ui->tableWidget->removeRow(ui->tableWidget->currentRow());
    ui->tableWidget->setCurrentItem(ui->tableWidget->currentItem());
}

void MainWindow::on_pushButton_clicked()
{

    //save current status first
    calypso->beginGroup(ui->tableWidget_2->currentItem()->text());
    calypso->setValue("name",ui->lineEdit->text());
    calypso->setValue("editMode",ui->checkBox1->isChecked());
    calypso->setValue("runMode",ui->checkBox2->isChecked());
    calypso->setValue("selectiveList",ui->checkBox3->isChecked());
    if (ui->checkBox3->isChecked())
        calypso->setValue("selectiveListValues",ui->lineEdit_2->text());
    calypso->setValue("editable",ui->checkBox4->isChecked());
    calypso->setValue("defaultValue",ui->lineEdit_3->text());
    calypso->endGroup();



    //start to write userfields.ini
    QFile input_file("./qt_settings_format.ini");
    input_file.open(QFile::ReadOnly|QFile::Text);
    input_file.flush();
    QTextStream in(&input_file);
    QFile target_file(fileName);

    //QFile target_file("C:\\sunxin\\temp.txt");
    target_file.open(QFile::WriteOnly|QFile::Text);
    QTextStream out(&target_file);
    //write some information for the progarm
    out<<
          "\"-------------------------------------------------------------------\"\n" <<
          "\" this userfields was generated by Userfields Interface automaticly \"\n" <<
          "\" you can use this file for Calypso to define header by yourself    \"\n" <<
          "\" if you had any problem or want give some suggestion,              \"\n" <<
          "\" please contact xin.sun@zeiss.com                                  \"\n" <<
          "\"-------------------------------------------------------------------\"\n";
    //first userfields names;
    out<<"[fieldnames]\n";
    int i;
    for(i=0;i<ui->tableWidget_2->rowCount();++i)
    {
        out<<ui->tableWidget_2->item(i,0)->text()<<"\n";
    }
    out<<"\n";
    calypso->sync();

    QString buf;

    while(1)
    {
        buf = in.readLine();
        if (buf.isNull())
            break;
        out<<buf<<"\n";
    }


}

void MainWindow::on_tableWidget_2_cellChanged(int row, int column)
{
    if (!do_display)
    {
        do_display = true;
        return;
    }
    do_save = false;

    QString old_name = u_field_list.at(row);
    QString new_name = ui->tableWidget_2->item(row,column)->text();
    u_field_list[row]=new_name;

    calypso->setValue(QString("%1/name").arg(new_name),calypso->value(QString("%1/name").arg(old_name)));
    calypso->setValue(QString("%1/editMode").arg(new_name),calypso->value(QString("%1/editMode").arg(old_name)));
    calypso->setValue(QString("%1/runMode").arg(new_name),calypso->value(QString("%1/runMode").arg(old_name)));
    calypso->setValue(QString("%1/selectiveList").arg(new_name),calypso->value(QString("%1/selectiveList").arg(old_name)));
    calypso->setValue(QString("%1/selectiveListValues").arg(new_name),calypso->value(QString("%1/selectiveListValues").arg(old_name)));
    calypso->setValue(QString("%1/defaultValue").arg(new_name),calypso->value(QString("%1/defaultValue").arg(old_name)));
    calypso->setValue(QString("%1/editable").arg(new_name),calypso->value(QString("%1/editable").arg(old_name)));
    calypso->remove(old_name);
}

void MainWindow::paintEvent(QPaintEvent *)
{
    ui->tableWidget->setColumnWidth(0,(ui->tableWidget->width()-ui->tableWidget->verticalHeader()->width())/2-1);
    ui->tableWidget->setColumnWidth(1,(ui->tableWidget->width()-ui->tableWidget->verticalHeader()->width())/2-1);
    ui->tableWidget_2->setColumnWidth(0,ui->tableWidget_2->width()-ui->tableWidget_2->verticalHeader()->width());
}

void MainWindow::on_line_filter_textChanged(const QString &arg1)
{
    int size = ui->tableWidget_2->rowCount();
    if (ui->line_filter->text().isNull())
    {
        for(int i=0;i<size;++i)
        {
            ui->tableWidget_2->showRow(i);
        }
        return;

    }
    for(int i=0;i<size;++i)
    {
        if (ui->tableWidget_2->item(i,0)->text().contains(ui->line_filter->text(),Qt::CaseInsensitive))
        {
            ui->tableWidget_2->showRow(i);
        }
        else
        {
            ui->tableWidget_2->hideRow(i);
        }
    }
    return;
}
