//
// Created by ebeuque on 17/03/2021.
//

#include <QVBoxLayout>
#include <QTreeView>

#include "QThreadInfosView.h"

QThreadInfosView::QThreadInfosView(QWidget* pParent)
{
	QBoxLayout* pMainLayout = new QVBoxLayout();
	setLayout(pMainLayout);

	m_pTreeView = new QTreeView(pParent);
	pMainLayout->addWidget(m_pTreeView);
}

QThreadInfosView::~QThreadInfosView()
{

}

QTreeView* QThreadInfosView::getTreeView() const
{
	return m_pTreeView;
}