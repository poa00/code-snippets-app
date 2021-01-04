#include "worker.h"
#include <QUrl>
#include <QUrlQuery>
#include "../presenter/mainwindow.h"


Worker::Worker(QObject *parent) : QObject(parent)
{

}

void Worker::get(QString& title, QString& lang, QString& author, QDateTime& date_from, QDateTime& date_to)
{
    qInfo() << "getting from server";
    QUrl url(LOCATION_);
    QUrlQuery query;

    query.addQueryItem("title", title);
    query.addQueryItem("lang", lang);
    query.addQueryItem("author", author);

    qint64 int_date_from = date_from.toSecsSinceEpoch();
    query.addQueryItem("from", QString::number(int_date_from));

    qint64 int_date_to = date_to.toSecsSinceEpoch();
    query.addQueryItem("to", QString::number(int_date_to));
    url.setQuery(query);

    qInfo() << url.query();
    qInfo() << url.toString();
    qInfo() << url.url();

    QNetworkRequest request(url);

    QNetworkReply* reply = manager_.get(request);

    connect(reply, &QNetworkReply::readyRead, this, &Worker::readyRead);
}

void Worker::post(Snippet& snip)
{
    qInfo() << "posting the msg";
    QNetworkRequest request = QNetworkRequest(QUrl(LOCATION_));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "json");


    QJsonObject json_data = snip.toJson();
    QByteArray data = QJsonDocument(json_data).toJson();
    // worker_.post("http://127.0.0.1:8000/app", QJsonDocument(data).toJson());

    QNetworkReply* reply = manager_.post(request, data);
    connect(reply, &QNetworkReply::readyRead, this, &Worker::readyRead);
}

void Worker::readyRead()
{
    qInfo() << "got answer";

    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    QByteArray byte_array = reply->readAll();
    emit data_received(byte_array);

    if (reply) qInfo() << reply->readAll();
    else qInfo() << "no reply??";
}
