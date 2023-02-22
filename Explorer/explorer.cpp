#include "explorer.h"

#include "ItemCreator.h"
#include "AlgorithmWrapper.h"
#include "Logger.h"

constexpr int sys_tick_timer = 100;
constexpr auto default_style_sheet = "border: 0px;";
auto default_geometry = QRect();

Explorer::Explorer(QWidget *parent)
	: QMainWindow(parent),
	current_directory_("C:/"),
	previous_directory_("."),
	t_sys_tick_(new QTimer(this)),
	current_size_item_(100),
	current_space_item_(6),
	current_item_count_(0),
	current_count_row_item_(0),
	previous_scroll_bar_value_(0),
	ui_(new Ui::ExplorerClass)
{
	ui_->setupUi(this);
	
	this->installEventFilter(this);
	ui_->le_path->setText(current_directory_);

	default_geometry = QRect(0, 0, current_size_item_, current_size_item_ - 22);

	current_general_frame_size_ = &ui_->general_frame->geometry(); // give ref to general frame geometry

	current_horizontal_allocated_space_ = 0;
	current_horizontal_unallocated_space_ = current_general_frame_size_->width();

	current_vertical_allocated_space_ = 0;
	current_vertical_unallocated_space_ = current_general_frame_size_->height();

	t_sys_tick_->start(sys_tick_timer);

	connect(ui_->v_bar_general, &QScrollBar::valueChanged, this, &Explorer::ScrollBarHandler);
	connect(t_sys_tick_.data(), &QTimer::timeout, this, &Explorer::SysTick);
	connect(ui_->le_path, &QLineEdit::textChanged, this, [&](const QString& path) -> void
	{
		if (path.isEmpty()) return;
		
		current_directory_ = path;
	});
	connect(ui_->btn_back, &QPushButton::clicked, this, [=]() { GoBack(); });
	this->show();

	OpenFolder();
}

QRect Explorer::FindOptimizeNextPosition() // this is not clear function
{
	if (current_item_count_ <= 0) // default position
	{
		in_row_count_item_.insert(1, 1);
		current_count_row_item_++;
		return QRect(
			current_space_item_, 
			current_space_item_, 
			current_size_item_, 
			current_size_item_);
	}

	if (current_horizontal_unallocated_space_ > current_size_item_ + current_space_item_ * 2) // if there is free space for item
	{
		const int &lastItemCount = in_row_count_item_.value(current_count_row_item_);
		in_row_count_item_.remove(current_count_row_item_);
		in_row_count_item_.insert(current_count_row_item_, lastItemCount + 1);

		return QRect(
			current_horizontal_allocated_space_ + current_space_item_,
			current_vertical_allocated_space_ - current_size_item_,
			current_size_item_, 
			current_size_item_);
	}

	current_count_row_item_++;
	in_row_count_item_.insert(current_count_row_item_, 1);
	return QRect(
		current_space_item_,
		current_vertical_allocated_space_ + current_space_item_,
		current_size_item_,
		current_size_item_);
}

void Explorer::RepaintItems()
{
	const auto startRepaintItems = clock();

	current_horizontal_allocated_space_ = 0;
	current_horizontal_unallocated_space_ = GetCurrentHorizontalUnallocatedSpace();

	current_vertical_allocated_space_ = 0;
	current_vertical_unallocated_space_ = GetCurrentVerticalUnallocatedSpace();

	current_count_row_item_ = 0;
	current_item_count_ = 0;
	in_row_count_item_.clear();

	for (const auto item : list_items_)
	{
		item->SetRect(FindOptimizeNextPosition());
		if (item->isHidden())
			item->show();
		current_item_count_++;
		current_horizontal_allocated_space_ = GetCurrentHorizontalAllocatedSpace();
		current_horizontal_unallocated_space_ = GetCurrentHorizontalUnallocatedSpace();

		current_vertical_allocated_space_ = GetCurrentVerticalAllocatedSpace();
		current_vertical_unallocated_space_ = GetCurrentVerticalUnallocatedSpace();
	}

	for (auto it = in_row_count_item_.begin(); it != in_row_count_item_.end(); ++it) {
		int first = it.key();
		int second = it.value();
		qDebug() << first << second;
	}
	qDebug() << "Duration repaint items =" << (static_cast<double>(clock()) - static_cast<double>(startRepaintItems)) / static_cast<double>(CLOCKS_PER_SEC) << " Count item =" << list_items_.size();
}

int Explorer::GetCurrentVerticalAllocatedSpace() const
{
	return current_count_row_item_ * current_size_item_ + current_count_row_item_ * current_space_item_;
}

int Explorer::GetCurrentHorizontalAllocatedSpace() const
{
	return in_row_count_item_.value(current_count_row_item_) * current_size_item_ + in_row_count_item_.value(current_count_row_item_) * current_space_item_;
}

int Explorer::GetCurrentVerticalUnallocatedSpace() const
{
	return current_general_frame_size_->height() - current_vertical_allocated_space_;
}

int Explorer::GetCurrentHorizontalUnallocatedSpace() const
{
	return current_general_frame_size_->width() - current_horizontal_allocated_space_;
}

void Explorer::RepaintItemsInStep(const int step)
{
	for (const auto item : list_items_)
	{
		item->SetRect(QRect(item->GetSize().left(),
							item->GetSize().top() - (current_size_item_ + current_space_item_) * step,
							current_size_item_, 
							current_size_item_));
	}
}

bool Explorer::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		if (watched == this)
		{
			Update();
		}
		return true;
	}

	if (event->type() == QEvent::Wheel)
	{
		ui_->v_bar_general->setValue(ui_->v_bar_general->value() + (dynamic_cast<QWheelEvent*>(event)->delta() < 0 ? 1 : -1));
	}

	if (event->type() == QEvent::MouseButtonPress)
	{
		if (dynamic_cast<QMouseEvent*>(event)->button() == Qt::BackButton)
		{
			GoBack();
		}
		else if (dynamic_cast<QMouseEvent*>(event)->button() == Qt::LeftButton)
		{
			if (list_items_.contains(qobject_cast<Item*>(watched)))
			{
				ui_->le_path->setText(qobject_cast<Item*>(watched)->GetAbsolutePath());
			}
		}
		return true;
	}

	if (event->type() == QEvent::KeyPress)
	{
		const auto key = dynamic_cast<QKeyEvent*>(event)->key();
		if (!key) return true;
		if (key == Qt::Key_Enter || key == Qt::Key_Return)
		{
			if (!GetAllFormatFiles().contains(FileManager::getFormatFile(current_directory_)))
			{
				OpenFolder();
			}
			else
			{
				OpenFile();
			}
		}
		else if (key == Qt::Key_Escape)
		{
			GoBack();
		}
		else if (key == Qt::Key_F5)
		{
			Update();
		}
		return true;
	}

	if (event->type() == QEvent::MouseButtonDblClick)
	{
		if (dynamic_cast<QMouseEvent*>(event)->button() != Qt::LeftButton) return true;
		
		qDebug() << "Double click";
		const auto start = clock();

		if (list_items_.contains(qobject_cast<Item*>(watched)))
		{
			if (!GetAllFormatFiles().contains(FileManager::getFormatFile(current_directory_)))
			{
				OpenFolder();
			}
			else
			{
				const auto startOpenFile = clock();
				OpenFile();
				qDebug() << "Duration open file =" << (static_cast<double>(clock()) - static_cast<double>(startOpenFile)) / static_cast<double>(CLOCKS_PER_SEC);
			}
		}
		qDebug() << "Duration =" << (static_cast<double>(clock()) - static_cast<double>(start)) / static_cast<double>(CLOCKS_PER_SEC);

		return true;
	}
	return false;
}

int Explorer::GetMaximumValueVerticalBar() const
{
	if (current_size_item_ < current_vertical_unallocated_space_ + current_size_item_)
		return 0;

	return (current_vertical_allocated_space_ - (current_vertical_unallocated_space_ < 0 ? 0 : current_vertical_unallocated_space_)) / (current_size_item_ + current_space_item_);
}

void Explorer::Update()
{
	const auto vBarValue = ui_->v_bar_general->value();
	ui_->v_bar_general->setValue(0);
	RepaintItems();
	ui_->v_bar_general->setValue(vBarValue);
}

void Explorer::GoBack()
{
	if (current_directory_.isEmpty() 
		|| FileManager::getSystemDrives().contains(current_directory_)
		|| !FileManager::isDirectory(current_directory_)
		|| !FileManager::isDirExists(current_directory_)) {
		return;
	}
	ui_->le_path->setText(ui_->le_path->text().left(ui_->le_path->text().lastIndexOf("/")));
	OpenFolder();
}

void Explorer::SysTick() const
{
	ui_->v_bar_general->setMaximum(GetMaximumValueVerticalBar());
}

void Explorer::OpenFolder()
{
	const auto startOpenFolder = clock();
	if (current_directory_.isEmpty()) return;
	const QDir dir(current_directory_);
	if (!dir.exists())
		return;

	previous_directory_ = current_directory_;
	for (const auto item : list_items_)
	{
		item->hide();
		delete item;
	}
	list_items_.clear();

	const auto startAddFolders = clock();

	const auto& contents = FileManager::getDirectoryContents(current_directory_);
	for (const auto& content : contents)
	{
		const auto& absolute_path = current_directory_ + "/" + content;
		const auto& type = Item::GetItemTypeByPath(absolute_path);

		try {
			auto item = ItemCreator::CreateItem(
				ItemFactory::ItemFactoryData{
					type,
					absolute_path,
					ui_->general_frame,
					default_geometry
				},
				default_style_sheet,
				this);
			list_items_.append(std::move(item));
		}
		catch (const std::runtime_error& er)
		{
			qWarning() << "Error: " << er.what();
		}
	}

	qDebug() << "Duration add folders =" << (static_cast<double>(clock()) - static_cast<double>(startAddFolders)) / static_cast<double>(CLOCKS_PER_SEC);
	const auto startAddfiles = clock();
	
	qDebug() << "Duration add files =" << (static_cast<double>(clock()) - static_cast<double>(startAddfiles)) / static_cast<double>(CLOCKS_PER_SEC);

	ui_->v_bar_general->setValue(0);
	RepaintItems();
	qDebug() << "Duration open folder =" << (static_cast<double>(clock()) - static_cast<double>(startOpenFolder)) / static_cast<double>(CLOCKS_PER_SEC);
}

void Explorer::OpenFile() const
{
	FileManager::openFile(current_directory_);
}

void Explorer::ScrollBarHandler(const int value)
{
	const auto startScrollRepaint = clock();
	RepaintItemsInStep(value - previous_scroll_bar_value_);
	previous_scroll_bar_value_ = value;
	qDebug() << "Duration scroll repaint =" << (static_cast<double>(clock()) - static_cast<double>(startScrollRepaint)) / static_cast<double>(CLOCKS_PER_SEC) << " Count item =" << list_items_.size();
}
