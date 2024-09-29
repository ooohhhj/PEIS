#ifndef DELEGATE_H
#define DELEGATE_H

#include <QObject>
#include <QStyledItemDelegate>
#include <QPushButton>
#include <QPainter>

class Delegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    explicit Delegate(QObject *parent = nullptr);

    // 重写创建编辑器的函数
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 重写绘制函数
    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    // 禁止编辑行为
    void setEditorData(QWidget *editor, const QModelIndex &index) const override {}
    void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const override {}

signals:
    void buttonClicked(const QModelIndex &index) const;


};

#endif // DELEGATE_H
