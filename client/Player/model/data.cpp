#include "data.h"

namespace model{

/////////////////////////////////////////////////
/// 分类和标签
/////////////////////////////////////////////////

int KindAndTag::id = 10000;
KindAndTag::KindAndTag()
{
    // 构建分类及其id
    QList<QString> kinds = {"历史", "美食", "游戏", "科技", "运动", "动物", "旅行", "电影"};
    for(auto &kind : kinds){
        kindIds.insert(kind, id++);
    }

    // 构建分类对应的标签
    QHash<QString, QList<QString>> kindsAndTags = {
        {"历史", {"中国史", "世界史", "历史⼈物", "艺术", "⽂化", "奇闻"}},
        {"美食", {"美⻝测评", "美⻝制作", "美⻝攻略", "美⻝记录", "探店", "⽔果", "海鲜"}},
        {"游戏", {"游戏攻略", "单机游戏", "电⼦竞技", "⼿机游戏", "⽹络游戏", "游戏赛事", "桌游棋牌"}},
        {"科技", {"数码", "软件应⽤", "智能家居", "⼿机", "电脑", "⼈⼯智能", "基础设施"}},
        {"运动", {"篮球", "⾜球", "乒乓球", "⽻⽑球", "健⾝", "竞技体育", "运动装备"}},
        {"动物", {"哈基米", "大狗嚼", "宠物知识", "动物资讯", "野⽣动物", "动物世界", "萌宠"}},
        {"旅行", {"旅游攻略", "旅⾏Vlog", "⾃驾游", "交通", "环球旅⾏", "露营", "野外⽣存"}},
        {"电影", {"电影解说", "电影推荐", "电影剪辑", "搞笑", "吐槽", "悬疑", "经典"}}};

    // 构建分类下：各个标签及其对应id
    for(auto &kind : kinds)
    {
        // 构建kind下所有标签及其id
        QList<QString> &tags = kindsAndTags[kind];
        QHash<QString, int> tagIdsOfKind;
        for(auto &tag : tags)
        {
            tagIdsOfKind.insert(tag, id++);
        }

        tagIds.insert(kind, tagIdsOfKind);
    }
}

const QList<QString> KindAndTag::getAllKinds() const
{
    return kindIds.keys();
}

const QHash<QString, int> KindAndTag::getTagsByKind(QString kind) const
{
    return tagIds[kind];
}

int KindAndTag::getKindId(QString kind) const
{
    return kindIds[kind];
}

int KindAndTag::getTagId(QString kind, QString tag) const
{
    return tagIds[kind][tag];
}


} //end model

