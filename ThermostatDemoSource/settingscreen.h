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

    void setLicenseString(QString licenseString);
    
signals:
    void valueChanged();
    void cityChanged();
    
public slots:
    void setTempFormatF();
    void setTempFormatC();

    void setTimeFormat12h();
    void setTimeFormat24h();

    void commitChanges();

private:
    QComboBox *cityBox;
    QLabel *licenseStringLabel;

    QPushButton *FButton;
    QPushButton *CButton;

    QPushButton *Button12h;
    QPushButton *Button24h;

    QKeyboardLineEdit* m_proxyHostLineEdit;
    QKeyboardLineEdit* m_proxyPortLineEdit;

    GlobalSettings* m_globalSettings;

    QWidget* buildNetworkSettingsWidget();
    QWidget* buildGeneralSettingsWidget();
    QWidget* buildFormatSettingsWidget();
    
};

#endif // SETTINGSCREEN_H
