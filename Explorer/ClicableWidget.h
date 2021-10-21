#pragma once

#include <QWidget>

class ClicableWidget : public QWidget
{
	Q_OBJECT

public:
	ClicableWidget(QWidget* parent = nullptr);
	~ClicableWidget();

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;

};
