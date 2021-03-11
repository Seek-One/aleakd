//
// Created by ebeuque on 10/03/2021.
//

#include <QEvent>

#include "QApplicationWindowController.h"
#include "QMemoryOperationEvent.h"

#include "QMainEventFilter.h"

QMainEventFilter::QMainEventFilter(QObject *parent)
	: QObject(parent)
{
	m_pApplicationWindowController = NULL;
}

QMainEventFilter::~QMainEventFilter()
{

}

void QMainEventFilter::setApplicationWindowController(QApplicationWindowController* pApplicationWindowController)
{
	m_pApplicationWindowController = pApplicationWindowController;
}

bool QMainEventFilter::eventFilter(QObject *watched, QEvent *event)
{
	if(event->type() == MemoryOperationEvent)
	{
		QMemoryOperationEvent* pEvent = (QMemoryOperationEvent*)event;
		m_pApplicationWindowController->addMemoryOperation(pEvent->m_pMemoryOperation);
		return true;
	}
	return false;
}