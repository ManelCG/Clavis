#pragma once

#include <memory>
#include <string>

#include <image/image.h>

namespace Clavis {
    namespace QRCode {
        // Renders `text` as a QR code, black modules on a white background with a quiet zone.
        //
        // The result is deliberately NOT theme-inverted: scanners rely on dark-on-light polarity
        // and on a light quiet zone, so an inverted code in dark mode would fail to scan on a
        // meaningful share of devices.
        std::shared_ptr<Image> Render(const std::string& text,
                                      int targetPixelSize = 320,
                                      int quietZoneModules = 4);
    }
}
