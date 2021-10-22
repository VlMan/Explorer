#include "Executable.h"

void Executable::SetPixmap(const QPixmap& pix)
{
	this->setPixmap(pix);
}

void Executable::Show()
{
	SetPixmap();
	show();
}