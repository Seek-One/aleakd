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
class QCheckBox;

class QApplicationWindow : public QMainWindow
{
public:
	QApplicationWindow(QWidget* parent = NULL);
	virtual ~QApplicationWindow();

	enum StatusBarRows {
		StatusBarRow_Headers,
		StatusBarRow_Search,
		StatusBarRow_Global
	};

	enum StatusBarCols {
		StatusBarCol_Title,
		StatusBarCol_OpCount,
		// Global
		StatusBarCol_TotalAllocCount,
		StatusBarCol_TotalAllocSize,
		StatusBarCol_TotalFreeCount,
		StatusBarCol_TotalFreeSize,
		StatusBarCol_TotalRemainingCount,
		StatusBarCol_TotalRemainingSize,
		// Alloc operation
		StatusBarCol_malloc,
		StatusBarCol_calloc,
		StatusBarCol_realloc,
		StatusBarCol_free,
		StatusBarCol_posix_memalign,
		StatusBarCol_aligned_alloc,
		StatusBarCol_memalign,
		StatusBarCol_valloc,
		StatusBarCol_pvalloc,
		StatusBarCol_ColCount,
	};

public:
	QCheckBox* getNotFreeOnlyCheckBox() const;
	QPushButton* getFilterButton() const;

	QTreeView* getTreeView() const;

	void setData(int iRow, int iCol, const QString& szValue);

private:
	QWidget* createFilterForm(QWidget* pParent);
	QWidget* createStatusBar(QWidget* pParent);

	static QString getColName(int iCol);

private:
	QCheckBox* m_pFreedOnlyCheckBox;
	QPushButton* m_pFilterButton;

	QTreeView* m_pTreeView;

	QList<QLabel*> m_listStatusRow1;
	QList<QLabel*> m_listStatusRow2;
};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
