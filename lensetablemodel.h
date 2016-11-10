#ifndef LENSETABLEMODEL_H
#define LENSETABLEMODEL_H

#include <QAbstractTableModel>
#include <QVector>
class LenseTableModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit LenseTableModel(QObject *parent = 0);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    bool setHeaderData(int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole) override;

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex &index, const QVariant &value,
                 int role = Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;


    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void setLenseData(QStringList data);
    void setLensData( QVector<QStringList> *data);
    void setCurrentRow(int row);
private:
    QVector<QStringList> *m_data;
    int m_current;

};

#endif // LENSETABLEMODEL_H
