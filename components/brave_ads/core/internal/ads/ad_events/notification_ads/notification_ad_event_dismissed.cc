/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/ads/ad_events/notification_ads/notification_ad_event_dismissed.h"

#include "base/functional/bind.h"
#include "brave/components/brave_ads/core/confirmation_type.h"
#include "brave/components/brave_ads/core/internal/ads/ad_events/ad_events.h"
#include "brave/components/brave_ads/core/internal/common/logging_util.h"
#include "brave/components/brave_ads/core/notification_ad_info.h"

namespace brave_ads {

void NotificationAdEventDismissed::FireEvent(const NotificationAdInfo& ad) {
  BLOG(3, "Dismissed notification ad with placement id "
              << ad.placement_id << " and creative instance id "
              << ad.creative_instance_id);

  LogAdEvent(ad, ConfirmationType::kDismissed,
             base::BindOnce([](const bool success) {
               if (!success) {
                 BLOG(1, "Failed to log notification ad dismissed event");
                 return;
               }

               BLOG(6, "Successfully logged notification ad dismissed event");
             }));
}

}  // namespace brave_ads
