//
// Created by ebeuque on 11/03/2021.
//

#include <QColor>

#include "Global/Utils.h"

#include "QMemoryOperationModel.h"

QMemoryOperationModel::QMemoryOperationModel()
{
	m_iLoadedCount = 0;
	m_pListMemoryOperation = NULL;
}

QMemoryOperationModel::~QMemoryOperationModel()
{

}

void QMemoryOperationModel::setMemoryOperationList(MemoryOperationList* pMemoryOperationList)
{
	m_pListMemoryOperation = pMemoryOperationList;
}

QModelIndex QMemoryOperationModel::index(int row, int column, const QModelIndex &parent) const
{
	QModelIndex retIndex;

	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	retIndex = createIndex(row, column, (void*)NULL);

	return retIndex;
}

QModelIndex QMemoryOperationModel::parent(const QModelIndex &child) const
{
	if (!child.isValid()) {
		return QModelIndex();
	}
	QModelIndex parentIndex;
	parentIndex = QModelIndex();
	return parentIndex;
}

int QMemoryOperationModel::rowCount(const QModelIndex &parent) const
{
	if(parent == QModelIndex()) {
		return m_iLoadedCount;
	}
	return 0;
}

int QMemoryOperationModel::columnCount(const QModelIndex &parent) const
{
	return 7;
}

QVariant QMemoryOperationModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()){
		return QVariant();
	}


	if (role == Qt::DisplayRole) {
		MemoryOperationSharedPtr pMemoryOperation = m_pListMemoryOperation->value(index.row());
		if(!pMemoryOperation){
			return QString();
		}

		switch(index.column()) {
		case MsgNum:
			return QString::number(pMemoryOperation->m_iMsgNum);
		case TimeStampColumn:
			return QString("%0,%1").arg(pMemoryOperation->m_tvOperation.tv_sec).arg(pMemoryOperation->m_tvOperation.tv_usec, 6, 10, QChar('0'));
		case ThreadColumn:
			return QString::number(pMemoryOperation->m_iCallerThreadId);
		case OperationColumn:
			return ALeakD_MsgLabel(pMemoryOperation->m_iMsgCode);
		case AllocSizeColumn:
			if(pMemoryOperation->m_iAllocSize) {
				return Utils::getBeautifulNumberString(QString::number(pMemoryOperation->m_iAllocSize));
			}
			return QString();
		case AllocPtrColumn:
			if(pMemoryOperation->m_iAllocPtr) {
				return "0x" + QString::number(pMemoryOperation->m_iAllocPtr, 16);
			}
			return QString();
		case AllocNumColumn:
			if(pMemoryOperation->m_iAllocNum) {
				return QString::number(pMemoryOperation->m_iAllocNum);
			}
			return QString();
		case FreePtrColumn:
			if(pMemoryOperation->m_iFreePtr) {
				return "0x" + QString::number(pMemoryOperation->m_iFreePtr, 16);
			}
			return QString();
		}
		return QVariant();
	}

	if (role == Qt::BackgroundColorRole) {
		MemoryOperationSharedPtr pMemoryOperation = m_pListMemoryOperation->value(index.row());
		if(pMemoryOperation){
			if(pMemoryOperation->m_iMsgCode == ALeakD_MsgCode_free) {
				return QColor(180, 180, 180);
			}else{
				if (!pMemoryOperation->m_bFreed) {
					return QColor(255, 180, 180);
				} else {
					return QColor(180, 255, 180);
				}
			}
		}
	}

	return QVariant();
}

void QMemoryOperationModel::fetchTo(int iRow)
{
	int iItemsToFetch = iRow - m_iLoadedCount;
	beginInsertRows(QModelIndex(), m_iLoadedCount, m_iLoadedCount+iItemsToFetch-1);
	m_iLoadedCount += iItemsToFetch;
	endInsertRows();
}

void QMemoryOperationModel::clear()
{
	beginResetModel();
	m_iLoadedCount = 0;
	endResetModel();
}

QVariant QMemoryOperationModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
	{
		switch (section) {
		case MsgNum:
			return tr("Msg num");
		case TimeStampColumn:
			return tr("Timestamp");
		case ThreadColumn:
			return tr("Thread");
		case OperationColumn:
			return tr("Operation");
		case AllocSizeColumn:
			return tr("Alloc size");
		case AllocPtrColumn:
			return tr("Alloc ptr");
		case AllocNumColumn:
			return tr("Alloc num");
		case FreePtrColumn:
			return tr("Free ptr");
		default:
			return QVariant();
		}
	}

	return QVariant();
}

