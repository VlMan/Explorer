#include "Item.h"

void Item::SetPixmap(const QPixmap& pix)
{
	QPixmap pixTmp = pix;
	if (pixTmp.isNull())
		pixTmp = QPixmap("images/unknown.png", "PNG");

	pixTmp = pixTmp.scaled(SizeLabel().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->setPixmap(pixTmp);
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

	lbl_path_->setGeometry(10, 80, this->size().width(), 15);
	lbl_path_->setText(path_);
	
	//this->setLayout(layout_.data());
	lbl_->setGeometry(SizeLabel());
}
