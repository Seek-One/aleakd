//
// Created by ebeuque on 30/03/2021.
//

#include <QGridLayout>
#include <QLabel>

#include "QMemoryStatsView.h"

QMemoryStatsView::QMemoryStatsView(QWidget* parent)
{
	QGridLayout* pGridLayout;
	pGridLayout = new QGridLayout();
	setLayout(pGridLayout);

	// Global row
	QLabel* pTmpLabel;
	for(int i=0; i<ColCount; i++)
	{
		// Header row
		pTmpLabel = new QLabel();
		if(i == Title) {
			pTmpLabel->setText(QString());
		}else{
			pTmpLabel->setText(getColName(i));
			pTmpLabel->setAlignment(Qt::AlignCenter);
		}
		pGridLayout->addWidget(pTmpLabel, 0, i);

		// Search row
		pTmpLabel = new QLabel();
		if(i == Title) {
			pTmpLabel->setText(tr("Filter:"));
		}else{
			pTmpLabel->setText("0");
			pTmpLabel->setAlignment(Qt::AlignCenter);
		}
		m_listStatusRow1.append(pTmpLabel);
		pGridLayout->addWidget(pTmpLabel, 1, i);

		// Global row
		pTmpLabel = new QLabel();
		if(i == Title) {
			pTmpLabel->setText(tr("Global:"));
		}else{
			pTmpLabel->setText("0");
			pTmpLabel->setAlignment(Qt::AlignCenter);
		}
		m_listStatusRow2.append(pTmpLabel);
		pGridLayout->addWidget(pTmpLabel, 2, i);
	}
}

QMemoryStatsView::~QMemoryStatsView()
{

}

void QMemoryStatsView::setData(int iRow, int iCol, const QString& szValue)
{
	if(iRow == Filter){
		m_listStatusRow1.value(iCol)->setText(szValue);
	}
	if(iRow == Global){
		m_listStatusRow2.value(iCol)->setText(szValue);
	}
}

QString QMemoryStatsView::getColName(int iCol)
{
	switch(iCol)
	{
		case OpCount:
			return tr("Op count");
		case TotalAllocCount:
			return tr("Alloc total count");
		case TotalAllocSize:
			return tr("Alloc total size");
		case TotalFreeCount:
			return tr("Free total count");
		case TotalFreeSize:
			return tr("Free total size");
		case TotalRemainingCount:
			return tr("Remaining total count");
		case TotalRemainingSize:
			return tr("Remaining total size");
		case malloc:
			return tr("malloc");
		case calloc:
			return tr("calloc");
		case realloc:
			return tr("realloc");
		case free:
			return tr("free");
		case posix_memalign:
			return tr("posix_memalign");
		case aligned_alloc:
			return tr("aligned_alloc");
		case memalign:
			return tr("memalign");
		case valloc:
			return tr("valloc");
		case pvalloc:
			return tr("pvalloc");
		default:
			break;
	}

	return QString();
}