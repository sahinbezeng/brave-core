/* Copyright (c) 2019 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_ADS_SERVICE_IMPL_H_
#define BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_ADS_SERVICE_IMPL_H_

#include <cstdint>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "base/memory/raw_ptr.h"
#include "base/memory/weak_ptr.h"
#include "base/task/cancelable_task_tracker.h"
#include "base/time/time.h"
#include "base/timer/timer.h"
#include "brave/browser/brave_ads/background_helper/background_helper.h"
#include "brave/components/brave_adaptive_captcha/brave_adaptive_captcha_service.h"
#include "brave/components/brave_ads/browser/ads_service.h"
#include "brave/components/brave_ads/browser/component_updater/resource_component_observer.h"
#include "brave/components/brave_ads/common/interfaces/brave_ads.mojom.h"
#include "brave/components/brave_rewards/common/mojom/ledger.mojom-forward.h"
#include "brave/components/services/bat_ads/public/interfaces/bat_ads.mojom.h"
#include "components/history/core/browser/history_service.h"
#include "components/prefs/pref_change_registrar.h"
#include "mojo/public/cpp/bindings/associated_receiver.h"
#include "mojo/public/cpp/bindings/associated_remote.h"
#include "mojo/public/cpp/bindings/remote.h"
#include "third_party/abseil-cpp/absl/types/optional.h"
#include "ui/base/idle/idle.h"

class GURL;
class NotificationDisplayService;
class PrefService;
class Profile;

namespace base {
class OneShotTimer;
class SequencedTaskRunner;
}  // namespace base

namespace brave_federated {
class AsyncDataStore;
}  // namespace brave_federated

namespace brave_rewards {
class RewardsService;
}  // namespace brave_rewards

namespace network {
class SimpleURLLoader;
}  // namespace network

namespace brave_ads {

class AdsTooltipsDelegate;
class BatAdsServiceFactory;
class Database;
class DeviceId;
struct NewTabPageAdInfo;

class AdsServiceImpl : public AdsService,
                       public bat_ads::mojom::BatAdsClient,
                       BackgroundHelper::Observer,
                       public ResourceComponentObserver,
                       public brave_rewards::RewardsServiceObserver,
                       public base::SupportsWeakPtr<AdsServiceImpl> {
 public:
  explicit AdsServiceImpl(
      Profile* profile,
      brave_adaptive_captcha::BraveAdaptiveCaptchaService*
          adaptive_captcha_service,
      std::unique_ptr<AdsTooltipsDelegate> ads_tooltips_delegate,
      std::unique_ptr<DeviceId> device_id,
      std::unique_ptr<BatAdsServiceFactory> bat_ads_service_factory,
      history::HistoryService* history_service,
      brave_rewards::RewardsService* rewards_service,
      brave_federated::AsyncDataStore* notification_ad_timing_data_store);

  AdsServiceImpl(const AdsServiceImpl&) = delete;
  AdsServiceImpl& operator=(const AdsServiceImpl&) = delete;

  AdsServiceImpl(AdsServiceImpl&&) noexcept = delete;
  AdsServiceImpl& operator=(AdsServiceImpl&&) noexcept = delete;

  ~AdsServiceImpl() override;

 private:
  using SimpleURLLoaderList =
      std::list<std::unique_ptr<network::SimpleURLLoader>>;

  void InitializeNotificationsForCurrentProfile() const;

  void MigrateConfirmationState();
  void OnMigrateConfirmationState(bool success);
  void GetDeviceId();
  void OnGetDeviceId(std::string device_id);

  bool UserHasOptedIn() const;

  bool CanStartBatAdsService() const;
  void MaybeStartBatAdsService();
  void StartBatAdsService();
  void RestartBatAdsServiceAfterDelay();
  void CancelRestartBatAdsService();
  bool ShouldProceedInitialization(size_t current_start_number) const;

  void OnBatAdsServiceCreated(size_t current_start_number);
  void OnInitializeBasePathDirectory(size_t current_start_number, bool success);
  void Initialize(size_t current_start_number);
  void InitializeDatabase();

  bool ShouldRewardUser() const;
  void InitializeRewardsWallet(size_t current_start_number);
  void OnInitializeRewardsWallet(size_t current_start_number,
                                 brave_rewards::mojom::RewardsWalletPtr wallet);
  void InitializeBatAds();
  void OnInitializeBatAds(bool success);

  void ShutdownAndResetState();

  void SetSysInfo();
  void SetBuildChannel();
  void SetFlags();

  void CleanUpOnFirstRun();
  void RemoveDeprecatedFiles() const;

  bool ShouldShowOnboardingNotification();
  void MaybeShowOnboardingNotification();

  void CloseAdaptiveCaptcha();

  void InitializePrefChangeRegistrar();
  void OnEnabledPrefChanged();
  void OnIdleTimeThresholdPrefChanged();
  void OnBraveNewsOptedInPrefChanged();
  void OnNewTabPageShowTodayPrefChanged();
  void NotifyPrefChanged(const std::string& path) const;

  void GetRewardsWallet();
  void OnGetRewardsWallet(brave_rewards::mojom::RewardsWalletPtr wallet);

  // TODO(https://github.com/brave/brave-browser/issues/14666) Decouple idle
  // state business logic.
  void CheckIdleStateAfterDelay();
  void CheckIdleState();
  void ProcessIdleState(ui::IdleState idle_state, base::TimeDelta idle_time);

  // TODO(https://github.com/brave/brave-browser/issues/23974) Decouple
  // notification ad business logic.
  bool CheckIfCanShowNotificationAds();
  bool ShouldShowCustomNotificationAds();
  void StartNotificationAdTimeOutTimer(const std::string& placement_id);
  bool StopNotificationAdTimeOutTimer(const std::string& placement_id);
  void NotificationAdTimedOut(const std::string& placement_id);
  void CloseAllNotificationAds();

  // TODO(https://github.com/brave/brave-browser/issues/26192) Decouple new
  // tab page ad business logic.
  void OnPrefetchNewTabPageAd(absl::optional<base::Value::Dict> dict);

  // TODO(https://github.com/brave/brave-browser/issues/26193) Decouple open
  // new tab with ad business logic.
  void MaybeOpenNewTabWithAd();
  void OpenNewTabWithAd(const std::string& placement_id);
  void OnOpenNewTabWithAd(absl::optional<base::Value::Dict> dict);
  void RetryOpeningNewTabWithAd(const std::string& placement_id);

  void OpenNewTabWithUrl(const GURL& url);

  // TODO(https://github.com/brave/brave-browser/issues/14676) Decouple URL
  // request business logic.
  void OnURLRequest(SimpleURLLoaderList::iterator url_loader_iter,
                    UrlRequestCallback callback,
                    std::unique_ptr<std::string> response_body);

  PrefService* GetPrefService();
  const PrefService* GetPrefService() const;

  // TODO(https://github.com/brave/brave-browser/issues/14673) Decouple
  // migration business logic.
  bool IsUpgradingFromPreBraveAdsBuild();
  void MigratePrefs();
  bool MigratePrefs(int source_version,
                    int dest_version,
                    bool is_dry_run = false);

  void DisableAdsIfUpgradingFromPreBraveAdsBuild();
  void DisableAdsIfUnsupportedRegion();
  void MigratePrefsVersion1To2();
  void MigratePrefsVersion2To3();
  void MigratePrefsVersion3To4();
  void MigratePrefsVersion4To5();
  void MigratePrefsVersion5To6();
  void MigratePrefsVersion6To7();
  void MigratePrefsVersion7To8();
  void MigratePrefsVersion8To9();
  void MigratePrefsVersion9To10();
  void MigratePrefsVersion10To11();
  void MigratePrefsVersion11To12();

  // KeyedService:
  void Shutdown() override;

  // AdsService:
  bool IsEnabled() const override;
  void SetEnabled(bool is_enabled) override;

  int64_t GetMaximumNotificationAdsPerHour() const override;
  void SetMaximumNotificationAdsPerHour(int64_t ads_per_hour) override;

  bool ShouldAllowSubdivisionTargeting() const override;
  std::string GetSubdivisionTargetingCode() const override;
  void SetSubdivisionTargetingCode(
      const std::string& subdivision_targeting_code) override;
  std::string GetAutoDetectedSubdivisionTargetingCode() const override;
  void SetAutoDetectedSubdivisionTargetingCode(
      const std::string& subdivision_targeting_code) override;

  bool NeedsBrowserUpgradeToServeAds() const override;

  void ShowScheduledCaptcha(const std::string& payment_id,
                            const std::string& captcha_id) override;
  void SnoozeScheduledCaptcha() override;

  void OnNotificationAdShown(const std::string& placement_id) override;
  void OnNotificationAdClosed(const std::string& placement_id,
                              bool by_user) override;
  void OnNotificationAdClicked(const std::string& placement_id) override;

  void GetDiagnostics(GetDiagnosticsCallback callback) override;

  void GetStatementOfAccounts(GetStatementOfAccountsCallback callback) override;

  void MaybeServeInlineContentAd(
      const std::string& dimensions,
      MaybeServeInlineContentAdAsDictCallback callback) override;
  void TriggerInlineContentAdEvent(
      const std::string& placement_id,
      const std::string& creative_instance_id,
      mojom::InlineContentAdEventType event_type) override;

  absl::optional<NewTabPageAdInfo> GetPrefetchedNewTabPageAdForDisplay()
      override;
  void PrefetchNewTabPageAd() override;
  void OnFailedToPrefetchNewTabPageAd(
      const std::string& placement_id,
      const std::string& creative_instance_id) override;
  void TriggerNewTabPageAdEvent(
      const std::string& placement_id,
      const std::string& creative_instance_id,
      mojom::NewTabPageAdEventType event_type) override;

  void TriggerPromotedContentAdEvent(
      const std::string& placement_id,
      const std::string& creative_instance_id,
      mojom::PromotedContentAdEventType event_type) override;

  void TriggerSearchResultAdEvent(
      mojom::SearchResultAdInfoPtr ad_mojom,
      mojom::SearchResultAdEventType event_type) override;

  void PurgeOrphanedAdEventsForType(
      mojom::AdType ad_type,
      PurgeOrphanedAdEventsForTypeCallback callback) override;

  void GetHistory(base::Time from_time,
                  base::Time to_time,
                  GetHistoryCallback callback) override;

  void ToggleLikeAd(base::Value::Dict value,
                    ToggleLikeAdCallback callback) override;
  void ToggleDislikeAd(base::Value::Dict value,
                       ToggleDislikeAdCallback callback) override;
  void ToggleLikeCategory(const std::string& category,
                          mojom::UserReactionType user_reaction_type,
                          ToggleLikeCategoryCallback callback) override;
  void ToggleDislikeCategory(const std::string& category,
                             mojom::UserReactionType user_reaction_type,
                             ToggleDislikeCategoryCallback callback) override;
  void ToggleSaveAd(base::Value::Dict value,
                    ToggleSaveAdCallback callback) override;
  void ToggleMarkAdAsInappropriate(
      base::Value::Dict value,
      ToggleMarkAdAsInappropriateCallback callback) override;

  void NotifyTabTextContentDidChange(int32_t tab_id,
                                     const std::vector<GURL>& redirect_chain,
                                     const std::string& text) override;
  void NotifyTabHtmlContentDidChange(int32_t tab_id,
                                     const std::vector<GURL>& redirect_chain,
                                     const std::string& html) override;
  void NotifyTabDidStartPlayingMedia(int32_t tab_id) override;
  void NotifyTabDidStopPlayingMedia(int32_t tab_id) override;
  void NotifyTabDidChange(int32_t tab_id,
                          const std::vector<GURL>& redirect_chain,
                          bool is_visible,
                          bool is_incognito) override;
  void NotifyDidCloseTab(int32_t tab_id) override;

  void NotifyUserGestureEventTriggered(int32_t page_transition_type) override;

  void NotifyBrowserDidBecomeActive() override;
  void NotifyBrowserDidResignActive() override;

  // bat_ads::mojom::BatAdsClient:
  void IsNetworkConnectionAvailable(
      IsNetworkConnectionAvailableCallback callback) override;

  void IsBrowserActive(IsBrowserActiveCallback callback) override;
  void IsBrowserInFullScreenMode(
      IsBrowserInFullScreenModeCallback callback) override;

  void CanShowNotificationAds(CanShowNotificationAdsCallback callback) override;
  void CanShowNotificationAdsWhileBrowserIsBackgrounded(
      CanShowNotificationAdsWhileBrowserIsBackgroundedCallback callback)
      override;
  void ShowNotificationAd(base::Value::Dict dict) override;
  void CloseNotificationAd(const std::string& placement_id) override;

  void ShowReminder(mojom::ReminderType type) override;

  void UpdateAdRewards() override;

  void RecordAdEventForId(const std::string& id,
                          const std::string& type,
                          const std::string& confirmation_type,
                          base::Time time) override;
  void GetAdEventHistory(const std::string& ad_type,
                         const std::string& confirmation_type,
                         GetAdEventHistoryCallback callback) override;
  void ResetAdEventHistoryForId(const std::string& id) override;

  void GetBrowsingHistory(int max_count,
                          int days_ago,
                          GetBrowsingHistoryCallback callback) override;

  // TODO(https://github.com/brave/brave-browser/issues/14676) Decouple URL
  // request business logic.
  void UrlRequest(mojom::UrlRequestInfoPtr url_request,
                  UrlRequestCallback callback) override;

  // TODO(https://github.com/brave/brave-browser/issues/26194) Decouple
  // load/save file business logic.
  void Save(const std::string& name,
            const std::string& value,
            SaveCallback callback) override;
  void Load(const std::string& name, LoadCallback callback) override;

  // TODO(https://github.com/brave/brave-browser/issues/26195) Decouple load
  // resources business logic.
  void LoadFileResource(const std::string& id,
                        int version,
                        LoadFileResourceCallback callback) override;
  void LoadDataResource(const std::string& name,
                        LoadDataResourceCallback callback) override;

  void GetScheduledCaptcha(const std::string& payment_id,
                           GetScheduledCaptchaCallback callback) override;
  void ShowScheduledCaptchaNotification(const std::string& payment_id,
                                        const std::string& captcha_id) override;
  void ClearScheduledCaptcha() override;

  void RunDBTransaction(mojom::DBTransactionInfoPtr transaction,
                        RunDBTransactionCallback callback) override;

  // TODO(https://github.com/brave/brave-browser/issues/14666) Decouple P2A
  // business logic.
  void RecordP2AEvent(const std::string& name, base::Value::List list) override;

  void AddTrainingSample(std::vector<brave_federated::mojom::CovariateInfoPtr>
                             training_sample) override;

  void GetBooleanPref(const std::string& path,
                      GetBooleanPrefCallback callback) override;
  void GetIntegerPref(const std::string& path,
                      GetIntegerPrefCallback callback) override;
  void GetDoublePref(const std::string& path,
                     GetDoublePrefCallback callback) override;
  void GetStringPref(const std::string& path,
                     GetStringPrefCallback callback) override;
  void GetInt64Pref(const std::string& path,
                    GetInt64PrefCallback callback) override;
  void GetUint64Pref(const std::string& path,
                     GetUint64PrefCallback callback) override;
  void GetTimePref(const std::string& path,
                   GetTimePrefCallback callback) override;
  void GetDictPref(const std::string& path,
                   GetDictPrefCallback callback) override;
  void GetListPref(const std::string& path,
                   GetListPrefCallback callback) override;
  void HasPrefPath(const std::string& path,
                   HasPrefPathCallback callback) override;

  void SetBooleanPref(const std::string& path, bool value) override;
  void SetIntegerPref(const std::string& path, int value) override;
  void SetDoublePref(const std::string& path, double value) override;
  void SetStringPref(const std::string& path,
                     const std::string& value) override;
  void SetInt64Pref(const std::string& path, int64_t value) override;
  void SetUint64Pref(const std::string& path, uint64_t value) override;
  void SetTimePref(const std::string& path, base::Time value) override;
  void SetDictPref(const std::string& path, base::Value::Dict value) override;
  void SetListPref(const std::string& path, base::Value::List value) override;

  void ClearPref(const std::string& path) override;

  void Log(const std::string& file,
           int32_t line,
           int32_t verbose_level,
           const std::string& message) override;

  // BackgroundHelper::Observer:
  void OnBrowserDidEnterForeground() override;
  void OnBrowserDidEnterBackground() override;

  // ResourceComponentObserver:
  void OnDidUpdateResourceComponent(const std::string& id) override;

  // RewardsServiceObserver:
  void OnRewardsWalletUpdated() override;
  void OnExternalWalletConnected() override;
  void OnCompleteReset(bool success) override;

  bool is_bat_ads_initialized_ = false;
  bool did_cleanup_on_first_run_ = false;
  bool needs_browser_upgrade_to_serve_ads_ = false;
  bool is_upgrading_from_pre_brave_ads_build_ = false;
  // Brave Ads Service starts count is needed to avoid possible double Brave Ads
  // initialization.
  // TODO(https://github.com/brave/brave-browser/issues/30247): Refactor Brave
  // Ads startup logic.
  size_t service_starts_count_ = 0;

  PrefChangeRegistrar pref_change_registrar_;

  base::OneShotTimer restart_bat_ads_service_timer_;

  mojom::SysInfo sys_info_;

  std::unique_ptr<Database> database_;

  base::RepeatingTimer idle_state_timer_;
  ui::IdleState last_idle_state_ = ui::IdleState::IDLE_STATE_ACTIVE;
  base::TimeDelta last_idle_time_;

  std::map<std::string, std::unique_ptr<base::OneShotTimer>>
      notification_ad_timers_;

  absl::optional<NewTabPageAdInfo> prefetched_new_tab_page_ad_;
  bool is_prefetching_new_tab_page_ad_ = false;

  std::string retry_opening_new_tab_for_ad_with_placement_id_;

  base::CancelableTaskTracker history_service_task_tracker_;

  SimpleURLLoaderList url_loaders_;

  const raw_ptr<Profile> profile_ = nullptr;  // NOT OWNED

  const raw_ptr<history::HistoryService> history_service_ =
      nullptr;  // NOT OWNED

  const raw_ptr<brave_adaptive_captcha::BraveAdaptiveCaptchaService>
      adaptive_captcha_service_ = nullptr;  // NOT OWNED
  const std::unique_ptr<AdsTooltipsDelegate> ads_tooltips_delegate_;

  const std::unique_ptr<DeviceId> device_id_;

  const std::unique_ptr<BatAdsServiceFactory> bat_ads_service_factory_;

  const scoped_refptr<base::SequencedTaskRunner> file_task_runner_;

  const base::FilePath base_path_;

  const raw_ptr<NotificationDisplayService> display_service_ =
      nullptr;  // NOT OWNED
  const raw_ptr<brave_rewards::RewardsService> rewards_service_{
      nullptr};  // NOT OWNED

  const raw_ptr<brave_federated::AsyncDataStore>
      notification_ad_timing_data_store_ = nullptr;  // NOT OWNED

  mojo::Remote<bat_ads::mojom::BatAdsService> bat_ads_service_;
  mojo::AssociatedReceiver<bat_ads::mojom::BatAdsClient> bat_ads_client_;
  mojo::AssociatedRemote<bat_ads::mojom::BatAds> bat_ads_;
  mojo::Remote<bat_ads::mojom::BatAdsClientNotifier> bat_ads_client_notifier_;
};

}  // namespace brave_ads

#endif  // BRAVE_COMPONENTS_BRAVE_ADS_BROWSER_ADS_SERVICE_IMPL_H_
