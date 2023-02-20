#pragma once

#include "ItemFactory.h"
#include <stdexcept>

class ItemCreator{
public:
    static Item * CreateItem(const ItemFactory::ItemFactoryData& data, const QString& default_style_sheet, QWidget* parent) {
        static const ItemFactory item_factory;

        Item* item = item_factory.createItem(ItemFactory::ItemFactoryData{ data.type, data.absolute_path, data.parent, data.geometry });
        if (!item) {
            throw std::runtime_error("Failed to create item");
        }

        item->SetPixmap(FileManager::getIcon(data.absolute_path, data.geometry));
        item->SetType(data.type);
        item->setObjectName(data.absolute_path);
        item->setStyleSheet(default_style_sheet);
        item->installEventFilter(parent);

        return item;
    }
};
