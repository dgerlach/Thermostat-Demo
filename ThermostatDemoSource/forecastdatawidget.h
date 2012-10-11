#ifndef FORECASTDATAWIDGET_H
#define FORECASTDATAWIDGET_H

#include <QWidget>
#include <QVariant>

class ForecastData;
class GlobalSettings;
class QLabel;
class QGridLayout;


//this class just groups together the widgets that belong to each row so they can easily be managed with an array
class ForecastDataWidgetGroup
{
public:
    QLabel *highTemp;
    QLabel *lowTemp;
    QLabel *day;
    QLabel *iconHolderLabel;
    QPixmap *iconPixmap;
};


class ForecastDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ForecastDataWidget(QWidget *parent = 0);

signals:
    
public slots:
    void updateData();
    void setData(QList<ForecastData* > data);
    void scaleContents();
    QVariantHash getCurrentData();

private:
    ForecastDataWidgetGroup m_forecastDataWidgetGroup[3];

    QList<ForecastData *>m_data;
    GlobalSettings* m_globalSettings;
    QGridLayout* widgetLayout;

    QString iconNameToPixmap(QString icon);
};


#endif // FORECASTDATAWIDGET_H
