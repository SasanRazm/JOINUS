#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QLineEdit>
#include "mainwindow.h"

struct FileDataString readcsvdata(QString DataFile,QString delimator)
{
    QFile file(DataFile);
    QStringList fields;
    fields.clear();
    if (!file.exists())
    {
        return {file.errorString(),fields};
    }
    if (!file.open(QFile::ReadOnly | QFile::Text))
    {
        return {file.errorString(),fields};
    }

    QTextStream in(&file);

    while(!in.atEnd()) {
        QString line = in.readLine();
        fields.append(line.split(delimator));
    }

    file.close();
    in.flush();
return {"Perfect",fields};
}
