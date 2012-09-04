#ifndef SETTINGSCREEN_H
#define SETTINGSCREEN_H

#include <QtGui>

class GlobalSettings;
class QKeyboardLineEdit;

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
    void commitChanges();

private:
    QPushButton *FButton;
    QPushButton *CButton;

    QKeyboardLineEdit* m_proxyHostLineEdit;
    QKeyboardLineEdit* m_proxyPortLineEdit;

    GlobalSettings* m_globalSettings;
    
};

#endif // SETTINGSCREEN_H
