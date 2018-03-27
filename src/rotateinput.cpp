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
#include "rotateinput.h"
#include <QtX11Extras/QX11Info>
#include <QList>

#include <X11/extensions/XInput2.h>
#include <X11/Xatom.h>

using namespace std;

struct InputProperty {
  InputProperty(int deviceId, Display *display, Atom atom);
  int deviceId;
  Display *display;
  Atom atom;
  Atom type;
  int format;
  QString name;
  
  bool isRotationMatrix() const;
  void setRotationMatrix(const vector<float> &matrix);
};


struct InputDevice {
  InputDevice(XIDeviceInfo *info, Display *display);
  Display *display;
  QString name;
  int id;
  bool isTouchScreen = false;
  bool hasRotationMatrix = false;
  QList<InputProperty> properties();
};


InputDevice::InputDevice(XIDeviceInfo* info, Display *display) : display{display}
{
  name = QString{info->name};
  id = info->deviceid;
  for(int j=0; j<info->num_classes; j++) {
    if(info->classes[j]->type == XITouchClass) {
      qDebug() << "Device" << info->name << "seems to be a touchscreen device";
      isTouchScreen = true;
    }
  }
  for(auto property: properties()) {
    if(property.isRotationMatrix())
      this->hasRotationMatrix = true;
  }
}

QList<InputProperty> InputDevice::properties()
{
  QList<InputProperty> properties;
  int properties_size;
  Atom *atoms = XIListProperties(display, id, &properties_size);
  for(int i=0; i<properties_size; i++)
    properties.push_back(InputProperty{id, display, atoms[i]});
  XFree(atoms);
  return properties;
}


InputProperty::InputProperty(int deviceId, Display* display, Atom atom) : deviceId{deviceId}, display{display}, atom{atom}
{
  char *name_c = XGetAtomName(display, atom);
  name = QString{name_c};
  XFree(name_c);
  Atom property_type;
  int property_format;
  uint64_t items_count, bytes_after;
  uint8_t *data;
  if(XIGetProperty(display, deviceId, atom, 0, 0, False, AnyPropertyType, &property_type, &property_format, &items_count, &bytes_after, &data) == Success) {
    type = property_type;
    format = property_format;
    qDebug() << "Property name=" << name << ", atom=" << atom << ", format=" << format << ", type=" << type;
    XFree(data);
  } else {
    qDebug() << "Unable to get data for property " << name;
  }
}

bool InputProperty::isRotationMatrix() const
{
  return name == "Coordinate Transformation Matrix";
}

void InputProperty::setRotationMatrix(const vector<float>& matrix)
{
  union {
    unsigned char *c;
    float *f;
  } data;
  data.f = new float[matrix.size()];
  int data_size = 0;
  for(auto item: matrix) {
    data.f[data_size++] = item;
  }
  XIChangeProperty(display, deviceId, atom, type, format, PropModeReplace, data.c, matrix.size());
  delete data.f;
}



class RotateInput::Private {
public:
  Display *display;
  QList<InputDevice> devices;
};

RotateInput::RotateInput(QObject* parent) : QObject{parent}, d{new Private}
{
  d->display = QX11Info::display();
  int devices;
  XIDeviceInfo *deviceInfo = XIQueryDevice(d->display, XIAllDevices, &devices);
  for(int i=0; i<devices; i++) {
    InputDevice device{&deviceInfo[i], d->display};
    if(device.hasRotationMatrix)
      d->devices.push_back(device);
  }
  XIFreeDeviceInfo(deviceInfo);
  
  for(auto device: d->devices) {
    for(auto property: device.properties()) {
      qDebug() << "Device" << device.name << ", property:" << property.name << ", atom=" << property.atom << ", type: " << property.type;
    }
  }
}
RotateInput::~RotateInput()
{
}

void RotateInput::rotate(Orientation orientation)
{
#ifdef USE_XINPUT
  static QHash<Orientation, QStringList>  orientation_matrix_map {
    {TopUp, {"1", "0", "0", "0", "1", "0", "0", "0", "1"}},
    {TopDown, {"-1", "0", "1", "0", "-1", "1", "0", "0", "1"}},
    {LeftUp, {"0", "-1", "1", "1", "0", "0", "0", "0", "1"}},
    {RightUp, {"0", "1", "0", "-1", "0", "1", "0", "0", "1"}},
  };
  auto orientation_matrix = orientation_matrix_map[orientation];
  for(auto device: d->devices) {
    do_set_prop_xi2(d->display, device.id, None, 0, "Coordinate Transformation Matrix", orientation_matrix);
  }
#else
  static QHash<Orientation, vector<float>>  orientation_matrix_map {
    {TopUp, {1, 0, 0, 0, 1, 0, 0, 0, 1}},
    {TopDown, {-1, 0, 1, 0, -1, 1, 0, 0, 1}},
    {LeftUp, {0, -1, 1, 1, 0, 0, 0, 0, 1}},
    {RightUp, {0, 1, 0, -1, 0, 1, 0, 0, 1}},
  };
  auto orientation_matrix = orientation_matrix_map[orientation];
  qDebug() << "Setting rotation matrix: " << orientation_matrix;
  for(auto device: d->devices) {
    for(auto property: device.properties()) {
      if(property.isRotationMatrix()) {
        qDebug() << "Changing orientation matrix for device " << device.name << ", property " << property.name;
        property.setRotationMatrix(orientation_matrix);
      }
    }
  }
#endif
}
