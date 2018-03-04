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

#include <QDebug>
#include "displaymanager.h"
#include <QtX11Extras/QX11Info>
#include <X11/extensions/Xrandr.h>
#include <QTimer>
#include <QHash>

struct DisplayManagerX11Mediator {
  Display *display;
  unsigned long appRootWindow;
  int appScreen;
  XRRScreenConfiguration *screenConfiguration;
  Rotation rotation;
  SizeID configurationId;
  
  DisplayManagerX11Mediator();
  ~DisplayManagerX11Mediator();
  bool setRotation(Rotation rotation);
};

DisplayManagerX11Mediator::DisplayManagerX11Mediator()
{
  display = QX11Info::display();
  appScreen = QX11Info::appScreen();
  appRootWindow = QX11Info::appRootWindow(appScreen);
  screenConfiguration = XRRGetScreenInfo(display, appRootWindow);
  configurationId = XRRConfigCurrentConfiguration(screenConfiguration, &rotation);
}

DisplayManagerX11Mediator::~DisplayManagerX11Mediator()
{
  XRRFreeScreenConfigInfo(screenConfiguration);
}

bool DisplayManagerX11Mediator::setRotation(Rotation rotation)
{
  return XRRSetScreenConfig(display, screenConfiguration, appRootWindow, configurationId, rotation,  QX11Info::getTimestamp());
}


class DisplayManager::Private {
public:
  Orientation currentOrientation;
  Orientation to_orientation(Rotation rotation);
};

QHash<Orientation, Rotation> orientation2rotation {
  {TopUp, RR_Rotate_0},
  {TopDown, RR_Rotate_180},
  {LeftUp, RR_Rotate_90},
  {RightUp, RR_Rotate_270},
};

Orientation DisplayManager::Private::to_orientation(Rotation rotation)
{
  for(auto key: orientation2rotation.keys())
    if(orientation2rotation[key] == rotation)
      return key;
    // TODO: fallback value?
    return Orientation::LeftUp;
}



DisplayManager::DisplayManager(QObject* parent) : QObject{parent}, d{new Private}
{
  DisplayManagerX11Mediator mediator;
  d->currentOrientation = d->to_orientation(mediator.rotation);
  qDebug() << "Current orientation: " << d->currentOrientation << ", rotation: " << mediator.rotation;
}

DisplayManager::~DisplayManager()
{
}

void DisplayManager::setOrientation(Orientation orientation)
{
  DisplayManagerX11Mediator mediator;
  auto rotation = orientation2rotation[orientation];
  if(mediator.rotation == rotation)
    return;
  mediator.setRotation(rotation);
  d->currentOrientation = orientation;
}
