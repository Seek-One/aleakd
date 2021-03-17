//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOW_H
#define ALEAKD_QAPPLICATIONWINDOW_H

#include <QMainWindow>

class QLabel;

class QMemoryOperationView;

class QApplicationWindow : public QMainWindow
{
public:
	QApplicationWindow(QWidget* parent = NULL);
	virtual ~QApplicationWindow();

public:
	QMemoryOperationView* getMemoryOperationView() const;

	// Status bar
	void setCaptureMemoryOperationCount(const QString& szValue);
	void setCaptureMemorySizeUsed(const QString& szValue);
	void setCaptureThreadCount(const QString& szValue);

private:
	QWidget* createTabs(QWidget* pParent);
	QStatusBar* createStatusBar(QWidget* pParent);

private:
	// Tabs
	QTabWidget* m_pTabWidget;
	QMemoryOperationView* m_pMemoryOperationView;

	// Status bar
	QStatusBar* m_pStatusBar;
	QLabel* m_pCaptureMemoryOperationCountLabel;
	QLabel* m_pCaptureMemorySizeUsedLabel;
	QLabel* m_pCaptureThreadCountLabel;
};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
