//
// Created by ebeuque on 11/03/2021.
//

#include <QTreeView>
#include <QVBoxLayout>

#include "QApplicationWindow.h"

QApplicationWindow::QApplicationWindow(QWidget* parent)
	: QMainWindow(parent)
{
	QWidget* pMainWidget = new QWidget(this);
	setCentralWidget(pMainWidget);

	QBoxLayout* pMainLayout = new QVBoxLayout();
	pMainWidget->setLayout(pMainLayout);

	m_pTreeView = new QTreeView(pMainWidget);
	pMainLayout->addWidget(m_pTreeView);
}

QApplicationWindow::~QApplicationWindow()
{

}

QTreeView* QApplicationWindow::getTreeView() const
{
	return m_pTreeView;
}
