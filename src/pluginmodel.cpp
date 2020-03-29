#include "pluginmodel.hpp"

PluginModel::PluginModel(QObject* parent)
    : QAbstractListModel(parent)
{
}

int PluginModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : mydata.size();
}

QVariant PluginModel::data(const QModelIndex& index, int role) const
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

QVariant PluginModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool PluginModel::setData(const QModelIndex& index, const QVariant& value, int role)
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

Qt::ItemFlags PluginModel::flags(const QModelIndex& index) const
{
    if (!index.isValid())
        return Qt::ItemIsEnabled;
    return QAbstractListModel::flags(index);
}

std::string& PluginModel::getPlugin(size_t index)
{
    return mydata[index];
}

void PluginModel::addPlugin(std::string plugin)
{
    if (std::find(mydata.begin(), mydata.end(), plugin) != mydata.end())
        return;
    beginInsertRows(QModelIndex(), mydata.size(), mydata.size());
    mydata.push_back(plugin);
    endInsertRows();
}

void PluginModel::removePlugin(std::string plugin)
{
    auto iter = std::find(mydata.begin(), mydata.end(), plugin);
    if (iter == mydata.end())
        return;
    int index = std::distance(mydata.begin(), iter);

    beginRemoveRows(QModelIndex(), index, index);
    mydata.erase(iter);
    endRemoveRows();
}