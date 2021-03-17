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
class QMemoryOperationModel;

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
	void updateThreadInfosAddAlloc(uint64_t iThreadId, uint64_t iSize);
	void updateThreadInfosAddFree(uint64_t iThreadId, uint64_t iSize);
	ThreadInfosSharedPtr getThreadInfos(uint64_t iThreadId);

private:
	QApplicationWindow* m_pApplicationWindow;

	QTimer m_timerUpdate;

	// List of memory operation
	QReadWriteLock m_lockListMemoryOperation;
	MemoryOperationList m_listMemoryOperation;

	// Stats
	QReadWriteLock m_lockGlobalStats;
	MemoryStats m_globalStats;
	ThreadInfosList m_listThreadInfos;

	// Search display
	MemoryStats m_searchStats;
	MemoryOperationList m_listFilterMemoryOperation;
	QMemoryOperationModel* m_pModels;
};


#endif //ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
