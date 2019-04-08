#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QLineEdit>
#include "mainwindow.h"

struct FileDataString readoutdata(QString DataFile,QString delim)
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
        fields.append(line.split(delim));
    }

    file.close();
    in.flush();
return {"Perfect",fields};
}
