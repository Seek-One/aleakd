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
	enum TitleItem {
		Headers,
		Filter,
		Global
	};

	enum DataItem {
		Title,
		OpCount,
		// Global
		TotalAllocCount,
		TotalAllocSize,
		TotalFreeCount,
		TotalFreeSize,
		TotalRemainingCount,
		TotalRemainingSize,
		// Alloc operation
		malloc,
		calloc,
		realloc,
		free,
		posix_memalign,
		aligned_alloc,
		memalign,
		valloc,
		pvalloc,

		ColCount,
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
