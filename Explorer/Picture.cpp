#include "Picture.h"

void Picture::SetPixmap(const QPixmap& pix)
{
	if (!pix.isNull())
		this->setPixmap(pix);
	
	QPixmap pixTmp(GetAbsolutePath());
	pixTmp = pixTmp.scaled(SizeLabel().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->setPixmap(pixTmp);
}

void Picture::Show()
{
	SetPixmap();
	show();
}