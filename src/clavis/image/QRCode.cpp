#include <image/QRCode.h>

#include <algorithm>
#include <exception>

#include <error/ClavisError.h>
#include <language/Language.h>
#include <tools/qrcodegen.hpp>

namespace Clavis::QRCode {
    std::shared_ptr<Image> Render(const std::string& text, int targetPixelSize, int quietZoneModules) {
        quietZoneModules = std::max(0, quietZoneModules);

        std::unique_ptr<qrcodegen::QrCode> code;
        try {
            code = std::make_unique<qrcodegen::QrCode>(
                qrcodegen::QrCode::encodeText(text.c_str(), qrcodegen::QrCode::Ecc::MEDIUM));
        } catch (const std::exception&) {
            // Realistically only reachable if the URI exceeds the format's capacity.
            RaiseClavisError(_(ERROR_QR_GENERATION_FAILED));
        }

        const int moduleCount = code->getSize();
        const int totalModules = moduleCount + 2 * quietZoneModules;

        // An integer scale keeps every module the same size; a fractional one would make some
        // modules a pixel wider than others and confuse scanners at small sizes.
        const int scale = std::max(2, targetPixelSize / std::max(1, totalModules));
        const auto dimension = static_cast<size_t>(totalModules * scale);

        auto image = std::make_shared<Image>(dimension, dimension);

        constexpr Image::Pixel white = { 255, 255, 255, 255 };
        constexpr Image::Pixel black = { 0, 0, 0, 255 };

        for (size_t y = 0; y < dimension; y++)
            for (size_t x = 0; x < dimension; x++)
                image->Set(x, y, white);

        for (int moduleY = 0; moduleY < moduleCount; moduleY++) {
            for (int moduleX = 0; moduleX < moduleCount; moduleX++) {
                if (!code->getModule(moduleX, moduleY))
                    continue;

                const auto originX = static_cast<size_t>((moduleX + quietZoneModules) * scale);
                const auto originY = static_cast<size_t>((moduleY + quietZoneModules) * scale);

                for (int offsetY = 0; offsetY < scale; offsetY++)
                    for (int offsetX = 0; offsetX < scale; offsetX++)
                        image->Set(originX + offsetX, originY + offsetY, black);
            }
        }

        return image;
    }
}
