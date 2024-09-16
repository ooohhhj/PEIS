#include "protocol.h"

Packet Protocol::createPacket(int requestType,  const QJsonObject &jsonMessage)
{
    Packet packet;
    packet.requestType = requestType;

    //将QJsonObject 转换为QByteArray
    QJsonDocument doc(jsonMessage);
    packet.message =doc.toJson(QJsonDocument::Compact);


    //设置消息长度
    packet.length = packet.message.size();

    return packet;
}

QJsonObject Protocol::parsePacket(const Packet &packet)
{
    QJsonDocument doc = QJsonDocument::fromJson(packet.message);
    if(!doc.isNull() && doc.isObject())
    {
        return doc.object();
    }
    else
    {
        qDebug()<<"JSON 解析失败";
    }


}

QByteArray Protocol::serializePacket(const Packet &packet)
{
    QByteArray serializedDate;

    //拼接数据包
    serializedDate.append(QString::number(packet.requestType)+"|");
    serializedDate.append(QString::number(packet.length)+"|");
    serializedDate.append(packet.message);

    return serializedDate;
}

Packet Protocol::deserializePacket(const QByteArray &data)
{
    Packet packet;
    QList<QByteArray> parts =data.split('|');

    if(parts.size()<3)
    {
        qDebug()<<"数据包格式错误";
    }

    //提取请求类型和消息长度
    packet.requestType =parts[0].toInt();
    packet.length =parts[1].toInt();

    //获取消息内容
    packet.message =parts[2];

    return packet;
}


