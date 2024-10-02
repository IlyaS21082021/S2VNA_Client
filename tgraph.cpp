#include "tgraph.h"

TGraph::TGraph(QWidget *parent) : QWidget{parent}
{
    font.setFamily("Arial");
    font.setPointSize(10);
}

unsigned int TGraph::convertToWidgetX(double x)
{
    return (x - freq1) * (this->width()-10)/(freq2 - freq1) + 5;
}

unsigned int TGraph::convertToWidgetY(double y)
{
    return this->height() - (y - minVal) * (this->height() - 20)/(maxVal - minVal) - 15;
}

void TGraph::calcMinMax()
{
    maxVal = graphData->front().val1;
    minVal = maxVal;

    for (const auto &i : *graphData)
    {
        if (i.val1 > maxVal)
            maxVal = i.val1;

        if (i.val1 < minVal)
            minVal = i.val1;
    }

    np = graphData->size();
    dataReady = true;
}

void TGraph::paintEvent(QPaintEvent *event)
{
    painter.begin(this);
    if (dataReady)
    {
        painter.setPen(QPen(Qt::yellow, 3));
        for (auto i = 1; i < np; i++)
            painter.drawLine(convertToWidgetX(freq1 + (freq2 - freq1)/(np-1)*(i-1)), convertToWidgetY(graphData->at(i-1).val1),
                             convertToWidgetX(freq1 + (freq2 - freq1)/(np-1)*i), convertToWidgetY(graphData->at(i).val1));

         //print value
        painter.setPen(QPen(Qt::gray, 1));
        painter.drawText(0, height(), QString::number(freq1));
        painter.drawText(width()-30, height(), QString::number(freq2));
        painter.drawText(0, height()-20, QString::number(minVal));
        painter.drawText(0, 20, QString::number(maxVal));
        for (int i = 1; i < 6; i++)
        {
            painter.drawText((width()-10)/6*i-5, height(), QString::number(freq1 + (freq2 - freq1)/6*i));  //horizontal
            painter.drawText(0, height() - (5+(height()-20)/6*i), QString::number(minVal + (maxVal - minVal)/6*i));   //vertical
        }

        //print graph lables
        painter.setPen(QPen(Qt::yellow, 1));
        painter.setFont(font);
        painter.drawText(10, height() - 40, "[" + sParamLbl + "]/[" + sParamChrctLbl + "]");
    }


    //draw coord lines
    painter.setPen(QPen(Qt::gray, 1));
    painter.drawLine(5, 5, 5, height()-15); //vertical
    painter.drawLine(width()-5, 5, width()-5, height()-15);
    painter.drawLine(5, 5, width()-5, 5); //horizontal
    painter.drawLine(5, height() - 15, width()-5, height() - 15);
    for (int i = 1; i < 6; i++)
    {
        painter.drawLine(5+(width()-10)/6*i, 5, 5+(width()-10)/6*i, height()-15); //vertical
        painter.drawLine(5, 5+(height()-20)/6*i, width()-5, 5+(height()-20)/6*i); //horizontal
    }
    painter.end();
}

void TGraph::SetFreq(double f1, double f2)
{
    freq1 = f1;
    freq2 = f2;
}

void TGraph::setSParamLbl(const QString& lbl)
{
    sParamLbl = lbl;
}

void TGraph::setSParamChrctLbl(const QString& lbl)
{
    sParamChrctLbl = lbl;
}
