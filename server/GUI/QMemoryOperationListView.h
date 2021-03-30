//
// Created by ebeuque on 17/03/2021.
//

#ifndef ALEAKD_QMEMORYOPERATIONLISTVIEW_H
#define ALEAKD_QMEMORYOPERATIONLISTVIEW_H

#include <QWidget>

class QTreeView;
class QPushButton;
class QCheckBox;
class QComboBox;
class QLineEdit;

class QMemoryOperationListView : public QWidget
{
public:
	QMemoryOperationListView(QWidget* pParent);
	virtual ~QMemoryOperationListView();

public:
	QLineEdit* getTimeStampMinLineEdit() const;
	QLineEdit* getTimeStampMaxLineEdit() const;
	QComboBox* getThreadIdComboBox() const;
	QCheckBox* getNotFreeOnlyCheckBox() const;
	QPushButton* getFilterButton() const;

	QTreeView* getTreeView() const;

private:
	QWidget* createFilterForm(QWidget* pParent);

private:
	QLineEdit* m_pTimeStampMinLineEdit;
	QLineEdit* m_pTimeStampMaxLineEdit;
	QCheckBox* m_pFreedOnlyCheckBox;
	QComboBox* m_pThreadIdComboBox;
	QPushButton* m_pFilterButton;

	QTreeView* m_pTreeView;
};


#endif //ALEAKD_QMEMORYOPERATIONLISTVIEW_H
