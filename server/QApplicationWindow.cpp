//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QLabel>

#include "QApplicationWindow.h"

QApplicationWindow::QApplicationWindow(QWidget* parent)
	: QMainWindow(parent)
{
	QWidget* pMainWidget = new QWidget(this);
	setCentralWidget(pMainWidget);

	QBoxLayout* pMainLayout = new QVBoxLayout();
	pMainWidget->setLayout(pMainLayout);

	QBoxLayout* pTmpLayout;

	{
		pTmpLayout = new QHBoxLayout();
		pMainLayout->addLayout(pTmpLayout);

		m_pTreeView = new QTreeView(pMainWidget);
		pTmpLayout->addWidget(m_pTreeView);

		m_pScrollBar = new QScrollBar();
		pTmpLayout->addWidget(m_pScrollBar);
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

QTreeView* QApplicationWindow::getTreeView() const
{
	return m_pTreeView;
}

QScrollBar* QApplicationWindow::getScrollBar() const
{
	return m_pScrollBar;
}

QLabel* QApplicationWindow::getMemoryOperationCount() const
{
	return m_pMemoryOperationCountLabel;
}