//
// Created by ebeuque on 10/03/2021.
//

#include <QEvent>

#include "QMemoryOperationEvent.h"

#include "QMainEventFilter.h"

QMainEventFilter::QMainEventFilter(QObject *parent)
	: QObject(parent)
{

}

QMainEventFilter::~QMainEventFilter()
{

}

bool QMainEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	if(event->type() == MemoryOperationEvent)
	{
		return true;
	}
	return false;
}