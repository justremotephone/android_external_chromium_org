// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_INVALIDATION_FAKE_INVALIDATION_SERVICE_H_
#define CHROME_BROWSER_INVALIDATION_FAKE_INVALIDATION_SERVICE_H_

#include <list>
#include <utility>

#include "base/basictypes.h"
#include "base/callback_forward.h"
#include "chrome/browser/invalidation/invalidation_auth_provider.h"
#include "chrome/browser/invalidation/invalidation_service.h"
#include "chrome/browser/signin/fake_profile_oauth2_token_service.h"
#include "sync/notifier/invalidator_registrar.h"
#include "sync/notifier/mock_ack_handler.h"

namespace content {
class BrowserContext;
}

namespace syncer {
class Invalidation;
}

namespace invalidation {

class InvalidationLogger;

// Fake invalidation auth provider implementation.
class FakeInvalidationAuthProvider : public InvalidationAuthProvider {
 public:
  FakeInvalidationAuthProvider();
  virtual ~FakeInvalidationAuthProvider();

  // InvalidationAuthProvider:
  virtual OAuth2TokenService* GetTokenService() OVERRIDE;
  virtual std::string GetAccountId() OVERRIDE;
  virtual bool ShowLoginUI() OVERRIDE;

  FakeProfileOAuth2TokenService* fake_token_service() {
    return &token_service_;
  }

 private:
  FakeProfileOAuth2TokenService token_service_;

  DISALLOW_COPY_AND_ASSIGN(FakeInvalidationAuthProvider);
};

// An InvalidationService that emits invalidations only when
// its EmitInvalidationForTest method is called.
class FakeInvalidationService : public InvalidationService {
 public:
  FakeInvalidationService();
  virtual ~FakeInvalidationService();

  static KeyedService* Build(content::BrowserContext* context);

  virtual void RegisterInvalidationHandler(
      syncer::InvalidationHandler* handler) OVERRIDE;
  virtual void UpdateRegisteredInvalidationIds(
      syncer::InvalidationHandler* handler,
      const syncer::ObjectIdSet& ids) OVERRIDE;
  virtual void UnregisterInvalidationHandler(
      syncer::InvalidationHandler* handler) OVERRIDE;

  virtual syncer::InvalidatorState GetInvalidatorState() const OVERRIDE;
  virtual std::string GetInvalidatorClientId() const OVERRIDE;
  virtual InvalidationLogger* GetInvalidationLogger() OVERRIDE;
  virtual void RequestDetailedStatus(
      base::Callback<void(const base::DictionaryValue&)> caller) OVERRIDE;
  virtual InvalidationAuthProvider* GetInvalidationAuthProvider() OVERRIDE;

  void SetInvalidatorState(syncer::InvalidatorState state);

  const syncer::InvalidatorRegistrar& invalidator_registrar() const {
    return invalidator_registrar_;
  }

  void EmitInvalidationForTest(const syncer::Invalidation& invalidation);

  // Emitted invalidations will be hooked up to this AckHandler.  Clients can
  // query it to assert the invalidaitons are being acked properly.
  syncer::MockAckHandler* GetMockAckHandler();

 private:
  std::string client_id_;
  syncer::InvalidatorRegistrar invalidator_registrar_;
  syncer::MockAckHandler mock_ack_handler_;
  FakeInvalidationAuthProvider auth_provider_;

  DISALLOW_COPY_AND_ASSIGN(FakeInvalidationService);
};

}  // namespace invalidation

#endif  // CHROME_BROWSER_INVALIDATION_FAKE_INVALIDATION_SERVICE_H_
