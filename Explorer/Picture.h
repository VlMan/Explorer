#pragma once

#include "Item.h"

enum class picture_format
{
	unknown,
	png,
	jpg,
	jpeg,
	gif
};

class Picture final: public Item
{
	Q_OBJECT

	QString format_;

public:
	explicit Picture(QWidget* w, const QRect rect,  QString format, QString path = "", QString absolute_path = "")
	: Item(w, rect, std::move(path), std::move(absolute_path)), format_(std::move(format)) { }
	~Picture() override { }

	void SetPixmap(const QPixmap& pix = QPixmap()) override;
	void Show() override;
};
