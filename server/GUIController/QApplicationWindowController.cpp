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
#include <QComboBox>
#include <QLineEdit>
#include <QStandardItemModel>
#include <QLineSeries>
#include <QDateTime>

#include "Global/Utils.h"

#include "GUI/QApplicationWindow.h"
#include "GUI/QMemoryUsageView.h"
#include "GUI/QMemoryOperationListView.h"
#include "GUI/QMemoryOperationView.h"
#include "GUI/QThreadInfosView.h"
#include "GUI/QMemoryStatsView.h"

#include "GUIModel/QMemoryOperationModel.h"
#include "GUIModel/QThreadInfosModel.h"

#include "GUIController/QApplicationWindowController.h"

#define timerset(dst, sec, usec) \
		(dst)->tv_sec = sec; \
		(dst)->tv_usec = usec;
#define timercpy(dst, src) \
		(dst)->tv_sec = (src)->tv_sec; \
		(dst)->tv_usec = (src)->tv_usec;

#define timer_eq(a, b) !timercmp(a, b, !=)
#define timer_ne(a, b) timercmp(a, b, !=)
#define timer_gt(a, b) timercmp(a, b, >)
#define timer_ge(a, b) !timercmp(a, b, <)
#define timer_lt(a, b) timercmp(a, b, <)
#define timer_le(a, b) !timercmp(a, b, >)


QApplicationWindowController::QApplicationWindowController()
{
	m_pApplicationWindow = NULL;
	m_pMemoryUsageView = NULL;
	m_pThreadInfosView = NULL;
	m_pMemoryOperationListView = NULL;
	m_pModelMemoryOperation = NULL;
	m_pModelThreadInfos = NULL;
	m_pMemoryStatsView = NULL;

	m_iLastUpdateTime = 0;
	m_iLastUpdateValue = 0;
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

	// Memory usage
	{
		m_pMemoryUsageView = pApplicationWindow->getMemoryUsageView();
	}

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
		pTreeView->header()->resizeSections(QHeaderView::Interactive);

		// Double clicked
		connect(m_pThreadInfosView->getTreeView(), SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onThreadInfosDoubleClicked(const QModelIndex &)));
	}

	// Memory infos tab
	{
		m_pMemoryOperationListView = pApplicationWindow->getMemoryOperationListView();

		m_pModelMemoryOperation = new QMemoryOperationModel();
		m_pModelMemoryOperation->setMemoryOperationList(&m_listFilterMemoryOperation);

		QTreeView *pTreeView = m_pMemoryOperationListView->getTreeView();
		pTreeView->setIndentation(0);
		pTreeView->setModel(m_pModelMemoryOperation);
		pTreeView->header()->resizeSection(QMemoryOperationModel::MsgNum, 75);
		pTreeView->header()->resizeSection(QMemoryOperationModel::TimeStampColumn, 200);
		pTreeView->header()->resizeSection(QMemoryOperationModel::ThreadColumn, 150);
		pTreeView->header()->resizeSection(QMemoryOperationModel::OperationColumn, 100);
		pTreeView->header()->resizeSection(QMemoryOperationModel::AllocSizeColumn, 100);
		pTreeView->header()->resizeSection(QMemoryOperationModel::AllocPtrColumn, 150);
		pTreeView->header()->resizeSection(QMemoryOperationModel::AllocNumColumn, 100);
		pTreeView->header()->resizeSection(QMemoryOperationModel::FreePtrColumn, 150);
		pTreeView->header()->resizeSections(QHeaderView::Interactive);

		connect(m_pMemoryOperationListView->getFilterButton(), SIGNAL(clicked()), this, SLOT(onFilterButtonClicked()));

		// Double clicked
		connect(m_pMemoryOperationListView->getTreeView(), SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(onMemoryOperationDoubleClicked(const QModelIndex &)));

	}

	// Memory stats
	{
		m_pMemoryStatsView = pApplicationWindow->getMemoryStatsView();
	}

	// Refresh timer
	m_timerUpdate.setInterval(1000);
	connect(&m_timerUpdate, SIGNAL(timeout()), this, SLOT(onTimerUpdate()));
	m_timerUpdate.start();

	return true;
}

void QApplicationWindowController::clearData()
{
	m_lockListMemoryOperation.lockForWrite();
	m_listMemoryOperation.clear();
	m_listMemoryOperationNonFreed.clear();
	m_listMemoryOperationWithoutBacktrace.clear();
	m_lockListMemoryOperation.unlock();

	m_lockSymbolInfos.lockForWrite();
	m_listSymbolInfos.clear();
	m_lockSymbolInfos.unlock();

	m_searchStats.reset();
	m_lockGlobalStats.lockForWrite();
	m_globalStats.reset();
	m_listThreadInfos.clear();
	m_lockGlobalStats.unlock();

	m_listFilterMemoryOperation.clear();
	m_pModelMemoryOperation->clear();
	m_listFilterThreadInfos.clear();
	m_pModelThreadInfos->clear();

	m_pMemoryOperationListView->getThreadIdComboBox()->clear();
	m_pMemoryUsageView->getLineSeries()->clear();

	m_iLastUpdateTime = 0;
	m_iLastUpdateValue = 0;
}

void QApplicationWindowController::addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation)
{
	MemoryOperationSharedPtr pMemoryOperationFreed;

	m_lockListMemoryOperation.lockForWrite();
	if(pMemoryOperation->m_iFreePtr){
		pMemoryOperationFreed = m_listMemoryOperationNonFreed.takeByPtrNotFreed(pMemoryOperation->m_iFreePtr);
		if(pMemoryOperationFreed) {
			pMemoryOperationFreed->m_bFreed = true;
		}
	}
	m_listMemoryOperation.append(pMemoryOperation);
	m_listMemoryOperationWithoutBacktrace.append(pMemoryOperation);
	if(pMemoryOperation->hasAllocOperation()) {
		m_listMemoryOperationNonFreed.append(pMemoryOperation);
	}

	m_lockGlobalStats.lockForWrite();
	m_globalStats.m_iMessageCount++;
	if(!timerisset(&m_globalStats.m_tvStartTime)){
		timercpy(&m_globalStats.m_tvStartTime, &pMemoryOperation->m_tvOperation);
	}
	timercpy(&m_globalStats.m_tvLastTime, &pMemoryOperation->m_tvOperation);
	m_globalStats.m_iMemoryOperationCount++;
	m_globalStats.m_iMemoryOperationSize += sizeof(MemoryOperation);
	m_globalStats.m_iTotalAllocSize += pMemoryOperation->m_iAllocSize;
	m_globalStats.m_iTotalRemainingSize += pMemoryOperation->m_iAllocSize;
	if(m_globalStats.m_iTotalRemainingSize > m_globalStats.m_iPeakRemainingSize){
		m_globalStats.m_iPeakRemainingSize = m_globalStats.m_iTotalRemainingSize;
	}
	if(pMemoryOperationFreed){
		m_globalStats.m_iTotalFreeSize += pMemoryOperationFreed->m_iAllocSize;
		m_globalStats.m_iTotalRemainingSize -= pMemoryOperationFreed->m_iAllocSize;
		updateThreadInfosAddFree(pMemoryOperationFreed->m_iCallerThreadId, pMemoryOperationFreed->m_iMsgNum, pMemoryOperationFreed->m_iAllocSize, pMemoryOperation->m_tvOperation);
	}
	m_globalStats.m_memoryUsage.update(pMemoryOperation->m_tvOperation.tv_sec, m_globalStats.m_iTotalRemainingSize);
	if(pMemoryOperation->m_iAllocPtr) {
		updateThreadInfosAddAlloc(pMemoryOperation->m_iCallerThreadId, pMemoryOperation->m_iMsgNum, pMemoryOperation->m_iAllocSize, pMemoryOperation->m_tvOperation);
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
		if(pMemoryOperation->m_iFreePtr) {
			m_globalStats.m_iTotalFreeCount++;
		}
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
	bool bThreadCreation = false;
	if (pThreadOperation->m_iMsgCode == ALeakD_MsgCode_pthread_create) {
		bThreadCreation = true;
	}
	m_lockGlobalStats.lockForWrite();
	m_globalStats.m_iMessageCount++;
	ThreadInfosSharedPtr pThreadInfos = getThreadInfos(pThreadOperation->m_iThreadId, bThreadCreation, pThreadOperation->m_tvOperation);
	if (pThreadOperation->m_iMsgCode == ALeakD_MsgCode_pthread_create) {
		if(pThreadInfos) {
			pThreadInfos->m_iCreationMsgNum = pThreadOperation->m_iMsgNum;
			pThreadInfos->m_iLastAllocMsgNum = pThreadOperation->m_iMsgNum;
		}
	}
	if (pThreadOperation->m_iMsgCode == ALeakD_MsgCode_pthread_set_name) {
		if(pThreadInfos) {
			pThreadInfos->m_szThreadName = pThreadOperation->m_szThreadName;
		}
	}
	m_lockGlobalStats.unlock();
}

void QApplicationWindowController::updateThreadInfosAddAlloc(uint64_t iThreadId, uint32_t iMsgNum, uint64_t iSize, struct timeval& tvOperation)
{
	ThreadInfosSharedPtr pThreadInfos = getThreadInfos(iThreadId, false, tvOperation);
	if(pThreadInfos){
		if(pThreadInfos->m_iCreationMsgNum == 0){
			pThreadInfos->m_iCreationMsgNum = iMsgNum;
		}
		pThreadInfos->m_iLastAllocMsgNum = iMsgNum;
		pThreadInfos->m_iCurrentSize += iSize;
		pThreadInfos->m_iCurrentAllocCount++;
		pThreadInfos->m_iTotalAllocCount++;
		if(pThreadInfos->m_iCurrentSize > pThreadInfos->m_iPeakSize){
			pThreadInfos->m_iPeakSize = pThreadInfos->m_iCurrentSize;
		}
	}
}

void QApplicationWindowController::updateThreadInfosAddFree(uint64_t iThreadId, uint32_t iAllocMsgNum, uint64_t iSize, struct timeval& tvOperation)
{
	ThreadInfosSharedPtr pThreadInfos = getThreadInfosForMsgNum(iThreadId, iAllocMsgNum);
	if(!pThreadInfos){
		// Not supposed to occurs
		pThreadInfos = getThreadInfos(iThreadId, false, tvOperation);
	}
	if(pThreadInfos){
		pThreadInfos->m_iCurrentSize -= iSize;
		pThreadInfos->m_iCurrentAllocCount--;
	}
}

ThreadInfosSharedPtr QApplicationWindowController::getThreadInfos(uint64_t iThreadId, bool bThreadCreation, struct timeval& tvOperation)
{
	// Assume m_listThreadInfos has been protected by caller
	ThreadInfosSharedPtr pThreadInfos;
	pThreadInfos = m_listThreadInfosAlive.getById(iThreadId);
	if(pThreadInfos && bThreadCreation){
		// This is a recycled thread, we create a new entry
		pThreadInfos->m_bIsTerminated = true;
		pThreadInfos->m_tvTermination = tvOperation;
		pThreadInfos = ThreadInfosSharedPtr();
		m_listThreadInfosAlive.removeById(iThreadId);
	}
	if(!pThreadInfos){
		pThreadInfos = ThreadInfosSharedPtr(new ThreadInfos());
		pThreadInfos->m_iThreadId = iThreadId;
		pThreadInfos->m_tvCreation = tvOperation;
		if(m_listThreadInfos.isEmpty()){
			pThreadInfos->m_szThreadName = "main";
		}
		m_listThreadInfos.append(pThreadInfos);
		m_listThreadInfosAlive.append(pThreadInfos);
	}

	return pThreadInfos;
}

ThreadInfosSharedPtr QApplicationWindowController::getThreadInfosForMsgNum(uint64_t iThreadId, uint32_t iMsgNum)
{
	// Assume m_listThreadInfos has been protected by caller
	ThreadInfosSharedPtr pThreadInfos;
	// Check first in thread alive
	pThreadInfos = m_listThreadInfosAlive.getByIdAndMsgNum(iThreadId, iMsgNum);
	if(pThreadInfos){
		return pThreadInfos;
	}
	// Fallback into global list
	pThreadInfos = m_listThreadInfos.getByIdAndMsgNum(iThreadId, iMsgNum);
	return pThreadInfos;
}


void QApplicationWindowController::addBacktrace(const BacktraceSharedPtr& pBacktrace)
{
	m_lockListMemoryOperation.lockForWrite();
	MemoryOperationSharedPtr pMemoryOperation = m_listMemoryOperationWithoutBacktrace.takeByMsgNum(pBacktrace->m_iOriginMsgNum);
	if(pMemoryOperation){
		pMemoryOperation->m_pBackTrace = pBacktrace;
	}
	m_lockListMemoryOperation.unlock();

	m_lockGlobalStats.lockForWrite();
	m_globalStats.m_iMessageCount++;
	m_lockGlobalStats.unlock();
}

void QApplicationWindowController::addSymbolInfos(const SymbolInfosSharedPtr& pSymbolInfos)
{
	m_lockSymbolInfos.lockForWrite();
	if(!m_listSymbolInfos.contains(pSymbolInfos->m_iAddr)){
		m_listSymbolInfos.insert(pSymbolInfos->m_iAddr, pSymbolInfos);
	}
	m_lockSymbolInfos.unlock();

	m_lockGlobalStats.lockForWrite();
	m_globalStats.m_iMessageCount++;
	m_lockGlobalStats.unlock();
}

void QApplicationWindowController::onFilterButtonClicked()
{
	QElapsedTimer timer;
	qDebug("[aleakd-server] Start search");
	timer.start();

	QString szTmp;
	QStringList tokens;
	struct timeval tvMin;
	struct timeval tvMax;
	timerclear(&tvMin);
	timerclear(&tvMax);

	szTmp = m_pMemoryOperationListView->getTimeStampMinLineEdit()->text();
	textToTimeval(szTmp, tvMin);
	szTmp = m_pMemoryOperationListView->getTimeStampMaxLineEdit()->text();
	textToTimeval(szTmp, tvMax);

	qulonglong iThreadId = 0;
	if(m_pMemoryOperationListView->getThreadIdComboBox()->currentIndex() != -1){
		iThreadId = m_pMemoryOperationListView->getThreadIdComboBox()->currentData().toULongLong();
	}else{
		iThreadId = m_pMemoryOperationListView->getThreadIdComboBox()->currentText().toULongLong();
	}
	bool bNotFreed = m_pMemoryOperationListView->getNotFreeOnlyCheckBox()->isChecked();

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
		if(iThreadId > 0){
			if(pMemoryOperation->m_iCallerThreadId != iThreadId){
				bAccept = false;
			}
		}
		if(timerisset(&tvMin)){
			if(timer_lt(&pMemoryOperation->m_tvOperation, &tvMin)){
				bAccept = false;
			}
		}
		if(timerisset(&tvMax)){
			if(timer_gt(&pMemoryOperation->m_tvOperation, &tvMax)){
				bAccept = false;
			}
		}
		if(bAccept) {
			m_searchStats.m_iMemoryOperationCount++;
			m_searchStats.m_iTotalAllocSize += pMemoryOperation->m_iAllocSize;
			m_searchStats.m_iTotalRemainingSize += pMemoryOperation->m_iAllocSize;
			if(m_searchStats.m_iTotalRemainingSize > m_searchStats.m_iPeakRemainingSize){
				m_searchStats.m_iPeakRemainingSize = m_searchStats.m_iTotalRemainingSize;
			}
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
				if(pMemoryOperation->m_iFreePtr) {
					m_searchStats.m_iTotalFreeCount++;
				}
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
	MemoryUsage memoryUsageUpdate;
	quint64 iPeakMemoryUsage = 0;
	struct timeval tvStart;

	m_lockGlobalStats.lockForRead();

	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::OpCount, QString::number(m_searchStats.m_iMemoryOperationCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::TotalAllocCount, QString::number(m_searchStats.m_iTotalAllocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::TotalAllocSize, QString::number(m_searchStats.m_iTotalAllocSize));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::TotalFreeCount, QString::number(m_searchStats.m_iTotalFreeCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::TotalFreeSize, QString::number(m_searchStats.m_iTotalFreeSize));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::TotalRemainingCount, QString::number(m_searchStats.m_iTotalRemainingCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::TotalRemainingSize, QString::number(m_searchStats.m_iTotalRemainingSize));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::malloc, QString::number(m_searchStats.m_iMallocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::calloc, QString::number(m_searchStats.m_iCallocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::realloc, QString::number(m_searchStats.m_iReallocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::free, QString::number(m_searchStats.m_iFreeCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::posix_memalign, QString::number(m_searchStats.m_iPosixMemalignCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::aligned_alloc, QString::number(m_searchStats.m_iAlignedAllocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::memalign, QString::number(m_searchStats.m_iMemAlignCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::valloc, QString::number(m_searchStats.m_iVAllocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Filter, QMemoryStatsView::pvalloc, QString::number(m_searchStats.m_iPVAllocCount));

	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::TotalAllocCount, QString::number(m_globalStats.m_iTotalAllocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::OpCount, QString::number(m_globalStats.m_iMemoryOperationCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::TotalAllocSize, QString::number(m_globalStats.m_iTotalAllocSize));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::TotalFreeCount, QString::number(m_globalStats.m_iTotalFreeCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::TotalFreeSize, QString::number(m_globalStats.m_iTotalFreeSize));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::TotalRemainingCount, QString::number(m_globalStats.m_iTotalRemainingCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::TotalRemainingSize, QString::number(m_globalStats.m_iTotalRemainingSize));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::malloc, QString::number(m_globalStats.m_iMallocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::calloc, QString::number(m_globalStats.m_iCallocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::realloc, QString::number(m_globalStats.m_iReallocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::free, QString::number(m_globalStats.m_iFreeCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::posix_memalign, QString::number(m_globalStats.m_iPosixMemalignCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::aligned_alloc, QString::number(m_globalStats.m_iAlignedAllocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::memalign, QString::number(m_globalStats.m_iMemAlignCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::valloc, QString::number(m_globalStats.m_iVAllocCount));
	m_pMemoryStatsView->setData(QMemoryStatsView::Global, QMemoryStatsView::pvalloc, QString::number(m_globalStats.m_iPVAllocCount));

	m_pApplicationWindow->setCaptureMessageCount(Utils::getBeautifulNumberString(QString::number(m_globalStats.m_iMessageCount)));
	m_pApplicationWindow->setCaptureMemoryOperationCount(Utils::getBeautifulNumberString(QString::number(m_globalStats.m_iMemoryOperationCount)));
	m_pApplicationWindow->setCaptureMemorySizeUsed(Utils::getBeautifulNumberString(QString::number(m_globalStats.m_iMemoryOperationSize)));
	m_pApplicationWindow->setCaptureThreadCount(Utils::getBeautifulNumberString(QString::number(m_listThreadInfos.count())));
	m_lockSymbolInfos.lockForRead();
	m_pApplicationWindow->setCaptureSymbolInfosCount(Utils::getBeautifulNumberString(QString::number(m_listSymbolInfos.count())));
	m_lockSymbolInfos.unlock();

	QComboBox* pThreadIdComboBox = m_pMemoryOperationListView->getThreadIdComboBox();
	m_listFilterThreadInfos.clear();
	if(pThreadIdComboBox->count() == 0)
	{
		pThreadIdComboBox->addItem(tr("All threads"), 0);
	}
	ThreadInfosList::const_iterator iter;
	for(iter = m_listThreadInfos.constBegin(); iter != m_listThreadInfos.constEnd(); ++iter)
	{
		const ThreadInfosSharedPtr& pThreadInfos = (*iter);
		qulonglong iThreadId = (qulonglong)pThreadInfos->m_iThreadId;
		QString szEntryLabel;
		if(pThreadInfos->m_szThreadName.isEmpty()) {
			szEntryLabel = QString::number(iThreadId);
		}else{
			szEntryLabel = QString("%0 (%1)").arg(iThreadId).arg(pThreadInfos->m_szThreadName);
		}
		int iFind = pThreadIdComboBox->findData(iThreadId);
		if(iFind == -1){
			pThreadIdComboBox->addItem(szEntryLabel, iThreadId);
		}else{
			pThreadIdComboBox->setItemText(iFind, szEntryLabel);
		}

		m_listFilterThreadInfos.append(pThreadInfos);
	}
	m_pModelThreadInfos->refresh();

	iPeakMemoryUsage = m_globalStats.m_iPeakRemainingSize;
	timercpy(&tvStart, &m_globalStats.m_tvStartTime);

	quint64 iStartTime = (m_iLastUpdateTime == 0 ? m_globalStats.m_memoryUsage.m_iFirstTime : m_iLastUpdateTime);
	if(iStartTime != 0)
	{
		quint64 iLastSize = 0;

		for (quint64 i = iStartTime; i <= m_globalStats.m_memoryUsage.m_iLastTime; i++)
		{
			if(m_globalStats.m_memoryUsage.contains(i)) {
				quint64 iSize = m_globalStats.m_memoryUsage.value(i);

				if(i == m_iLastUpdateTime){
					if(m_iLastUpdateValue != iSize){
						memoryUsageUpdate.update(i, iSize);
					}
				}else{
					memoryUsageUpdate.update(i, iSize);
				}
				iLastSize = iSize;
			}
		}
		if(memoryUsageUpdate.count() > 0) {
			m_iLastUpdateTime = memoryUsageUpdate.m_iLastTime;
			m_iLastUpdateValue = iLastSize;
		}
	}

	m_lockGlobalStats.unlock();

	if(memoryUsageUpdate.count() != 0)
	{
		QtCharts::QLineSeries *pMemoryUsageSeries = m_pMemoryUsageView->getLineSeries();
		for(quint64 iTimestamp = memoryUsageUpdate.m_iFirstTime; iTimestamp<=memoryUsageUpdate.m_iLastTime; iTimestamp++)
		{
			if(memoryUsageUpdate.contains(iTimestamp)) {
				quint64 iSize = memoryUsageUpdate.value(iTimestamp);
				pMemoryUsageSeries->append(iTimestamp, iSize);
				m_pMemoryUsageView->updateChartRange(tvStart.tv_sec, iTimestamp, 0, iPeakMemoryUsage);
			}
		}
	}
}

void QApplicationWindowController::onMemoryOperationDoubleClicked(const QModelIndex &index)
{
	MemoryOperationSharedPtr pMemoryOperation = m_listFilterMemoryOperation.value(index.row());
	if(pMemoryOperation) {
		BacktraceSharedPtr pBackTrace = pMemoryOperation->m_pBackTrace;

		QStandardItemModel modelBacktrace;

		QStringList listHeaders;
		listHeaders.append(tr("Address"));
		listHeaders.append(tr("Symbol addr"));
		listHeaders.append(tr("Symbol name"));
		listHeaders.append(tr("Object addr"));
		listHeaders.append(tr("Object name"));
		modelBacktrace.setHorizontalHeaderLabels(listHeaders);

		if(pBackTrace){
			QStandardItem* pItem;
			QList<quint64>::const_iterator iter;
			m_lockSymbolInfos.lockForRead();
			for(iter = pBackTrace->m_listAddr.constBegin(); iter != pBackTrace->m_listAddr.constEnd(); ++iter)
			{
				quint64 iAddr = *iter;

				QList<QStandardItem*> listCols;

				// Addr
				pItem = new QStandardItem("0x" + QString::number(iAddr, 16));
				pItem->setEditable(false);
				listCols.append(pItem);

				SymbolInfosSharedPtr pSymbolInfos = m_listSymbolInfos.getByAddr(iAddr);
				if(pSymbolInfos){
					// Symbol addr
					pItem = new QStandardItem("0x" + QString::number(pSymbolInfos->m_iSymbolAddr, 16));
					pItem->setEditable(false);
					listCols.append(pItem);

					// Symbol name
					quint64 iDiff;
					if(pSymbolInfos->m_iSymbolAddr){
						iDiff = pSymbolInfos->m_iAddr - pSymbolInfos->m_iSymbolAddr;
					}else{
						iDiff = pSymbolInfos->m_iAddr - pSymbolInfos->m_iObjectAddr;
					}
					pItem = new QStandardItem(QString("%0+0x%1").arg(pSymbolInfos->m_szSymbolName).arg(QString::number(iDiff, 16)));
					pItem->setEditable(false);
					listCols.append(pItem);

					// Object addr
					pItem = new QStandardItem("0x" + QString::number(pSymbolInfos->m_iObjectAddr, 16));
					pItem->setEditable(false);
					listCols.append(pItem);

					// Object name
					pItem = new QStandardItem(pSymbolInfos->m_szObjectName);
					pItem->setEditable(false);
					listCols.append(pItem);
				}

				modelBacktrace.appendRow(listCols);
			}
			m_lockSymbolInfos.unlock();
		}

		QMemoryOperationView dialog(m_pMemoryOperationListView);
		dialog.setBacktraceModel(&modelBacktrace);
		QTreeView *pTreeView = dialog.getBacktraceTreeView();
		pTreeView->header()->resizeSection(0, 150);
		pTreeView->header()->resizeSection(1, 120);
		pTreeView->header()->resizeSection(2, 200);
		pTreeView->header()->resizeSection(3, 120);
		pTreeView->header()->resizeSection(4, 400);
		pTreeView->header()->resizeSections(QHeaderView::Interactive);

		dialog.exec();

	}
}

void QApplicationWindowController::onThreadInfosDoubleClicked(const QModelIndex &index)
{
	ThreadInfosSharedPtr pThreadInfos = m_listThreadInfos.value(index.row());
	if(pThreadInfos) {
		QString szTmp;
		if(timerisset(&pThreadInfos->m_tvCreation)) {
			szTmp = QString("%0,%1").arg(pThreadInfos->m_tvCreation.tv_sec).arg(pThreadInfos->m_tvCreation.tv_usec, 6, 10, QChar('0'));
		}else{
			szTmp = QString();
		}
		m_pMemoryOperationListView->getTimeStampMinLineEdit()->setText(szTmp);

		if(timerisset(&pThreadInfos->m_tvTermination)) {
			szTmp = QString("%0,%1").arg(pThreadInfos->m_tvTermination.tv_sec).arg(
					pThreadInfos->m_tvTermination.tv_usec, 6, 10, QChar('0'));
		}else{
			szTmp = QString();
		}
		m_pMemoryOperationListView->getTimeStampMaxLineEdit()->setText(szTmp);

		m_pMemoryOperationListView->getThreadIdComboBox()->setCurrentIndex(-1);
		szTmp = QString::number(pThreadInfos->m_iThreadId);
		m_pMemoryOperationListView->getThreadIdComboBox()->setEditText(szTmp);
	}

	m_pApplicationWindow->getTabWidget()->setCurrentIndex(QApplicationWindow::TabMemoryOperation);
}

void QApplicationWindowController::onNewConnection()
{
	clearData();
	m_lockGlobalStats.lockForWrite();
	m_globalStats.m_iMessageCount++;
	m_lockGlobalStats.unlock();
}

void QApplicationWindowController::onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation)
{
	addMemoryOperation(pMemoryOperation);
}

void QApplicationWindowController::onThreadOperationReceived(const ThreadOperationSharedPtr& pThreadOperation)
{
	updateThreadInfos(pThreadOperation);
}

void QApplicationWindowController::onBacktraceReceived(const BacktraceSharedPtr& pBacktrace)
{
	addBacktrace(pBacktrace);
}

void QApplicationWindowController::onSymbolInfosReceived(const SymbolInfosSharedPtr& pSymbolInfos)
{
	addSymbolInfos(pSymbolInfos);
}

void QApplicationWindowController::textToTimeval(const QString& szText, struct timeval& tv)
{
	if(!szText.isEmpty()) {
		QStringList tokens = szText.split(",");
		if (tokens.count() == 2) {
			tv.tv_sec = tokens[0].toULong();
			tv.tv_usec = tokens[1].toULong();
		} else {
			tv.tv_sec = tokens[0].toULong();
			tv.tv_usec = 0;
		}
	}
}
