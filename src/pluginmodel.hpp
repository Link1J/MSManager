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

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;

    void addPlugin(std::string plugin);
    void removePlugin(std::string plugin);
    bool setData(const QModelIndex& index, const QVariant& value, int role);

    Qt::ItemFlags flags(const QModelIndex& index) const;
    std::string& getPlugin(size_t index);
    const_iterator begin()const { return mydata.begin(); }
    const_iterator end()const { return mydata.end(); }
};