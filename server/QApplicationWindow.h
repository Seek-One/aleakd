//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOW_H
#define ALEAKD_QAPPLICATIONWINDOW_H

#include <QMainWindow>

class QTreeView;
class QScrollBar;
class QLabel;
class QPushButton;

class QApplicationWindow : public QMainWindow
{
public:
	QApplicationWindow(QWidget* parent = NULL);
	virtual ~QApplicationWindow();

public:
	QPushButton* getSearchButton() const;

	QTreeView* getTreeView() const;
	QLabel* getMemoryOperationCount() const;

private:
	QWidget* createSearchForm(QWidget* parent);

private:
	QPushButton* m_pSearchButton;

	QTreeView* m_pTreeView;

	QLabel* m_pMemoryOperationCountLabel;
};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
