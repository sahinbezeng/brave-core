/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "brave/components/brave_ads/core/internal/ads/serving/eligible_ads/pipelines/new_tab_page_ads/eligible_new_tab_page_ads_v1.h"

#include <memory>
#include <utility>

#include "base/functional/bind.h"
#include "brave/components/brave_ads/core/internal/ads/serving/eligible_ads/pacing/pacing_random_util.h"
#include "brave/components/brave_ads/core/internal/ads/serving/targeting/user_model_builder_unittest_util.h"
#include "brave/components/brave_ads/core/internal/ads/serving/targeting/user_model_info.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_base.h"
#include "brave/components/brave_ads/core/internal/common/unittest/unittest_container_util.h"
#include "brave/components/brave_ads/core/internal/creatives/new_tab_page_ads/creative_new_tab_page_ad_unittest_util.h"
#include "brave/components/brave_ads/core/internal/creatives/new_tab_page_ads/creative_new_tab_page_ads_database_util.h"
#include "brave/components/brave_ads/core/internal/creatives/new_tab_page_ads/new_tab_page_ad_builder.h"
#include "brave/components/brave_ads/core/internal/deprecated/client/client_state_manager.h"
#include "brave/components/brave_ads/core/internal/geographic/subdivision_targeting/subdivision_targeting.h"
#include "brave/components/brave_ads/core/internal/resources/behavioral/anti_targeting/anti_targeting_resource.h"
#include "brave/components/brave_ads/core/new_tab_page_ad_info.h"

// npm run test -- brave_unit_tests --filter=BraveAds*

namespace brave_ads {

class BraveAdsEligibleNewTabPageAdsV1Test : public UnitTestBase {
 protected:
  void SetUp() override {
    UnitTestBase::SetUp();

    subdivision_targeting_ = std::make_unique<SubdivisionTargeting>();
    anti_targeting_resource_ = std::make_unique<AntiTargetingResource>();
    eligible_ads_ = std::make_unique<EligibleNewTabPageAdsV1>(
        *subdivision_targeting_, *anti_targeting_resource_);
  }

  std::unique_ptr<SubdivisionTargeting> subdivision_targeting_;
  std::unique_ptr<AntiTargetingResource> anti_targeting_resource_;
  std::unique_ptr<EligibleNewTabPageAdsV1> eligible_ads_;
};

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, GetAdsForChildSegment) {
  // Arrange
  CreativeNewTabPageAdList creative_ads;

  CreativeNewTabPageAdInfo creative_ad_1 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_1.segment = "technology & computing";
  creative_ads.push_back(creative_ad_1);

  CreativeNewTabPageAdInfo creative_ad_2 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_2.segment = "technology & computing-software";
  creative_ads.push_back(creative_ad_2);

  database::SaveCreativeNewTabPageAds(creative_ads);

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad_2};

  eligible_ads_->GetForUserModel(
      BuildUserModel({/*interest_segments*/ "technology & computing-software"},
                     /*latent_interest_segments*/ {},
                     /*purchase_intent_segments*/ {},
                     /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, GetAdsForParentSegment) {
  // Arrange
  CreativeNewTabPageAdInfo creative_ad =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad.segment = "technology & computing";
  database::SaveCreativeNewTabPageAds({creative_ad});

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad};

  eligible_ads_->GetForUserModel(
      BuildUserModel({/*interest_segments*/ "technology & computing-software"},
                     /*latent_interest_segments*/ {},
                     /*purchase_intent_segments*/ {},
                     /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, GetAdsForUntargetedSegment) {
  // Arrange
  CreativeNewTabPageAdInfo creative_ad =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad.segment = "untargeted";
  database::SaveCreativeNewTabPageAds({creative_ad});

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad};

  eligible_ads_->GetForUserModel(
      BuildUserModel({/*interest_segments*/ "finance-banking"},
                     /*latent_interest_segments*/ {},
                     /*purchase_intent_segments*/ {},
                     /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, GetAdsForMultipleSegments) {
  // Arrange
  CreativeNewTabPageAdList creative_ads;

  CreativeNewTabPageAdInfo creative_ad_1 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_1.segment = "technology & computing";
  creative_ads.push_back(creative_ad_1);

  CreativeNewTabPageAdInfo creative_ad_2 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_2.segment = "finance-banking";
  creative_ads.push_back(creative_ad_2);

  CreativeNewTabPageAdInfo creative_ad_3 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_3.segment = "food & drink";
  creative_ads.push_back(creative_ad_3);

  database::SaveCreativeNewTabPageAds(creative_ads);

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad_1,
                                                    creative_ad_3};

  eligible_ads_->GetForUserModel(
      BuildUserModel(
          {/*interest_segments*/ "technology & computing", "food & drink"},
          /*latent_interest_segments*/ {},
          /*purchase_intent_segments*/ {},
          /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_TRUE(ContainersEq(expected_creative_ads, creative_ads));
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, GetAdsForNoSegments) {
  // Arrange
  CreativeNewTabPageAdInfo creative_ad =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad.segment = "untargeted";
  database::SaveCreativeNewTabPageAds({creative_ad});

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad};

  eligible_ads_->GetForUserModel(
      /*user_model*/ {},
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, DoNotGetAdsForUnmatchedSegments) {
  // Arrange
  CreativeNewTabPageAdInfo creative_ad =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad.segment = "technology & computing";
  database::SaveCreativeNewTabPageAds({creative_ad});

  // Act
  eligible_ads_->GetForUserModel(
      BuildUserModel({/*interest_segments*/ "UNMATCHED"},
                     /*latent_interest_segments*/ {},
                     /*purchase_intent_segments*/ {},
                     /*text_embedding_html_events*/ {}),
      base::BindOnce([](const bool had_opportunity,
                        const CreativeNewTabPageAdList& creative_ads) {
        // Assert
        EXPECT_FALSE(had_opportunity);
        EXPECT_TRUE(creative_ads.empty());
      }));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, DoNotGetAdsIfNoEligibleAds) {
  // Arrange

  // Act
  eligible_ads_->GetForUserModel(
      BuildUserModel(
          {/*interest_segments*/ "technology & computing", "food & drink"},
          /*latent_interest_segments*/ {},
          /*purchase_intent_segments*/ {},
          /*text_embedding_html_events*/ {}),
      base::BindOnce([](const bool had_opportunity,
                        const CreativeNewTabPageAdList& creative_ads) {
        // Assert
        EXPECT_FALSE(had_opportunity);
        EXPECT_TRUE(creative_ads.empty());
      }));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, DoNotGetAdsIfAlreadySeen) {
  // Arrange
  CreativeNewTabPageAdList creative_ads;

  CreativeNewTabPageAdInfo creative_ad_1 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_1.segment = "technology & computing";
  creative_ads.push_back(creative_ad_1);

  CreativeNewTabPageAdInfo creative_ad_2 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_2.segment = "food & drink";
  creative_ads.push_back(creative_ad_2);

  database::SaveCreativeNewTabPageAds(creative_ads);

  const NewTabPageAdInfo ad = BuildNewTabPageAd(creative_ad_1);
  ClientStateManager::GetInstance().UpdateSeenAd(ad);

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad_2};

  eligible_ads_->GetForUserModel(
      BuildUserModel(
          {/*interest_segments*/ "technology & computing", "food & drink"},
          /*latent_interest_segments*/ {},
          /*purchase_intent_segments*/ {},
          /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, DoNotGetPacedAds) {
  // Arrange
  CreativeNewTabPageAdList creative_ads;

  CreativeNewTabPageAdInfo creative_ad_1 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_1.segment = "technology & computing";
  creative_ad_1.ptr = 0.1;
  creative_ads.push_back(creative_ad_1);

  CreativeNewTabPageAdInfo creative_ad_2 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_2.segment = "food & drink";
  creative_ad_2.ptr = 0.5;
  creative_ads.push_back(creative_ad_2);

  database::SaveCreativeNewTabPageAds(creative_ads);

  // Act
  const ScopedPacingRandomNumberSetterForTesting scoped_setter(0.3);

  CreativeNewTabPageAdList expected_creative_ads = {creative_ad_2};

  eligible_ads_->GetForUserModel(
      BuildUserModel(
          {/*interest_segments*/ "technology & computing", "food & drink"},
          /*latent_interest_segments*/ {},
          /*purchase_intent_segments*/ {},
          /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

TEST_F(BraveAdsEligibleNewTabPageAdsV1Test, GetPrioritizedAds) {
  // Arrange
  CreativeNewTabPageAdList creative_ads;

  CreativeNewTabPageAdInfo creative_ad_1 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_1.segment = "technology & computing";
  creative_ad_1.priority = 1;
  creative_ads.push_back(creative_ad_1);

  CreativeNewTabPageAdInfo creative_ad_2 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_2.segment = "finance-banking";
  creative_ad_2.priority = 1;
  creative_ads.push_back(creative_ad_2);

  CreativeNewTabPageAdInfo creative_ad_3 =
      BuildCreativeNewTabPageAd(/*should_use_random_guids*/ true);
  creative_ad_3.segment = "food & drink";
  creative_ad_3.priority = 2;
  creative_ads.push_back(creative_ad_3);

  database::SaveCreativeNewTabPageAds(creative_ads);

  // Act
  CreativeNewTabPageAdList expected_creative_ads = {creative_ad_1};

  eligible_ads_->GetForUserModel(
      BuildUserModel(
          {/*interest_segments*/ "technology & computing", "food & drink"},
          /*latent_interest_segments*/ {},
          /*purchase_intent_segments*/ {},
          /*text_embedding_html_events*/ {}),
      base::BindOnce(
          [](const CreativeNewTabPageAdList& expected_creative_ads,
             const bool had_opportunity,
             const CreativeNewTabPageAdList& creative_ads) {
            // Assert
            EXPECT_TRUE(had_opportunity);
            EXPECT_EQ(expected_creative_ads, creative_ads);
          },
          std::move(expected_creative_ads)));
}

}  // namespace brave_ads
