//
// Created by ebeuque on 11/03/2021.
//

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QScrollBar>
#include <QLabel>
#include <QStatusBar>

#include "GUI/QThreadInfosView.h"
#include "GUI/QMemoryOperationListView.h"

#include "QApplicationWindow.h"

QApplicationWindow::QApplicationWindow(QWidget* parent)
	: QMainWindow(parent)
{
	QWidget* pMainWidget = new QWidget(this);
	pMainWidget->setContentsMargins(0, 0, 0, 0);
	setCentralWidget(pMainWidget);

	QBoxLayout* pMainLayout = new QVBoxLayout();
	pMainLayout->setContentsMargins(0, 0, 0, 0);
	pMainWidget->setLayout(pMainLayout);

	QWidget* pTmpWidget;

	pTmpWidget = createTabs(pMainWidget);
	pMainLayout->addWidget(pTmpWidget);

	QStatusBar* pStatusBar = createStatusBar(pMainWidget);
	setStatusBar(pStatusBar);
}

QApplicationWindow::~QApplicationWindow()
{

}

QWidget* QApplicationWindow::createTabs(QWidget* pParent)
{
	m_pTabWidget = new QTabWidget(pParent);

	m_pThreadInfosView = new QThreadInfosView(m_pTabWidget);
	m_pTabWidget->addTab(m_pThreadInfosView, tr("Threads"));

	m_pMemoryOperationListView = new QMemoryOperationListView(m_pTabWidget);
	m_pTabWidget->addTab(m_pMemoryOperationListView, tr("Memory operation"));

	return m_pTabWidget;
}

QStatusBar* QApplicationWindow::createStatusBar(QWidget* pParent)
{
	QStatusBar* pMainWidget = new QStatusBar(pParent);

	m_pCaptureMessageCountLabel = new QLabel();
	pMainWidget->addPermanentWidget(m_pCaptureMessageCountLabel);
	setCaptureMessageCount("0");

	m_pCaptureMemoryOperationCountLabel = new QLabel();
	pMainWidget->addPermanentWidget(m_pCaptureMemoryOperationCountLabel);
	setCaptureMemoryOperationCount("0");

	m_pCaptureMemorySizeUsedLabel = new QLabel();
	pMainWidget->addPermanentWidget(m_pCaptureMemorySizeUsedLabel);
	setCaptureMemorySizeUsed("0");

	m_pCaptureThreadCountLabel = new QLabel();
	pMainWidget->addPermanentWidget(m_pCaptureThreadCountLabel);
	setCaptureThreadCount("0");

	m_pCaptureSymbolInfosCountLabel = new QLabel();
	pMainWidget->addPermanentWidget(m_pCaptureSymbolInfosCountLabel);
	setCaptureSymbolInfosCount("0");

	return pMainWidget;
}

QThreadInfosView* QApplicationWindow::getThreadInfosView() const
{
	return m_pThreadInfosView;
}

QMemoryOperationListView* QApplicationWindow::getMemoryOperationListView() const
{
	return m_pMemoryOperationListView;
}

void QApplicationWindow::setCaptureMessageCount(const QString& szValue)
{
	QString szTmp = QString("Message count: %0").arg(szValue);
	m_pCaptureMessageCountLabel->setText(szTmp);
}

void QApplicationWindow::setCaptureMemoryOperationCount(const QString& szValue)
{
	QString szTmp = QString("Operation count: %0").arg(szValue);
	m_pCaptureMemoryOperationCountLabel->setText(szTmp);
}

void QApplicationWindow::setCaptureMemorySizeUsed(const QString& szValue)
{
	QString szTmp = QString("Memory usage: %0 bytes").arg(szValue);
	m_pCaptureMemorySizeUsedLabel->setText(szTmp);
}

void QApplicationWindow::setCaptureThreadCount(const QString& szValue)
{
	QString szTmp = QString("Threads: %0").arg(szValue);
	m_pCaptureThreadCountLabel->setText(szTmp);
}

void QApplicationWindow::setCaptureSymbolInfosCount(const QString& szValue)
{
	QString szTmp = QString("Symbols: %0").arg(szValue);
	m_pCaptureSymbolInfosCountLabel->setText(szTmp);
}