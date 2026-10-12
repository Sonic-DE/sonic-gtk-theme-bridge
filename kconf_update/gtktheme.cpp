/*
 * SPDX-FileCopyrightText: 2020 Mikhail Zolotukhin <zomial@protonmail.com>
 *
 * SPDX-License-Identifier: GPL-2.0-only OR GPL-3.0-only OR LicenseRef-KDE-Accepted-GPL
 */

#include <QDir>
#include <QRegularExpression>
#include <QString>
#include <QVariant>

#include <gio/gio.h>

#include "config_editor/gsettings.h"
#include "config_editor/gtk2.h"
#include "config_editor/settings_ini.h"
#include "config_editor/utils.h"
#include "config_editor/xsettings.h"

QString gtk2Theme();
void upgradeGtk2Theme();
void upgradeGtk3Theme();

int main()
{
    upgradeGtk2Theme();
    upgradeGtk3Theme();
    g_settings_sync();
    return 0;
}

void upgradeGtk2Theme()
{
    QString currentGtk2Theme = gtk2Theme();
    if (currentGtk2Theme == QStringLiteral("Breeze-Dark")) {
        Gtk2ConfigEditor::setValue(QStringLiteral("gtk-theme-name"), QStringLiteral("Silver-Dark"));
        return;
    }
    if (currentGtk2Theme.isEmpty() //
        || currentGtk2Theme == QStringLiteral("Breeze") //
        || currentGtk2Theme == QStringLiteral("oxygen-gtk") //
        || currentGtk2Theme == QStringLiteral("BreezyGTK") //
        || currentGtk2Theme == QStringLiteral("Orion")) {
        Gtk2ConfigEditor::setValue(QStringLiteral("gtk-theme-name"), QStringLiteral("Silver"));
    }
}

void upgradeGtk3Theme()
{
    for (auto gtkVersion : Utils::s_gtkVersions) {
        const QString currentTheme = SettingsIniEditor::value(QStringLiteral("gtk-theme-name"), gtkVersion);
        QString newTheme;
        if (currentTheme == QStringLiteral("Breeze-Dark")) {
            newTheme = QStringLiteral("Silver-Dark");
        } else if (currentTheme.isEmpty() //
            || currentTheme == QStringLiteral("Breeze") //
            || currentTheme == QStringLiteral("oxygen-gtk") //
            || currentTheme == QStringLiteral("BreezyGTK") //
            || currentTheme == QStringLiteral("Orion")) {
            newTheme = QStringLiteral("Silver");
        }
        if (newTheme.isEmpty()) {
            continue;
        }
        SettingsIniEditor::setValue(QStringLiteral("gtk-theme-name"), newTheme, gtkVersion);
        if (gtkVersion == 3) {
            GSettingsEditor::setValue("gtk-theme", newTheme);
            XSettingsEditor::setValue(QStringLiteral("Net/ThemeName"), newTheme);
        }
    }
}

QString gtk2Theme()
{
    QString gtkrcPath = QDir::homePath() + QStringLiteral("/.gtkrc-2.0");
    QFile gtkrc(gtkrcPath);
    if (gtkrc.open(QIODevice::ReadWrite | QIODevice::Text)) {
        const QRegularExpression regExp(QStringLiteral("gtk-theme-name=[^\n]*($|\n)"));
        while (!gtkrc.atEnd()) {
            QString line = gtkrc.readLine();
            if (line.contains(regExp)) {
                return line.split('"')[1];
            }
        }
    }

    return QString();
}
