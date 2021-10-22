#include "Folder.h"

void Folder::SetPixmap(const QPixmap& pix)
{
	this->setPixmap(pix);
}

void Folder::Show()
{
	SetPixmap();
	show();
}