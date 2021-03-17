//
// Created by ebeuque on 17/03/2021.
//

#include "Model/ThreadInfos.h"

#include "QThreadInfosModel.h"

QThreadInfosModel::QThreadInfosModel()
{
	m_pThreadInfoList = NULL;
}

QThreadInfosModel::~QThreadInfosModel()
{

}

void QThreadInfosModel::setThreadInfosList(ThreadInfosList* pThreadInfosList)
{
	m_pThreadInfoList = pThreadInfosList;
}

QModelIndex QThreadInfosModel::index(int row, int column, const QModelIndex &parent) const
{
	QModelIndex retIndex;

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	retIndex = createIndex(row, column, (void*)NULL);

	return retIndex;
}

QModelIndex QThreadInfosModel::parent(const QModelIndex &child) const
{
	if (!child.isValid()) {
		return QModelIndex();
	}
	QModelIndex parentIndex;
	parentIndex = QModelIndex();
	return parentIndex;
}

int QThreadInfosModel::rowCount(const QModelIndex &parent) const
{
	if(parent == QModelIndex()) {
		return m_pThreadInfoList->count();
	}
	return 0;
}

int QThreadInfosModel::columnCount(const QModelIndex &parent) const
{
	return 4;
}

QVariant QThreadInfosModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()){
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		ThreadInfosSharedPtr pThreadInfos = m_pThreadInfoList->value(index.row());
		if(!pThreadInfos){
			return QString();
		}

		switch(index.column()) {
		case ThreadIdColumn:
			return QString::number(pThreadInfos->m_iThreadId);
		case ThreadNameColumn:
			return pThreadInfos->m_szThreadName;
		case CurrentSizeColumn:
			return QString::number(pThreadInfos->m_iCurrentSize);
		case PeakSizeColumn:
			return QString::number(pThreadInfos->m_iPeakSize);
		}
		return QVariant();
	}

	return QVariant();
}


QVariant QThreadInfosModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section) {
			case ThreadIdColumn:
				return tr("Thread id");
			case ThreadNameColumn:
				return tr("Thread name");
			case CurrentSizeColumn:
				return tr("Current size");
			case PeakSizeColumn:
				return tr("Peak size");
			default:
				return QVariant();
		}
	}

	return QVariant();
}

void QThreadInfosModel::clear()
{
	beginResetModel();
	endResetModel();
}

void QThreadInfosModel::refresh()
{
	beginResetModel();
	endResetModel();
}