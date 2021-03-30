//
// Created by ebeuque on 30/03/2021.
//

#include <QGridLayout>
#include <QLabel>

#include "Global/Utils.h"

#include "QMemoryStatsView.h"

QMemoryStatsView::QMemoryStatsView(QWidget* parent)
{
	QBoxLayout* pMainLayout = new QVBoxLayout();
	setLayout(pMainLayout);

	QGridLayout* pGridLayout;
	pGridLayout = new QGridLayout();
	pMainLayout->addLayout(pGridLayout);

	// Global row
	QLabel* pTmpLabel;
	for(int i=0; i<DataCount; i++)
	{
		// Header row
		pTmpLabel = new QLabel();
		if(i == Title) {
			pTmpLabel->setText(QString());
		}else{
			pTmpLabel->setText(getColName(i) + ":");
			pTmpLabel->setAlignment(Qt::AlignLeft);
		}
		pGridLayout->addWidget(pTmpLabel, i, 0);

		// Search row
		pTmpLabel = new QLabel();
		if(i == Title) {
			pTmpLabel->setText(tr("Filter"));
			QFont font = pTmpLabel->font();
			font.setBold(true);
			pTmpLabel->setFont(font);
		}else{
			pTmpLabel->setText("0");
		}
		pTmpLabel->setAlignment(Qt::AlignHCenter);
		m_listStatusRow1.append(pTmpLabel);
		pGridLayout->addWidget(pTmpLabel, i, 1);

		// Global row
		pTmpLabel = new QLabel();
		if(i == Title) {
			pTmpLabel->setText(tr("Global"));
			QFont font = pTmpLabel->font();
			font.setBold(true);
			pTmpLabel->setFont(font);
		}else{
			pTmpLabel->setText("0");
		}
		pTmpLabel->setAlignment(Qt::AlignHCenter);
		m_listStatusRow2.append(pTmpLabel);
		pGridLayout->addWidget(pTmpLabel, i, 2);
	}

	pMainLayout->addStretch();
}

QMemoryStatsView::~QMemoryStatsView()
{

}

void QMemoryStatsView::setData(int iRow, int iCol, const QString& szValue)
{
	QString szTmp = Utils::getBeautifulNumberString(szValue);

	if(iRow == Filter){
		m_listStatusRow1.value(iCol)->setText(szTmp);
	}
	if(iRow == Global){
		m_listStatusRow2.value(iCol)->setText(szTmp);
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