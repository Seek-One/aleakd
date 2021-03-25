//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QMEMORYOPERATIONMODEL_H
#define ALEAKD_QMEMORYOPERATIONMODEL_H

#include <QAbstractItemModel>
#include <QHash>
#include <QSharedPointer>

#include "Model/MemoryOperation.h"

class QMemoryOperationModel : public QAbstractItemModel
{
public:
	QMemoryOperationModel();
	virtual ~QMemoryOperationModel();

public:
	enum Cols {
		MsgNum = 0,
		TimeStampColumn,
		ThreadColumn,
		OperationColumn,
		AllocSizeColumn,
		AllocPtrColumn,
		AllocNumColumn,
		FreePtrColumn,
	};

public:
	void setMemoryOperationList(MemoryOperationList* pMemoryOperationList);

	void fetchTo(int iRow);
	void clear();

public:
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
	int m_iLoadedCount;
	MemoryOperationList* m_pListMemoryOperation;
};


#endif //ALEAKD_QMEMORYOPERATIONMODEL_H
