#include "Folder.h"

void Folder::SetPixmap(const QPixmap& pix)
{
	QPixmap pixTmp = pix;
	if (pixTmp.isNull())
		pixTmp = QPixmap("images/folder.png", "PNG");

	pixTmp = pixTmp.scaled(SizeLabel().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->setPixmap(pixTmp);
}

void Folder::Show()
{
	SetPixmap();
	show();
}