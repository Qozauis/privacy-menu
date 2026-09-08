#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;
using namespace cocos2d::extension;

void privacyCCHttpSend(
    CCHttpClient* self,
    CCHttpRequest* request
) {
    if (Mod::get()->getSettingValue<bool>("privacy-lockdown")) {
        return;
    }

    self->send(request);
}

$execute {
    // Geode WebRequest firewall
    web::WebRequestInterceptEvent().listen(
        [](auto, auto&) {
            if (Mod::get()->getSettingValue<bool>("privacy-lockdown")) {
                return ListenerResult::Stop;
            }

            return ListenerResult::Propagate;
        },
        Priority::VeryEarly
    );

    // Geometry Dash / Cocos2D-X HTTP firewall
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
