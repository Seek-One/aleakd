//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOW_H
#define ALEAKD_QAPPLICATIONWINDOW_H

#include <QMainWindow>

class QTreeView;
class QScrollBar;
class QLabel;

class QApplicationWindow : public QMainWindow
{
public:
	QApplicationWindow(QWidget* parent = NULL);
	virtual ~QApplicationWindow();

public:
	QTreeView* getTreeView() const;
	QScrollBar* getScrollBar() const;
	QLabel* getMemoryOperationCount() const;

private:
	QTreeView* m_pTreeView;
	QScrollBar* m_pScrollBar;

	QLabel* m_pMemoryOperationCountLabel;
};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
