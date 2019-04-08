#ifndef JSIMSYNTAX_H
#define JSIMSYNTAX_H
#include <QFile>
#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

class jsimsyntax
{
public:
    jsimsyntax();
    void compilejsim(QString NetlistFile);
};

#endif // JSIMSYNTAX_H
