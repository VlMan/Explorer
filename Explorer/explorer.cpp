#include "explorer.h"

constexpr int sys_tick_timer = 100;
constexpr auto default_style_sheet = "border: 0px;";
constexpr auto default_geometry = QRect(0, 0, 100, 100);

static QVector<QString> list_format_file {  // NOLINT(clang-diagnostic-exit-time-destructors)
	".exe",
	".png"
};

Explorer::Explorer(QWidget *parent)
	: QMainWindow(parent),
	current_directory_("C:/"),
	previous_directory_("."),
	lbl_unknown_(new QPixmap("images/unknown.png", "PNG")),
	lbl_folder_(new QPixmap("images/folder.png", "PNG")),
	lbl_executable_(new QPixmap("images/exe.png", "PNG")),
	current_size_item_(100),
	current_space_item_(10),
	current_item_count_(0),
	current_count_row_item_(0),
	previous_scroll_bar_value_(0),
	ui_(new Ui::ExplorerClass)
{
	ui_->setupUi(this);
	this->installEventFilter(this);
	ui_->le_path->setText(current_directory_);

	current_general_frame_size_ = &ui_->general_frame->geometry();
	current_horizontal_allocated_space_ = 0;
	current_horizontal_unallocated_space_ = current_general_frame_size_->width();

	current_vertical_allocated_space_ = 0;
	current_vertical_unallocated_space_ = current_general_frame_size_->height();

	t_sys_tick_ = new QTimer(this);
	t_sys_tick_->start(sys_tick_timer);

	connect(ui_->v_bar_general, &QScrollBar::valueChanged, this, &Explorer::ScrollBarHandler);
	connect(t_sys_tick_, &QTimer::timeout, this, &Explorer::SysTick);
	connect(ui_->le_path, &QLineEdit::textChanged, this, [&](const QString& path) -> void
	{
		if (path.isEmpty()) return;
		
		current_directory_ = path;
	});
	this->show();

	OpenFolder();
}

Item* Explorer::AddNewItem(const QString& path, const QString& absolute_path, const item_type type)
{
	Item* item = nullptr;
	switch(type)
	{
	case item_type::folder:
	{ item = new Folder(ui_->general_frame, default_geometry, path, absolute_path); item->SetPixmap(*lbl_folder_); break; }

	case item_type::exe:
	{ item = new Executable(ui_->general_frame, default_geometry, path, absolute_path); item->SetPixmap(*lbl_executable_); break; }

	case item_type::unknown:
	{ item = new Item(ui_->general_frame, default_geometry, path, absolute_path); item->SetPixmap(*lbl_unknown_); break; }
	}
	item->SetType(type);
	item->setObjectName(absolute_path);
	item->setStyleSheet(default_style_sheet);
	item->installEventFilter(this);

	connect(item, &Item::clicked, this, [&]() -> void
	{
		ui_->le_path->setText(qobject_cast<Item*>(sender())->objectName());
	});

	item->show();
	return item;
}

QRect Explorer::FindOptimizeNextPosition()
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

	if (current_horizontal_unallocated_space_ > (current_size_item_ + current_space_item_ * 2)) // if there is free space for item
	{
		const int lastItemCount = in_row_count_item_.value(current_count_row_item_);
		in_row_count_item_.remove(current_count_row_item_);
		in_row_count_item_.insert(current_count_row_item_, lastItemCount + 1);

		return QRect(
			current_horizontal_allocated_space_ + current_space_item_,
			(current_vertical_allocated_space_ - current_size_item_),
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
	current_horizontal_unallocated_space_ = current_general_frame_size_->width() - current_horizontal_allocated_space_;

	current_vertical_allocated_space_ = 0;
	current_vertical_unallocated_space_ = current_general_frame_size_->height() - current_horizontal_allocated_space_;

	current_count_row_item_ = 0;
	current_item_count_ = 0;
	in_row_count_item_.clear();

	for (const auto item : list_items_)
	{
		switch (item->GetType())
		{
		case item_type::folder:
		{
			const auto folder = qobject_cast<Folder*>(item);
			folder->SetRect(FindOptimizeNextPosition());

		} break;
		case item_type::exe:
		{
			const auto exe = qobject_cast<Executable*>(item);
			exe->SetRect(FindOptimizeNextPosition());

		} break;
		case item_type::unknown:
		{
			item->SetRect(FindOptimizeNextPosition());

		} break;
		}
		current_item_count_++;
		current_horizontal_allocated_space_ = ((in_row_count_item_.value(current_count_row_item_) * current_size_item_) + (in_row_count_item_.value(current_count_row_item_) * current_space_item_));
		current_horizontal_unallocated_space_ = current_general_frame_size_->width() - current_horizontal_allocated_space_;

		current_vertical_allocated_space_ = ((current_count_row_item_ * current_size_item_) + (current_count_row_item_ * current_space_item_));
		current_vertical_unallocated_space_ = current_general_frame_size_->height() - current_vertical_allocated_space_;
		
	}

	qDebug() << "Duration repaint items =" << (static_cast<double>(clock()) - static_cast<double>(startRepaintItems)) / static_cast<double>(CLOCKS_PER_SEC) << " Count item =" << list_items_.size();
}

void Explorer::RepaintItemsInStep(const int step)
{
	for (const auto item : list_items_)
	{
		item->SetRect(QRect(item->GetSize().left(), item->GetSize().top() - (current_size_item_ + current_space_item_) * step, current_size_item_, current_size_item_));
	}
}

bool Explorer::eventFilter(QObject* watched, QEvent* event)
{
	if (event->type() == QEvent::Resize)
	{
		if (watched == this)
		{
			ui_->v_bar_general->setValue(0);
			RepaintItems();
		}
		return true;
	}

	if (event->type() == QEvent::Wheel)
	{
		const auto wheel = dynamic_cast<QWheelEvent*>(event);
		int step;

		if (wheel->delta() < 0) step = 1;
		else step = -1;

		ui_->v_bar_general->setValue(ui_->v_bar_general->value() + step);
	}

	if (event->type() == QEvent::KeyPress)
	{
		if (const auto key = dynamic_cast<QKeyEvent*>(event)->key(); key == Qt::Key_Enter || key == Qt::Key_Return)
		{
			if (std::none_of(
				list_format_file.begin(),
				list_format_file.end(),
				[=](const QString& format) -> bool { return current_directory_.endsWith(format); }))
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
			ui_->le_path->setText(ui_->le_path->text().left(ui_->le_path->text().lastIndexOf("/")));
			current_directory_ = ui_->le_path->text();
			OpenFolder();			
		}
		return true;
	}

	if (event->type() == QEvent::MouseButtonDblClick)
	{
		qDebug() << "Double click";
		const auto start = clock();

		if (std::any_of(
			list_items_.begin(),
			list_items_.end(),
			[=](const Item* item) -> bool { return watched->objectName() == item->objectName(); }))
		{
			if (std::none_of(
			list_format_file.begin(),
			list_format_file.end(),
			[=](const QString& format) -> bool { return watched->objectName().endsWith(format); }))
			{
				OpenFolder();
			}
			else
			{
				const auto startOpenFiler = clock();
				OpenFile();
				qDebug() << "Duration open file =" << (static_cast<double>(clock()) - static_cast<double>(startOpenFiler)) / static_cast<double>(CLOCKS_PER_SEC);
			}
		}
		qDebug() << "Duration =" << (static_cast<double>(clock()) - static_cast<double>(start)) / static_cast<double>(CLOCKS_PER_SEC);

		return true;
	}
	return false;
}

int Explorer::GetCurrentValueVerticalBar() const
{
	if (current_size_item_ < (current_vertical_unallocated_space_ + current_size_item_))
		return 0;

	return (current_vertical_allocated_space_ - (current_vertical_unallocated_space_ < 0 ? 0 : current_vertical_unallocated_space_)) / current_size_item_;
}

void Explorer::SysTick() const
{
	ui_->v_bar_general->setMaximum(GetCurrentValueVerticalBar());
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
	const auto listFolders = dir.entryList(QDir::Dirs);
	const auto listFiles = dir.entryList(QDir::Files);

	const auto startAddFolders = clock();

	for (auto& folderName : listFolders)
		list_items_.append(AddNewItem(folderName, current_directory_ + "/" + folderName, item_type::folder));

	qDebug() << "Duration add folders =" << (static_cast<double>(clock()) - static_cast<double>(startAddFolders)) / static_cast<double>(CLOCKS_PER_SEC);
	const auto startAddExe = clock();

	for (auto& fileName : listFiles)
		if (fileName.endsWith(".exe"))
			list_items_.append(AddNewItem(fileName, current_directory_ + "/" + fileName, item_type::exe));

	qDebug() << "Duration add exe =" << (static_cast<double>(clock()) - static_cast<double>(startAddExe)) / static_cast<double>(CLOCKS_PER_SEC);
	const auto startAddUnknown = clock();

	for (auto& fileName : listFiles)
		if (std::none_of(list_format_file.begin(), list_format_file.end(), [=](const QString& format) -> bool { return fileName.endsWith(format); }))
			list_items_.append(AddNewItem(fileName, current_directory_ + "/" + fileName, item_type::unknown));
	qDebug() << "Duration add unknown =" << (static_cast<double>(clock()) - static_cast<double>(startAddUnknown)) / static_cast<double>(CLOCKS_PER_SEC);

	ui_->v_bar_general->setValue(0);
	RepaintItems();
	qDebug() << "Duration open folder =" << (static_cast<double>(clock()) - static_cast<double>(startOpenFolder)) / static_cast<double>(CLOCKS_PER_SEC);
}

void Explorer::OpenFile()
{
	if (current_directory_.isEmpty()) return;

	const auto th = new QThread;
	const auto pr = new QProcess;
	connect(th, &QThread::started, pr, [=]() { pr->start(current_directory_); });
	connect(th, &QThread::finished, pr, &QProcess::deleteLater);
	connect(th, &QThread::finished, th, &QThread::deleteLater);

	pr->moveToThread(th);

	th->start();
}

void Explorer::ScrollBarHandler(const int value)
{
	RepaintItemsInStep(value - previous_scroll_bar_value_);
	previous_scroll_bar_value_ = value;
}
