#include "jsimsyntax.h"
#include <QTextCharFormat>
#include <QBrush>
#include <QColor>
#include <QDate>

Jsimsyntax::Jsimsyntax(QTextDocument *parent)
    : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    QStringList keywordPatterns;
    keywordPatterns << "\\.\\bmodel\\b" << "\\.\\bprint\\b" << "\\.\\bfile\\b"
                    << "\\.\\btran\\b";
    //Qt::CaseInsensitive;
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    elementFormat.setForeground(Qt::darkMagenta);
    elementFormat.setFontWeight(QFont::Bold);
    QStringList elementPatterns;
    elementPatterns << "^L\\S+" << "^R\\S+" << "^C\\S+"
                    << "^K\\S+" << "^B\\S+" << "^I\\S+"
                    << "^V\\S+" << "^X\\S+" <<"^P\\S+";
    for (const QString &pattern : elementPatterns) {
        rule.pattern = QRegularExpression(pattern);
        rule.pattern.setPatternOptions(QRegularExpression::CaseInsensitiveOption);
        rule.format = elementFormat;
        highlightingRules.append(rule);
    }

    classFormat.setFontWeight(QFont::Bold);
    classFormat.setForeground(Qt::darkMagenta);
    rule.pattern = QRegularExpression("\\bQ[A-Za-z]+\\b");
    rule.format = classFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression("\\*[^\n]*");
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    //multiLinesubFormat.setForeground(Qt::red);
    multiLinesubFormat.setBackground(Qt::lightGray);

    quotationFormat.setForeground(Qt::red);
    rule.pattern = QRegularExpression("\".*\"");
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression("\\b[A-Za-z0-9_]+(?=\\()");
    rule.format = functionFormat;
    highlightingRules.append(rule);

    subcktStartExpression = QRegularExpression("\\.subckt[^\n]*");
    subcktEndExpression = QRegularExpression("\\.ends[^\n]*");
}

void Jsimsyntax::highlightBlock(const QString &text)
{

    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(subcktStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = subcktEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLinesubFormat);
        startIndex = text.indexOf(subcktStartExpression, startIndex + commentLength);
    }

    for (const HighlightingRule &rule : highlightingRules) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            rule.format.background();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }

}

QStringList Jsimsyntax::convertToJSIM(QStringList netlistCommands)
{
    QStringList NoCommentCommands;
    QStringList NewCommands;
    QStringList NewCommandswithCommands;


    for (int lineCNTR=0;lineCNTR<netlistCommands.length();lineCNTR++)
    {
        QStringList temp = netlistCommands.at(lineCNTR).split('*');
        if (!temp.at(0).isEmpty())
            NoCommentCommands.append(temp.at(0));
        temp.clear();
    }

    for (int cmdCNTR=0;cmdCNTR<NoCommentCommands.length();cmdCNTR++){

        QString newConvertLine=NoCommentCommands.at(cmdCNTR);

        //Do the changes to the netlist here


        NewCommands.append(newConvertLine);
    }

    int fileIndex=0;
    NewCommandswithCommands.append("*** Converted netlist to JSIM format at " + QDate::currentDate().toString()+" ***");
    for (int lineCNTR=0;lineCNTR<netlistCommands.length();lineCNTR++)
        {


        if (netlistCommands.at(lineCNTR).contains(NoCommentCommands.at(fileIndex)) && fileIndex<NewCommands.length())
            {
                QString NewLine=netlistCommands.at(lineCNTR);
                NewLine.replace(NoCommentCommands.at(fileIndex),NewCommands.at(fileIndex),Qt::CaseInsensitive);
                NewCommandswithCommands.append(NewLine);
                //qDebug()<<QString::number(fileIndex)<<" : "<<NewLine;
                fileIndex++;
            }
            else
            {
                NewCommandswithCommands.append(netlistCommands.at(lineCNTR));
                //qDebug()<<netlistCommands.at(lineCNTR);
            }

        }

    return NewCommandswithCommands;
}

