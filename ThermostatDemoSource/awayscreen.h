#ifndef AWAYSCREEN_H
#define AWAYSCREEN_H

#include <QtGui>

class GlobalSettings;

class AwayScreen : public QWidget
{
    Q_OBJECT
public:
    explicit AwayScreen(QWidget *parent = 0);
    
signals:
    
public slots:
    void updateCurrentTemp();
    void updateUnit();

private:
    QLabel *awayLabel;
    QLabel *ecoLabel;
    QLabel *currentTempLabel;
    QSpinBox *upperBound;
    QSpinBox *lowerBound;

    GlobalSettings* m_globalSettings;
};

#endif // AWAYSCREEN_H
