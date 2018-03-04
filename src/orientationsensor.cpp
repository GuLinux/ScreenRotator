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

#include "orientationsensor.h"
#include <QOrientationSensor>
#include <QHash>
#include <QDebug>

class OrientationSensor::Private {
public:
  QOrientationSensor sensor;
  QHash<QOrientationReading::Orientation, Orientation> to_orientation;
};
OrientationSensor::OrientationSensor(QObject* parent) : QObject{parent}, d{new Private}
{
  d->to_orientation = {
    { QOrientationReading::TopUp, TopUp },
    { QOrientationReading::TopDown, TopDown },
    { QOrientationReading::RightUp, RightUp },
    { QOrientationReading::LeftUp, LeftUp },
  };
  connect(&d->sensor, &QOrientationSensor::readingChanged, this, [this]{
    auto reading = d->sensor.reading();
    if(! reading)
      return;
    if(d->to_orientation.keys().contains(reading->orientation())) {
      emit this->reading(d->to_orientation[reading->orientation()]);
    }
  });
  connect(&d->sensor, &QOrientationSensor::sensorError, this, [this](int error){
    qDebug() << "Sensor error: " << error;
  });
  auto started = d->sensor.start();
  
}

OrientationSensor::~OrientationSensor()
{
}
