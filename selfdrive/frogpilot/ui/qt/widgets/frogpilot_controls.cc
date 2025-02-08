#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

#include "selfdrive/ui/ui.h"

void updateFrogPilotToggles() {
  static Params params_memory{"/dev/shm/params"};
  params_memory.putBool("FrogPilotTogglesUpdated", true);
}

QColor loadThemeColors(const QString &colorKey, bool clearCache) {
  static QJsonObject cachedColorData;

  if (clearCache) {
    QFile file("../frogpilot/assets/active_theme/colors/colors.json");

    while (!file.exists()) {
      util::sleep_for(UI_FREQ);
    }

    if (!file.open(QIODevice::ReadOnly)) {
      return QColor();
    }

    cachedColorData = QJsonDocument::fromJson(file.readAll()).object();
  }

  if (cachedColorData.isEmpty()) {
    return QColor();
  }

  QJsonObject colorObj = cachedColorData.value(colorKey).toObject();
  return QColor(
    colorObj.value("red").toInt(255),
    colorObj.value("green").toInt(255),
    colorObj.value("blue").toInt(255),
    colorObj.value("alpha").toInt(255)
  );
}

bool FrogPilotConfirmationDialog::toggleReboot(QWidget *parent) {
  ConfirmationDialog d(tr("Reboot required to take effect."), tr("Reboot Now"), tr("Reboot Later"), false, parent);
  return d.exec();
}

bool FrogPilotConfirmationDialog::yesorno(const QString &prompt_text, QWidget *parent) {
  ConfirmationDialog d(prompt_text, tr("Yes"), tr("No"), false, parent);
  return d.exec();
}
