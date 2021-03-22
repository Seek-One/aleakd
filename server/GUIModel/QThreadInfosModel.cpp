//
// Created by ebeuque on 17/03/2021.
//

#include <QColor>

#include "Model/ThreadInfos.h"

#include "Global/Utils.h"

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
	return ColCount;
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
		case TimeCreation:
			return QString("%0,%1").arg(pThreadInfos->m_tvCreation.tv_sec).arg(pThreadInfos->m_tvCreation.tv_usec, 6, 10, QChar('0'));
		case TimeTermination:
			if(timerisset(&pThreadInfos->m_tvTermination)) {
				return QString("%0,%1").arg(pThreadInfos->m_tvTermination.tv_sec).arg(
						pThreadInfos->m_tvTermination.tv_usec, 6, 10, QChar('0'));
			}else{
				return QString();
			}
		case ThreadNameColumn:
			return pThreadInfos->m_szThreadName;
		case CurrentAllocCountColumn:
			return Utils::getBeautifulNumberString(QString::number(pThreadInfos->m_iCurrentAllocCount));
		case CurrentSizeColumn:
			return Utils::getBeautifulNumberString(QString::number(pThreadInfos->m_iCurrentSize));
		case TotalAllocCountColumn:
			return Utils::getBeautifulNumberString(QString::number(pThreadInfos->m_iTotalAllocCount));
		case PeakSizeColumn:
			return Utils::getBeautifulNumberString(QString::number(pThreadInfos->m_iPeakSize));
		}
		return QVariant();
	}

	if (role == Qt::BackgroundRole){
		ThreadInfosSharedPtr pThreadInfos = m_pThreadInfoList->value(index.row());
		if(!pThreadInfos){
			return QString();
		}
		if(pThreadInfos->m_bIsTerminated){
			return QColor(180, 180, 180);
		}
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
		case TimeCreation:
			return tr("Time creation");
		case TimeTermination:
			return tr("Time termination");
		case CurrentAllocCountColumn:
			return tr("Alloc count");
		case CurrentSizeColumn:
			return tr("Current size");
		case TotalAllocCountColumn:
			return tr("Total alloc count");
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
	emit layoutChanged();
}