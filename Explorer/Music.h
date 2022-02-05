#pragma once
#include "Item.h"
class Music :
    public Item
{
	Q_OBJECT

public:
	explicit Music(QWidget* w, const QRect rect, QString path = "", QString absolute_path = "")
		: Item(w, rect, std::move(path), std::move(absolute_path)) { }
	~Music() override { }

	void SetPixmap(const QPixmap& pix = QPixmap()) override;
	void Show() override;
};

