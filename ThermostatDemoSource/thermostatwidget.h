#ifndef THERMOSTATWIDGET_H
#define THERMOSTATWIDGET_H

#include <QtGui>

class GlobalSettings;

class ThermostatWidget : public QWidget
{
    Q_OBJECT
public:
    explicit ThermostatWidget(QWidget *parent = 0);
    static int currentTempStatic;
    QLabel *currentTemp;

public slots:
    void updateUnit();
    void setCurrentTempPtr(int * currentTemp);
    void increaseTemp();
    void decreaseTemp();
    QHash<QString, QVariant> getCurrentData();
    
signals:
    void timeout();
    void setpointIsReached(bool);
    
private slots:
    void update();
    void resizeEvent(QResizeEvent *);

private:
    int *currentTempInt;
    int setpointTempInt;
    QLabel *setpointTemp;
    QPushButton *increaseTempButton;
    QPushButton *decreaseTempButton;
    QTimer *updateTimer;
    bool setpointReached;
    GlobalSettings *m_globalSettings;
};

#endif // THERMOSTATWIDGET_H
