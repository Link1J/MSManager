/*
    MSManager - A Minecraft server manager
    Copyright (C) 2020 Link1J

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

#include "usermodel.hpp"

UserModel::UserModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int UserModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : (int)mydata.size();
}

QVariant UserModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= mydata.size() || index.row() < 0)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        return QString::fromStdString(mydata[index.row()]);
    }
    return QVariant();
}

QVariant UserModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return tr("Firstname");
        case 1:
            return tr("Lastname");
        case 2:
            return tr("Email");
        }
    }
    return QVariant();
}

bool UserModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && role == Qt::EditRole && !(index.row() >= mydata.size() || index.row() < 0))
    {
        int row = index.row();

        mydata[row] = value.toString().toStdString();
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

Qt::ItemFlags UserModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractListModel::flags(index);
}

std::string& UserModel::getUser(size_t index)
{
    return mydata[index];
}

void UserModel::addUser(std::string user)
{
    if (std::find(mydata.begin(), mydata.end(), user) != mydata.end())
        return;
    beginInsertRows(QModelIndex(), (int)mydata.size(), (int)mydata.size());
    mydata.push_back(user);
    endInsertRows();
}

void UserModel::removeUser(std::string user)
{
    auto iter = std::find(mydata.begin(), mydata.end(), user);
    if (iter == mydata.end())
        return;
    int index = std::distance(mydata.begin(), iter);

    beginRemoveRows(QModelIndex(), index, index);
    mydata.erase(iter);
    endRemoveRows();
}