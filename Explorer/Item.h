#pragma once

#include <QGridLayout>
#include <QDebug>
#include "ClicableLabel.h"
#include "ClicableWidget.h"

enum class item_type
{
	unknown = 0,
	folder,
	exe
};

#define NODISCARD [[nodiscard]]

class Item : public ClicableWidget
{
	Q_OBJECT

	QScopedPointer<QPixmap> pix_;
	QScopedPointer<QLabel> lbl_;
	QScopedPointer<QLabel> lbl_path_;
	QScopedPointer<QRect> size_;
	QScopedPointer<QRect> size_lbl_;
	QScopedPointer<QGridLayout> layout_;

	item_type type_;

	QString path_;
	QString absolute_path_;


public:
	explicit Item(QWidget* w, const QRect& rect, QString path, QString absolute_path)
	: ClicableWidget(w),
	pix_(new QPixmap),
	lbl_(new QLabel(this)),
	lbl_path_(new QLabel(this)),
	size_(new QRect(rect)),
	size_lbl_(new QRect(rect)),
	layout_(new QGridLayout(this)),
	type_(item_type::unknown),
	path_(std::move(path)),
	absolute_path_(std::move(absolute_path)) { }

	~Item() override { lbl_.reset(); lbl_path_.reset(); pix_.reset(); size_.reset(); }

	void SetType(const item_type type) { type_ = type; }
	NODISCARD item_type GetType() const { return type_; }

	virtual void SetPixmap(const QPixmap& pix = QPixmap());
	virtual void Show();

	void SetRect(const QRect& rect) {
		*size_ = rect;
		size_lbl_.reset(new QRect(this->rect()));
		size_lbl_->setHeight(size_lbl_->height());
		this->setGeometry(rect);
	}
	NODISCARD QRect GetSize() const { return *size_; }
protected:
	void setPixmap(const QPixmap& pix);
	NODISCARD QRect SizeLabel() const { return *size_lbl_; }

};
