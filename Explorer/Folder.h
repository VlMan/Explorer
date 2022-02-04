#pragma once

#include "Item.h"

class Folder final : public Item
{
	Q_OBJECT
		
public:
	explicit Folder(QWidget* w, const QRect rect, QString path = "", QString absolute_path = "")
		: Item(w, rect, std::move(path), std::move(absolute_path)) { }
	~Folder() override { }

	void SetPixmap(const QPixmap& pix = QPixmap()) override;
	void Show() override;
};
