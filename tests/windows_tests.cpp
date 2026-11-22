#include <QtTest>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonArray>
#include <QJsonDocument>
#include <QSettings>
#include <QTemporaryDir>
#include <memory>
#include "flow.h"
#include "models.h"
#include "backend.h"
#include "theme.h"
#include "translator.h"
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickStyle>

class FakeServer : public QObject {
public:
    QTcpServer server;
    QList<QJsonObject> submissions;
    int polls = 0;
    bool broken = false;
    FakeServer() {
        server.listen(QHostAddress::LocalHost);
        connect(&server, &QTcpServer::newConnection, this, [this] {
            while (auto *socket = server.nextPendingConnection()) {
                auto buffer = std::make_shared<QByteArray>();
                connect(socket, &QTcpSocket::readyRead, socket, [this, socket, buffer] {
                    *buffer += socket->readAll();
                    const int end = int(buffer->indexOf("\r\n\r\n"));
                    if (end < 0) return;
                    const auto headers = buffer->left(end).split('\n');
                    int length = 0;
                    for (const auto &line : headers)
                        if (line.toLower().startsWith("content-length:")) length = line.mid(15).trimmed().toInt();
                    if (buffer->size() < end + 4 + length) return;
                    const auto path = headers.first().split(' ').value(1);
                    QByteArray response;
                    if (path == "/release_task") {
                        submissions << QJsonDocument::fromJson(buffer->mid(end + 4, length)).object();
                        response = R"({"data":{"task_id":"abc"},"error":null})";
                    } else if (path == "/query_result") {
                        ++polls;
                        if (broken) response = R"({"data":[{"task_id":"abc","status":2}],"error":null})";
                        else if (polls == 1) response = R"({"data":[{"task_id":"abc","status":0}],"error":null})";
                        else {
                            QJsonObject result{{"file", "/v1/audio?path=test.mp3"}, {"metas", QJsonObject{{"bpm", 88}, {"keyscale", "C minor"}}}};
                            response = QJsonDocument(QJsonObject{{"data", QJsonArray{QJsonObject{{"task_id", "abc"}, {"status", 1}, {"result", QString::fromUtf8(QJsonDocument(QJsonArray{result}).toJson(QJsonDocument::Compact))}}}}, {"error", QJsonValue::Null}}).toJson();
                        }
                    } else if (path.startsWith("/v1/audio?")) response = "fake mp3 bytes";
                    else response = "{}";
                    socket->write("HTTP/1.1 200 OK\r\nContent-Length: " + QByteArray::number(response.size()) + "\r\nConnection: close\r\n\r\n" + response);
                    socket->disconnectFromHost();
                    buffer->clear();
                });
                connect(socket, &QTcpSocket::disconnected, socket, &QObject::deleteLater);
            }
        });
    }
    QString url() const { return QString("http://127.0.0.1:%1").arg(server.serverPort()); }
};

class WindowsTests : public QObject {
    Q_OBJECT
private slots:
    void initTestCase() {
        QCoreApplication::setOrganizationName("flowtunes-tests");
        QCoreApplication::setApplicationName("flowtunes-tests");
        QSettings().clear();
    }
    void windowLoads() {
        QQuickStyle::setStyle("Material");
        Theme theme; Backend backend; Translator translator;
        QQmlApplicationEngine engine;
        engine.rootContext()->setContextProperty("theme", &theme);
        engine.rootContext()->setContextProperty("backend", &backend);
        engine.rootContext()->setContextProperty("translator", &translator);
        engine.load(QUrl("qrc:/Main.qml"));
        QVERIFY(!engine.rootObjects().isEmpty());
    }
    void generationAndQueue() {
        QTemporaryDir music;
        FakeServer server;
        Models models;
        QVERIFY(!models.setServerUrl("file:///invalid"));
        QVERIFY(models.setServerUrl(server.url()));
        Flow flow(&models, music.path());
        flow.setLyrics(false);
        flow.play("jazz");
        QTRY_VERIFY_WITH_TIMEOUT(!flow.track().isEmpty(), 10000);
        QCOMPARE(flow.bpm(), 88);
        QVERIFY(QFile::exists(flow.track().toLocalFile()));
        QVERIFY(server.polls >= 2);
        QCOMPARE(server.submissions.first().value("audio_format").toString(), QString("mp3"));
        QCOMPARE(server.submissions.first().value("lyrics").toString(), QString("[Instrumental]"));
        QTRY_VERIFY_WITH_TIMEOUT(server.submissions.size() >= 3, 10000);
        QTRY_VERIFY_WITH_TIMEOUT(!flow.composing(), 10000);
        flow.next();
        QCOMPARE(flow.number(), 2);
        flow.stop();
    }
    void failureAndRetry() {
        QTemporaryDir music;
        FakeServer server;
        server.broken = true;
        Models models;
        QVERIFY(models.setServerUrl(server.url()));
        Flow flow(&models, music.path());
        flow.retryDelayMs = 10;
        flow.play("ambient");
        QTRY_VERIFY_WITH_TIMEOUT(!flow.error().isEmpty(), 10000);
        server.broken = false;
        flow.retry();
        QTRY_VERIFY_WITH_TIMEOUT(!flow.track().isEmpty(), 10000);
        flow.stop();
    }
};
QTEST_MAIN(WindowsTests)
#include "windows_tests.moc"
