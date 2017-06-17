
/* by jxgz_zoulingmin@foxmail.com */

#include <QtGui>
#include <QDialog>
#include "tdialog.h"

static TDialog* m_TDialog;

TDialog::TDialog()
{   
    m_nTimerId = startTimer(0);    
}

TDialog::~TDialog()
{
    if(0 != m_nTimerId)  
    {
       killTimer(m_nTimerId); 
    }
}

void TDialog::timerEvent(QTimerEvent *event)
{
    static int x = 0, y = 0;

    if(x < 600 && y == 0)
    {
        x += 10;
    }
    else if(x == 600 && y < 300)
    {
        y += 10;
    }
    else if(x > 0 && y == 300)
    {
        x -= 10;
    }
    else if(x == 0 && y > 0)
    {
        y -= 10;
    }
   
    this->move(x, y);
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    m_TDialog = new TDialog();

    m_TDialog->resize(200, 120);

    m_TDialog->move(0, 0);

    m_TDialog->show();

    return app.exec();
}

