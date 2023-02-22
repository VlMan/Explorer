#include "Item.h"
#include "FileManager.h"

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

item_type Item::GetItemTypeByPath(const QString& absolute_path)
{
	item_type type = item_type::unknown;
	if (FileManager::isDirectory(absolute_path))
	{
		type = item_type::folder;
	}
	else if (FileManager::isExecutable(absolute_path))
	{
		type = item_type::exe;
	}
	else if (FileManager::isImage(absolute_path))
	{
		type = item_type::picture;
	}
	else if (FileManager::isMusic(absolute_path))
	{
		type = item_type::music;
	}
	else if ((!GetAllFormatFiles().contains(FileManager::getFormatFile(absolute_path))))
	{
		type = item_type::unknown;
	}
	return type;
}