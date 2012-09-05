#include "qkeyboardlineedit.h"

#include <QFocusEvent>
#include <QtDebug>
#include "keyboard/keyboard.h"

QKeyboardLineEdit::QKeyboardLineEdit(QWidget *parent) :
    QLineEdit(parent)
{
    //connect(this, SIGNAL(selectionChanged()), this, SLOT(showKeyboard()));
    m_keyboardWidget=NULL;
    m_isBeingEdited = false;
}

QKeyboardLineEdit::~QKeyboardLineEdit()
{
}

void QKeyboardLineEdit::mousePressEvent(QMouseEvent *e)
{
    if(!m_isBeingEdited)
    {
        m_isBeingEdited = true;
        m_keyboardWidget = new Keyboard;
        connect(m_keyboardWidget, SIGNAL(closed()), this, SLOT(keyboardClosed()));
        m_keyboardWidget->setLineEdit(this);
        m_keyboardWidget->showMaximized();
    }
    if(m_isBeingEdited && m_keyboardWidget)
        qApp->setActiveWindow(m_keyboardWidget);
    QLineEdit::mousePressEvent(e);
}

void QKeyboardLineEdit::keyboardClosed()
{
    m_isBeingEdited = false;
    m_keyboardWidget->deleteLater();
    m_keyboardWidget = NULL;
}
