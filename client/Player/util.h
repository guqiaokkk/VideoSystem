#ifndef UTIL_H
#define UTIL_H

#include <QDebug>
#include <QString>
#include <QFileInfo>

#include <QFile>
#include <QIcon>

#include <QPainter>
#include <QPainterPath>


static inline QString getFileName(const QString &path)
{
    QFileInfo fileInfo(path);
    return fileInfo.fileName();
}

// 封装⼀个 "宏" 作为打印⽇志的⽅式
#define TAG QString("[%1:%2]").arg(getFileName(__FILE__), QString::number(__LINE__))
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

// 根据 QByteArray, 转成 QIcon
static inline QIcon makeIcon(const QByteArray& byteArray)
{
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    QIcon icon(pixmap);
    return icon;
}

// 根据 QByteArray, 转化成圆形的 QIcon
static inline QIcon makeIcon(const QByteArray &byteArray, int radius) //第⼆个参数为得到的圆形图⽚的半径
{
    QPixmap pixmap;
    pixmap.loadFromData(byteArray);
    if(pixmap.isNull())
    {
        return QIcon();
    }

    // 把 pixmap 缩放到指定的 2*radius ⼤⼩.
    // 保持比例 KeepAspectRatioByExpanding  SmoothTransformation平滑缩放, 获得更⾼的图⽚质量, 但是会牺牲⼀定速度
    pixmap = pixmap.scaled(2 * radius, 2 * radius, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation);

    // 构造绘图设置，画图的画布
    QPixmap output = QPixmap(pixmap.size());
    output.fill(Qt::transparent);
    QPainter painter(&output);

    painter.setRenderHint(QPainter::Antialiasing);  // 抗锯齿

    // 创建圆形路径
    QPainterPath path;
    // 设置裁剪路径，只有在裁剪路径内的区域才会被裁剪
    path.addEllipse(0, 0, 2*radius, 2 * radius);

    painter.setClipPath(path);                    // 设置裁剪区域

    // 绘制圆形图⽚
    painter.drawPixmap(0, 0, pixmap);
    painter.end();

    QIcon icon(output);
    return icon;
}

// 整数转换为字符串
// ⼩于10000直接转成数字字符串,⼤于10000转成m.n万
static QString intToString(int64_t value)
{
    if(value < 10000)
    {
        return QString::number(value);
    }
    else
    {
        return QString::asprintf("%.1lf万", value/10000.0);
    }
}


static QString intToString2(int64_t value)
{
    if(value < 10000)
    {
        return QString::number(value);
    }
    else
    {
        return QString::asprintf("%.2lfw", value/10000.0);
    }
}



#endif // UTIL_H
