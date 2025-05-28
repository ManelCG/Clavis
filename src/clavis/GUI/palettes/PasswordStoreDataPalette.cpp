#include <GUI/palettes/PasswordStoreDataPalette.h>

#include <language/Language.h>

namespace Clavis::GUI {
   PasswordStoreDataPalette::PasswordStoreDataPalette(const PasswordStore& passwordStore) :
      Palette(_(PASSWORD_STORE_DATA_PALETTE_TITLE)),

      mainVBox(Gtk::Orientation::VERTICAL)
   {
      store = passwordStore;
      set_default_size(320, -1);

      passwordStoreDirectoryLabel.set_text(_(PASSWORD_STORE_DATA_PALETTE_DIRECTORY_LABEL));
      passwordStoreDirectoryEntry.set_text(store.GetRoot().string());
      passwordStoreDirectoryEntry.set_editable(false);
      passwordStoreDirectoryEntry.set_margin_top(5);

      numPasswordsLabel.set_text(_(PASSWORD_STORE_DATA_PALETTE_NUM_PASSWORDS_LABEL));
      numPasswordsLabel.set_halign(Gtk::Align::END);
      numFoldersLabel.set_text(_(PASSWORD_STORE_DATA_PALETTE_NUM_FOLDERS_LABEL));
      numFoldersLabel.set_halign(Gtk::Align::END);

      tableLabelsVBox.append(numPasswordsLabel);
      tableLabelsVBox.append(numFoldersLabel);

      numPasswordsValueLabel.set_text(std::to_string(store.GetNumberOfPasswords()));
      numFoldersValueLabel.set_text(std::to_string(store.GetNumberOfFolders()));
      numPasswordsValueLabel.set_halign(Gtk::Align::START);
      numFoldersValueLabel.set_halign(Gtk::Align::START);

      tableValuesVBox.append(numPasswordsValueLabel);
      tableValuesVBox.append(numFoldersValueLabel);

      mainVBox.append(passwordStoreDirectoryLabel);
      mainVBox.append(passwordStoreDirectoryEntry);

      Gtk::Separator sep(Gtk::Orientation::HORIZONTAL);
      sep.set_margin(5);
      mainVBox.append(sep);

      tableHBox.set_orientation(Gtk::Orientation::HORIZONTAL);
      tableHBox.append(tableLabelsVBox);
      tableHBox.append(tableValuesVBox);
      tableLabelsVBox.set_orientation(Gtk::Orientation::VERTICAL);
      tableValuesVBox.set_orientation(Gtk::Orientation::VERTICAL);

      tableHBox.set_hexpand(true);
      tableHBox.set_halign(Gtk::Align::CENTER);
      tableLabelsVBox.set_margin_end(30);

      mainVBox.append(tableHBox);
      mainVBox.set_margin(10);

      set_child(mainVBox);
   }
}