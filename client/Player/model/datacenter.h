#ifndef DATACENTER_H
#define DATACENTER_H

#include "data.h"

#include <QObject>

namespace model{


class DataCenter : public QObject
{
    Q_OBJECT
public:
    // 获取DataCenter对象实例
    static DataCenter *getInstance();

    // 获取所有分类
    const KindAndTag *getKindAndTagsClassPtr();

private:
    explicit DataCenter(QObject *parent = nullptr);
    static DataCenter *instance;

    // 分类和标签实例指针
    KindAndTag *kindAndTags = nullptr;
};



}


#endif // DATACENTER_H
