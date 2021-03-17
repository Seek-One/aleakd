//
// Created by ebeuque on 17/03/2021.
//

#ifndef ALEAKD_QTHREADINFOSVIEW_H
#define ALEAKD_QTHREADINFOSVIEW_H

#include <QWidget>

class QTreeView;

class QThreadInfosView : public QWidget
{
public:
	QThreadInfosView(QWidget* pParent);
	virtual ~QThreadInfosView();

public:
	QTreeView* getTreeView() const;

private:
	QTreeView* m_pTreeView;
};


#endif //ALEAKD_QTHREADINFOSVIEW_H
