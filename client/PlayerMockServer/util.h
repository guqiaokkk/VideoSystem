#ifndef UTIL_H
#define UTIL_H

#include <QString>
#include <QDebug>
#include <QFileInfo>

static inline QString getFileName(const QString& path)
{
    QFileInfo fileInfo(path);
    return fileInfo.fileName();
}

// 封装⼀个 "宏" 作为打印⽇志的⽅式.
#define TAG QString("[%1:%2]").arg(getFileName(__FILE__), QString::number(__LINE__))
// #define TAG "[" << __LINE__ << "]"

// qDebug 打印字符串的时候, 就会⾃动加上 " "
#define LOG() qDebug().noquote() << TAG


// 读  从指定⽂件中, 读取所有的⼆进制内容. 得到⼀个 QByteArray
static inline QByteArray loadFileToByteArray(const QString &path)
{
    QFile file(path);
    bool ok = file.open(QFile::ReadOnly);
    if(!ok)
    {
        LOG() << "⽂件打开失败!";
        return QByteArray();
    }
    //ok ，打开成功，开读
    QByteArray content = file.readAll();
    file.close();
    return content;
}

// 写  把 QByteArray 中的内容, 写⼊到某个指定⽂件⾥
static inline void writeByteArrayToFile(const QString &path, const QByteArray &content)
{
    QFile file(path);
    bool ok = file.open(QFile::WriteOnly);
    if (!ok)
    {
        LOG() << "⽂件打开失败!";
        return;
    }
    //ok ，打开成功，开写
    file.write(content);
    file.flush();
    file.close();
}

#endif // UTIL_H
