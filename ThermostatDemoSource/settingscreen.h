#ifndef SETTINGSCREEN_H
#define SETTINGSCREEN_H

#include <QtGui>

class GlobalSettings;

class SettingScreen : public QWidget
{
    Q_OBJECT
public:
    explicit SettingScreen(QWidget *parent = 0);
    static bool unit;
    
signals:
    void valueChanged();
    void cityChanged(QString);
    
public slots:
    void unitIsF();
    void unitIsC();
    void changeCity(QString city);

private:
    QPushButton *FButton;
    QPushButton *CButton;

    GlobalSettings* m_globalSettings;
    
};

#endif // SETTINGSCREEN_H
