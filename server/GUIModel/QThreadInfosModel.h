//
// Created by ebeuque on 17/03/2021.
//

#ifndef ALEAKD_QTHREADINFOSMODEL_H
#define ALEAKD_QTHREADINFOSMODEL_H

#include <QAbstractItemModel>

class ThreadInfosList;

class QThreadInfosModel : public QAbstractItemModel
{
public:
	QThreadInfosModel();
	virtual ~QThreadInfosModel();

public:
	enum Cols {
		ThreadIdColumn = 0,
		ThreadNameColumn,
		CurrentSizeColumn,
		PeakSizeColumn
	};

public:
	void setThreadInfosList(ThreadInfosList* pThreadInfosList);

	void clear();
	void refresh();

public:
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &child) const override;
	int rowCount(const QModelIndex &parent) const override;
	int columnCount(const QModelIndex &parent) const override;
	QVariant data(const QModelIndex &index, int role) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

private:
	ThreadInfosList* m_pThreadInfoList;
};


#endif //ALEAKD_QTHREADINFOSMODEL_H
