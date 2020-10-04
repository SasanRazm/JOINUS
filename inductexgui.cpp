#include "inductexgui.h"
#include "ui_inductexgui.h"
#include "myglobalvars.h"
#include <windows.h>
#include <QProcess>
#include <QFile>
#include <QTextStream>
#include <QFileDialog>
#include <QMessageBox>
#include <QRegExp>
#include <QDebug>
#include <QScreen>

int tableSelect;
QString KlayoutPath="";

InductexGUI::InductexGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InductexGUI)
{
    ui->setupUi(this);
    QPalette pal = this->palette();
    QLinearGradient gradient(0, 0, 400, 400);
    gradient.setColorAt(0, QColor(50, 230, 255));
    gradient.setColorAt(0.7, QColor(150, 255, 255));
    gradient.setColorAt(1, QColor(230, 255, 255));
    pal.setBrush(QPalette::Window, QBrush(gradient));
    this->setPalette(pal);

    QScreen *screen = QGuiApplication::primaryScreen();
    QRect screenGeometry = screen->geometry();

    this->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    this->setGeometry(20,50,screenGeometry.width()*4/5,screenGeometry.height()*5/6);


    m_jsimsyntax = new Jsimsyntax(ui->plainTextEditNetlist->document());
    ui->frameClassic->show();
    ui->frameNew->hide();
    ui->tableViewK->hide();
    ui->tableViewJJ->hide();

    connect(ui->buttonBox->button(QDialogButtonBox::RestoreDefaults),SIGNAL(clicked()),SLOT(on_restoredefaults_clicked()));
    connect(ui->buttonBox->button(QDialogButtonBox::SaveAll),SIGNAL(clicked()),SLOT(on_saveall_clicked()));

    QProcess *process=new QProcess(this);
    #ifdef __linux__


    #elif _WIN32
        process->setProgram("cmd.exe");
        process->setArguments({"/c","inductex"});
        process->setCreateProcessArgumentsModifier([] ( QProcess::CreateProcessArguments *args) {
                       args->flags &= CREATE_NO_WINDOW;});
        process->start();
        process->waitForFinished(-1);
    #else


    #endif
    QString stderrInductex = process->readAllStandardError();
    QString stdmssInductex = process->readAllStandardOutput();
    //ui->plainTextEdit->appendPlainText(stdmssInductex);

    if (!stdmssInductex.isEmpty())
    {
        QStringList spoiledData=stdmssInductex.split('\n');
        for (int spoiledDataCntr=0;spoiledDataCntr<spoiledData.length();spoiledDataCntr++)
            if (spoiledData.at(spoiledDataCntr).contains("Licensed to: "))
            {
                QRegExp r = QRegExp("(Licensed to: )(.+)(\\.)", Qt::CaseInsensitive);
                QRegExp r2 = QRegExp("(\\[)(.+)(\\])", Qt::CaseInsensitive);
                r.indexIn(spoiledData.at(spoiledDataCntr));
                r2.indexIn(spoiledData.at(spoiledDataCntr));
                QString lblNewVal=ui->lblLicense->text().replace("Not-Found",r.cap(2));
                lblNewVal.replace("Demo-version-1.0.0",r2.cap(2));

                ui->lblLicense->setText(lblNewVal);
            }
        ui->plainTextEdit->appendPlainText(stdmssInductex);
        ui->plainTextEdit->appendPlainText("**********************************************");

    }else
    {
        ui->plainTextEdit->appendPlainText(stderrInductex);
    }
    restoredefaults();

//    if (QFile(ui->lineEditGDS->text()).exists())
//    {

//        LoadGDSImage(ui->lineEditGDS->text());
//        QFileInfo fi(ui->lineEditGDS->text());
//        QString cirFileName=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
//                +"cir";
//        if (!QFile(cirFileName).exists())
//        {
//            QMessageBox::warning(this,"Circuit netlist","Cannot find \""+cirFileName+"\" in the layout file folder.");
//            return;
//        }
//        LoadNetlist(cirFileName);
//    }

}

InductexGUI::~InductexGUI()
{
    delete ui;
}

void InductexGUI::on_pushButton_clicked()
{


    QProcess *process=new QProcess(this);

    QString layoutArg=ui->lineEditGDS->text();
    QString ldfArg=ui->lineEditLDF->text();


    QString infArg=ui->lineEditVal_6->text();
    QString simEngine=ui->radioButtonFH->isChecked()?"-fh":"-th";

    QString ixiArg=ui->lineEditGDS_2->text();

    QStringList allArgs;

    if (ui->radioButtonNormal->isChecked())
    {
        allArgs<<"/c"<<"inductex"<<layoutArg<<"-l"<<ldfArg<<"-i"<<infArg<<simEngine;
        if (ui->checkBoxCurr->isChecked())
            allArgs<<"-j";
    }
    else
        allArgs<<"/c"<<"inductex"<<ixiArg;




    ui->labelStatue->setText("InductEx is running!");

    QFileInfo fi(layoutArg);
    QString InductexFolderPath=fi.absolutePath();
    QString folderPath=QDir::currentPath();
    QDir::setCurrent(InductexFolderPath);

    #ifdef __linux__


    #elif _WIN32
//        process->setProgram("cmd.exe");
//        process->start("cd "+InductexFolderPath);
//        process->waitForFinished(-1);
//        process->reset();

        process->setProgram("cmd.exe");
        process->setArguments(allArgs);
        process->setCreateProcessArgumentsModifier([] ( QProcess::CreateProcessArguments *args) {
                        args->flags &= CREATE_NO_WINDOW;});
        process->start();
        process->waitForFinished(-1);
    #else


    #endif
    QDir::setCurrent(folderPath);

    QString stderrInductex = process->readAllStandardError();
    ui->plainTextEdit->appendPlainText(stderrInductex);

    QString stdmssInductex = process->readAllStandardOutput();
    ui->plainTextEdit->appendPlainText(stdmssInductex);

    load_Table(stdmssInductex);

    ui->labelStatue->setText("Process finished.");
}

void InductexGUI::on_radioButtonNormal_toggled(bool checked)
{
    if (checked)
    {
        ui->frameClassic->show();
        ui->frameNew->hide();
    }else{
        ui->frameClassic->hide();
        ui->frameNew->show();
    }
}

void InductexGUI::on_toolButtonLoadGDS_clicked()
{
    QString gdsFile = QFileDialog::getOpenFileName(this, tr("Open layout File..."),documentFolderPath, tr("Layout files (*.gds);;All files (*)"));
    if (!gdsFile.isEmpty())
    {
        ui->lineEditGDS->setText(gdsFile);
        //LoadGDSImage(gdsFile);
        QFileInfo fi(gdsFile);
        QString cirFileName=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
                +"cir";
        if (!QFile(cirFileName).exists())
        {
            QMessageBox::warning(this,"Circuit netlist","Cannot find \""+cirFileName+"\" in the layout file folder.");
            return;
        }
        ui->lineEditVal_6->setText(fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)+"inf");
        LoadNetlist(cirFileName);
    }
}

void InductexGUI::on_toolButtonLoadLDF_clicked()
{
    QString ldfFile = QFileDialog::getOpenFileName(this, tr("Open LDF File..."),documentFolderPath, tr("Layer Definition Files (*.ldf);;All files (*)"));
    if (!ldfFile.isEmpty())
        ui->lineEditLDF->setText(ldfFile);
}

void InductexGUI::on_toolButtonLoadGDS_2_clicked()
{
    QString ixiFile = QFileDialog::getOpenFileName(this, tr("Open IXI File..."),documentFolderPath, tr("Layer Definition Files (*.ixi);;All files (*)"));
    if (!ixiFile.isEmpty())
        ui->lineEditGDS_2->setText(ixiFile);
}

void InductexGUI::on_restoredefaults_clicked()
{
    restoredefaults();
}

void InductexGUI::restoredefaults()
{
    QStringList loadmaterial={};
    QString inductexStatFile=documentFolderPath+"/inductex.stat";

    if (QFile(inductexStatFile).exists())
    {
        QFile file(inductexStatFile);
        if (!file.open(QFile::ReadOnly | QFile::Text)){
             QMessageBox::warning(this,"Error!",file.errorString());
        }
        else{
            QTextStream stream(&file);
            while(!stream.atEnd()) {
                QString line = stream.readLine();
                loadmaterial.append(line);
            }
            file.close();
            stream.flush();

            ui->radioButtonNormal->setChecked(loadmaterial.at(0).toInt());
            ui->lineEditGDS->setText(loadmaterial.at(1));
            ui->lineEditLDF->setText(loadmaterial.at(2));
            ui->lineEditVal_6->setText(loadmaterial.at(3));
            ui->lineEditGDS_2->setText(loadmaterial.at(4));
            ui->radioButtonFH->setChecked(loadmaterial.at(5).toInt());
            KlayoutPath=loadmaterial.at(6);
        }
    }else
    {
        ui->radioButtonNormal->setChecked(true);
        ui->lineEditGDS->setText(documentFolderPath+"/inductex/test.gds");
        ui->lineEditLDF->setText(documentFolderPath+"/inductex/test.ldf");
        ui->lineEditVal_6->setText(documentFolderPath+"/inductex/test.inp");
        ui->lineEditGDS_2->setText(documentFolderPath+"/inductex/test.ixi");
        ui->radioButtonFH->setChecked(true);
    }

    if (QFile(ui->lineEditGDS->text()).exists())
    {

        //LoadGDSImage(ui->lineEditGDS->text());
        QFileInfo fi(ui->lineEditGDS->text());
        QString cirFileName=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
                +"cir";
        if (!QFile(cirFileName).exists())
        {
            QMessageBox::warning(this,"Circuit netlist","Cannot find \""+cirFileName+"\" in the layout file folder.");
            return;
        }
        LoadNetlist(cirFileName);
    }
}

void InductexGUI::on_saveall_clicked()
{
    QString inductexStatFile=documentFolderPath+"/inductex.stat";
    QStringList savematerial={};

    savematerial.append(QString::number(ui->radioButtonNormal->isChecked()));
    savematerial.append(ui->lineEditGDS->text());
    savematerial.append(ui->lineEditLDF->text());
    savematerial.append(ui->lineEditVal_6->text());
    savematerial.append(ui->lineEditGDS_2->text());
    savematerial.append(QString::number(ui->radioButtonFH->isChecked()));
    savematerial.append(KlayoutPath);


    QFile file(inductexStatFile);
    if (!file.open(QFile::WriteOnly | QFile::Text)){
         QMessageBox::warning(this,"Error!",file.errorString());
    }
    else{
            QTextStream stream(&file);
            for (int fcntr=0; fcntr<savematerial.size();fcntr++)
                stream << savematerial.at(fcntr)<<'\n';
            file.close();
            stream.flush();
    }
}

void InductexGUI::load_Table(QString InductexOUT)
{

    QList<QString> PN0, ID0, IE0, RD0, RE0, AD0, PD0;
    QList<QString> PN1, ID1, IE1, RD1, RE1, AD1, PD1;
    QList<QString> PN2, ID2, IE2, RD2, RE2, AD2, PD2;

    if (!InductexOUT.isEmpty())
    {
        QStringList spoiledData=InductexOUT.split('\n');
        for (int spoiledDataCntr=0;spoiledDataCntr<spoiledData.length();spoiledDataCntr++)
            if (spoiledData.at(spoiledDataCntr).contains("Extracted Design"))
            {
                ui->plainTextEdit->appendPlainText("**********************End of calculation***********************");
                spoiledDataCntr++;
                bool AlreadyRun;

                while (!(spoiledData.at(spoiledDataCntr).contains("Mutual Inductance")
                         ||spoiledData.at(spoiledDataCntr).contains("Ports")||spoiledData.at(spoiledDataCntr).contains("Deallocating memory")))
                {
                    QString dataStringline=spoiledData.at(spoiledDataCntr);
                    dataStringline.remove("\r");

                    if (!dataStringline.isEmpty())
                    {
                       //qDebug()<<dataStringline;
                       QStringList parametersInductex=dataStringline.split(" ",QString::SkipEmptyParts);
                       for (int dataCNTR=0;dataCNTR<parametersInductex.length();dataCNTR++)
                       {
                               switch(dataCNTR)
                               {case 0:
                                   PN0.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 1:
                                   ID0.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 2:
                                   IE0.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 3:
                                   RD0.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 4:
                                   RE0.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 5:
                                   AD0.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 6:
                                   PD0.append(parametersInductex.at(dataCNTR));
                                   break;
                               default:
                                   break;
                               }
                       }

                    }

                    spoiledDataCntr++;
                }

                AlreadyRun=true;
                spoiledDataCntr++;
                while (!(spoiledData.at(spoiledDataCntr).contains("Ports")||spoiledData.at(spoiledDataCntr).contains("Deallocating memory")))
                {
                    QString dataStringline=spoiledData.at(spoiledDataCntr);
                    dataStringline.remove("\r");
//                    if(AlreadyRun)
//                    {
//                        PN0.append("Couple_Name");
//                        ID0.append("Design");
//                        IE0.append("Extracted");
//                        RD0.append("Abs_Diff");
//                        RE0.append("Perc_Diff");
//                        AD0.append("K");
//                        PD0.append("--");
//                        AlreadyRun=false;
//                    }
                    if (!dataStringline.isEmpty())
                    {
                       //qDebug()<<dataStringline;
                       QStringList parametersInductex=dataStringline.split(" ",QString::SkipEmptyParts);
                       PD1.append("--");
                       for (int dataCNTR=0;dataCNTR<parametersInductex.length();dataCNTR++)
                       {
                               switch(dataCNTR)
                               {case 0:
                                   PN1.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 1:
                                   ID1.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 2:
                                   IE1.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 3:
                                   RD1.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 4:
                                   RE1.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 5:
                                   AD1.append(parametersInductex.at(dataCNTR));
                                   break;

                               default:
                                   break;
                               }
                       }

                    }

                    spoiledDataCntr++;
                }

                AlreadyRun=true;
                spoiledDataCntr++;

                while (!spoiledData.at(spoiledDataCntr).contains("Deallocating memory"))
                {
                    QString dataStringline=spoiledData.at(spoiledDataCntr);
                    dataStringline.remove("\r");
                    if(AlreadyRun)
                    {
                        PN2.append("JJ_Name");
                        ID2.append("Design");
                        IE2.append("Extracted");
                        RD2.append("--");
                        RE2.append("--");
                        AD2.append("--");
                        PD2.append("--");
                        AlreadyRun=false;
                    }

                    if (!dataStringline.isEmpty())
                    {
                       //qDebug()<<dataStringline;
                       QStringList parametersInductex=dataStringline.split(" ",QString::SkipEmptyParts);
                       RE2.append("--");
                       PD2.append("--");
                       AD2.append("--");
                       RD2.append("--");
                       for (int dataCNTR=0;dataCNTR<parametersInductex.length();dataCNTR++)
                       {
                               switch(dataCNTR)
                               {case 0:
                                   PN2.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 1:
                                   ID2.append(parametersInductex.at(dataCNTR));
                                   break;
                               case 2:
                                   IE2.append(parametersInductex.at(dataCNTR));
                                   break;
                               default:
                                   break;
                               }
                       }

                    }

                    spoiledDataCntr++;
                }

                break;
            }
        // Create model:
        tableSelect=0;
        TestModel *ParamModel = new TestModel(this);
        ParamModel->populateData(PN0, ID0, IE0, RD0, RE0, AD0, PD0);
        ui->tableViewParams->setModel(ParamModel);
        ui->tableViewParams->horizontalHeader()->setVisible(true);
        //ui->plainTextEdit->appendPlainText(QString::number(ParamModel->rowCount()));
        ui->tableViewParams->show();

        if (!PN1.isEmpty())
        {
            //tableSelect=1;
            TestModel *CoupleModel = new TestModel(this);
            CoupleModel->populateData(PN1, ID1, IE1, RD1, RE1, AD1, PD1);
            ui->tableViewK->setModel(CoupleModel);
            ui->tableViewK->horizontalHeader()->setVisible(false);
            ui->tableViewK->show();
        }

        if (!PN2.isEmpty())
        {
            //tableSelect=2;
            TestModel *JJModel = new TestModel(this);
            JJModel->populateData(PN2, ID2, IE2, RD2, RE2, AD2, PD2);
            ui->tableViewJJ->setModel(JJModel);
            ui->tableViewJJ->horizontalHeader()->setVisible(false);
            ui->tableViewJJ->show();
        }

    }

}

void TestModel::populateData(const QList<QString> &PN,const QList<QString> &ID,const QList<QString> &IE,
                             const QList<QString> &RD,const QList<QString> &RE,const QList<QString> &AD,const QList<QString> &PD)
{
    paramName.clear();
    paramName=PN;
    inductanceDesign.clear();
    inductanceDesign=ID;
    inductanceExtracted.clear();
    inductanceExtracted=IE;
    resistanceDesign.clear();
    resistanceDesign=RD;
    resistanceExtracted.clear();
    resistanceExtracted=RE;
    absDiff.clear();
    absDiff=AD;
    percDiff.clear();
    percDiff=PD;
    return;
}

int TestModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return paramName.length();
}

int TestModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 7;
}

QVariant TestModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole) {
        return QVariant();
    }
    switch (index.column())
    {
        case 0:
            return paramName[index.row()];
        case 1:
            return inductanceDesign[index.row()];
        case 2:
            return inductanceExtracted[index.row()];
        case 3:
            return resistanceDesign[index.row()];
        case 4:
            return resistanceExtracted[index.row()];
        case 5:
            return absDiff[index.row()];
        case 6:
            return percDiff[index.row()];
    }

    return QVariant();
}

QVariant TestModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
        switch (section)
        {
            case 0:
                return QString("Name");
            case 1:
                if (tableSelect==0)
                    return QString("Ind_design");
                else if (tableSelect==1)
                    return QString("Design");
                else
                    return QString("JJ_design");

            case 2:
                if (tableSelect==0)
                    return QString("Ind_extracted");
                else if (tableSelect==1)
                    return QString("Extracted");
                else
                    return QString("Extracted");

            case 3:
                if (tableSelect==0)
                    return QString("Res_design");
                else if (tableSelect==1)
                    return QString("Abs_Diff");
                else
                    return QString("--");

            case 4:
                if (tableSelect==0)
                    return QString("Res_extracted");
                else if (tableSelect==1)
                    return QString("Perc_Diff");
                else
                    return QString("--");

            case 5:
                if (tableSelect==0)
                    return QString("Abs_Diff");
                else if (tableSelect==1)
                    return QString("K");
                else
                    return QString("--");

            case 6:
                if (tableSelect==0)
                    return QString("Perc_Diff");
                else
                    return QString("--");
        }
    }
    return QVariant();
}

TestModel::TestModel(QObject *parent) : QAbstractTableModel(parent)
{
}

void InductexGUI::on_pushButtonExport_clicked()
{

}

void InductexGUI::on_pushButtonBackAnnotate_clicked()
{

}

//void InductexGUI::LoadGDSImage(QString LayoutFile)
//{
    //Set the image for start
//    QFileInfo fi(NetlistFile);
//    QString PicFileName=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
//            +"png";
//    QPixmap pix;
//    ui->lblNetlistPic->setScaledContents(true);
//    ui->lblNetlistPic->setSizePolicy(QSizePolicy::Preferred,QSizePolicy::Preferred);
//    ui->lblNetlistPic->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
//        if (pix.load(PicFileName)){
//            ui->lblNetlistPic->setPixmap(pix);
//        }
//        else
//        {
//            pix.load(":/image/Image/actions/cancel.png");
//            ui->lblNetlistPic->setPixmap(pix);
//        }
//}
void InductexGUI::LoadNetlist(QString LayoutFile)
{
    QFile file(LayoutFile);
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
         QMessageBox::warning(this,"Error!",file.errorString());
    }
    QByteArray dataNetlist = file.readAll();
    ui->plainTextEditNetlist->setPlainText(dataNetlist);
    file.close();
}

void InductexGUI::on_pushButtonClear_clicked()
{
    ui->plainTextEdit->clear();
    ui->plainTextEdit->appendPlainText(">> Inductex output :");
}

void InductexGUI::on_pushButtonSave_clicked()
{
    QFileInfo fi(ui->lineEditGDS->text());
    QString NetlistFile=fi.absolutePath()+"/"+fi.fileName().remove(fi.completeSuffix(),Qt::CaseInsensitive)
            +"cir";

        QFile file(NetlistFile);
        if (!file.open(QFile::WriteOnly | QFile::Text)){
             QMessageBox::warning(this,"Error!",file.errorString());
        }
        else{
                QTextStream stream(&file);
                stream << ui->plainTextEditNetlist->toPlainText();
                ui->plainTextEdit->appendPlainText(">> Netlist file saved successfully!");
                file.close();
                stream.flush();
        }
}

void InductexGUI::on_pushButtonLayout_clicked()
{

    if (KlayoutPath.isEmpty())
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Klayout Path", "Path not Found, select a path?",
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {

            QString KlayoutFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
            if (!KlayoutFolder.isEmpty())
            {
                KlayoutPath=KlayoutFolder;
                QStringList allArgs;
                QString layoutArg=ui->lineEditGDS->text();
                //QString layersArg;
                QProcess *process=new QProcess(this);
                allArgs<<"/c"<<KlayoutPath+"/klayout_app"<<"-e"<<layoutArg;//<<"-l"<<layersArg;
                        #ifdef __linux__


                        #elif _WIN32
                        //        process->setProgram("cmd.exe");
                        //        process->start("cd "+InductexFolderPath);
                        //        process->waitForFinished(-1);
                        //        process->reset();

                            process->setProgram("cmd.exe");
                            process->setArguments(allArgs);
                            process->setCreateProcessArgumentsModifier([] ( QProcess::CreateProcessArguments *args) {
                                            args->flags &= CREATE_NO_WINDOW;});
                            process->startDetached();
                        #else


                        #endif
            }
        }
        else if (reply == QMessageBox::No)
            return;
    }
    else
    {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "Klayout Path", "Is \"" + KlayoutPath + "\" Correct?",
                                      QMessageBox::Yes|QMessageBox::No);

        if (reply == QMessageBox::Yes) {

            QStringList allArgs;
            QString layoutArg=ui->lineEditGDS->text();
            //QString layersArg;
            QProcess *process=new QProcess(this);
            allArgs<<"/c"<<KlayoutPath+"/klayout_app"<<"-e"<<layoutArg;//<<"-l"<<layersArg;
                    #ifdef __linux__


                    #elif _WIN32
                    //        process->setProgram("cmd.exe");
                    //        process->start("cd "+InductexFolderPath);
                    //        process->waitForFinished(-1);
                    //        process->reset();

                        process->setProgram("cmd.exe");
                        process->setArguments(allArgs);
                        process->setCreateProcessArgumentsModifier([] ( QProcess::CreateProcessArguments *args) {
                                        args->flags &= CREATE_NO_WINDOW;});
                        process->startDetached();
                    #else


                    #endif

        }
        else if (reply == QMessageBox::No)
        {
            QString KlayoutFolder = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"$(pwd)",QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
            if (!KlayoutFolder.isEmpty())
            {
                KlayoutPath=KlayoutFolder;
                QStringList allArgs;
                QString layoutArg=ui->lineEditGDS->text();
                //QString layersArg;
                QProcess *process=new QProcess(this);
                allArgs<<"/c"<<KlayoutPath+"/klayout_app"<<"-e"<<layoutArg;//<<"-l"<<layersArg;
                        #ifdef __linux__


                        #elif _WIN32
                        //        process->setProgram("cmd.exe");
                        //        process->start("cd "+InductexFolderPath);
                        //        process->waitForFinished(-1);
                        //        process->reset();

                            process->setProgram("cmd.exe");
                            process->setArguments(allArgs);
                            process->setCreateProcessArgumentsModifier([] ( QProcess::CreateProcessArguments *args) {
                                            args->flags &= CREATE_NO_WINDOW;});
                            process->startDetached();
                        #else


                        #endif
            }
        }

    }
}
