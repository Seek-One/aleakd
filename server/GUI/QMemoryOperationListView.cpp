//
// Created by ebeuque on 17/03/2021.
//

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QTreeView>
#include <QPushButton>
#include <QCheckBox>
#include <QComboBox>
#include <QLineEdit>

#include "QMemoryOperationListView.h"

QMemoryOperationListView::QMemoryOperationListView(QWidget* pParent)
	: QWidget(pParent)
{
	QBoxLayout* pMainLayout = new QVBoxLayout();
	setLayout(pMainLayout);

	QWidget* pTmpWidget;

	// Add search form
	pTmpWidget = createFilterForm(this);
	pMainLayout->addWidget(pTmpWidget);

	// Add tree view
	m_pTreeView = new QTreeView(this);
	pMainLayout->addWidget(m_pTreeView);
}

QMemoryOperationListView::~QMemoryOperationListView()
{

}

QWidget* QMemoryOperationListView::createFilterForm(QWidget* pParent)
{
	QWidget* pMainWidget = new QWidget(pParent);
	pMainWidget->setContentsMargins(0, 0, 0, 0);

	QBoxLayout* pMainLayout;
	pMainLayout = new QHBoxLayout();
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainWidget->setLayout(pMainLayout);

	pMainLayout->addWidget(new QLabel(tr("Timestamp from:")));
	m_pTimeStampMinLineEdit = new QLineEdit();
	pMainLayout->addWidget(m_pTimeStampMinLineEdit);

	pMainLayout->addWidget(new QLabel(tr("to:")));
	m_pTimeStampMaxLineEdit = new QLineEdit();
	pMainLayout->addWidget(m_pTimeStampMaxLineEdit);

	pMainLayout->addWidget(new QLabel(tr("Thread:")));
	m_pThreadIdComboBox = new QComboBox();
	m_pThreadIdComboBox->setEditable(true);
	m_pThreadIdComboBox->setMinimumWidth(200);
	pMainLayout->addWidget(m_pThreadIdComboBox);

	pMainLayout->addWidget(new QLabel(tr("Not freed only:")));
	m_pFreedOnlyCheckBox = new QCheckBox();
	pMainLayout->addWidget(m_pFreedOnlyCheckBox);

	m_pFilterButton = new QPushButton(tr("Filter"));
	pMainLayout->addWidget(m_pFilterButton);


	pMainLayout->addStretch();

	return pMainWidget;
}

QLineEdit* QMemoryOperationListView::getTimeStampMinLineEdit() const
{
	return m_pTimeStampMinLineEdit;
}

QLineEdit* QMemoryOperationListView::getTimeStampMaxLineEdit() const
{
	return m_pTimeStampMaxLineEdit;
}

QCheckBox* QMemoryOperationListView::getNotFreeOnlyCheckBox() const
{
	return m_pFreedOnlyCheckBox;
}

QComboBox* QMemoryOperationListView::getThreadIdComboBox() const
{
	return m_pThreadIdComboBox;
}

QPushButton* QMemoryOperationListView::getFilterButton() const
{
	return m_pFilterButton;
}

QTreeView* QMemoryOperationListView::getTreeView() const
{
	return m_pTreeView;
}