#include "klayoutdlg.h"
#include "ui_klayoutdlg.h"
#include "inductexgui.h"
#include "myglobalvars.h"
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>

KlayoutDlg::KlayoutDlg(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KlayoutDlg)
{
    ui->setupUi(this);
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 400, 400);
    gradient.setColorAt(0, QColor(50, 230, 255));
    gradient.setColorAt(0.7, QColor(150, 255, 255));
    gradient.setColorAt(1, QColor(230, 255, 255));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);

    QString inductexStatFile=documentFolderPath+"/inductex.stat";
    QStringList loadmaterial={};

    if (QFile(inductexStatFile).exists())
    {
        QFile file(inductexStatFile);
        if (!file.open(QFile::ReadOnly | QFile::Text)){
             QMessageBox::warning(this,"No stat file detected!",file.errorString());
        }
        else{
            QTextStream stream(&file);
            while(!stream.atEnd()) {
                QString line = stream.readLine();
                loadmaterial.append(line);
            }
            file.close();
            stream.flush();

            KlayoutPath=loadmaterial.at(6);
            LayerPath=loadmaterial.at(7);

            if (LayerPath.isEmpty())
            {
                ui->setLayerFile->setChecked(false);
                ui->lineEditLayer->setEnabled(false);
                ui->toolButtonLayer->setEnabled(false);
            }
            else
            {
                ui->setLayerFile->setChecked(true);
                ui->lineEditLayer->setEnabled(true);
                ui->toolButtonLayer->setEnabled(true);
            }

            ui->lineEditPath->setText(KlayoutPath);
            ui->lineEditLayer->setText(LayerPath);

        }
    }else{
        ui->setLayerFile->setChecked(false);
        ui->lineEditLayer->setEnabled(false);
        ui->toolButtonLayer->setEnabled(false);
    }

}

KlayoutDlg::~KlayoutDlg()
{
    delete ui;
}

void KlayoutDlg::on_setLayerFile_clicked(bool checked)
{
    if (checked)
    {
            ui->lineEditLayer->setEnabled(true);
            ui->toolButtonLayer->setEnabled(true);

    }
    else if (!checked)
    {
        ui->lineEditLayer->setEnabled(false);
        ui->toolButtonLayer->setEnabled(false);
        LayerPath="";
    }
}

void KlayoutDlg::on_toolButtonPath_clicked()
{
    QString KlayoutFile = QFileDialog::getOpenFileName(this, tr("Open Klayout Executable File..."),documentFolderPath, tr("Layout files (*.exe);;All files (*)"));
    if (!KlayoutFile.isEmpty())
    {
        ui->lineEditPath->setText(KlayoutFile);
        KlayoutPath=ui->lineEditPath->text();

        //LoadGDSImage(gdsFile);
//        QFileInfo fi(KlayoutFile);
//        QString ExeFileName=fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive);
//        if (!QFile(ExeFileName).exists())
//        {
//            QMessageBox::warning(this,"Executable not found","Cannot find \""+ExeFileName+"\" in the selected location.");
//            return;
//        }
        #ifdef __linux__
          ui->lineEditPath->setText("klayout");
        #endif
    } else{

    }
}

void KlayoutDlg::on_toolButtonLayer_clicked()
{
    QString LayerFile = QFileDialog::getOpenFileName(this, tr("Open Klayout Executable File..."),documentFolderPath, tr("Layout files (*.lyp);;All files (*)"));
    if (!LayerFile.isEmpty())
    {
        ui->lineEditLayer->setText(LayerFile);
        LayerPath=ui->lineEditLayer->text();
    }
}
