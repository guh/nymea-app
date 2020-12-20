#include "devicecontrolapplication.h"

#include "engine.h"
#include "connection/discovery/nymeadiscovery.h"
#include "connection/nymeahosts.h"
#include "libnymea-app-core.h"
#include "../nymea-app/stylecontroller.h"
#include "../nymea-app/platformhelper.h"
#include "../nymea-app/nfchelper.h"
#include "../nymea-app/nfcthingactionwriter.h"
#include "../nymea-app/platformintegration/android/platformhelperandroid.h"

#include <QQmlApplicationEngine>
#include <QtDebug>
#include <QtQml>
#include <QtAndroid>
#include <QAndroidJniObject>
#include <QAndroidIntent>
#include <QNdefNfcUriRecord>

QObject *platformHelperProvider(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return new PlatformHelperAndroid();
}

DeviceControlApplication::DeviceControlApplication(int argc, char *argv[]) : QApplication(argc, argv)
{
    setApplicationName("nymea-app");
    setOrganizationName("nymea");

    QSettings settings;

    m_discovery = new NymeaDiscovery(this);
    AWSClient::instance()->setConfig(settings.value("cloudEnvironment").toString());
    m_discovery->setAwsClient(AWSClient::instance());

    m_engine = new Engine(this);

    m_qmlEngine = new QQmlApplicationEngine(this);
    registerQmlTypes();
    qmlRegisterSingletonType<PlatformHelper>("Nymea", 1, 0, "PlatformHelper", platformHelperProvider);
    qmlRegisterSingletonType(QUrl("qrc:///ui/utils/NymeaUtils.qml"), "Nymea", 1, 0, "NymeaUtils" );
    qmlRegisterType<NfcThingActionWriter>("Nymea", 1, 0, "NfcThingActionWriter");
    qmlRegisterSingletonType<NfcHelper>("Nymea", 1, 0, "NfcHelper", NfcHelper::nfcHelperProvider);

    StyleController *styleController = new StyleController("light", this);

    QQmlFileSelector *styleSelector = new QQmlFileSelector(m_qmlEngine);
    styleSelector->setExtraSelectors({styleController->currentStyle()});

    foreach (const QFileInfo &fi, QDir(":/ui/fonts/").entryInfoList()) {
        QFontDatabase::addApplicationFont(fi.absoluteFilePath());
    }
    foreach (const QFileInfo &fi, QDir(":/styles/" + styleController->currentStyle() + "/fonts/").entryInfoList()) {
        qDebug() << "Adding style font:" << fi.absoluteFilePath();
        QFontDatabase::addApplicationFont(fi.absoluteFilePath());
    }

    qmlRegisterSingletonType(QUrl("qrc:///styles/" + styleController->currentStyle() + "/Style.qml"), "Nymea", 1, 0, "Style" );

    m_qmlEngine->rootContext()->setContextProperty("styleController", styleController);
    m_qmlEngine->rootContext()->setContextProperty("engine", m_engine);
    m_qmlEngine->rootContext()->setContextProperty("_engine", m_engine);
    m_qmlEngine->rootContext()->setContextProperty("controlledThingId", ""); // Unknown at this point

    m_qmlEngine->load(QUrl(QLatin1String("qrc:/Main.qml")));

    jboolean startedByNfc = QtAndroid::androidActivity().callMethod<jboolean>("startedByNfc", "()Z");
    if (startedByNfc) {
        qDebug() << "**** Started by NFC";
        qDebug() << "Registering NFC handler and waiting for message.";

        QNearFieldManager *manager = new QNearFieldManager(this);
        manager->registerNdefMessageHandler(this, SLOT(handleNdefMessage(QNdefMessage,QNearFieldTarget*)));

    } else {
        qDebug() << "*** Started by other intent";
        qDebug() << "Expecing nymeaId and thingId in intent extras.";
        QString nymeaId = QtAndroid::androidActivity().callObjectMethod<jstring>("nymeaId").toString();
        QString thingId = QtAndroid::androidActivity().callObjectMethod<jstring>("thingId").toString();

        connectToNymea(nymeaId);
        m_qmlEngine->rootContext()->setContextProperty("controlledThingId", thingId);
    }
}

void DeviceControlApplication::handleNdefMessage(QNdefMessage message, QNearFieldTarget *target)
{
    Q_UNUSED(target)
    qDebug() << "************* NFC message!" << message.toByteArray();
    if (message.count() < 1) {
        qWarning() << "NFC message doesn't contain any records...";
        return;
    }
    // NOTE: At this point we're only supporting one NDEF record per message
    QNdefRecord record = message.first();
    QNdefNfcUriRecord uriRecord(record);

    QUrl url = uriRecord.uri();
    if (url.scheme() != "nymea") {
        qWarning() << "NDEF URI record scheme is not \"nymea://\"";
        return;
    }

    QUuid nymeaId = QUuid(url.host());
    if (nymeaId.isNull()) {
        qWarning() << "Invalid nymea UUID in NDEF record.";
        return;
    }

    QUuid thingId = QUuid(QUrlQuery(url).queryItemValue("t"));
    if (thingId.isNull()) {
        qWarning() << "Invalid thing in NDEF record";
        return;
    }

    m_pendingNfcAction = url;

    connectToNymea(nymeaId);
    m_qmlEngine->rootContext()->setContextProperty("controlledThingId", thingId);

    connect(m_engine->thingManager(), &DeviceManager::fetchingDataChanged, [this](){
        if (m_engine->jsonRpcClient()->connected() && !m_engine->thingManager()->fetchingData()) {
            qDebug() << "Ready to process commands";
            runNfcAction();
        }
    });
}

void DeviceControlApplication::connectToNymea(const QUuid &nymeaId)
{
    NymeaHost *host = m_discovery->nymeaHosts()->find(nymeaId);
    if (!host) {
        qWarning() << "No such nymea host:" << nymeaId;
        // TODO: We could wait here until the discovery finds it... But it really should be cached already...
        exit(1);
    }
    qDebug() << "Connecting to:" << host->name();
    m_engine->jsonRpcClient()->connectToHost(host);
}

void DeviceControlApplication::runNfcAction()
{
    if (!m_pendingNfcAction.isEmpty()) {
        qDebug() << "NFC action:" << m_pendingNfcAction;
    }
    QUrl url = m_pendingNfcAction;
    m_pendingNfcAction.clear();

    if (url.scheme() != "nymea") {
        qWarning() << "NDEF URI record scheme is not \"nymea://\" in" << url.toString();
        return;
    }

    QUuid nymeaId = QUuid(url.host());
    if (nymeaId.isNull()) {
        qWarning() << "Invalid nymea UUID" << url.host() << "in NDEF record" << url.toString();
        return;
    }

    QUuid thingId = QUuid(QUrlQuery(url).queryItemValue("t"));
    Device *thing = m_engine->thingManager()->things()->getThing(thingId);
    if (!thing) {
        qDebug() << "Thing" << thingId.toString() << "from" << url.toString() << "doesn't exist on nymea host" << nymeaId.toString();
        return;
    }

    QList<QPair<QString, QString>> queryItems = QUrlQuery(url.query()).queryItems();
    for (int i = 0; i < queryItems.count(); i++) {
        QString entryName = queryItems.at(i).first;
        if (entryName == "t") {
            continue;
        }
        if (!entryName.startsWith("a")) {
            qDebug() << "Only actions are supported. Skipping query item" << entryName;
            continue;
        }

        QString actionString = queryItems.at(i).second;
        QStringList parts = actionString.split("#");
        if (parts.count() == 0) {
            qDebug() << "Invalid action definition:" << actionString;
            continue;
        }

        if (parts.count() > 2) {
            // The parameters might contain a #, let's merge them again
            parts[1] = parts.mid(1).join('#');
        }

        QString actionTypeName = parts.at(0);
        ActionType *actionType = thing->thingClass()->actionTypes()->findByName(actionTypeName);
        if (!actionType) {
            qWarning() << "Invalid action name" << actionType << "in url:" << url.toString();
            continue;
        }

        QHash<QString, QVariant> paramsInUri;
        if (parts.count() > 1) {
            QString paramsString = parts.at(1);
            foreach (const QString &paramString, paramsString.split("+")) {
                QStringList parts = paramString.split(":");
                if (parts.count() != 2) {
                    qWarning() << "Invalid param format" << paramString << "in url:" << url.toString();
                    continue;
                }
                paramsInUri.insert(parts.at(0), parts.at(1));
            }
        }

        qDebug() << "Parameters in NFC uri:" << paramsInUri;

        QVariantList params;
        for (int j = 0; j < actionType->paramTypes()->rowCount(); j++) {
            ParamType *paramType = actionType->paramTypes()->get(j);
            QVariantMap param;
            param.insert("paramTypeId", paramType->id());
            if (paramsInUri.contains(paramType->name())) {
                param.insert("value", paramsInUri.value(paramType->name()));
            } else {
                param.insert("value", paramType->defaultValue());
            }
            params.append(param);
        }

        qDebug() << "Action parameters:" << qUtf8Printable(QJsonDocument::fromVariant(params).toJson());

        m_engine->thingManager()->executeAction(thingId, actionType->id(), params);
    }
}


