#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include "webdata.h"
#include "weatherwidget.h"

class GlobalSettings;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    
signals:
    
public slots:
    void updateUnit();
    void changeCity(QString);
    void closeEvent(QCloseEvent *);
    void setWebData(WeatherData* weatherData);
    void webDataFailed();

private:
    WebData *webData;
    QPushButton *energyButton;
    QPushButton *timeButton;
    QPushButton *dateButton;
    QPushButton *closeButton;
    QTimer *clockTimer;
    QTime clock;
    void setBackground(QString);
    WeatherWidget *weatherWidget;


    GlobalSettings* m_globalSettings;

private slots:
    void updateClock();
    void energySaving(bool);
    
};

#endif // MAINWINDOW_H
