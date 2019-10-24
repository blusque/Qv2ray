//
// This file handles some important migration
// from old to newer versions of Qv2ray.
//

#include "QvUtils.h"

#define UPDATELOG(msg) LOG(MODULE_CONFIG, "  [" + to_string(fromVersion) + "-" + to_string(fromVersion + 1) + "] --> " msg)

namespace Qv2ray
{
    namespace QvConfigModels
    {
        // Private member
        QJsonObject UpgradeConfig_Inc(int fromVersion, QJsonObject root)
        {
            switch (fromVersion) {
                case 1: {
                    auto v1_oldConfigVersion = root["config_version"].toString();
                    // From 1 to 2, we changed the config_version from 'string' to 'int'
                    root.remove("config_version");
                    root["config_version"] = 2;
                    UPDATELOG("Upgrading config_version from old value " + v1_oldConfigVersion.toStdString() + " to 2")
                    break;
                }

                case 2 : {
                    // We copied those files.
                    auto vCoreFilePath = root["v2CorePath"].toString();
                    auto vCoreDestPath = QV2RAY_DEFAULT_VCORE_PATH;
                    // We also need v2ctl
                    auto v2CtlFilePath = QFileInfo(vCoreFilePath).dir().path() + "/v2ctl";
                    auto v2CtlDestPath = QFileInfo(vCoreDestPath).dir().path() + "/v2ctl";
#ifdef __WIN32
                    v2CtlFilePath = v2CtlFilePath.append(".exe");
                    v2CtlDestPath = v2CtlDestPath.append(".exe");
#endif
                    QFile::copy(vCoreFilePath, vCoreDestPath);
                    QFile::copy(v2CtlFilePath, v2CtlDestPath);
                    root.remove("v2CorePath");
                    UPDATELOG("v2CorePath value from: " + vCoreFilePath.toStdString() + " to " +  vCoreDestPath.toStdString())
                    UPDATELOG("v2CtlFilePath value from: " + v2CtlFilePath.toStdString() + " to " + v2CtlDestPath.toStdString())
                    break;
                }

                case 3 : {
                    // We changed a key name in the config file.
                    //proxyDefault
                    auto oldProxyDefault = root["proxyDefault"].toBool();
                    root.remove("proxyDefault");
                    root["enableProxy"] = oldProxyDefault;
                    //enableProxy
                    UPDATELOG("key: proxyDefault->enableProxy, value from: " + to_string(oldProxyDefault) + " to " + to_string(oldProxyDefault))
                    break;
                }

                case 4: {
                    // From 2 to 3, we changed the "proxyCN" to "bypassCN" as it's easier to understand....
                    auto v2_oldProxyCN = root["proxyCN"].toBool();
                    root.remove("proxyCN");
                    root.insert("bypassCN", !v2_oldProxyCN);
                    UPDATELOG("Upgrading proxyCN to bypassCN and changed the value to " + to_string(!v2_oldProxyCN))
                    break;
                }

                case 5: {
                    auto v3_oldrunAsRoot = root["runAsRoot"].toBool();
                    // From 3 to 4, we changed 'runAsRoot' to 'tProxySupport'
                    root.remove("runAsRoot");
                    root.insert("tProxySupport", v3_oldrunAsRoot);
                    UPDATELOG("Upgrading runAsRoot to tProxySupport, the value is not changed: " + to_string(v3_oldrunAsRoot))
                    break;
                }

                case 7: {
                    QString path;
#ifdef _WIN32
                    path = QV2RAY_DEFAULT_VCORE_PATH + ".exe";
#else
                    path = QV2RAY_DEFAULT_VCORE_PATH;
#endif
                    root["v2CorePath"] = path;
                    UPDATELOG("Added ")
                    break;
                }
            }

            root["config_version"] = root["config_version"].toInt() + 1;
            return root;
        }

        // Exported function
        QJsonObject UpgradeConfig(int fromVersion, int toVersion, QJsonObject root)
        {
            LOG(MODULE_CONFIG, "Migrating config from version " + to_string(fromVersion) + " to " + to_string(toVersion))

            for (int i = fromVersion; i < toVersion; i++) {
                root = UpgradeConfig_Inc(i, root);
            }

            return root;
        }

    }
}