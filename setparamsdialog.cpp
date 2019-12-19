#include "setparamsdialog.h"
#include "ui_setparamsdialog.h"
#include "myglobalvars.h"
#include <QFile>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>


SetParamsDialog::SetParamsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SetParamsDialog)
{
    ui->setupUi(this);
    ui->frameAddNew->hide();

    //set background color
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 0, 400);
    gradient.setColorAt(0, QColor(120, 255, 255));
    gradient.setColorAt(0.7, QColor(180, 255, 255));
    gradient.setColorAt(1, QColor(230, 255, 255));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);
    //Finished setting background

#ifdef __linux__
    ui->label_4->setText("Operating system : Linux");


#elif _WIN32
    ui->label_4->setText("Operating system : Windows");

#elif __APPLE__
    ui->label_4->setText("Operating system : Apple");

#else
    ui->label_4->setText("Operating system : Not recognized!");

#endif

    QFile propfile(propertiesFile);
    if (propfile.exists())
       load_properties();
     else
       on_restoredefaults_clicked();

    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults),SIGNAL(clicked()),SLOT(on_restoredefaults_clicked()));

}

void SetParamsDialog::on_pushButtonAddNew_clicked()
{
    const QString EngineName=ui->lineEditName->text();
    ui->comboBoxEngine->insertItem(ui->comboBoxEngine->count()-1,EngineName);
}

void SetParamsDialog::on_comboBoxEngine_currentIndexChanged(int index)
{
    int engineNum = ui->comboBoxEngine->count();

    if (index==engineNum-1)
    {
        ui->frameAddNew->show();
        ui->lineEditVal_4->clear();
        ui->lineEditVal_6->clear();
    }else
        ui->frameAddNew->hide();

}

void SetParamsDialog::on_restoredefaults_clicked()
{

    QMessageBox::StandardButton overwriteCondition = QMessageBox::question(this,"Default Setting","Reset to default? All changes will be lost!",QMessageBox::Yes|QMessageBox::No);
    if (overwriteCondition==QMessageBox::No)
        return;


    QFile propfile(propertiesFile);
    if (!propfile.open(QFile::WriteOnly | QFile::Text))
    {
        QMessageBox::warning(this,"Error","Cannot open property file.");
        return;
    }

    QTextStream propStream (&propfile);
    propStream<<"Default"<<'\n';


#ifdef __linux__
ui->comboBoxEngine->clear();



#elif _WIN32
ui->comboBoxEngine->clear();

#elif __APPLE__
ui->comboBoxEngine->clear();

#else


#endif
    propStream.flush();
    propfile.close();
}

void SetParamsDialog::load_properties()
{

}

void SetParamsDialog::save_properties()
{


}

SetParamsDialog::~SetParamsDialog()
{
    delete ui;
}


void SetParamsDialog::on_buttonBox_accepted()
{
    save_properties();
}


void SetParamsDialog::on_toolButtonJSIM_clicked()
{
    QString SimFile = QFileDialog::getOpenFileName(this, tr("Open Netlist File..."),QString(), tr("Netlist Files (*.js *.inp);;All Files (*)"));
    if (!SimFile.isEmpty())
    {


    }
    else
    {
        QMessageBox::warning(this,"Error","No simulation engine is selected.");
    }
}
