//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_QMAINEVENTFILTER_H
#define ALEAKD_QMAINEVENTFILTER_H

#include <QObject>

class QMainEventFilter : public QObject
{
public:
	QMainEventFilter(QObject *parent = 0);
	virtual ~QMainEventFilter();

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);
};


#endif //ALEAKD_QMAINEVENTFILTER_H
