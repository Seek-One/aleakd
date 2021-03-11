//
// Created by ebeuque on 10/03/2021.
//

#ifndef ALEAKD_QMAINEVENTFILTER_H
#define ALEAKD_QMAINEVENTFILTER_H

#include <QObject>
#include <QSharedPointer>

#include "MemoryOperation.h"

class QApplicationWindowController;

class QMainEventFilter : public QObject
{
public:
	QMainEventFilter(QObject *parent = 0);
	virtual ~QMainEventFilter();

	void setApplicationWindowController(QApplicationWindowController* pApplicationWindowController);

protected:
	virtual bool eventFilter(QObject *watched, QEvent *event);

private:
	QApplicationWindowController* m_pApplicationWindowController;
};


#endif //ALEAKD_QMAINEVENTFILTER_H
