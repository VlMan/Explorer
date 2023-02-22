#pragma once

#include <QWidget>

#include "FileManager.h"
#include "Folder.h"
#include "Executable.h"
#include "Picture.h"
#include "Music.h"

class ItemFactory {
public:
    ItemFactory() = default;

    struct ItemFactoryData {
        item_type type;
        QString absolute_path;
        QWidget* parent;
        QRect geometry;
    };

    Item* createItem(const ItemFactoryData& data) const {
        const auto it = factories_.find(data.type);
        if (it != factories_.end()) {
            return it->second(FileManager::getPath(data.absolute_path), data.absolute_path, data.parent, data.geometry);
        }
        return nullptr;
    }

private:
    using ItemCreator = std::function<Item* (const QString&, const QString&, QWidget*, const QRect&)>;
    using FactoryMap = std::map<item_type, ItemCreator>;

    FactoryMap factories_{
        {item_type::folder, [](const QString& path, const QString& absolute_path, QWidget* parent, const QRect& geometry) { return new Folder(parent, geometry, path, absolute_path); }},
        {item_type::exe, [](const QString& path, const QString& absolute_path, QWidget* parent, const QRect& geometry) { return new Executable(parent, geometry, path, absolute_path); }},
        {item_type::picture, [](const QString& path, const QString& absolute_path, QWidget* parent, const QRect& geometry) { return new Picture(parent, geometry, "PNG", path, absolute_path); }},
        {item_type::music, [](const QString& path, const QString& absolute_path, QWidget* parent, const QRect& geometry) { return new Music(parent, geometry, path, absolute_path); }},
        {item_type::unknown, [](const QString& path, const QString& absolute_path, QWidget* parent, const QRect& geometry) { return new Item(parent, geometry, path, absolute_path); }},
    };
};