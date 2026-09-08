#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>
#include <Geode/cocos/extensions/network/HttpClient.h>
#include <Geode/cocos/extensions/network/HttpRequest.h>
#include <Geode/cocos/extensions/network/HttpResponse.h>

using namespace geode::prelude;
using namespace cocos2d;
using namespace cocos2d::extension;

void privacyCCHttpSend(
    CCHttpClient* self,
    CCHttpRequest* request
) {
    // Firewall aus:
    // Request sofort ganz normal senden.
    if (!Mod::get()->getSettingValue<bool>("privacy-lockdown")) {
        self->send(request);
        return;
    }

    // Firewall an:
    // Nicht senden, aber GD sauber mitteilen,
    // dass dieser Request fehlgeschlagen ist.

    auto response = new CCHttpResponse(request);

    response->setSucceed(false);
    response->setResponseCode(0);
    response->setErrorBuffer("Blocked by Privacy Menu firewall");

    auto target = request->_pTarget;
    auto selector = request->_pSelector;

    if (target && selector) {
        (target->*selector)(self, response);
    }

    response->release();
}

$execute {
    // Geode-WebRequests blockieren
    web::WebRequestInterceptEvent().listen(
        [](auto, auto&) {
            if (Mod::get()->getSettingValue<bool>("privacy-lockdown")) {
                return ListenerResult::Stop;
            }

            return ListenerResult::Propagate;
        },
        Priority::VeryEarly
    );

    // Geometry-Dash / Cocos2D HTTP
    Mod::get()->hook(
        reinterpret_cast<void*>(
            geode::addresser::getNonVirtual(
                geode::modifier::Resolve<CCHttpRequest*>::func(
                    &CCHttpClient::send
                )
            )
        ),
        &privacyCCHttpSend,
        "cocos2d::extension::CCHttpClient::send",
        tulip::hook::TulipConvention::Thiscall
    );
}
