/*
    MSManager - A Minecraft server manager
    Copyright (C) 2020 Jared Irwin

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#pragma once

#include <QAbstractListModel>
#include <vector>
#include <string>

class PluginModel : public QAbstractListModel
{
    Q_OBJECT
    std::vector<std::string> mydata;

public:
    typedef std::vector<std::string>::const_iterator const_iterator;

    explicit PluginModel(QObject* parent = 0);
    enum { FIRSTNAME = 0, LASTNAME, EMAIL, MAX_COLS };

    int rowCount(const QModelIndex& parent = QModelIndex{}) const override;
    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    void addPlugin(std::string plugin);
    void removePlugin(std::string plugin);
    bool setData(const QModelIndex& index, const QVariant& value, int role) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    std::string& getPlugin(size_t index);
    const_iterator begin()const { return mydata.begin(); }
    const_iterator end()const { return mydata.end(); }
};