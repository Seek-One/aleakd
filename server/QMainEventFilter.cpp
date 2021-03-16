//
// Created by ebeuque on 10/03/2021.
//

#include <QEvent>

#include "GUIController/QApplicationWindowController.h"

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
	return false;
}