#include <GUI/palettes/AboutPalette.h>

#include <language/Language.h>
#include <image/icons.h>

#include <extensions/GUIExtensions.h>

#include <GUI/palettes/CreditsPalette.h>
#include <GUI/palettes/LicensePalette.h>
#include <GUI/palettes/DonatePalette.h>

namespace Clavis::GUI {
    AboutPalette::AboutPalette() :
        Palette(_(ABOUT_PALETTE_TITLE)),
        mainVBox(Gtk::Orientation::VERTICAL)
    {
        set_default_size(420, 1);
        set_resizable(false);

        logo.SetIcon(Icons::Logos::ClavisLogo);
        logo.Resize(150, 150);
        logo.set_valign(Gtk::Align::START);
        logo.set_vexpand(false);
        logo.set_margin_bottom(5);
        mainVBox.append(logo);

        clavisLabel.set_markup("<b>Clavis</b>");
        clavisLabel.set_margin_bottom(5);
        mainVBox.append(clavisLabel);

        versionLabel.set_text(CLAVIS_VERSION);
        versionLabel.set_margin_bottom(5);
        mainVBox.append(versionLabel);

        descriptionLabel.set_text(_(ABOUT_PALETTE_DESCRIPTION_LABEL));
        descriptionLabel.set_margin_bottom(5);
        mainVBox.append(descriptionLabel);

        githubLabel.set_markup("<a href=\"https://github.com/ManelCG/Clavis\">" + _(ABOUT_PALETTE_GITHUB_LABEL) + "</a>");
        githubLabel.set_margin_bottom(5);
        mainVBox.append(githubLabel);

        copyrightLabel.set_markup("<span size=\"small\">" + _(ABOUT_PALETTE_COPYRIGHT_LABEL) + "</span>");
        copyrightLabel.set_margin_bottom(5);
        mainVBox.append(copyrightLabel);

        buttonsHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
        buttonsHBox.append(creditsButton);
        buttonsHBox.append(licenseButton);
        buttonsHBox.append(donateButton);
        buttonsHBox.append(closeButton);
        mainVBox.append(buttonsHBox);

        creditsButton.set_label(_(ABOUT_PALETTE_CREDITS_BUTTON));
        licenseButton.set_label(_(ABOUT_PALETTE_LICENSE_BUTTON));
        donateButton.set_label(_(ABOUT_PALETTE_DONATE_BUTTON));
        closeButton.set_label(_(MISC_CLOSE_BUTTON));

        creditsButton.property_width_request().set_value(90);
        licenseButton.property_width_request().set_value(90);
        donateButton.property_width_request().set_value(80);
        closeButton.property_width_request().set_value(90);

        creditsButton.set_margin_end(3);
        licenseButton.set_margin_end(3);
        closeButton.set_hexpand(true);
        closeButton.set_halign(Gtk::Align::END);
        buttonsHBox.set_hexpand(true);

        mainVBox.set_margin(10);
        set_child(mainVBox);

        creditsButton.signal_clicked().connect([this]() {
            Extensions::SpawnWindow<CreditsPalette>(this);
        });
        licenseButton.signal_clicked().connect([this]() {
            Extensions::SpawnWindow<LicensePalette>(this);
        });
        donateButton.signal_clicked().connect([this]() {
            Extensions::SpawnWindow<DonatePalette>(this);
        });
        closeButton.signal_clicked().connect([this]() {
            close();
        });
    }
}