/* Copyright (c) 2023 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/. */

#ifndef BRAVE_BROWSER_BRAVE_WALLET_BITCOIN_WALLET_SERVICE_FACTORY_H_
#define BRAVE_BROWSER_BRAVE_WALLET_BITCOIN_WALLET_SERVICE_FACTORY_H_

#include "base/memory/singleton.h"
#include "brave/components/brave_wallet/common/brave_wallet.mojom.h"
#include "components/keyed_service/content/browser_context_keyed_service_factory.h"
#include "components/keyed_service/core/keyed_service.h"
#include "content/public/browser/browser_context.h"
#include "mojo/public/cpp/bindings/pending_receiver.h"
#include "mojo/public/cpp/bindings/pending_remote.h"

namespace brave_wallet {

class BitcoinWalletService;

class BitcoinWalletServiceFactory : public BrowserContextKeyedServiceFactory {
 public:
  BitcoinWalletServiceFactory(const BitcoinWalletServiceFactory&) = delete;
  BitcoinWalletServiceFactory& operator=(const BitcoinWalletServiceFactory&) =
      delete;

  static mojo::PendingRemote<mojom::BitcoinWalletService> GetForContext(
      content::BrowserContext* context);
  static BitcoinWalletService* GetServiceForContext(
      content::BrowserContext* context);
  static BitcoinWalletServiceFactory* GetInstance();
  static void BindForContext(
      content::BrowserContext* context,
      mojo::PendingReceiver<mojom::BitcoinWalletService> receiver);

 private:
  friend struct base::DefaultSingletonTraits<BitcoinWalletServiceFactory>;

  BitcoinWalletServiceFactory();
  ~BitcoinWalletServiceFactory() override;

  KeyedService* BuildServiceInstanceFor(
      content::BrowserContext* context) const override;
  content::BrowserContext* GetBrowserContextToUse(
      content::BrowserContext* context) const override;
};

}  // namespace brave_wallet

#endif  // BRAVE_BROWSER_BRAVE_WALLET_BITCOIN_WALLET_SERVICE_FACTORY_H_
