#ifndef FORECASTDATAWIDGET_H
#define FORECASTDATAWIDGET_H

#include <QWidget>

class ForecastData;
class GlobalSettings;
class QLabel;
class QHBoxLayout;

class ForecastDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ForecastDataWidget(QWidget *parent = 0);

signals:
    
public slots:
    void updateData();
    void setData(ForecastData* data);
    void scaleContents();

private:
    QLabel *m_highTemp;
    QLabel *m_lowTemp;
    QLabel *m_day;
    QLabel *m_iconHolderLabel;
    QPixmap *m_iconPixmap;
    ForecastData *m_data;
    GlobalSettings* m_globalSettings;
    QHBoxLayout* widgetLayout;

    QString iconNameToPixmap(QString icon);
};

#endif // FORECASTDATAWIDGET_H
