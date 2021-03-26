//
// Created by ebeuque on 26/03/2021.
//

#ifndef ALEAKD_QMEMORYUSAGEVIEW_H
#define ALEAKD_QMEMORYUSAGEVIEW_H

#include <QWidget>
#include <QChartView>
#include <QChart>
#include <QLineSeries>

class QMemoryUsageView : public QWidget
{
public:
	QMemoryUsageView(QWidget* parent = 0);
	virtual ~QMemoryUsageView();

	QtCharts::QLineSeries* getLineSeries() const;
	void updateChartRange(qreal xMin, qreal xMax, qreal yMin, qreal yMax);

private:
	QtCharts::QLineSeries* m_pLineSeries;
	QtCharts::QChart* m_pCharts;
	QtCharts::QChartView* m_pChartView;
};


#endif //ALEAKD_QMEMORYUSAGEVIEW_H
