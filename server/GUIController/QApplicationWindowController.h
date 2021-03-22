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
class QMemoryOperationView;
class QMemoryOperationModel;
class QThreadInfosView;
class QThreadInfosModel;

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

private slots:
	void onFilterButtonClicked();
	void onTimerUpdate();

private:
	void clearData();
	void addMemoryOperation(const MemoryOperationSharedPtr& pMemoryOperation);
	void updateThreadInfos(const ThreadOperationSharedPtr& pThreadOperation);
	void updateThreadInfosAddAlloc(uint64_t iThreadId, uint64_t iSize, struct timeval& tvOperation);
	void updateThreadInfosAddFree(uint64_t iThreadId, uint64_t iSize, struct timeval& tvOperation);
	ThreadInfosSharedPtr getThreadInfos(uint64_t iThreadId, bool bThreadCreation, struct timeval& tvOperation);

private:
	QApplicationWindow* m_pApplicationWindow;
	QThreadInfosView* m_pThreadInfosView;
	QMemoryOperationView* m_pMemoryOperationView;

	QTimer m_timerUpdate;

	// List of memory operation
	QReadWriteLock m_lockListMemoryOperation;
	MemoryOperationList m_listMemoryOperation;
	MemoryOperationList m_listMemoryOperationNonFreed;
	ThreadInfosList m_listThreadInfos;

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
