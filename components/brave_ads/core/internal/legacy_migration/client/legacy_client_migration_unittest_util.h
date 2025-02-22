/* Copyright (c) 2022 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_LEGACY_MIGRATION_CLIENT_LEGACY_CLIENT_MIGRATION_UNITTEST_UTIL_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_LEGACY_MIGRATION_CLIENT_LEGACY_CLIENT_MIGRATION_UNITTEST_UTIL_H_

#include <cstdint>

namespace brave_ads::client {

void Migrate(bool should_migrate);

uint64_t GetHash();
void SetHash(uint64_t hash);

}  // namespace brave_ads::client

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_CORE_INTERNAL_LEGACY_MIGRATION_CLIENT_LEGACY_CLIENT_MIGRATION_UNITTEST_UTIL_H_
