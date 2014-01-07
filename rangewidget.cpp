#include "rangewidget.h"
#include "ui_rangewidget.h"

RangeWidget::RangeWidget(QWidget *parent, int ns_, int left_, int right_, double spStep_, int spLength_, QDir *dir_) :
    QWidget(parent),
    ui(new Ui::RangeWidget)
{
    ui->setupUi(this);
    ns = ns_;
    left = left_;
    right = right_;
    spLength = spLength_;
    spStep = spStep_;
    dirBC = new QDir();
    dirBC->setPath(dir_->absolutePath());

    rangeLimits = new int * [ns];
    for(int i=0; i<ns; ++i)
    {
        rangeLimits[i] = new int [2];

        rangeLimits[i][0] = 0;
        rangeLimits[i][1] = spLength-1;
    }


    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setGeometry(10,10,800,800);  //generality
    this->setWindowTitle("Specify spectra intervals");
    specLabel = new QLabel(this);
    specLabel->setGeometry(0,0,this->width(), this->height());
    pic = QPixmap(1600,1600);
    pic.fill();
    paint = new QPainter;
    paint->begin(&pic);


    //draw axes and channels' labels
    //using spLength, left, right,

    for(int c2=0; c2<19; ++c2)  //exept markers channel
    {
        //draw axes
        paint->setPen("black");
        paint->drawLine(QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2] - coords::scale * paint->device()->height())); //250 - length of axes generality
        paint->drawLine(QPointF(paint->device()->width() * coords::x[c2], paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2] + coords::scale * paint->device()->width(), paint->device()->height() * coords::y[c2])); //250 - length of axes generality

        //draw Herzes
        paint->setFont(QFont("Helvitica", int(8*(paint->device()->height()/1600.))));
        for(int k=0; k<int(coords::scale * paint->device()->width()); ++k) //for every Hz generality
        {
            if( (left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep - floor((left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep) < spLength/(coords::scale * paint->device()->width())*spStep/2. || ceil((left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep) - (left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep < spLength/(coords::scale * paint->device()->width())*spStep/2.)  //why spLength - generality 250 - length of axes
            {
                paint->drawLine(QPointF(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2]), QPointF(paint->device()->width() * coords::x[c2] + k, paint->device()->height() * coords::y[c2] + 5 * (paint->device()->height()/1600.)));

                helpInt = int((left + k*(spLength)/(coords::scale * paint->device()->width()))*spStep + 0.5);
                helpString.setNum(helpInt);
                if(helpInt<10)
                {
                    paint->drawText(QPointF(paint->device()->width() * coords::x[c2] + k - 3 * (paint->device()->width()/1600.), paint->device()->height() * coords::y[c2] + 15 * (paint->device()->height()/1600.)), helpString);  //-3 getFont->size
                }
                else
                {
                    paint->drawText(QPointF(paint->device()->width() * coords::x[c2] + k - 5 * (paint->device()->width()/1600.), paint->device()->height() * coords::y[c2] + 15 * (paint->device()->height()/1600.)), helpString);  //-5 getFont->size
                }
            }
        }

    }
//    cout<<"spectra drawn"<<endl;

    //write channels labels
    paint->setFont(QFont("Helvetica", int(24*paint->device()->height()/1600.), -1, false));
    for(int c2=0; c2<19; ++c2)  //exept markers channel
    {
        paint->drawText(QPointF((paint->device()->width() * coords::x[c2] - 20 * (paint->device()->width()/1600.)), (paint->device()->height() * coords::y[c2] - (coords::scale * paint->device()->height()) - 2)), QString(coords::lbl[c2]));
    }

    helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("specRange.jpg");
    pic.save(helpString, 0, 100);
    specLabel->setPixmap(pic.scaled(specLabel->size()));

    specLabel->installEventFilter(this);





}

RangeWidget::~RangeWidget()
{
    delete ui;
}

int findChannel(int x, int y, QSize siz)
{
    int a, b, num;
    a = floor( x*16./double(siz.width())/3. );
    b = floor( y*16./double(siz.height())/3. );
    num=0;
    switch(b)
    {
    case 0:
        {
            if(a==1) return 0;
            if(a==3) return 1;
        }
    case 1:{num+=2; break;}
    case 2:{num+=7; break;}
    case 3:{num+=12; break;}
    case 4:
        {
            if(a==1) return 17;
            if(a==3) return 18;
        }
    }
    switch(a)
    {
    case 0:{num+=0; break;}
    case 1:{num+=1; break;}
    case 2:{num+=2; break;}
    case 3:{num+=3; break;}
    case 4:{num+=4; break;}
    }
    return num;
}

bool RangeWidget::eventFilter(QObject *obj, QEvent *event)
{
    if (obj->inherits("QLabel"))
    {
        if (event->type() == QEvent::MouseButtonPress)
        {



            QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
            if(fmod(16*double(mouseEvent->y())/specLabel->height(), 3.) < 0.5)
            {
                return false;
            }

            if(fmod(16*double(mouseEvent->x())/specLabel->width(), 3.) < 0.5)
            {
                if(mouseEvent->button()==Qt::LeftButton)
                {
                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), specLabel->size());
                    rangeLimits[chanNum][0] = 0;
                }
                if(mouseEvent->button()==Qt::RightButton)
                {

                    chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), specLabel->size()) - 1;
                    rangeLimits[chanNum][1] = spLength-1;
                }
                return true;
            }

            paint->end();
            helpString = dirBC->absolutePath().append(QDir::separator()).append("Help").append(QDir::separator()).append("specRange.jpg");
            pic.load(helpString, 0, Qt::ColorOnly);
            paint->begin(&pic);


            chanNum = findChannel(mouseEvent->x(), mouseEvent->y(), specLabel->size());

            if(mouseEvent->button()==Qt::LeftButton)
            {

                rangeLimits[chanNum][0] = floor((mouseEvent->x()*paint->device()->width()/specLabel->width() - coords::x[chanNum]*paint->device()->width())/(coords::scale*paint->device()->width())*spLength);

            }
            if(mouseEvent->button()==Qt::RightButton)
            {
                rangeLimits[chanNum][1] = ceil((mouseEvent->x()*paint->device()->width()/specLabel->width() - coords::x[chanNum]*paint->device()->width())/(coords::scale*paint->device()->width())*spLength);
            }
//            cout<<paint->device()->width()<<endl;
//            cout<<pic.width()<<endl;
            for(int i=0; i<ns; ++i)
            {
                paint->setPen(QPen(QBrush("blue"), 2));
                paint->drawLine(QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][0]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height()), QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][0]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height() - coords::scale*paint->device()->height()));

                paint->setPen(QPen(QBrush("red"), 2));
                paint->drawLine(QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][1]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height()), QPointF(coords::x[i]*paint->device()->width() + rangeLimits[i][1]*coords::scale*paint->device()->width()/spLength, coords::y[i]*paint->device()->height() - coords::scale*paint->device()->height()));
            }



            specLabel->setPixmap(pic.scaled(specLabel->size()));

            return true;
        }
        else
        {
            return false;
        }
    }

    return QWidget::eventFilter(obj, event);
}
