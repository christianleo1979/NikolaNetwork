/* 
 * File:   NWFreeLayout.cpp
 * Author: christian
 * 
 * Created on April 23, 2016, 3:59 PM
 */

#include "NWFreeLayout.h"
#include "GUI/INikolaWidget.h"

NWFreeLayout::~NWFreeLayout() {    
    QLayoutItem *item;
    while ((item = takeAt(0)))
        delete item;    
}

void NWFreeLayout::addItem(QLayoutItem *item)
{
    list.append(item);
}
 
int NWFreeLayout::count() const
{
    // QList::size() returns the number of QLayoutItems in the list
    return list.size();
}

QLayoutItem *NWFreeLayout::itemAt(int idx) const
{
    // QList::value() performs index checking, and returns 0 if we are
    // outside the valid range
    return list.value(idx);
}

QLayoutItem *NWFreeLayout::takeAt(int idx)
{
    // QList::take does not do index checking
    return idx >= 0 && idx < list.size() ? list.takeAt(idx) : 0;
}

void NWFreeLayout::setGeometry(const QRect &r)
{
    QLayout::setGeometry(r);

    if (list.size() == 0)
        return;

    /*int w = r.width() - (list.count() - 1) * spacing();
    int h = r.height() - (list.count() - 1) * spacing();
    int i = 0;
    while (i < list.size()) {
        QLayoutItem *o = list.at(i);
        QRect geom(r.x() + i * spacing(), r.y() + i * spacing(), w, h);
        o->setGeometry(geom);
        ++i;
    }*/  
 
    int i = 0;
    while (i < list.size()) {
        QLayoutItem *o = list.at(i);
        //QPoint point = parentWidget()->pos(); //mapFromGlobal(QCursor::pos());
        INikolaWidget* nw = dynamic_cast<INikolaWidget*>(o->widget());
        QPoint point = nw->getWidgetDefinition().getPosition();
        QRect geom(point.x(), point.y(), o->widget()->width(), o->widget()->height());
        o->setGeometry(geom);
        ++i;
    }
}

QSize NWFreeLayout::sizeHint() const
{
    QSize s(0,0);
    int n = list.count();
    if (n > 0)
        s = QSize(100,70); //start with a nice default size
    int i = 0;
    while (i < n) {
        QLayoutItem *o = list.at(i);
        s = s.expandedTo(o->sizeHint());
        ++i;
    }
    return s + n*QSize(spacing(), spacing());
}

QSize NWFreeLayout::minimumSize() const
{
    QSize s(0,0);
    int n = list.count();
    int i = 0;
    while (i < n) {
        QLayoutItem *o = list.at(i);
        s = s.expandedTo(o->minimumSize());
        ++i;
    }
    return s + n*QSize(spacing(), spacing());
}