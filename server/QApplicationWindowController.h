//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
#define ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QTimer>

#include "MemoryOperation.h"

class QApplicationWindow;
class QMemoryOperationModel;

class QApplicationWindowController : public QObject {
	Q_OBJECT
public:
	QApplicationWindowController();
	virtual ~QApplicationWindowController();

	bool init(QApplicationWindow* pApplicationWindow);

	void addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation);
	void clearMemoryOperation();

private slots:
	void onScrollBarValueChanged(int value);

private:
	QApplicationWindow* m_pApplicationWindow;

	QList< QSharedPointer<MemoryOperation> > m_listMemoryOperation;
	QMemoryOperationModel* m_pModels;
};


#endif //ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
