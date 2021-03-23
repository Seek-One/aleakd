//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOW_H
#define ALEAKD_QAPPLICATIONWINDOW_H

#include <QMainWindow>

class QLabel;

class QThreadInfosView;
class QMemoryOperationView;

class QApplicationWindow : public QMainWindow
{
public:
	QApplicationWindow(QWidget* parent = NULL);
	virtual ~QApplicationWindow();

public:
	QThreadInfosView* getThreadInfosView() const;
	QMemoryOperationView* getMemoryOperationView() const;

	// Status bar
	void setCaptureMessageCount(const QString& szValue);
	void setCaptureMemoryOperationCount(const QString& szValue);
	void setCaptureMemorySizeUsed(const QString& szValue);
	void setCaptureThreadCount(const QString& szValue);

private:
	QWidget* createTabs(QWidget* pParent);
	QStatusBar* createStatusBar(QWidget* pParent);

private:
	// Tabs
	QTabWidget* m_pTabWidget;
	QThreadInfosView* m_pThreadInfosView;
	QMemoryOperationView* m_pMemoryOperationView;

	// Status bar
	QStatusBar* m_pStatusBar;
	QLabel* m_pCaptureMessageCountLabel;
	QLabel* m_pCaptureMemoryOperationCountLabel;
	QLabel* m_pCaptureMemorySizeUsedLabel;
	QLabel* m_pCaptureThreadCountLabel;
};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
