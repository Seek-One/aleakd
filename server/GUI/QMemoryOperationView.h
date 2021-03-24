//
// Created by ebeuque on 24/03/2021.
//

#ifndef ALEAKD_QMEMORYOPERATIONVIEW_H
#define ALEAKD_QMEMORYOPERATIONVIEW_H

#include <QDialog>

class QPushButton;
class QTreeView;
class QStandardItemModel;

class QMemoryOperationView : public QDialog {
	Q_OBJECT
public:
	QMemoryOperationView(QWidget* parent = 0);
	virtual ~QMemoryOperationView();

public:
	void setBacktraceModel(QStandardItemModel* pModel);

	QTreeView* getBacktraceTreeView() const;

private slots:
	void doCloseDialog();

private:
	QTreeView* m_pBacktraceTreeview;
	QStandardItemModel* m_pBacktraceModel;

	QPushButton *m_pCloseButton;
};


#endif //ALEAKD_QMEMORYOPERATIONVIEW_H
