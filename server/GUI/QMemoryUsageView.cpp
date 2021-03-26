//
// Created by ebeuque on 26/03/2021.
//

#include <QChart>
#include <QLineSeries>
#include <QHBoxLayout>

#include "QMemoryUsageView.h"

QMemoryUsageView::QMemoryUsageView(QWidget* parent) : QWidget(parent)
{
	setContentsMargins(0, 0, 0, 0);

	QBoxLayout* pMainLayout = new QHBoxLayout();
	pMainLayout->setContentsMargins(0, 0,0,0);
	this->setLayout(pMainLayout);

	QtCharts::QLineSeries* pSeries = new QtCharts::QLineSeries();
	m_pLineSeries = pSeries;

	QtCharts::QChart *pChart = new QtCharts::QChart();
	m_pCharts = pChart;
	pChart->legend()->hide();
	pChart->addSeries(pSeries);
	pChart->createDefaultAxes();
	//pChart->setTitle("Simple line chart example");

	QtCharts::QChartView *pChartView = new QtCharts::QChartView(pChart);
	m_pChartView = pChartView;
	pChartView->setRenderHint(QPainter::Antialiasing);
	pMainLayout->addWidget(pChartView);
}

QMemoryUsageView::~QMemoryUsageView()
{

}

QtCharts::QLineSeries* QMemoryUsageView::getLineSeries() const
{
	return m_pLineSeries;
}

void QMemoryUsageView::updateChartRange(qreal xMin, qreal xMax, qreal yMin, qreal yMax)
{
	m_pCharts->axisX()->setRange(xMin, xMax);
	m_pCharts->axisY()->setRange(yMin, yMax);
}