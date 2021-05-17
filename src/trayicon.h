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

#ifndef TRAYICON_H
#define TRAYICON_H

#include <QObject>
#include <QSystemTrayIcon>
#include "orientation.h"
#include <memory>
class TrayIcon : public QObject
{
    Q_OBJECT
public:
    TrayIcon(QObject* parent = nullptr);
    ~TrayIcon();
public slots:
  void setAutoRotating(bool autoRotating);
  void toggleAutoRotating();
  void orientationUpdated(Orientation orientation);
  void activated(QSystemTrayIcon::ActivationReason reason);
  void messageClicked();
private:
  class Private;
  std::unique_ptr<Private> d;
  void emitRotationIfHas();
signals:
  void emitRotation(Orientation orientation);
};

#endif // TRAYICON_H
