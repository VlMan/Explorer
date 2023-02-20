#pragma once

#include <QDebug>
#include <QDir>
#include <QTimer>
#include <QKeyEvent>
#include <QProcess>
#include <QDesktopServices>
#include <QThread>

#include "Executable.h"
#include "Folder.h"
#include "Picture.h"
#include "Music.h"
#include "ui_explorer.h"

constexpr auto program_version = "v0.1.050222.5";

class Explorer final : public QMainWindow
{
	Q_OBJECT

	QVector<Item*> list_items_;
	QVector<QString> list_format_file;
	QString current_directory_;
	QString previous_directory_;

	QScopedPointer<QTimer> t_sys_tick_;
	QRect const *current_general_frame_size_; // ref to size general frame
	QMap<int, int> in_row_count_item_; // count item from current row

	QScopedPointer<QPixmap> lbl_unknown_;
	QScopedPointer<QPixmap> lbl_folder_;
	QScopedPointer<QPixmap> lbl_executable_;
	QScopedPointer<QPixmap> lbl_music_;
	QScopedPointer<QPixmap> lbl_picture_png_;

	int current_size_item_;
	int current_space_item_;

	int current_item_count_;

	int current_horizontal_allocated_space_;
	int current_horizontal_unallocated_space_;

	int current_vertical_allocated_space_;
	int current_vertical_unallocated_space_;

	int current_count_row_item_;
	
	int previous_scroll_bar_value_;

public:
	explicit Explorer(QWidget *parent = Q_NULLPTR);
	~Explorer() override { for (const auto item : list_items_) delete item; }

private:
	QScopedPointer<Ui::ExplorerClass> ui_;

	NODISCARD QRect FindOptimizeNextPosition();
	void RepaintItems();
	int GetCurrentVerticalAllocatedSpace();
	int GetCurrentHorizontalAllocatedSpace();
	int GetCurrentVerticalUnallocatedSpace();
	int GetCurrentHorizontalUnallocatedSpace();
	void RepaintItemsInStep(const int step);

	bool eventFilter(QObject* watched, QEvent* event) override;

	NODISCARD int GetMaximumValueVerticalBar() const;

	void Update();
	void GoBack();

private slots:
	void SysTick() const;
	void OpenFolder();
	NODISCARD item_type GetItemTypeByPath(const QString& absolute_path) const;
	void OpenFile() const;
	void ScrollBarHandler(int);
};
