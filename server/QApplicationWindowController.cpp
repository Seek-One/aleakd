//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QHeaderView>

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

	QList<QString> listLabel;
	listLabel.append(tr("Timestamp"));
	listLabel.append(tr("Thread"));
	listLabel.append(tr("Operation"));
	listLabel.append(tr("Alloc size"));
	listLabel.append(tr("Alloc ptr"));
	listLabel.append(tr("Alloc num"));
	listLabel.append(tr("Free ptr"));
	m_pModels->setHorizontalHeaderLabels(listLabel);

	pTreeView->header()->resizeSection(0, 200);
	pTreeView->header()->resizeSection(1, 150);
	pTreeView->header()->resizeSection(2, 100);
	pTreeView->header()->resizeSection(3, 100);
	pTreeView->header()->resizeSection(4, 150);
	pTreeView->header()->resizeSection(5, 100);
	pTreeView->header()->resizeSection(6, 150);

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

	// Thread ID
	pItem = new QStandardItem();
	szTmp = QString::number(pMemoryOperation->m_iThreadId);
	pItem->setText(szTmp);
	listCols.append(pItem);

	// Operation type
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

	// Alloc num
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

void QApplicationWindowController::clearMemoryOperation()
{
	m_pModels->removeRows(0, m_pModels->rowCount());
}