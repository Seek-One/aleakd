//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QCheckBox>

#include "QApplicationWindow.h"

QApplicationWindow::QApplicationWindow(QWidget* parent)
	: QMainWindow(parent)
{
	QWidget* pMainWidget = new QWidget(this);
	setCentralWidget(pMainWidget);

	QBoxLayout* pMainLayout = new QVBoxLayout();
	pMainWidget->setLayout(pMainLayout);

	QWidget* pTmpWidget;
	QBoxLayout* pTmpLayout;

	// Add search form
	pTmpWidget = createFilterForm(pMainWidget);
	pMainLayout->addWidget(pTmpWidget);

	{
		pTmpLayout = new QHBoxLayout();
		pMainLayout->addLayout(pTmpLayout);

		m_pTreeView = new QTreeView(pMainWidget);
		pTmpLayout->addWidget(m_pTreeView);
	}

	// Add status bar
	pTmpWidget = createStatusBar(pMainWidget);
	pMainLayout->addWidget(pTmpWidget);
}

QApplicationWindow::~QApplicationWindow()
{

}

QWidget* QApplicationWindow::createFilterForm(QWidget* pParent)
{
	QWidget* pMainWidget = new QWidget(pParent);
	pMainWidget->setContentsMargins(0, 0, 0, 0);

	QBoxLayout* pMainLayout;
	pMainLayout = new QHBoxLayout();
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainWidget->setLayout(pMainLayout);

	pMainLayout->addWidget(new QLabel(tr("Not freed only:")));
	m_pFreedOnlyCheckBox = new QCheckBox();
	pMainLayout->addWidget(m_pFreedOnlyCheckBox);

	m_pFilterButton = new QPushButton(tr("Filter"));
	pMainLayout->addWidget(m_pFilterButton);

	pMainLayout->addStretch();

	return pMainWidget;
}

QWidget* QApplicationWindow::createStatusBar(QWidget* pParent)
{
	QWidget* pMainWidget = new QWidget(pParent);
	pMainWidget->setContentsMargins(0, 0, 0, 0);

	QGridLayout* pMainLayout;

	{
		QGridLayout* pGridLayout;
		pGridLayout = new QGridLayout();
		pGridLayout->setContentsMargins(0, 0, 0, 0);
		pMainWidget->setLayout(pGridLayout);

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
				pTmpLabel->setText(tr("Search:"));
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

	return pMainWidget;
}

QString QApplicationWindow::getColName(int iCol)
{
	switch(iCol)
	{
	case StatusBarCol_OpCount:
		return tr("Op count");
	case StatusBarCol_AllocSize:
		return tr("Alloc size");
	case StatusBarCol_FreeSize:
		return tr("Free size");
	case StatusBarCol_RemainingSize:
		return tr("Remaining size");
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

QCheckBox* QApplicationWindow::getNotFreeOnlyCheckBox() const
{
	return m_pFreedOnlyCheckBox;
}

QPushButton* QApplicationWindow::getFilterButton() const
{
	return m_pFilterButton;
}

QTreeView* QApplicationWindow::getTreeView() const
{
	return m_pTreeView;
}

void QApplicationWindow::setData(int iRow, int iCol, const QString& szValue)
{
	if(iRow == StatusBarRow_Search){
		m_listStatusRow1.value(iCol)->setText(szValue);
	}
	if(iRow == StatusBarRow_Global){
		m_listStatusRow2.value(iCol)->setText(szValue);
	}
}