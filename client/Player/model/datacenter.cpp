#include "datacenter.h"



namespace model{

DataCenter *DataCenter::instance = nullptr;

DataCenter::DataCenter(QObject *parent)
    : QObject{parent}
{}

DataCenter *model::DataCenter::getInstance()
{
    if(instance == nullptr)
    {
        instance = new DataCenter();
    }
    return instance;
}

const KindAndTag *DataCenter::getKindAndTagsClassPtr()
{
    if(kindAndTags == nullptr)
    {
        // 实例化
        kindAndTags = new KindAndTag();
    }
    return kindAndTags;
}

}


