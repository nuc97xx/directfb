
#ifndef TDIALOG_H_
#define TDIALOG_H_

#include <QtGui>
#include <QDialog>


static QDialog* m_Dialog;

class TDialog : public QDialog
{
    Q_OBJECT
public:
    TDialog();

    virtual ~TDialog();

protected:
    void timerEvent(QTimerEvent *event);

private:
    int m_nTimerId;
};

#endif

