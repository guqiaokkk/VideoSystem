#ifndef DATA_H
#define DATA_H

#include <QList>
#include <QHash>
#include <QString>


namespace model{


/////////////////////////////////////////////////
/// 分类和标签
/////////////////////////////////////////////////
class KindAndTag
{
public:
    KindAndTag();

    // 获取所有分类
    const QList<QString> getAllKinds()const;

    // 获取⼀个分类下所有标签及id
    const QHash<QString, int> getTagsByKind(QString kind)const;

    // 获取分类的id
    int getKindId(QString kind)const;

    // 获取kind分类下包含的tag的id
    int getTagId(QString kind, QString tag)const;

private:
    // key为分类名称，value为分类Id
    QHash<QString, int> kindIds;

    // 外层QHash的key为分类名称
    // 内层QHash的key为标签名称，value为标签Id
    QHash<QString, QHash<QString, int>> tagIds;

    static int id;
};


} //end model

#endif // DATA_H
