#include "Executable.h"

void Executable::SetPixmap(const QPixmap& pix)
{
	QPixmap pixTmp = pix;
	if (pixTmp.isNull())
		pixTmp = QPixmap("images/exe.png", "PNG");

	pixTmp = pixTmp.scaled(SizeLabel().size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	this->setPixmap(pixTmp);
}

void Executable::Show()
{
	SetPixmap();
	show();
}