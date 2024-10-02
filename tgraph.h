#ifndef TGRAPH_H
#define TGRAPH_H

#include <QWidget>
#include <QPainter>

struct dataPoint
{
    double val1;
    double val2;
};

class TGraph : public QWidget
{
    Q_OBJECT

    QPainter painter;
    double freq1 = 0;
    double freq2 = 0;
    double maxVal;
    double minVal;
    bool dataReady = false;
    size_t np;

    unsigned int convertToWidgetX(double x);
    unsigned int convertToWidgetY(double y);
    QString sParamLbl;
    QString sParamChrctLbl;
    QFont font;

protected:
    void paintEvent(QPaintEvent *event);

public:
    explicit TGraph(QWidget *parent = nullptr);
    void SetFreq(double f1, double f2);
    void calcMinMax();
    void setSParamLbl(const QString& lbl);
    void setSParamChrctLbl(const QString& lbl);

    std::vector<dataPoint> *graphData;

//signals:
};

#endif // TGRAPH_H
