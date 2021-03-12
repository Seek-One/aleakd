//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>

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

	return true;
}

void QApplicationWindowController::addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation)
{
	if(pMemoryOperation->m_iFreePtr){
		m_listMemoryOperation.setPtrFreed(pMemoryOperation->m_iFreePtr);
	}

	m_listMemoryOperation.append(pMemoryOperation);
	int iNewCount = m_listMemoryOperation.count();

	QLabel* pLabel = m_pApplicationWindow->getMemoryOperationCount();
	pLabel->setText(QString::number(iNewCount));
}

void QApplicationWindowController::clearMemoryOperation()
{
	m_listMemoryOperation.clear();
	m_pModels->clear();
}

void QApplicationWindowController::onSearchButtonClicked()
{
	bool bNotFreed = true;

	m_listFilterMemoryOperation.clear();
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
	m_pModels->clear();
	m_pModels->fetchTo(m_listFilterMemoryOperation.count());
}

void QApplicationWindowController::onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation)
{
	addMemoryOperation(pMemoryOperation);
}

void QApplicationWindowController::onNewConnection()
{
	clearMemoryOperation();
}