#include "jsimsyntax.h"
#include <QTextCharFormat>
#include "calcvals.h"
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
    QStringList ParamList={};
    QStringList newParamList={};
    QStringList NewCommandswithCommands;
    QStringList stringNodes={};


    for (int lineCNTR=0;lineCNTR<netlistCommands.length();lineCNTR++)
    {
        if (!netlistCommands.at(lineCNTR).contains(".param",Qt::CaseInsensitive))
        {
            QStringList temp = netlistCommands.at(lineCNTR).split('*');
            if (!temp.at(0).isEmpty())
                NoCommentCommands.append(temp.at(0));


            //Find if the nodes are string or digit
            QRegExp r = QRegExp("^[RLCBVIX]\\S+", Qt::CaseInsensitive);

            QStringList SeperatedCommand=temp.at(0).split(" ");
            r.indexIn(SeperatedCommand.at(0));
            QString elementName = r.cap(0);

            if (elementName.front()=='X')
            {
                for (int tempCntr=2;tempCntr<temp.length();tempCntr++)
                {
                    QRegExp re("\\d*");
                    if (!re.exactMatch(temp.at(tempCntr)))
                    {
                        stringNodes.append(temp.at(tempCntr));
                    }

                }
            }else if (elementName.front()=='R'||elementName.front()=='L'||elementName.front()=='C'||elementName.front()=='B'||elementName.front()=='V'||elementName.front()=='I')
            {
                QRegExp re("\\d*");
                if (!re.exactMatch(temp.at(1)))
                {
                    stringNodes.append(temp.at(1));
                }

                if (!re.exactMatch(temp.at(2)))
                {
                    stringNodes.append(temp.at(2));
                }
            }

            temp.clear();

        } else if (netlistCommands.at(lineCNTR).contains(".param",Qt::CaseInsensitive))
        {
                ParamList.append(netlistCommands.at(lineCNTR));
        }
    }

    if (!ParamList.isEmpty())
    {
        newParamList = annotateParam(ParamList);
    }

    for (int cmdCNTR=0;cmdCNTR<NoCommentCommands.length();cmdCNTR++){


            //Do the changes to the netlist here

        //Check to see if nodes are string and replace them with numbers
        QString newConvertLine = changeNodesToNum(stringNodes, NoCommentCommands.at(cmdCNTR));


            //Check for the params in the netlist and replace them
            for (int paramCntr=0; paramCntr<newParamList.size();paramCntr+=2)
               if (newConvertLine.contains(newParamList.at(paramCntr)))
                    newConvertLine.replace(newParamList.at(paramCntr),newParamList.at(paramCntr+1));


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



QStringList Jsimsyntax::annotateParam(QStringList ParamList)
{

    CalcVals *values= new CalcVals;

    QStringList paramVal;
    QStringList paramName;
    QStringList paramNameVal;

    char instr;

    QRegExp numPattern = QRegExp("^[0-9]+(\\.[0-9]*)?[pnµufmKG]?$", Qt::CaseInsensitive);
    QRegExp opPattern = QRegExp("[+*-\/]");

    QStringList SeperatedCommand;
    QStringList temp;
    QString paramValtemp;

    for (int cntr2=0; cntr2<ParamList.size();cntr2++)
    {
             SeperatedCommand = ParamList.at(cntr2).split(" ");
             temp.clear();
             for (int tempcntr=0;tempcntr<SeperatedCommand.length();tempcntr++){
                 if (!SeperatedCommand.at(tempcntr).isEmpty())
                     temp.append(SeperatedCommand.at(tempcntr));
             }

             int indexTemp=0;
             while (indexTemp<temp.length())
             {
                 if (temp.at(indexTemp).contains(".param",Qt::CaseInsensitive))
                 {
                     indexTemp++;
                     paramName.append(temp.at(indexTemp));
                     indexTemp++;
                 }else if (temp.at(indexTemp) == "=")
                 {
                     indexTemp++;
                 }else{
                     paramValtemp= "";
                     for (int cntr3=indexTemp; cntr3<temp.length(); cntr3++)
                     {
                         paramValtemp=paramValtemp+temp.at(cntr3);
                         indexTemp++;
                     }
                     paramVal.append(paramValtemp);
                 }
             }
    }


    for (int cntr1=0; cntr1<ParamList.size();cntr1++)
    {

        QString parName= ParamList.at(cntr1);
        QString parVal = ParamList.at(cntr1+1);
        if (parVal.contains('+')||parVal.contains('-')||parVal.contains('*')||parVal.contains('/'))
        {

        }else{

        }
        for (int cntr2=0; cntr2<ParamList.size();cntr2++)
        {

        }

        cntr1++;
    }



    return paramNameVal;
}

QString Jsimsyntax::changeNodesToNum(QStringList stringNodes, QString commandLine)
{

    QString newCommandLine= commandLine;

    //Start renaming the strings from this point, increase for bigger netlists.
    int startNode=9369;

    for (int nodeCntr=0; nodeCntr<stringNodes.size(); nodeCntr++)
    {
        if (commandLine.contains(stringNodes.at(nodeCntr)))
            newCommandLine.replace(stringNodes.at(nodeCntr),QString::number(startNode+nodeCntr));
    }

    return newCommandLine;
}

double Jsimsyntax::annotateConstants(QString constName)
{

    if (!QString::compare("PI", constName, Qt::CaseInsensitive))
    {
        return 3.141592653589793238463;
    }else if (!QString::compare("PHI_ZERO", constName, Qt::CaseInsensitive))
    {
        return 2.067833831170082e-15;
    } else if (!QString::compare("BOLTZMANN", constName, Qt::CaseInsensitive))
    {
        return 1.38064852E-23;
    } else if (!QString::compare("EV", constName, Qt::CaseInsensitive))
    {
        return 1.6021766208E-19;
    }else if (!QString::compare("HBAR", constName, Qt::CaseInsensitive))
    {
        return 1.0545718001391127E-34;
    }else if (!QString::compare("C", constName, Qt::CaseSensitive))
    {
        return 299792458;
    }else if (!QString::compare("MU0", constName, Qt::CaseInsensitive))
    {
        return 12.566370614E-7;
    }else if (!QString::compare("EPS0", constName, Qt::CaseInsensitive))
    {
        return 8.854187817E-12;
    }else if (!QString::compare("SIGMA", constName, Qt::CaseInsensitive))
    {
        return 3.291059757E-16;
    }

    return 0;

}


double Jsimsyntax::CalculateParam(double x, char oper, double y)
{
    if (oper=='+')
        return x+y;
    else if (oper=='*')
        return x*y;
    else if (oper=='-')
        return x-y;
    else if (oper=='/')
        return x/y;

 return 0;
}
