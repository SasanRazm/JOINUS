#ifndef JSIMSYNTAX_H
#define JSIMSYNTAX_H

#include <QSyntaxHighlighter>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
class QTextCharFormat;
QT_END_NAMESPACE

class Jsimsyntax : public QSyntaxHighlighter
{
  //  Q_OBJECT
public:
    Jsimsyntax(QTextDocument *parent = 0);
    QStringList convertToJSIM(QStringList netlistCommands);

protected:
    void highlightBlock(const QString &text) Q_DECL_OVERRIDE;

private:

    struct HighlightingRule
    {
        QRegularExpression pattern;
        QTextCharFormat format;
    };

    QVector<HighlightingRule> highlightingRules;
    QRegularExpression subcktStartExpression;
    QRegularExpression subcktEndExpression;

    QTextCharFormat keywordFormat;
    QTextCharFormat elementFormat;
    QTextCharFormat classFormat;
    QTextCharFormat singleLineCommentFormat;
    QTextCharFormat multiLinesubFormat;
    QTextCharFormat quotationFormat;
    QTextCharFormat functionFormat;

};

#endif // JSIMSYNTAX_H
