//
// Created by ebeuque on 24/03/2021.
//

#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGroupBox>
#include <QTreeView>
#include <QStandardItemModel>

#include "QMemoryOperationView.h"

QMemoryOperationView::QMemoryOperationView(QWidget* parent) : QDialog(parent) {
	setWindowTitle(tr("Memory operation details"));

	QBoxLayout *pMainLayout = new QVBoxLayout(this);
	setLayout(pMainLayout);

	QBoxLayout *pTmpBoxLayout;

	{
		QGroupBox* pGroupBox = new QGroupBox(tr("Backtrace"));
		pMainLayout->addWidget(pGroupBox);

		pTmpBoxLayout = new QHBoxLayout();
		pGroupBox->setLayout(pTmpBoxLayout);

		m_pBacktraceTreeview = new QTreeView(pGroupBox);
		m_pBacktraceTreeview->setMinimumSize(800, 480);
		pTmpBoxLayout->addWidget(m_pBacktraceTreeview);
	}

	{
		QFrame* pLineFrame = new QFrame(parent);
		pLineFrame->setFrameShape(QFrame::HLine);
		pLineFrame->setFrameShadow(QFrame::Sunken);
		pMainLayout->addWidget(pLineFrame);
	}

	// Close button
	{
		pTmpBoxLayout = new QHBoxLayout();
		pMainLayout->addLayout(pTmpBoxLayout);

		pTmpBoxLayout->addStretch();

		m_pCloseButton = new QPushButton(tr("Close"), this);
		pTmpBoxLayout->addWidget(m_pCloseButton);

		connect(m_pCloseButton, SIGNAL(clicked(bool)), this, SLOT(doCloseDialog()));
	}
}

QMemoryOperationView::~QMemoryOperationView()
{

}

void QMemoryOperationView::doCloseDialog()
{
	this->done(0);
}

void QMemoryOperationView::setBacktraceModel(QStandardItemModel* pModel)
{
	m_pBacktraceModel = pModel;
	m_pBacktraceTreeview->setModel(pModel);
}

QTreeView* QMemoryOperationView::getBacktraceTreeView() const
{
	return m_pBacktraceTreeview;
}