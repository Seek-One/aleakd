//
// Created by ebeuque on 11/03/2021.
//

#ifndef ALEAKD_QAPPLICATIONWINDOW_H
#define ALEAKD_QAPPLICATIONWINDOW_H

#include <QMainWindow>

class QTreeView;

class QApplicationWindow : public QMainWindow
{
public:
	QApplicationWindow(QWidget* parent = NULL);
	virtual ~QApplicationWindow();

public:
	QTreeView* getTreeView() const;

private:
	QTreeView* m_pTreeView;

};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
