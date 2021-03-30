//
// Created by ebeuque on 30/03/2021.
//

#ifndef ALEAKD_QMEMORYSTATSVIEW_H
#define ALEAKD_QMEMORYSTATSVIEW_H

#include <QList>
#include <QWidget>

class QLabel;

class QMemoryStatsView  : public QWidget
{
public:
	QMemoryStatsView(QWidget* parent);
	virtual ~QMemoryStatsView();

public:
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
	static QString getColName(int iCol);
	void setData(int iRow, int iCol, const QString& szValue);

private:
	// Statistics
	QList<QLabel*> m_listStatusRow1;
	QList<QLabel*> m_listStatusRow2;
};


#endif //ALEAKD_QMEMORYSTATSVIEW_H
