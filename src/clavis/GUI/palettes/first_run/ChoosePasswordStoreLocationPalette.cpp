#include <GUI/palettes/first_run/ChoosePasswordStoreLocationPalette.h>

#include <language/Language.h>

#include <settings/Settings.h>

namespace Clavis::GUI {
    ChoosePasswordStoreLocationPalette::ChoosePasswordStoreLocationPalette() :
        DualChoicePalette(_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_TITLE))
    {
        set_margin(10);
        set_default_size(420, -1);

        SetYesText(_(MISC_ACCEPT_BUTTON));
        SetNoText(_(MISC_CANCEL_BUTTON));

        choosePathLabel.set_text(_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_LABEL_TEXT));
        choosePathLabel.set_margin_bottom(5);

        choosePathEntry.set_text(Settings::PASSWORD_STORE_PATH.GetValue());

        append(choosePathLabel);
        append(choosePathEntry);
    }

    std::filesystem::path ChoosePasswordStoreLocationPalette::GetSelectedPath() {
        return std::string(choosePathEntry.get_text());
    }


}