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
#include <stdlib.h> /* getenv */

using namespace std;
class TrayIcon::Private {
public:
  QSystemTrayIcon tray;
  unique_ptr<QMenu> menu;
};

TrayIcon::TrayIcon(QObject* parent) : QObject{parent}, d{new Private}
{
  char* pTray;
  pTray = getenv ("SR_TRAY");
  d->tray.setIcon(QIcon::fromTheme("screenrotator"));
  d->menu.reset(new QMenu());
  d->tray.setToolTip(tr("Screen Rotator"));
  
  d->tray.setContextMenu(d->menu.get());
  d->menu->addAction(tr("Quit"), qApp, &QApplication::quit);
  if (pTray != NULL) {
      QTimer::singleShot(100, &d->tray, &QSystemTrayIcon::hide);
  } else {
      QTimer::singleShot(100, &d->tray, &QSystemTrayIcon::show);
  }
}

TrayIcon::~TrayIcon()
{
}
