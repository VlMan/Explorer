#include "ClicableLabel.h"

ClickableLabel::ClickableLabel(QWidget* parent)
	: QLabel(parent)
{

}

ClickableLabel::~ClickableLabel()
{
}

void ClickableLabel::mousePressEvent(QMouseEvent*)
{
	emit clicked();
}