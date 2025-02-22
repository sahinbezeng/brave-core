/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/common/unittest/command_line_switch_info.h"

#include <utility>

namespace brave_ads {

CommandLineSwitchInfo::CommandLineSwitchInfo() = default;

CommandLineSwitchInfo::CommandLineSwitchInfo(std::string key, std::string value)
    : key(std::move(key)), value(std::move(value)) {}

}  // namespace brave_ads
