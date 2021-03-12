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

	enum StatusBarRows {
		StatusBarRow_Headers,
		StatusBarRow_Search,
		StatusBarRow_Global
	};

	enum StatusBarCols {
		StatusBarCol_Title,
		StatusBarCol_OpCount,
		// Global size
		StatusBarCol_AllocSize,
		StatusBarCol_FreeSize,
		StatusBarCol_RemainingSize,
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
	QPushButton* getSearchButton() const;

	QTreeView* getTreeView() const;
	QLabel* getMemoryOperationCount() const;

	void setData(int iRow, int iCol, const QString& szValue);

private:
	QWidget* createSearchForm(QWidget* pParent);
	QWidget* createStatusBar(QWidget* pParent);

	static QString getColName(int iCol);

private:
	QPushButton* m_pSearchButton;

	QTreeView* m_pTreeView;

	QLabel* m_pMemoryOperationCountLabel;

	QList<QLabel*> m_listStatusRow1;
	QList<QLabel*> m_listStatusRow2;
};


#endif //ALEAKD_QAPPLICATIONWINDOW_H
