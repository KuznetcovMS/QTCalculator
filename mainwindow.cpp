#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QIntValidator>
#include <QDoubleValidator>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->L_Errors->hide();
    ui->L_Errors->setWordWrap(true);
    ui->L_Warnings->hide();
    ui->L_Warnings->setWordWrap(true);
    //Установка символов валют
    ui->L_CurrencyLabel->setText(ui->CB_CurrencyType->currentText());
    if (ui->L_CurrencyLabel->text() == "₽")
    {
        ui->L_CurrencyPart->setText("коп.");
    }
    else if (ui->L_CurrencyLabel->text() == "$")
    {
        ui->L_CurrencyPart->setText("цент");
    }

    //Установка валидаторов на edit line
    ui->LE_SumVkl->setValidator(new QIntValidator(1, 999999999, this));
    ui->LE_CurrencyPart->setValidator(new QIntValidator(1, 99, this));
    ui->LE_Proc->setValidator(new QDoubleValidator(0.0, 100.0, 2, this));
    ui->LE_PlacementPeriod->setValidator(new QIntValidator(1, 99999, this));

}

MainWindow::~MainWindow()
{
    delete ui;
}

//Смена иконок валют через комбобокс
void MainWindow::on_CB_CurrencyType_currentTextChanged(const QString &arg1)
{
    ui->L_CurrencyLabel->setText(arg1);
    if (ui->L_CurrencyLabel->text() == "₽")
    {
        ui->L_CurrencyPart->setText("коп.");
    }
    else if (ui->L_CurrencyLabel->text() == "$")
    {
        ui->L_CurrencyPart->setText("цент");
    }
}

void MainWindow::on_BTN_Calculate_clicked()
{
    QString RedBordStyle = "border-color: rgb(200, 0, 0); color: rgb(210, 210, 210); border-style: solid; border-width: 1px; border-radius: 3px";
    QString DefStyle = "border-color: rgb(30, 30, 30); color: rgb(210, 210, 210); border-style: solid; border-width: 1px; border-radius: 3px";

    //Скрытие сообщений об ошибках
    ui->L_Errors->hide();
    ui->L_Warnings->hide();

    //Установка стандартных стилей для всех Line Edit
    ui->LE_SumVkl->setStyleSheet(DefStyle);
    ui->LE_CurrencyPart->setStyleSheet(DefStyle);
    ui->LE_PlacementPeriod->setStyleSheet(DefStyle);
    ui->LE_Proc->setStyleSheet(DefStyle);

    //Установка красных краёв для пустых Line Edit
    if (ui->LE_SumVkl->text().length() == 0) ui->LE_SumVkl->setStyleSheet(RedBordStyle);
    if (ui->LE_CurrencyPart->text().length() == 0) ui->LE_CurrencyPart->setStyleSheet(RedBordStyle);
    if (ui->LE_PlacementPeriod->text().length() == 0) ui->LE_PlacementPeriod->setStyleSheet(RedBordStyle);
    if (ui->LE_Proc->text().length() == 0) ui->LE_Proc->setStyleSheet(RedBordStyle);


    //Предупреждение о пустых полях
    QString ErrorMes = "", WarnMes = "";
    if (ui->LE_SumVkl->text().length() == 0 || ui->LE_CurrencyPart->text().length() == 0 || ui->LE_PlacementPeriod->text().length() == 0 || ui->LE_Proc->text().length() == 0)
    {
        WarnMes.append("Пустые поля интерпретируются как 0; ");
    }

    //Считывание размера вклада
    long long int SumVkl = ui->LE_SumVkl->text().toLongLong() * 100 + ui->LE_CurrencyPart->text().toLongLong();
    double Sum = SumVkl;
    long long int SumVklBuf = SumVkl;



    //Проверка состояния чекбокса
    if (ui->CHB_Poln->isChecked()){
        Sum = (SumVkl / 100000) * 100000;
        SumVklBuf = (SumVkl / 100000) * 100000;

    }

    //Перевод введённого пользователем периода в дни
    int NumDays = 0;
    if (ui->CB_PeriodType->currentText() == "Дней") NumDays = ui->LE_PlacementPeriod->text().toInt();
    else if (ui->CB_PeriodType->currentText() == "Месяцев") NumDays = ui->LE_PlacementPeriod->text().toInt() * 30;
    else if (ui->CB_PeriodType->currentText() == "Лет") NumDays = ui->LE_PlacementPeriod->text().toInt() * 365;

    //Вычисление количества начислений процентов
    int ProcCount = 0;
    if (ui->CB_ProcType->currentText() == "В день") ProcCount = NumDays;
    else if (ui->CB_ProcType->currentText() == "В месяц") ProcCount = NumDays / 30;
    else if (ui->CB_ProcType->currentText() == "В год") ProcCount = NumDays / 365;

    //Предупреждение о колличестве начислений
    if (ProcCount > 365) WarnMes.append("Колличество начислений процентов больше 365, проверьте срок хранения вклада и период начисления процентов; ");

    //Вычисление начисленных процентов
    double proc = ui->LE_Proc->text().replace(",", ".").toDouble();
    proc = proc / 100 + 1;
    Sum *= pow(proc, ProcCount);

    //Формирование строк для вывода результатов расчётов
    unsigned long long int NachProc = Sum - SumVklBuf;

    QString NachProcRes = "", SumNaVklRes = "";

    if (Sum > 0 && Sum < 18446744073709551615.0){
        NachProcRes.append(QString::number(NachProc / 100) + " ");
        NachProcRes.append(ui->L_CurrencyLabel->text() + " ");
        NachProcRes.append(QString::number(NachProc % 100) + " " + ui->L_CurrencyPart->text());

        SumNaVklRes.append(QString::number((NachProc + SumVkl) / 100) + " ");
        SumNaVklRes.append(ui->L_CurrencyLabel->text() + " ");
        SumNaVklRes.append(QString::number((NachProc + SumVkl) % 100) + " " + ui->L_CurrencyPart->text());
    }
    else if(Sum == 0)
    {
        NachProcRes.append(QString::number(0) + " ");
        NachProcRes.append(ui->L_CurrencyLabel->text() + " ");
        NachProcRes.append(QString::number(0) + " " + ui->L_CurrencyPart->text());

        SumNaVklRes.append(QString::number(SumVkl / 100) + " ");
        SumNaVklRes.append(ui->L_CurrencyLabel->text() + " ");
        SumNaVklRes.append(QString::number(SumVkl % 100) + " " + ui->L_CurrencyPart->text());
    }
    if (NachProc + SumVkl >= ULLONG_MAX || Sum >= 18446744073709551615.0)
    {
        ErrorMes.append("Для введённых значений, размер начисленных процентов слишком велик");
        NachProcRes = "inf";
        SumNaVklRes = "inf";
    }

    //Вывод результатов расчётов
    ui->L_NachProcRes->setText(NachProcRes);
    ui->L_SumNaVklRes->setText(SumNaVklRes);

    //Вывод сообщений об ошибках
    if (WarnMes.length() > 0)
    {
        ui->L_Warnings->show();
        ui->L_Warnings->setText("Warnings: " + WarnMes);
    }

    if (ErrorMes.length() > 0)
    {
        ui->L_Errors->show();
        ui->L_Errors->setText("Errors: " + ErrorMes);
    }
}
