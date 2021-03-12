//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>

#include "QApplicationWindow.h"
#include "QMemoryOperationModel.h"

#include "QApplicationWindowController.h"

QApplicationWindowController::QApplicationWindowController()
{
	m_pApplicationWindow = NULL;
	m_pModels = NULL;
}

QApplicationWindowController::~QApplicationWindowController()
{

}

bool QApplicationWindowController::init(QApplicationWindow* pApplicationWindow)
{
	m_pApplicationWindow = pApplicationWindow;

	m_pModels = new QMemoryOperationModel();
	m_pModels->setMemoryOperationList(&m_listMemoryOperation);

	QTreeView* pTreeView = m_pApplicationWindow->getTreeView();
	pTreeView->setModel(m_pModels);
	pTreeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	pTreeView->header()->resizeSection(0, 200);
	pTreeView->header()->resizeSection(1, 150);
	pTreeView->header()->resizeSection(2, 100);
	pTreeView->header()->resizeSection(3, 100);
	pTreeView->header()->resizeSection(4, 150);
	pTreeView->header()->resizeSection(5, 100);
	pTreeView->header()->resizeSection(6, 150);

	QScrollBar* pScrollBar = m_pApplicationWindow->getScrollBar();
	pScrollBar->setMinimum(0);
	pScrollBar->setMaximum(0);
	connect(pScrollBar, SIGNAL(valueChanged(int)), this, SLOT(onScrollBarValueChanged(int)));

	return true;
}

void QApplicationWindowController::addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation)
{
	m_listMemoryOperation.append(pMemoryOperation);
	int iNewCount = m_listMemoryOperation.count();
	m_pApplicationWindow->getScrollBar()->setMaximum(iNewCount);

	QLabel* pLabel = m_pApplicationWindow->getMemoryOperationCount();
	pLabel->setText(QString::number(iNewCount));
}

void QApplicationWindowController::clearMemoryOperation()
{
	m_listMemoryOperation.clear();
	m_pModels->clear();
}

void QApplicationWindowController::onScrollBarValueChanged(int value)
{
	m_pModels->fetchTo(value);

	QTreeView* pTreeView = m_pApplicationWindow->getTreeView();

	QModelIndex modelIndex = m_pModels->index(value-1, 0);
	pTreeView->scrollTo(modelIndex);

	qDebug("scrollTo: %d", value);
}