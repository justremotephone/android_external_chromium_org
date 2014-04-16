// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "device/bluetooth/bluetooth_uuid.h"

#include "base/basictypes.h"
#include "base/logging.h"
#include "base/strings/string_util.h"

namespace device {

namespace {

const char* kCommonUuidPostfix = "-0000-1000-8000-00805f9b34fb";
const char* kCommonUuidPrefix = "0000";
const int kUuidSize = 36;

// Returns the canonical, 128-bit canonical, and the format of the UUID
// in |canonical|, |canonical_128|, and |format| based on |uuid|.
void GetCanonicalUuid(std::string uuid,
                      std::string* canonical,
                      std::string* canonical_128,
                      BluetoothUUID::Format* format) {
  // Initialize the values for the failure case.
  canonical->clear();
  canonical_128->clear();
  *format = BluetoothUUID::kFormatInvalid;

  if (uuid.empty())
    return;

  if (uuid.size() < 11 && uuid.find("0x") == 0)
    uuid = uuid.substr(2);

  if (!(uuid.size() == 4 || uuid.size() == 8 || uuid.size() == 36))
    return;

  if (uuid.size() == 4 || uuid.size() == 8) {
    for (size_t i = 0; i < uuid.size(); ++i) {
      if (!IsHexDigit(uuid[i]))
        return;
    }
    if (uuid.size() == 4) {
      canonical->assign(uuid);
      canonical_128->assign(kCommonUuidPrefix + uuid + kCommonUuidPostfix);
      *format = BluetoothUUID::kFormat16Bit;
      return;
    }
    canonical->assign(uuid);
    canonical_128->assign(uuid + kCommonUuidPostfix);
    *format = BluetoothUUID::kFormat32Bit;
    return;
  }

  for (int i = 0; i < kUuidSize; ++i) {
    if (i == 8 || i == 13 || i == 18 || i == 23) {
      if (uuid[i] != '-')
        return;
    } else {
      if (!IsHexDigit(uuid[i]))
        return;
      uuid[i] = tolower(uuid[i]);
    }
  }

  canonical->assign(uuid);
  canonical_128->assign(uuid);
  *format = BluetoothUUID::kFormat128Bit;
}

}  // namespace


BluetoothUUID::BluetoothUUID(const std::string& uuid) {
  GetCanonicalUuid(uuid, &value_, &canonical_value_, &format_);
}

BluetoothUUID::BluetoothUUID() : format_(kFormatInvalid) {
}

BluetoothUUID::~BluetoothUUID() {
}

bool BluetoothUUID::IsValid() const {
  return format_ != kFormatInvalid;
}

bool BluetoothUUID::operator<(const BluetoothUUID& uuid) const {
  return canonical_value_ < uuid.canonical_value_;
}

bool BluetoothUUID::operator==(const BluetoothUUID& uuid) const {
  return canonical_value_ == uuid.canonical_value_;
}

bool BluetoothUUID::operator!=(const BluetoothUUID& uuid) const {
  return canonical_value_ != uuid.canonical_value_;
}

void PrintTo(const BluetoothUUID& uuid, std::ostream* out) {
  *out << uuid.canonical_value();
}

}  // namespace device
