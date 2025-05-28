#include <GUI/palettes/DonatePalette.h>

#include <language/Language.h>
#include <system/Extensions.h>

#include <error/ClavisError.h>

namespace Clavis::GUI {
    DonatePalette::DonatePalette() :
        Palette(_(DONATE_PALETTE_TITLE))
    {
        set_default_size(700, 700);
        set_resizable(false);

        titleLabel.set_markup("<span size=\"x-large\">" + _(DONATE_PALETTE_TITLE_LABEL) + "</span>");
        titleLabel.set_margin(20);
        mainVBox.append(titleLabel);

        auto setupLabel =  [this](Gtk::Label& l, std::string text) {
            l.set_markup(text);
            l.set_wrap(true);
            l.set_wrap_mode(Pango::WrapMode::WORD);
            l.set_margin_start(70);
            l.set_margin_end(70);
            l.set_margin_bottom(20);
            l.set_justify(Gtk::Justification::CENTER);
            mainVBox.append(l);
        };

        setupLabel(label1, _(DONATE_PALETTE_DESCRIPTION_LABEL_1));
        setupLabel(label2, _(DONATE_PALETTE_DESCRIPTION_LABEL_2));
        setupLabel(label3, _(DONATE_PALETTE_DESCRIPTION_LABEL_3));
        setupLabel(label4, _(DONATE_PALETTE_DESCRIPTION_LABEL_4));
        setupLabel(label5, _(DONATE_PALETTE_DESCRIPTION_LABEL_5));

        Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
        sep.set_margin_start(5);
        sep.set_margin_end(5);
        mainVBox.append(sep);

        AddDonationCard("Monero (XMR)", Icons::Currencies::XMR_32x32, Icons::Currencies::XMR_QR, "85Sd3zhghTJKULdEH6VXfn5BgxMB29MCnKhyvVggkWJcDywzSE8Nud3NK48T9Je7DtavooV8RufTsPPAsXNSqSvDQZLCqeG", "donate@clavis.manelcg.com", 19);
        AddDonationCard("Bitcoin (BTC)", Icons::Currencies::BTC_32x32, Icons::Currencies::BTC_QR, "bc1qdtr7r6rrm098sxd9nvdasf7z23jgpprk2wegmu", "donate@clavis.manelcg.com", 21);
        donationsTabber.set_margin(20);
        mainVBox.append(donationsTabber);


        mainVBox.set_orientation(Gtk::Orientation::VERTICAL);
        set_child(mainVBox);
    }

    void DonatePalette::AddDonationCard(const std::string &currency, Icons::IconDefinition icon, Icons::IconDefinition qr, const std::string &address, const std::string &openalias, int addressLabelLen) {
        auto tabber = Gtk::make_managed<Tabber>();
        donationsTabber.AddTab(currency, icon, tabber);

        auto createLabel = [](std::string text, int labelLength) {
            // Outer container to center the label widget itself
            auto outerBox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
            outerBox->set_halign(Gtk::Align::CENTER);  // Center the label block itself
            outerBox->set_valign(Gtk::Align::START);   // Optional: top-aligned

            // Label with wrap and styling
            auto label = Gtk::make_managed<Gtk::Label>();
            label->set_markup("<span foreground=\"#f265ab\" insert-hyphens=\"no\"><tt>" + text + "</tt></span>"); // orange color
            label->set_margin(10);
            label->set_wrap(true);
            label->set_wrap_mode(Pango::WrapMode::CHAR);
            label->set_xalign(0.5f);

            if (labelLength != -1) {
                label->set_width_chars(labelLength);
                label->set_max_width_chars(labelLength);
            }

            // Optional clipboard support
            auto click = Gtk::GestureClick::create();
            click->set_propagation_phase(Gtk::PropagationPhase::CAPTURE);
            click->signal_pressed().connect([text](int, double, double) {
                System::CopyToClipboard(text);
            });
            label->add_controller(click);

            // Add label to outer centering box
            outerBox->append(*label);

            return outerBox;

        };


        auto openaliasLabel = createLabel(openalias, -1);
        auto addressLabel = createLabel(address, addressLabelLen);

        auto qrimg = Gtk::make_managed<PictureInsert>();
        qrimg->SetIcon(qr);
        qrimg->Resize(150, 150);

        tabber->AddTab(_(DONATE_PALETTE_OPENALIAS_TAB), openaliasLabel);
        tabber->AddTab(_(DONATE_PALETTE_ADDRESS_TAB), addressLabel);
        tabber->AddTab(_(DONATE_PALETTE_QRCODE_TAB), qrimg);

        tabber->set_margin(10);
    }


}
