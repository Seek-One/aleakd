//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
#define ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H

#include <QObject>
#include <QSharedPointer>
#include <QStandardItemModel>

#include "MemoryOperation.h"

class QApplicationWindow;

class QApplicationWindowController : public QObject {
	Q_OBJECT
public:
	QApplicationWindowController();
	virtual ~QApplicationWindowController();

	bool init(QApplicationWindow* pApplicationWindow);

	void addMemoryOperation(const QSharedPointer<MemoryOperation>& pMemoryOperation);
	void clearMemoryOperation();

private:
	QApplicationWindow* m_pApplicationWindow;

	QStandardItemModel* m_pModels;
};


#endif //ALEAKD_QAPPLICATIONWINDOWCONTROLLER_H
