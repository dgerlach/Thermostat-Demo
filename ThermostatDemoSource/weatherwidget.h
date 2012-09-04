#ifndef WEATHERWIDGET_H
#define WEATHERWIDGET_H

#include <QtGui>

#include <QtSvg/QSvgWidget>

class QLabel;
class WeatherData;
class WeatherDataWidget;
class ForecastDataWidget;

class WeatherWidget : public QWidget
{
    Q_OBJECT
public:
    explicit WeatherWidget(QWidget *parent = 0);
    void setCity(QString);
    void setClock(QTime);

    void setWeatherData(WeatherData* weatherData);

signals:
    void valueChanged();
public slots:
    void setStatusUpdated();
    void setStatusUpdating();
    void setStatusFailed();

    //void resizeEvent(QResizeEvent *);


protected:

private:
    QString weatherStyleSheet;
    QTime *clock;
    QLabel *statusLabel;
    QLabel *statusMovieLabel;
    QMovie *statusMovie;
    WeatherDataWidget* weatherDataWidget;
    ForecastDataWidget* forecastDataWidget[3];
    WeatherData* m_weatherData;
};

#endif // WEATHERWIDGET_H
