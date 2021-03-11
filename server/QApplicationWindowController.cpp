//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>

#include "QApplicationWindow.h"

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

	m_pModels = new QStandardItemModel();

	QTreeView* pTreeView = m_pApplicationWindow->getTreeView();
	pTreeView->setModel(m_pModels);

	return true;
}

void QApplicationWindowController::addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation)
{
	QList<QStandardItem*> listCols;
	QString szTmp;

	QStandardItem* pItem;

	// Timestamp
	pItem = new QStandardItem();
	szTmp = QString("%0,%1").arg(pMemoryOperation->m_tvOperation.tv_sec).arg(pMemoryOperation->m_tvOperation.tv_usec, 6, 10, QChar('0'));
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Operation type
	pItem = new QStandardItem();
	szTmp = QString::number(pMemoryOperation->m_iThreadId);
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Alloc type
	pItem = new QStandardItem();
	szTmp = ALeakD_TypeName(pMemoryOperation->m_iMemOpType);
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Alloc size
	pItem = new QStandardItem();
	if(pMemoryOperation->m_iAllocSize) {
		szTmp = QString::number(pMemoryOperation->m_iAllocSize);
	}else{
		szTmp = "";
	}
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Alloc ptr
	pItem = new QStandardItem();
	if(pMemoryOperation->m_iAllocPtr) {
		szTmp = "0x" + QString::number(pMemoryOperation->m_iAllocPtr, 16);
	}else{
		szTmp = "";
	}
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Alloc size
	pItem = new QStandardItem();
	if(pMemoryOperation->m_iAllocNum) {
		szTmp = QString::number(pMemoryOperation->m_iAllocNum);
	}else{
		szTmp = "";
	}
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Free
	pItem = new QStandardItem();
	if(pMemoryOperation->m_iFreePtr) {
		szTmp = "0x" + QString::number(pMemoryOperation->m_iFreePtr, 16);
	}else{
		szTmp = "";
	}
	pItem->setText(szTmp);
	listCols.append(pItem);

	m_pModels->appendRow(listCols);
}