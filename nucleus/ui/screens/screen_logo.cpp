/**
 * (c) 2014-2016 Alexandro Sanchez Bach. All rights reserved.
 * Released under GPL v2 license. Read LICENSE for more details.
 */

#include "screen_logo.h"
#include "nucleus/ui/ui.h"
#include "nucleus/ui/screens/list.h"
#include "nucleus/ui/widgets/list.h"
#include "nucleus/ui/transitions.h"
#include "nucleus/ui/length.h"

namespace ui {

ScreenLogo::ScreenLogo(UI* manager) : Screen(manager) {
    const auto* defaultFont = manager->fontRegular.get();
    const auto* lightFont = manager->fontLight.get();

    // Body
    body.style.height = 100.0_pct;
    body.style.width = 100.0_pct;
    body.style.alignH = ALIGN_HORIZONTAL_CENTER;
    body.style.alignV = ALIGN_VERTICAL_CENTER;
    body.style.background = {0.157f, 0.157f, 0.360f, 1.0f};

    // Create widgets
    logo = new WidgetImage("logo");
    logo->manager = manager;
    logo->update(core::RES_IMAGE_NUCLEUS_LOGO);
    logo->style.width = 50_pct;

    version = new WidgetText("version");
    version->manager = manager;
    version->update(lightFont, 30_px, "Version v0.1.0 from 2016-04-01");
    version->style.margin.top = 0_px;
    version->style.width = 25_pct;
    version->style.color = Color{1,1,1,1};

    author = new WidgetText("author");
    author->manager = manager;
    author->update(defaultFont, 60_px, "Created by Alexandro Sanchez Bach");
    author->style.margin.top = 40_px;
    author->style.width = 40_pct;
    author->style.color = Color{1,1,1,1};

    license = new WidgetText("license");
    license->manager = manager;
    license->update(defaultFont, 60_px, "Licensed under GPL v2.0");
    license->style.margin.top = 0_px;
    license->style.width = 27_pct;
    license->style.color = Color{1,1,1,1};

    // Add widgets
    body.addElement(logo);
    body.addElement(version);
    body.addElement(author);
    body.addElement(license);
}

void ScreenLogo::update() {
    bool bootApp = !config.boot.empty();
    // Logo and text fade out
    if (dtime > 2000ms) {
        auto opacity = 1.0f - transition::apply(transition::easeOut, dtime, 2000ms, 3000ms);
        logo->style.opacity = opacity;
        version->style.opacity = opacity;
        author->style.opacity = opacity;
        license->style.opacity = opacity;
    }
    // Background goes black if an application is loaded
    if (dtime > 2700ms && bootApp) {
        float clarity = 1.0f - transition::apply(transition::easeOut, dtime, 2700ms, 3000ms);
        body.style.background = {0.157f * clarity, 0.157f * clarity, 0.360f * clarity, 1.0f};
    }
    // Next screen is loaded
    if (dtime > 3000ms) {
        if (bootApp) {
            manager->pushScreen(std::make_unique<ScreenEmulator>(manager));
        } else {
            manager->pushScreen(std::make_unique<ScreenMain>(manager));
        }
        finished = true;
    }
}

}  // namespace ui
