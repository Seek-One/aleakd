//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QHeaderView>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QElapsedTimer>
#include <QCheckBox>

#include "Global/Utils.h"

#include "GUI/QApplicationWindow.h"
#include "GUI/QMemoryOperationView.h"
#include "GUI/QThreadInfosView.h"

#include "GUIModel/QMemoryOperationModel.h"
#include "GUIModel/QThreadInfosModel.h"

#include "GUIController/QApplicationWindowController.h"

QApplicationWindowController::QApplicationWindowController()
{
	m_pApplicationWindow = NULL;
	m_pModelMemoryOperation = NULL;
	m_pModelThreadInfos = NULL;
}

QApplicationWindowController::~QApplicationWindowController()
{
	if(m_pModelMemoryOperation){
		delete m_pModelMemoryOperation;
		m_pModelMemoryOperation = NULL;
	}
	if(m_pModelThreadInfos){
		delete m_pModelThreadInfos;
		m_pModelThreadInfos = NULL;
	}
}

bool QApplicationWindowController::init(QApplicationWindow* pApplicationWindow)
{
	m_pApplicationWindow = pApplicationWindow;

	// Thread infos tab
	{
		m_pThreadInfosView = pApplicationWindow->getThreadInfosView();

		m_pModelThreadInfos = new QThreadInfosModel();
		m_pModelThreadInfos->setThreadInfosList(&m_listFilterThreadInfos);

		QTreeView *pTreeView = m_pThreadInfosView->getTreeView();
		pTreeView->setModel(m_pModelThreadInfos);

		pTreeView->header()->resizeSection(0, 200);
		pTreeView->header()->resizeSection(1, 200);
		pTreeView->header()->resizeSection(2, 150);
		pTreeView->header()->resizeSection(3, 150);
		pTreeView->header()->resizeSection(4, 150);
		pTreeView->header()->resizeSection(5, 150);
	}

	// Memory infos tab
	{
		m_pMemoryOperationView = pApplicationWindow->getMemoryOperationView();

		m_pModelMemoryOperation = new QMemoryOperationModel();
		m_pModelMemoryOperation->setMemoryOperationList(&m_listFilterMemoryOperation);

		QTreeView *pTreeView = m_pMemoryOperationView->getTreeView();
		pTreeView->setModel(m_pModelMemoryOperation);

		pTreeView->header()->resizeSection(0, 200);
		pTreeView->header()->resizeSection(1, 150);
		pTreeView->header()->resizeSection(2, 100);
		pTreeView->header()->resizeSection(3, 100);
		pTreeView->header()->resizeSection(4, 150);
		pTreeView->header()->resizeSection(5, 100);
		pTreeView->header()->resizeSection(6, 150);

		connect(m_pMemoryOperationView->getFilterButton(), SIGNAL(clicked()), this, SLOT(onFilterButtonClicked()));
	}

	m_timerUpdate.setInterval(1000);
	connect(&m_timerUpdate, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
	m_timerUpdate.start();

	return true;
}

void QApplicationWindowController::clearData()
{
	m_lockListMemoryOperation.lockForWrite();
	m_listMemoryOperation.clear();
	m_lockListMemoryOperation.unlock();

	m_searchStats.reset();
	m_lockGlobalStats.lockForWrite();
	m_globalStats.reset();
	m_listThreadInfos.clear();
	m_lockGlobalStats.unlock();

	m_listFilterMemoryOperation.clear();
	m_pModelMemoryOperation->clear();
	m_listFilterThreadInfos.clear();
	m_pModelThreadInfos->clear();
}

void QApplicationWindowController::addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation)
{
	MemoryOperationSharedPtr pMemoryOperationFreed;

	m_lockListMemoryOperation.lockForWrite();
	if(pMemoryOperation->m_iFreePtr){
		pMemoryOperationFreed = m_listMemoryOperation.getPtrNotFreed(pMemoryOperation->m_iFreePtr);
		if(pMemoryOperationFreed) {
			pMemoryOperationFreed->m_bFreed = true;
		}
	}
	m_listMemoryOperation.append(pMemoryOperation);

	m_lockGlobalStats.lockForWrite();
	m_globalStats.m_iOpCount++;
	m_globalStats.m_iOperationSize += sizeof(MemoryOperation);
	m_globalStats.m_iTotalAllocSize += pMemoryOperation->m_iAllocSize;
	m_globalStats.m_iTotalRemainingSize += pMemoryOperation->m_iAllocSize;
	if(pMemoryOperationFreed){
		m_globalStats.m_iTotalFreeSize += pMemoryOperationFreed->m_iAllocSize;
		m_globalStats.m_iTotalRemainingSize -= pMemoryOperationFreed->m_iAllocSize;
		updateThreadInfosAddFree(pMemoryOperationFreed->m_iCallerThreadId, pMemoryOperationFreed->m_iAllocSize);
	}
	if(pMemoryOperation->m_iAllocPtr) {
		updateThreadInfosAddAlloc(pMemoryOperation->m_iCallerThreadId, pMemoryOperation->m_iAllocSize);
	}
	switch (pMemoryOperation->m_iMsgCode) {
	case ALeakD_MsgCode_malloc:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iMallocCount++;
		break;
	case ALeakD_MsgCode_calloc:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iCallocCount++;
		break;
	case ALeakD_MsgCode_realloc:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iTotalFreeCount++;
		m_globalStats.m_iReallocCount++;
		break;
	case ALeakD_MsgCode_free:
		m_globalStats.m_iTotalFreeCount++;
		m_globalStats.m_iFreeCount++;
		break;
	case ALeakD_MsgCode_posix_memalign:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iPosixMemalignCount++;
		break;
	case ALeakD_MsgCode_aligned_alloc:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iAlignedAllocCount++;
		break;
	case ALeakD_MsgCode_memalign:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iMemAlignCount++;
		break;
	case ALeakD_MsgCode_valloc:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iVAllocCount++;
		break;
	case ALeakD_MsgCode_pvalloc:
		m_globalStats.m_iTotalAllocCount++;
		m_globalStats.m_iPVAllocCount++;
		break;
	}
	m_globalStats.m_iTotalRemainingCount = m_globalStats.m_iTotalAllocCount - m_globalStats.m_iTotalFreeCount;
	m_lockGlobalStats.unlock();

	m_lockListMemoryOperation.unlock();
}

void QApplicationWindowController::updateThreadInfos(const ThreadOperationSharedPtr& pThreadOperation)
{
	m_lockGlobalStats.lockForWrite();
	ThreadInfosSharedPtr pThreadInfos = getThreadInfos(pThreadOperation->m_iThreadId);
	if (pThreadOperation->m_iMsgCode == ALeakD_MsgCode_pthread_set_name) {
		if(pThreadInfos) {
			pThreadInfos->m_szThreadName = pThreadOperation->m_szThreadName;
		}
	}
	m_lockGlobalStats.unlock();
}

void QApplicationWindowController::updateThreadInfosAddAlloc(uint64_t iThreadId, uint64_t iSize)
{
	ThreadInfosSharedPtr pThreadInfos = getThreadInfos(iThreadId);
	if(pThreadInfos){
		pThreadInfos->m_iCurrentSize += iSize;
		pThreadInfos->m_iCurrentAllocCount++;
		pThreadInfos->m_iTotalAllocCount++;
		if(pThreadInfos->m_iCurrentSize > pThreadInfos->m_iPeakSize){
			pThreadInfos->m_iPeakSize = pThreadInfos->m_iCurrentSize;
		}
	}
}

void QApplicationWindowController::updateThreadInfosAddFree(uint64_t iThreadId, uint64_t iSize)
{
	ThreadInfosSharedPtr pThreadInfos = getThreadInfos(iThreadId);
	if(pThreadInfos){
		pThreadInfos->m_iCurrentSize -= iSize;
		pThreadInfos->m_iCurrentAllocCount--;
	}
}

ThreadInfosSharedPtr QApplicationWindowController::getThreadInfos(uint64_t iThreadId)
{
	// Assume m_listThreadInfos has been protected by caller
	ThreadInfosSharedPtr pThreadInfos;
	pThreadInfos = m_listThreadInfos.getById(iThreadId);
	if(!pThreadInfos){
		pThreadInfos = ThreadInfosSharedPtr(new ThreadInfos());
		pThreadInfos->m_iThreadId = iThreadId;
		if(m_listThreadInfos.isEmpty()){
			pThreadInfos->m_szThreadName = "main";
		}
		m_listThreadInfos.append(pThreadInfos);
	}

	return pThreadInfos;
}

void QApplicationWindowController::onFilterButtonClicked()
{
	QElapsedTimer timer;
	qDebug("[aleakd-server] Start search");
	timer.start();

	bool bNotFreed = m_pMemoryOperationView->getNotFreeOnlyCheckBox()->isChecked();

	m_searchStats.reset();

	m_listFilterMemoryOperation.clear();
	m_lockListMemoryOperation.lockForRead();
	MemoryOperationList::const_iterator iter;
	for(iter = m_listMemoryOperation.constBegin(); iter != m_listMemoryOperation.constEnd(); ++iter)
	{
		bool bAccept = true;
		MemoryOperationSharedPtr pMemoryOperation = (*iter);
		if(bNotFreed){
			if(pMemoryOperation->m_iMsgCode == ALeakD_MsgCode_free){
				bAccept = false;
			}
			if(pMemoryOperation->m_bFreed){
				bAccept = false;
			}
		}
		if(bAccept) {
			m_searchStats.m_iOpCount++;
			m_searchStats.m_iTotalAllocSize += pMemoryOperation->m_iAllocSize;
			m_searchStats.m_iTotalRemainingSize += pMemoryOperation->m_iAllocSize;
			if(pMemoryOperation->m_bFreed){
				m_searchStats.m_iTotalFreeSize += pMemoryOperation->m_iAllocSize;
				m_searchStats.m_iTotalRemainingSize -= pMemoryOperation->m_iAllocSize;
			}
			switch (pMemoryOperation->m_iMsgCode) {
			case ALeakD_MsgCode_malloc:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iMallocCount++;
				break;
			case ALeakD_MsgCode_calloc:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iCallocCount++;
				break;
			case ALeakD_MsgCode_realloc:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iTotalFreeCount++;
				m_searchStats.m_iReallocCount++;
				break;
			case ALeakD_MsgCode_free:
				m_searchStats.m_iTotalFreeCount++;
				m_searchStats.m_iFreeCount++;
				break;
			case ALeakD_MsgCode_posix_memalign:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iPosixMemalignCount++;
				break;
			case ALeakD_MsgCode_aligned_alloc:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iAlignedAllocCount++;
				break;
			case ALeakD_MsgCode_memalign:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iMemAlignCount++;
				break;
			case ALeakD_MsgCode_valloc:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iVAllocCount++;
				break;
			case ALeakD_MsgCode_pvalloc:
				m_searchStats.m_iTotalAllocCount++;
				m_searchStats.m_iPVAllocCount++;
				break;
			}
			m_searchStats.m_iTotalRemainingCount = m_searchStats.m_iTotalAllocCount - m_searchStats.m_iTotalFreeCount;

			m_listFilterMemoryOperation.append(*iter);
		}
	}
	m_lockListMemoryOperation.unlock();
	m_pModelMemoryOperation->clear();
	m_pModelMemoryOperation->fetchTo(m_listFilterMemoryOperation.count());

	qDebug("[aleakd-server] Search done in %ld ms", timer.elapsed());
}

void QApplicationWindowController::onTimerUpdate()
{
	m_lockGlobalStats.lockForRead();

	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_OpCount, QString::number(m_searchStats.m_iOpCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_TotalAllocCount, QString::number(m_searchStats.m_iTotalAllocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_TotalAllocSize, QString::number(m_searchStats.m_iTotalAllocSize));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_TotalFreeCount, QString::number(m_searchStats.m_iTotalFreeCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_TotalFreeSize, QString::number(m_searchStats.m_iTotalFreeSize));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_TotalRemainingCount, QString::number(m_searchStats.m_iTotalRemainingCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_TotalRemainingSize, QString::number(m_searchStats.m_iTotalRemainingSize));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_malloc, QString::number(m_searchStats.m_iMallocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_calloc, QString::number(m_searchStats.m_iCallocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_realloc, QString::number(m_searchStats.m_iReallocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_free, QString::number(m_searchStats.m_iFreeCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_posix_memalign, QString::number(m_searchStats.m_iPosixMemalignCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_aligned_alloc, QString::number(m_searchStats.m_iAlignedAllocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_memalign, QString::number(m_searchStats.m_iMemAlignCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_valloc, QString::number(m_searchStats.m_iVAllocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Search, QMemoryOperationView::StatusBarCol_pvalloc, QString::number(m_searchStats.m_iPVAllocCount));

	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_TotalAllocCount, QString::number(m_globalStats.m_iTotalAllocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_OpCount, QString::number(m_globalStats.m_iOpCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_TotalAllocSize, QString::number(m_globalStats.m_iTotalAllocSize));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_TotalFreeCount, QString::number(m_globalStats.m_iTotalFreeCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_TotalFreeSize, QString::number(m_globalStats.m_iTotalFreeSize));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_TotalRemainingCount, QString::number(m_globalStats.m_iTotalRemainingCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_TotalRemainingSize, QString::number(m_globalStats.m_iTotalRemainingSize));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_malloc, QString::number(m_globalStats.m_iMallocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_calloc, QString::number(m_globalStats.m_iCallocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_realloc, QString::number(m_globalStats.m_iReallocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_free, QString::number(m_globalStats.m_iFreeCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_posix_memalign, QString::number(m_globalStats.m_iPosixMemalignCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_aligned_alloc, QString::number(m_globalStats.m_iAlignedAllocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_memalign, QString::number(m_globalStats.m_iMemAlignCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_valloc, QString::number(m_globalStats.m_iVAllocCount));
	m_pMemoryOperationView->setData(QMemoryOperationView::StatusBarRow_Global, QMemoryOperationView::StatusBarCol_pvalloc, QString::number(m_globalStats.m_iPVAllocCount));

	m_pApplicationWindow->setCaptureMemoryOperationCount(Utils::getBeautifulNumberString(QString::number(m_globalStats.m_iOpCount)));
	m_pApplicationWindow->setCaptureMemorySizeUsed(Utils::getBeautifulNumberString(QString::number(m_globalStats.m_iOperationSize)));
	m_pApplicationWindow->setCaptureThreadCount(Utils::getBeautifulNumberString(QString::number(m_listThreadInfos.count())));

	m_listFilterThreadInfos.clear();
	ThreadInfosList::const_iterator iter;
	for(iter = m_listThreadInfos.constBegin(); iter != m_listThreadInfos.constEnd(); ++iter)
	{
		m_listFilterThreadInfos.append(*iter);
	}
	m_pModelThreadInfos->refresh();
	m_lockGlobalStats.unlock();
}

void QApplicationWindowController::onNewConnection()
{
	clearData();
}

void QApplicationWindowController::onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation)
{
	addMemoryOperation(pMemoryOperation);
}

void QApplicationWindowController::onThreadOperationReceived(const ThreadOperationSharedPtr& pThreadOperation)
{
	updateThreadInfos(pThreadOperation);
}