#include "jsimsyntax.h"

jsimsyntax::jsimsyntax()
{

}

void jsimsyntax::compilejsim(QString NetlistFile)
{
        QFile file(NetlistFile);
        if (!file.open(QFile::ReadOnly | QFile::Text))
        {
            return;
        }
        QByteArray dataNetlist = file.readAll();
        file.close();

}
