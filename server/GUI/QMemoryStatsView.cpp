//
// Created by ebeuque on 30/03/2021.
//

#include <QGridLayout>
#include <QLabel>

#include "QMemoryStatsView.h"

QMemoryStatsView::QMemoryStatsView(QWidget* parent)
{
	setContentsMargins(0, 0, 0, 0);


	QGridLayout* pGridLayout;
	pGridLayout = new QGridLayout();
	pGridLayout->setContentsMargins(0, 0, 0, 0);
	setLayout(pGridLayout);

	// Global row
	QLabel* pTmpLabel;
	for(int i=0; i<StatusBarCol_ColCount; i++)
	{
		// Header row
		pTmpLabel = new QLabel();
		if(i == StatusBarCol_Title) {
			pTmpLabel->setText(QString());
		}else{
			pTmpLabel->setText(getColName(i));
			pTmpLabel->setAlignment(Qt::AlignCenter);
		}
		pGridLayout->addWidget(pTmpLabel, 0, i);

		// Search row
		pTmpLabel = new QLabel();
		if(i == StatusBarCol_Title) {
			pTmpLabel->setText(tr("Filter:"));
		}else{
			pTmpLabel->setText("0");
			pTmpLabel->setAlignment(Qt::AlignCenter);
		}
		m_listStatusRow1.append(pTmpLabel);
		pGridLayout->addWidget(pTmpLabel, 1, i);

		// Global row
		pTmpLabel = new QLabel();
		if(i == StatusBarCol_Title) {
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
	if(iRow == StatusBarRow_Search){
		m_listStatusRow1.value(iCol)->setText(szValue);
	}
	if(iRow == StatusBarRow_Global){
		m_listStatusRow2.value(iCol)->setText(szValue);
	}
}

QString QMemoryStatsView::getColName(int iCol)
{
	switch(iCol)
	{
		case StatusBarCol_OpCount:
			return tr("Op count");
		case StatusBarCol_TotalAllocCount:
			return tr("Alloc total count");
		case StatusBarCol_TotalAllocSize:
			return tr("Alloc total size");
		case StatusBarCol_TotalFreeCount:
			return tr("Free total count");
		case StatusBarCol_TotalFreeSize:
			return tr("Free total size");
		case StatusBarCol_TotalRemainingCount:
			return tr("Remaining total count");
		case StatusBarCol_TotalRemainingSize:
			return tr("Remaining total size");
		case StatusBarCol_malloc:
			return tr("malloc");
		case StatusBarCol_calloc:
			return tr("calloc");
		case StatusBarCol_realloc:
			return tr("realloc");
		case StatusBarCol_free:
			return tr("free");
		case StatusBarCol_posix_memalign:
			return tr("posix_memalign");
		case StatusBarCol_aligned_alloc:
			return tr("aligned_alloc");
		case StatusBarCol_memalign:
			return tr("memalign");
		case StatusBarCol_valloc:
			return tr("valloc");
		case StatusBarCol_pvalloc:
			return tr("pvalloc");
		default:
			break;
	}

	return QString();
}