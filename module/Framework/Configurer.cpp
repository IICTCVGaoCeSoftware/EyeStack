#include "Eyestack/Framework/Configurer.hpp"
#include "Eyestack/Framework/Application.hpp"

namespace Eyestack::Framework {

static QLoggingCategory sLogcat("Configurer");

void
Configurer::register_config(uint64_t uuid, Config& config)
{
  Q_ASSERT(_configs.find(uuid) == _configs.end());
  _configs[uuid] = &config;
}

void
Configurer::reset_configs() noexcept
{
  for (auto&& i : _configs)
    i->reset_config();
}

void
Configurer::dump_configs(const QString& path) noexcept
{
  auto& notifier = Application::notifier();

  QFile file(path);
  if (!file.open(QFile::WriteOnly)) {
    notifier.notify_text(QObject::tr("无法写入文件 \"%1\"。").arg(path),
                         QtMsgType::QtCriticalMsg,
                         QObject::tr("配置管理器"));
    return;
  }

  QMap<uint64_t, QByteArray> map;
  QDataStream s(&file);

  for (auto iter = _configs.begin(); iter != _configs.end(); iter++) {
    try {
      QByteArray ba;
      QDataStream ds(&ba, QIODevice::WriteOnly);
      iter.value()->dump_config(ds);
      map[iter.key()] = std::move(ba);

    } catch (Base::exc::Exception& e) {
      auto repr = e.repr();
      qWarning(sLogcat) << "导出配置" << iter.key() << "失败：" << repr;
      notifier.notify_text(repr,
                           QtMsgType::QtWarningMsg,
                           QObject::tr("配置 %1 保存失败。").arg(iter.key()));

    } catch (std::exception& e) {
      qWarning(sLogcat) << "导出配置" << iter.key() << "失败：" << e.what();
      notifier.notify_text(e.what(),
                           QtMsgType::QtWarningMsg,
                           QObject::tr("配置 %1 保存失败。").arg(iter.key()));

    } catch (...) {
      qWarning(sLogcat) << "导出配置" << iter.key() << "失败：未知异常";
      notifier.notify_text(QObject::tr("未知错误"),
                           QtMsgType::QtWarningMsg,
                           QObject::tr("配置 %1 保存失败。").arg(iter.key()));
    }
  }

  s << map;

  file.close();
}

void
Configurer::load_configs(const QString& path) noexcept
{
  auto& notifier = Application::notifier();

  QFile file(path);
  if (!file.open(QFile::ReadOnly)) {
    notifier.notify_text(QObject::tr("无法读取文件 \"%1\"。").arg(path),
                         QtMsgType::QtCriticalMsg,
                         QObject::tr("配置管理器"));
    return;
  }

  QMap<uint64_t, QByteArray> map;
  QDataStream s(&file);

  s >> map;

  for (auto iter = _configs.begin(); iter != _configs.end(); iter++) {
    try {
      auto ba = map.find(iter.key());
      if (ba == map.end()) {
        Application::notifier().notify_text(
          QObject::tr("配置 %1 加载失败：无数据。").arg(iter.key()),
          QtMsgType::QtWarningMsg);
        continue;
      }

      QDataStream ds(ba.value());
      iter.value()->load_config(ds);
      continue;

    } catch (Base::exc::Exception& e) {
      auto repr = e.repr();
      iter.value()->reset_config();
      qWarning(sLogcat) << "导入配置" << iter.key() << "失败：" << repr;
      notifier.notify_text(
        repr,
        QtMsgType::QtWarningMsg,
        QObject::tr("配置 %1 加载失败，已重置为默认值。").arg(iter.key()));

    } catch (std::exception& e) {
      iter.value()->reset_config();
      qWarning(sLogcat) << "导入配置" << iter.key() << "失败：" << e.what();
      notifier.notify_text(
        e.what(),
        QtMsgType::QtWarningMsg,
        QObject::tr("配置 %1 加载失败，已重置为默认值。").arg(iter.key()));

    } catch (...) {
      iter.value()->reset_config();
      qWarning(sLogcat) << "导入配置" << iter.key() << "失败：未知异常";
      notifier.notify_text(
        QObject::tr("未知错误"),
        QtMsgType::QtWarningMsg,
        QObject::tr("配置 %1 加载失败，已重置为默认值。").arg(iter.key()));
    }
  }

  file.close();
}

}
