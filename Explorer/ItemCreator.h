#pragma once

#include "ItemFactory.h"
#include <stdexcept>

class ItemCreator{
public:
    static Item * CreateItem(const ItemFactory::ItemFactoryData& data, const QString& default_style_sheet, QWidget* watcher) {
        static const ItemFactory item_factory;

        Item* item = item_factory.createItem(data);
        if (!item) {
            throw std::runtime_error("Failed to create item");
        }

        item->SetPixmap(FileManager::getIcon(data.absolute_path, data.geometry));
        item->SetType(data.type);
        item->setObjectName(data.absolute_path);
        item->setStyleSheet(default_style_sheet);
        item->installEventFilter(watcher);

        return item;
    }
};
