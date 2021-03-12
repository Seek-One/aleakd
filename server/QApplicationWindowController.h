//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
#define ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

#include "MemoryOperation.h"
#include "IMemOpRcptServerHandler.h"

class QApplicationWindow;
class QMemoryOperationModel;

class QApplicationWindowController : public QObject, public IMemOpRcptServerHandler {
	Q_OBJECT
public:
	QApplicationWindowController();
	virtual ~QApplicationWindowController();

	bool init(QApplicationWindow* pApplicationWindow);

	void addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation);
	void clearMemoryOperation();

	// IMemOpRcptServerHandler
	void onMemoryOperationReceived(const MemoryOperationSharedPtr& pMemoryOperation);
	void onNewConnection();

private slots:
	void onScrollBarValueChanged(int value);

private:
	QApplicationWindow* m_pApplicationWindow;

	QList< QSharedPointer<MemoryOperation> > m_listMemoryOperation;
	QMemoryOperationModel* m_pModels;
};


#endif //ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
