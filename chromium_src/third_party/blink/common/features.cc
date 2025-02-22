/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "src/third_party/blink/common/features.cc"

#include "base/feature_override.h"

namespace blink {
namespace features {

OVERRIDE_FEATURE_DEFAULT_STATES({{
    // Upgrade all mixed content
    {kMixedContentAutoupgrade, base::FEATURE_ENABLED_BY_DEFAULT},
    {kPrefetchPrivacyChanges, base::FEATURE_ENABLED_BY_DEFAULT},
    {kReducedReferrerGranularity, base::FEATURE_ENABLED_BY_DEFAULT},

    {kAdInterestGroupAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAllowURNsInIframes, base::FEATURE_DISABLED_BY_DEFAULT},
    {kAnonymousIframeOriginTrial, base::FEATURE_DISABLED_BY_DEFAULT},
    {kBackgroundResourceFetch, base::FEATURE_DISABLED_BY_DEFAULT},
    {kBrowsingTopics, base::FEATURE_DISABLED_BY_DEFAULT},
    {kBrowsingTopicsXHR, base::FEATURE_DISABLED_BY_DEFAULT},
    {kClientHintsMetaEquivDelegateCH, base::FEATURE_DISABLED_BY_DEFAULT},
    {kComputePressure, base::FEATURE_DISABLED_BY_DEFAULT},
    {kConversionMeasurement, base::FEATURE_DISABLED_BY_DEFAULT},
    {kCssSelectorFragmentAnchor, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFencedFrames, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFledge, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFledgeConsiderKAnonymity, base::FEATURE_DISABLED_BY_DEFAULT},
    {kFledgeEnforceKAnonymity, base::FEATURE_DISABLED_BY_DEFAULT},
    {kInterestGroupStorage, base::FEATURE_DISABLED_BY_DEFAULT},
    {kParakeet, base::FEATURE_DISABLED_BY_DEFAULT},
    {kPrerender2, base::FEATURE_DISABLED_BY_DEFAULT},
    {kPrivacySandboxAdsAPIs, base::FEATURE_DISABLED_BY_DEFAULT},
    {kPrivateAggregationApi, base::FEATURE_DISABLED_BY_DEFAULT},
    {kReduceUserAgentMinorVersion, base::FEATURE_ENABLED_BY_DEFAULT},
    {kSharedStorageAPI, base::FEATURE_DISABLED_BY_DEFAULT},
    {kSharedStorageSelectURLLimit, base::FEATURE_DISABLED_BY_DEFAULT},
    {kSharedStorageReportEventLimit, base::FEATURE_DISABLED_BY_DEFAULT},
    {kSpeculationRulesHeaderEnableThirdPartyOriginTrial,
     base::FEATURE_DISABLED_BY_DEFAULT},
    {kSpeculationRulesPrefetchFuture, base::FEATURE_DISABLED_BY_DEFAULT},
    {kSpeculationRulesPrefetchProxy, base::FEATURE_DISABLED_BY_DEFAULT},
    {kTextFragmentAnchor, base::FEATURE_DISABLED_BY_DEFAULT},
}});

// Allow certain client hints in request header.
BASE_FEATURE(kAllowCertainClientHints,
             "AllowCertainClientHints",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Clamp platform version client hint's patch field.
// Platform version client hint is only sent when requested and when the
// AllowCertainClientHints feature is turned on.
BASE_FEATURE(kClampPlatformVersionClientHint,
             "ClampPlatformVersionClientHint",
             base::FEATURE_ENABLED_BY_DEFAULT);

constexpr base::FeatureParam<std::string>
    kClampPlatformVersionClientHintPatchValue{&kClampPlatformVersionClientHint,
                                              "patch_value", "0"};

BASE_FEATURE(kFileSystemAccessAPI,
             "FileSystemAccessAPI",
             base::FEATURE_DISABLED_BY_DEFAULT);

BASE_FEATURE(kNavigatorConnectionAttribute,
             "NavigatorConnectionAttribute",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enable blink::MemoryCache partitioning for non SameSite requests.
BASE_FEATURE(kPartitionBlinkMemoryCache,
             "PartitionBlinkMemoryCache",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enable WebSockets connection pool limit per eTLD+1 for each renderer.
BASE_FEATURE(kRestrictWebSocketsPool,
             "RestrictWebSocketsPool",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enables protection against fingerprinting on screen dimensions.
BASE_FEATURE(kBraveBlockScreenFingerprinting,
             "BraveBlockScreenFingerprinting",
             base::FEATURE_DISABLED_BY_DEFAULT);

// Enables protection against fingerprinting via high-resolution time stamps.
BASE_FEATURE(kBraveRoundTimeStamps,
             "BraveRoundTimeStamps",
             base::FEATURE_ENABLED_BY_DEFAULT);

// Enable EventSource connection pool limit per eTLD+1.
BASE_FEATURE(kRestrictEventSourcePool,
             "RestrictEventSourcePool",
#if BUILDFLAG(IS_ANDROID)
             base::FEATURE_DISABLED_BY_DEFAULT
#else
             base::FEATURE_ENABLED_BY_DEFAULT
#endif
);

bool IsPrerender2Enabled() {
  return false;
}

}  // namespace features
}  // namespace blink
