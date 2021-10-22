#include "Picture.h"

void Picture::SetPixmap(const QPixmap& pix)
{
	this->setPixmap(pix);
}

void Picture::Show()
{
	SetPixmap();
	show();
}