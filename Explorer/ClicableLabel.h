﻿#pragma once

#include <QLabel>

class ClickableLabel : public QLabel
{
	Q_OBJECT

public:
	ClickableLabel(QWidget* parent = 0);
	~ClickableLabel();

signals:
	void clicked();

protected:
	void mousePressEvent(QMouseEvent* event) override;

};
