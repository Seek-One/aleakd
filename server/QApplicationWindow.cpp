//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QPushButton>

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
	pTmpWidget = createSearchForm(pMainWidget);
	pMainLayout->addWidget(pTmpWidget);

	{
		pTmpLayout = new QHBoxLayout();
		pMainLayout->addLayout(pTmpLayout);

		m_pTreeView = new QTreeView(pMainWidget);
		pTmpLayout->addWidget(m_pTreeView);
	}

	{
		pTmpLayout = new QHBoxLayout();
		pMainLayout->addLayout(pTmpLayout);

		pTmpLayout->addWidget(new QLabel(tr("Alloc count:")));

		m_pMemoryOperationCountLabel = new QLabel("0");
		pTmpLayout->addWidget(m_pMemoryOperationCountLabel);

		pTmpLayout->addStretch();
	}
}

QApplicationWindow::~QApplicationWindow()
{

}

QWidget* QApplicationWindow::createSearchForm(QWidget* pParent)
{
	QWidget* pMainWidget = new QWidget(pParent);
	pMainWidget->setContentsMargins(0, 0, 0, 0);

	QBoxLayout* pMainLayout;
	pMainLayout = new QHBoxLayout();
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainWidget->setLayout(pMainLayout);

	pMainLayout->addWidget(new QLabel("Recherche:"));

	m_pSearchButton = new QPushButton(tr("Search"));
	pMainLayout->addWidget(m_pSearchButton);

	pMainLayout->addStretch();

	return pMainWidget;
}

QPushButton* QApplicationWindow::getSearchButton() const
{
	return m_pSearchButton;
}

QTreeView* QApplicationWindow::getTreeView() const
{
	return m_pTreeView;
}

QLabel* QApplicationWindow::getMemoryOperationCount() const
{
	return m_pMemoryOperationCountLabel;
}