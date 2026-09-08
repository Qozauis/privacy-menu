#include <Geode/Geode.hpp>
#include <Geode/utils/web.hpp>

using namespace geode::prelude;

$execute {
    web::WebRequestInterceptEvent().listen(
        [](auto, auto&) {
            if (
                Mod::get()->getSettingValue<bool>("privacy-lockdown")
            ) {
                return ListenerResult::Stop;
            }

            return ListenerResult::Propagate;
        },
        Priority::VeryEarly
    );
}
