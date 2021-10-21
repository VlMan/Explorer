#include "ClicableWidget.h"

ClicableWidget::ClicableWidget(QWidget* parent)
	: QWidget(parent)
{

}

ClicableWidget::~ClicableWidget()
{
}

void ClicableWidget::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}