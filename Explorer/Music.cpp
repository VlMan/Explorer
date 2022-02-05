#include "Music.h"

void Music::SetPixmap(const QPixmap& pix)
{
	this->setPixmap(pix);
}

void Music::Show()
{
	SetPixmap();
	show();
}
