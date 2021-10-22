#include "Item.h"

void Item::SetPixmap(const QPixmap& pix)
{
	this->setPixmap(pix);
}

void Item::Show()
{
	SetPixmap(QPixmap());
	show();
}

void Item::setPixmap(const QPixmap& pix)
{
	*pix_ = pix;

	lbl_->setPixmap(pix);

	lbl_path_->setGeometry(0, SizeLabel().height(), SizeLabel().width(), 15);
	lbl_path_->setAlignment(Qt::AlignCenter);
	lbl_path_->setText(path_);
	
	//this->setLayout(layout_.data()); // TODO: create layout
	lbl_->setGeometry(SizeLabel());
}
