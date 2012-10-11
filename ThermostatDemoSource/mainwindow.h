#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "webdata.h"


class GlobalSettings;
class OptionsWidget;
class ThermostatWidget;
class WeatherWidget;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

    int m_currentThermostatTemp;
protected:
    void paintEvent(QPaintEvent *);
    
signals:
    
public slots:
    void loadWebData();
    void closeEvent(QCloseEvent *);
    void setWebData(WeatherData* weatherData);
    void webDataFailed();
    QHash<QString, QVariant> processCommand(QHash<QString, QVariant> command);

private:
    WebData *webData;
    QPushButton *energyButton;
    QPushButton *timeButton;
    QPushButton *dateButton;
    QPushButton *closeButton;
    QTimer *clockTimer;
    QDateTime dateTime;
    void setBackground(QString, QTime);
    WeatherWidget *weatherWidget;
    OptionsWidget *optionsWidget;
    ThermostatWidget *thermostatWidget;

    GlobalSettings* m_globalSettings;

    void createScreenLayout();

private slots:
    void updateClock();
    void energySaving(bool);
    
};

#endif // MAINWINDOW_H
