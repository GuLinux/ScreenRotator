/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2018  Marco Gulino <marco@gulinux.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "trayicon.h"
#include <QSystemTrayIcon>
#include <QTimer>
#include <QMenu>
#include <QApplication>
#include <QDebug>
#include <QProcessEnvironment>

using namespace std;
class TrayIcon::Private {
public:
  QSystemTrayIcon tray;
  unique_ptr<QMenu> menu;
  Orientation lastOrientation;
  bool hasLastOrientation;
  bool autoRotating;
};

#define APP_NAME tr("Screen Rotator")
#define APP_ICON QIcon::fromTheme("screenrotator")

TrayIcon::TrayIcon(QObject* parent) : QObject{parent}, d{new Private}
{
  d->tray.setIcon(APP_ICON);
  d->menu.reset(new QMenu());
  d->tray.setToolTip(APP_NAME);

  connect(&d->tray, &QSystemTrayIcon::activated, this, &TrayIcon::activated);
  connect(&d->tray, &QSystemTrayIcon::messageClicked, this, &TrayIcon::messageClicked);
  
  d->tray.setContextMenu(d->menu.get());
  d->menu->addAction(tr("Rotate"), this, &TrayIcon::emitRotationIfHas);
  d->menu->addAction(tr("Toggle auto-rotation"), this, &TrayIcon::toggleAutoRotating);
  d->menu->addAction(tr("Quit"), qApp, &QApplication::quit);
  d->hasLastOrientation = false;

  QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
  d->autoRotating = !env.contains("DEFAULT_AUTO_ROTATION_OFF");
  QTimer::singleShot(100, &d->tray, &QSystemTrayIcon::show);
}

TrayIcon::~TrayIcon()
{
}

void TrayIcon::setAutoRotating(bool autoRotating)
{
  d->autoRotating = autoRotating;
  if (d->autoRotating) {
    d->tray.showMessage(APP_NAME, tr("Auto-rotation enabled"), APP_ICON, 3000);
  } else {
    d->tray.showMessage(APP_NAME, tr("Auto-rotation disabled"), APP_ICON, 3000);
  }
}

void TrayIcon::toggleAutoRotating()
{
  this->setAutoRotating(!d->autoRotating);
}

void TrayIcon::orientationUpdated(Orientation orientation)
{
  d->lastOrientation = orientation;
  d->hasLastOrientation = true;
  if (d->autoRotating) {
    this->emitRotationIfHas();
  } else {
    d->tray.showMessage(APP_NAME, tr("Orientaion change detected, click tray icon to rotate"), APP_ICON, 3000);
    qApp->processEvents();
  }
}

void TrayIcon::activated(QSystemTrayIcon::ActivationReason reason)
{
  this->emitRotationIfHas();
}

void TrayIcon::messageClicked()
{
  this->emitRotationIfHas();
}

void TrayIcon::emitRotationIfHas()
{
  if (d->hasLastOrientation) {
    emit this->emitRotation(d->lastOrientation);
  } else {
    d->tray.showMessage(APP_NAME, tr("No orientaion change detected, try rotate the devise again"), APP_ICON, 5000);
  }
}
