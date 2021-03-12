//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QElapsedTimer>

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
	m_pModels->setMemoryOperationList(&m_listFilterMemoryOperation);

	QTreeView* pTreeView = m_pApplicationWindow->getTreeView();
	pTreeView->setModel(m_pModels);
	//pTreeView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	pTreeView->header()->resizeSection(0, 200);
	pTreeView->header()->resizeSection(1, 150);
	pTreeView->header()->resizeSection(2, 100);
	pTreeView->header()->resizeSection(3, 100);
	pTreeView->header()->resizeSection(4, 150);
	pTreeView->header()->resizeSection(5, 100);
	pTreeView->header()->resizeSection(6, 150);

	connect(m_pApplicationWindow->getSearchButton(), SIGNAL(clicked()), this, SLOT(onSearchButtonClicked()));

	m_timerUpdate.setInterval(1000);
	connect(&m_timerUpdate, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
	m_timerUpdate.start();

	return true;
}

void QApplicationWindowController::addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation)
{
	m_lockListMemoryOperation.lockForWrite();
	if(pMemoryOperation->m_iFreePtr){
		m_listMemoryOperation.setPtrFreed(pMemoryOperation->m_iFreePtr);
	}
	m_listMemoryOperation.append(pMemoryOperation);
	m_lockListMemoryOperation.unlock();
}

void QApplicationWindowController::clearMemoryOperation()
{
	m_lockListMemoryOperation.lockForWrite();
	m_listMemoryOperation.clear();
	m_lockListMemoryOperation.unlock();

	m_listFilterMemoryOperation.clear();
	m_pModels->clear();
}

void QApplicationWindowController::onSearchButtonClicked()
{
	bool bNotFreed = true;

	QElapsedTimer timer;
	qDebug("[aleakd-server] Start search");
	timer.start();

	m_listFilterMemoryOperation.clear();
	m_lockListMemoryOperation.lockForRead();
	MemoryOperationList::const_iterator iter;
	for(iter = m_listMemoryOperation.constBegin(); iter != m_listMemoryOperation.constEnd(); ++iter)
	{
		bool bAccept = true;
		MemoryOperationSharedPtr pMemoryOperation = (*iter);
		if(bNotFreed){
			if(pMemoryOperation->m_iMemOpType == ALeakD_free){
				bAccept = false;
			}
			if(pMemoryOperation->m_bFreed){
				bAccept = false;
			}
		}
		if(bAccept) {
			m_listFilterMemoryOperation.append(*iter);
		}
	}
	m_lockListMemoryOperation.unlock();
	m_pModels->clear();
	m_pModels->fetchTo(m_listFilterMemoryOperation.count());

	qDebug("[aleakd-server] Search done in %ld ms", timer.elapsed());
}

void QApplicationWindowController::onTimerUpdate()
{
	int iNewCount = m_listMemoryOperation.count();

	m_pApplicationWindow->setData(QApplicationWindow::StatusBarRow_Global, QApplicationWindow::StatusBarCol_OpCount, QString::number(iNewCount));
}

void QApplicationWindowController::onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation)
{
	addMemoryOperation(pMemoryOperation);
}

void QApplicationWindowController::onNewConnection()
{
	clearMemoryOperation();
}