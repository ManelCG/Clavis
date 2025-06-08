#include <GUI/palettes/first_run/GitServerConfigPalette.h>

namespace Clavis::GUI {
    GitServerConfigPalette::GitServerConfigPalette() :
        DualChoicePalette(_(GIT_SERVER_CONFIG_TITLE))
    {
        set_margin(10);
        set_default_size(420, -1);

        SetYesText(_(MISC_ACCEPT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        append(notImplementedLabel);
        notImplementedLabel.set_text(_(ERROR_NOT_IMPLEMENTED));
    }


}