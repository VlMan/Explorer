#pragma once

#include "Item.h"

class Executable final : public Item
{
	Q_OBJECT

public:
	explicit Executable(QWidget* w, const QRect rect, QString path = "", QString absolute_path = "")
	: Item(w, rect, std::move(path), std::move(absolute_path)) { }
	~Executable() override { }

	void SetPixmap(const QPixmap& pix = QPixmap()) override;
	void Show() override;
};
