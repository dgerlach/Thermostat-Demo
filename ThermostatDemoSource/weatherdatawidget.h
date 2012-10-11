#ifndef WEATHERDATAWIDGET_H
#define WEATHERDATAWIDGET_H

#include <QWidget>
#include <QVariant>

class GlobalSettings;
class QLabel;
class WeatherData;

class WeatherDataWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WeatherDataWidget(QWidget *parent = 0);
    
signals:
    
public slots:
    void updateData();
    void setData(WeatherData* data);
    void scaleContents();
    QVariantHash getCurrentData();


private:
    QString iconNameToPixmap(QString icon);

    QLabel* m_currentTemp;
    QLabel* m_currentCity;
    QLabel* m_currentIconHolderLabel;
    QPixmap* m_currentIconPixmap;

    WeatherData* m_data;

    GlobalSettings* m_globalSettings;
};

#endif // WEATHERDATAWIDGET_H
