//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
#define ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>
#include <QReadWriteLock>

#include "Model/MemoryOperation.h"
#include "Model/MemoryStats.h"
#include "Model/ThreadInfos.h"

#include "Server/IMemOpRcptServerHandler.h"

class QApplicationWindow;
class QMemoryUsageView;
class QMemoryOperationListView;
class QMemoryOperationModel;
class QThreadInfosView;
class QThreadInfosModel;
class QMemoryStatsView;

class QApplicationWindowController : public QObject, public IMemOpRcptServerHandler {
	Q_OBJECT
public:
	QApplicationWindowController();
	virtual ~QApplicationWindowController();

	bool init(QApplicationWindow* pApplicationWindow);

public:
	// IMemOpRcptServerHandler
	void onNewConnection();
	void onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation);
	void onThreadOperationReceived(const ThreadOperationSharedPtr& pThreadOperation);
	void onBacktraceReceived(const BacktraceSharedPtr& pBacktrace);
	void onSymbolInfosReceived(const SymbolInfosSharedPtr& pSymbolInfos);

private slots:
	void onFilterButtonClicked();
	void onTimerUpdate();
	void onMemoryOperationDoubleClicked(const QModelIndex &index);
	void onThreadInfosDoubleClicked(const QModelIndex &index);

private:
	void clearData();
	void addMemoryOperation(const MemoryOperationSharedPtr& pMemoryOperation);
	void updateThreadInfos(const ThreadOperationSharedPtr& pThreadOperation);
	void updateThreadInfosAddAlloc(uint64_t iThreadId, uint32_t iMsgNum, uint64_t iSize, struct timeval& tvOperation);
	void updateThreadInfosAddFree(uint64_t iThreadId, uint32_t iAllocMsgNum, uint64_t iSize, struct timeval& tvOperation);
	ThreadInfosSharedPtr getThreadInfos(uint64_t iThreadId, bool bThreadCreation, struct timeval& tvOperation);
	ThreadInfosSharedPtr getThreadInfosForMsgNum(uint64_t iThreadId, uint32_t iMsgNum);
	void addBacktrace(const BacktraceSharedPtr& pBacktrace);
	void addSymbolInfos(const SymbolInfosSharedPtr& pSymbolInfos);

	void textToTimeval(const QString& szText, struct timeval& tv);

private:
	QApplicationWindow* m_pApplicationWindow;
	QMemoryUsageView* m_pMemoryUsageView;
	QThreadInfosView* m_pThreadInfosView;
	QMemoryOperationListView* m_pMemoryOperationListView;
	QMemoryStatsView* m_pMemoryStatsView;

	QTimer m_timerUpdate;
	quint64 m_iLastUpdateTime;
	quint64 m_iLastUpdateValue;

	// List data
	QReadWriteLock m_lockListMemoryOperation;
	MemoryOperationList m_listMemoryOperation;
	MemoryOperationList m_listMemoryOperationNonFreed; // Used to speed up the search
	MemoryOperationList m_listMemoryOperationWithoutBacktrace; // Used to speed up the search
	ThreadInfosList m_listThreadInfos;
	ThreadInfosList m_listThreadInfosAlive;

	QReadWriteLock m_lockSymbolInfos;
	SymbolInfosList m_listSymbolInfos;

	// Global infos
	QReadWriteLock m_lockGlobalStats;
	MemoryStats m_globalStats;

	// Thread tab
	QThreadInfosModel* m_pModelThreadInfos;
	ThreadInfosList m_listFilterThreadInfos;

	// Search display
	MemoryStats m_searchStats;
	MemoryOperationList m_listFilterMemoryOperation;
	QMemoryOperationModel* m_pModelMemoryOperation;
};


#endif //ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
