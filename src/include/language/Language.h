#pragma once

#include <array>
#include <string>
#include <vector>
#include <map>
#include <filesystem>

#include <glibmm/ustring.h>

//// // Uncomment to assert at compile-time that all languages have valid translations
//#define TRANSLATE_CHECK 1
//#define TRANSLATE_CHECK ENG

#include <language/LanguageDefinitions.h>


namespace Clavis::Language {
    struct StrArg {
        std::string value;
        StrArg(std::string s) : value(std::move(s)) {}
        StrArg(const char* s) : value(s) {}
        StrArg(const std::filesystem::path& p) : value(p.string()) {}
    };

    Glib::ustring GetResourceString(_I18N_MAP_ strings, std::vector<StrArg> arguments);

    std::pair<std::string, std::string> GetLanguageData(LanguagesEnum language);
    std::string GetLanguageCode(LanguagesEnum language);
    std::string GetLanguageName(LanguagesEnum language);
    LanguagesEnum GetLanguageFromCode(const std::string &code);

    std::vector<LanguagesEnum> GetAllLanguages();
}

using namespace Clavis::Language;
#define __(l, s) { LanguagesStr::l, u8##s##sv }

#pragma region File Menu

_(MAINMENU_FILE_MENU,
    __(ENG, "File"),
    __(ESP, "Archivo"),
    __(VAL, "Arxiu")
)

_(MAINMENU_EDIT_MENU,
    __(ENG, "Edit"),
    __(ESP, "Editar"),
    __(VAL, "Editar")
)
_(MAINMENU_HELP_MENU,
    __(ENG, "Help"),
    __(ESP, "Ayuda"),
    __(VAL, "Ajuda")
)

_(MAINMENU_FILE_MENU_PASSWORD_STORE_DATA,
    __(ENG, "Password Store Data"),
    __(ESP, "Datos del Repositorio de Contraseñas"),
    __(VAL, "Dades del Repositori de Contrasenyes")
)

_(MAINMENU_FILE_MENU_NEW_FOLDER,
    __(ENG, "New Folder"),
    __(ESP, "Nueva Carpeta"),
    __(VAL, "Nova Carpeta")
)

_(MAINMENU_FILE_MENU_NEW_PASSWORD,
    __(ENG, "New Password"),
    __(ESP, "Nueva Contraseña"),
    __(VAL, "Nova Contrasenya")
)

_(MAINMENU_FILE_MENU_IMPORT_PASSWORD_STORE,
    __(ENG, "Import Password Store"),
    __(ESP, "Importar Repositorio de Contraseñas"),
    __(VAL, "Importar Repositori de Contrasenyes")
)
_(MAINMENU_FILE_MENU_EXPORT_PASSWORD_STORE,
    __(ENG, "Export Password Store"),
    __(ESP, "Exportar Repositorio de Contraseñas"),
    __(VAL, "Exportar Repositori de Contrasenyes")
)


_(MAINMENU_FILE_MENU_EXPORT_PUBLIC_GPG,
    __(ENG, "Export Public GPG Key"),
    __(ESP, "Exportar Clave GPG Pública"),
    __(VAL, "Exportar Clau GPG Pública")
)
_(MAINMENU_FILE_MENU_EXPORT_PRIVATE_GPG,
    __(ENG, "Export Private GPG Key"),
    __(ESP, "Exportar Clave GPG Privada"),
    __(VAL, "Exportar Clau GPG Privada")
)

_(MAINMENU_FILE_MENU_LOCK_VAULT,
    __(ENG, "Lock vault"),
    __(ESP, "Bloquear el almacén"),
    __(VAL, "Bloquejar el magatzem")
)

_(MAINMENU_FILE_MENU_QUIT,
    __(ENG, "Quit"),
    __(ESP, "Salir"),
    __(VAL, "Eixir")
)

#pragma endregion

#pragma region Edit Menu

_(MAINMENU_EDIT_MENU_GIT_SERVER_SETTINGS,
    __(ENG, "Git server settings"),
    __(ESP, "Configurar servidor Git"),
    __(VAL, "Configurar servidor Git")
)

_(MAINMENU_EDIT_MENU_GPG_KEY_SETTINGS,
    __(ENG, "GPG key Settings"),
    __(ESP, "Configurar clave GPG"),
    __(VAL, "Configurar clau GPG")
)

_(MAINMENU_EDIT_MENU_SETTINGS,
    __(ENG, "Settings"),
    __(ESP, "Configuración"),
    __(VAL, "Configuració")
)

#pragma endregion

#pragma region Help Menu

_(MAINMENU_HELP_MENU_HELP,
    __(ENG, "Help"),
    __(ESP, "Ayuda"),
    __(VAL, "Ajuda")
)

_(MAINMENU_HELP_MENU_ABOUT,
    __(ENG, "About"),
    __(ESP, "Acerca de"),
    __(VAL, "Sobre")
)

#pragma endregion

#pragma region Password Store Data Palette

_(PASSWORD_STORE_DATA_PALETTE_TITLE,
    __(ENG, "Password Store Data"),
    __(ESP, "Datos del Repositorio de Contraseñas"),
    __(VAL, "Dades del Repositori de Contrasenyes")
)

_(PASSWORD_STORE_DATA_PALETTE_DIRECTORY_LABEL,
    __(ENG, "Password Store Directory:"),
    __(ESP, "Directorio del Repositorio de Contraseñas:"),
    __(VAL, "Directori del Repositori de Contrasenyes:")
)

_(PASSWORD_STORE_DATA_PALETTE_NUM_PASSWORDS_LABEL,
    __(ENG, "Number of passwords:"),
    __(ESP, "Número de contraseñas:"),
    __(VAL, "Nombre de contrasenyes:")
)

_(PASSWORD_STORE_DATA_PALETTE_NUM_FOLDERS_LABEL,
    __(ENG, "Number of folders:"),
    __(ESP, "Número de carpetas:"),
    __(VAL, "Nombre de carpetes:")
)

#pragma endregion

#pragma region About Palette

_(ABOUT_PALETTE_TITLE,
    __(ENG, "About Clavis"),
    __(ESP, "Acerca de Clavis"),
    __(VAL, "Sobre Clavis")
)

_(ABOUT_PALETTE_DESCRIPTION_LABEL,
    __(ENG, "Clavis is an easy to use Password Manager"),
    __(ESP, "Clavis es un gestor de contraseñas fácil de usar"),
    __(VAL, "Clavis és un gestor de contrasenyes fàcil d'utilitzar")
)

_(ABOUT_PALETTE_GITHUB_LABEL,
    __(ENG, "Github repository"),
    __(ESP, "Repositorio de Github"),
    __(VAL, "Repositori de Github"
))

_(ABOUT_PALETTE_COPYRIGHT_LABEL,
    __(ENG, "Copyright © Manel Castillo Giménez"),
    __(ESP, "Copyright © Manel Castillo Giménez"),
    __(VAL, "Copyright © Manel Castillo Giménez")
)

_(ABOUT_PALETTE_CREDITS_BUTTON,
    __(ENG, "Credits"),
    __(ESP, "Créditos"),
    __(VAL, "Crédits")
)

_(ABOUT_PALETTE_LICENSE_BUTTON,
    __(ENG, "License"),
    __(ESP, "Licencia"),
    __(VAL, "Llicència")
)

_(ABOUT_PALETTE_DONATE_BUTTON,
    __(ENG, "Donate"),
    __(ESP, "Donar"),
    __(VAL, "Donar")
)

#pragma endregion

#pragma region Donate Palette

_(DONATE_PALETTE_TITLE,
    __(ENG, "Donate to Clavis"),
    __(ESP, "Donar a Clavis"),
    __(VAL, "Donar a Clavis")
)

_(DONATE_PALETTE_TITLE_LABEL,
    __(ENG, "Thank you so much for considering donating to Clavis!"),
    __(ESP, "¡Gracias por considerar donar a Clavis!"),
    __(VAL, "Gràcies per considerar donar a Clavis!")
)

_(DONATE_PALETTE_DESCRIPTION_LABEL_1,
    __(ENG, "Clavis is a Free and Open Source Project, and as such, it is completely free to use. There are no locked features, no paywalls, and no hidden charges—nor will there ever be."),
    __(ESP, "Clavis es un proyecto libre y de código abierto, y como tal, es completamente gratuito. No hay funciones bloqueadas, ni muros de pago, ni cargos ocultos, y nunca los habrá."),
    __(VAL, "Clavis és un projecte lliure i de codi obert, i per tant, és completament gratuït. No hi ha funcions bloquejades, ni murs de pagament, ni càrrecs ocults, i mai n'hi haurà."),
)

_(DONATE_PALETTE_DESCRIPTION_LABEL_2,
    __(ENG, "Clavis has been developed entirely by one person (hi, that’s me!), driven by a passion for privacy and the desire to help people manage their passwords securely and efficiently, to stay in control of their own data and credentials in a world where people keep getting burned by scams and leaks, and locked in centralized services and ecosystems."),
    __(ESP, "Clavis ha sido desarrollado íntegramente por una sola persona (¡hola, ese soy yo!), movido por una pasión por la privacidad y el deseo de ayudar a la gente a gestionar sus contraseñas de forma segura y eficiente, manteniendo el control sobre sus propios datos y credenciales en un mundo donde la gente sigue siendo víctima de estafas, filtraciones y servicios centralizados."),
    __(VAL, "Clavis ha sigut desenvolupat íntegrament per una sola persona (hola, soc jo!), mogut per una passió per la privacitat i el desig d’ajudar a la gent a gestionar les seues contrasenyes de manera segura i eficient, mantenint el control sobre les seues dades i credencials en un món on la gent continua sent víctima d’estafes, filtracions i serveis centralitzats."),
)

_(DONATE_PALETTE_DESCRIPTION_LABEL_3,
    __(ENG, "But building and maintaining a project like Clavis takes a lot of time, energy, and care. And while it brings in no revenue, it continues to grow and improve—thanks to the dedication behind it."),
    __(ESP, "Pero construir y mantener un proyecto como Clavis requiere mucho tiempo, energía y cuidado. Y aunque no genera ingresos, sigue creciendo y mejorando gracias a la dedicación que hay detrás."),
    __(VAL, "Però construir i mantindre un projecte com Clavis requerix molt de temps, energia i dedicació. I encara que no genera ingressos, continua creixent i millorant gràcies a l’esforç que hi ha darrere."),
)

_(DONATE_PALETTE_DESCRIPTION_LABEL_4,
    __(ENG, "If you find Clavis useful, consider making a donation. Your support helps ensure the project stays alive and independent, and it means a lot."),
    __(ESP, "Si Clavis te resulta útil, considera hacer una donación. Tu apoyo ayuda a que el proyecto siga vivo e independiente, y significa mucho."),
    __(VAL, "Si Clavis et resulta útil, considera fer una donació. El teu suport ajuda a mantindre viu i independent el projecte, i significa molt."),
)

_(DONATE_PALETTE_DESCRIPTION_LABEL_5,
    __(ENG, "Thank you for using Clavis. ❤️"),
    __(ESP, "Gracias por usar Clavis. ❤️"),
    __(VAL, "Gràcies per utilitzar Clavis. ❤️"),
)

_(DONATE_PALETTE_ADDRESS_TAB,
    __(ENG, "Address"),
    __(ESP, "Dirección"),
    __(VAL, "Adreça")
)
_(DONATE_PALETTE_OPENALIAS_TAB,
    __(ENG, "OpenAlias"),
    __(ESP, "OpenAlias"),
    __(VAL, "OpenAlias")
)
_(DONATE_PALETTE_QRCODE_TAB,
    __(ENG, "QR Code"),
    __(ESP, "Código QR"),
    __(VAL, "Codi QR")
)

#pragma endregion

#pragma region Misc Palettes

_(NEW_FOLDER_PALETTE_TITLE,
    __(ENG, "New folder"),
    __(ESP, "Nueva carpeta"),
    __(VAL, "Nova carpeta")
)
_(NEW_FOLDER_PALETTE_LABEL_TITLE,
    __(ENG, "New folder name:"),
    __(ESP, "Nombre de la carpeta:"),
    __(VAL, "Nom de la carpeta:")
)
_(RENAME_ELEMENT_PALETTE_TITLE,
    __(ENG, "Rename"),
    __(ESP, "Renombrar"),
    __(VAL, "Renombrar")
)
_(RENAME_ELEMENT_PALETTE_LABEL_TITLE,
    __(ENG, "Rename {0} to:"),
    __(ESP, "Renombrar {0} a:"),
    __(VAL, "Renombrar {0} a:")
)

_(MISC_LANGUAGE,
    __(ENG, "Language"),
    __(ESP, "Idioma"),
    __(VAL, "Idioma")
)

#pragma endregion

#pragma region New Password Palette

_(NEW_PASSWORD_PALETTE_TITLE,
    __(ENG, "Add new password to Clavis"),
    __(ESP, "Añadir nueva contraseña a Clavis"),
    __(VAL, "Afegir nova contrasenya a Clavis")
)

_(NEW_PASSWORD_PALETTE_LABEL_TITLE,
    __(ENG, "Add new password:"),
    __(ESP, "Añadir nueva contraseña:"),
    __(VAL, "Afegir nova contrasenya:")
)

_(NEW_PASSWORD_PALETTE_SET_PASSWORD_NAME_LABEL_TEXT,
    __(ENG, "Set password name:"),
    __(ESP, "Nombre de la contraseña:"),
    __(VAL, "Nom de la contrasenya")
)

_(NEW_PASSWORD_PALETTE_EDIT_TITLE,
    __(ENG, "Edit existing password"),
    __(ENG, "Editar contraseña"),
    __(VAL, "Editar contrasenya")
)
_(NEW_PASSWORD_PALETTE_EDIT_LABEL_TITLE,
    __(ENG, "Edit password:"),
    __(ENG, "Editar contraseña:"),
    __(VAL, "Editar contrasenya:")
)

_(NEW_PASSWORD_PALETTE_CURRENT_PASSWORD_NAME_LABEL_TEXT,
    __(ENG, "Password name:"),
    __(ESP, "Nombre de la contraseña:"),
    __(VAL, "Nom de la contrasenya")
)

_(NEW_PASSWORD_PALETTE_DEFAULT_PASSWORD_NAME,
    __(ENG, "new_password"),
    __(ESP, "nueva_contra"),
    __(VAL, "nova_contrasenya")
)
_(NEW_PASSWORD_PALETTE_NEW_PASSWORD_LABEL_TEXT,
    __(ENG, "New password:"),
    __(ESP, "Nueva contraseña:"),
    __(VAL, "Nova contrasenya:")
)
_(NEW_PASSWORD_PALETTE_PASSWORD_ENTRY_PLACEHOLDER,
    __(ENG, "Password"),
    __(ESP, "Contraseña"),
    __(VAL, "Contrasenya")
)

_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_LABEL_TEXT,
    __(ENG, "Password generator:"),
    __(ESP, "Generador de contraseñas:"),
    __(VAL, "Generador de contrasenyes")
)

_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_LENGTH_LABEL_TEXT,
    __(ENG, "Password length:"),
    __(ESP, "Longitud de la contraseña:"),
    __(VAL, "Longitud de la contrasenya")
)
_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_LOWERCASE,
    __(ENG, "Lowercase"),
    __(ESP, "Minúsculas"),
    __(VAL, "Minúscules")
)
_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_UPPERCASE,
    __(ENG, "Uppercase"),
    __(ESP, "Mayúsculas"),
    __(VAL, "Majúscules")
)
_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_NUMERALS,
    __(ENG, "Numerals"),
    __(ESP, "Numerales"),
    __(VAL, "Numerals")
)
_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_SYMBOLS,
    __(ENG, "Symbols"),
    __(ESP, "Símbolos"),
    __(VAL, "Símbols")
)
_(NEW_PASSWORD_PALETTE_PASSWORD_GENERATOR_PRONOUNCEABLE,
    __(ENG, "Pronounceable"),
    __(ESP, "Pronunciable"),
    __(VAL, "Pronunciable")
)

_(NEW_PASSWORD_PALETTE_ELEMENT_ALREADY_EXISTS_TITLE,
    __(ENG, "Overwrite?"),
    __(ESP, "¿Sobreescribir?"),
    __(VAL, "Sobreescriure?")
)

_(NEW_PASSWORD_PALETTE_ELEMENT_ALREADY_EXISTS_TEXT,
    __(ENG, "{0} already exists. Overwrite?"),
    __(ESP, "{0} ya existe. Sobreescribir?"),
    __(VAL, "{0} ja existeix. Sobreescriure?")
)

#pragma endregion

#pragma region Help Palette

_(HELP_PALETTE_TITLE,
    __(ENG, "Help"),
    __(ESP, "Ayuda"),
    __(VAL, "Ajuda")
)

#pragma endregion

#pragma region Password Store Manager

_(FILTER_FILES_SEARCHBAR_PLACEHOLDER,
    __(ENG, "Filter files"),
    __(ESP, "Filtrar archivos"),
    __(VAL, "Filtrar arxius")
)

_(FILTER_FILES_SEARCHBAR_PLACEHOLDER_RECURSIVE,
    __(ENG, "Recursive search"),
    __(ESP, "Búsqueda recursiva"),
    __(VAL, "Cerca recursiva")
)

_(PASSWORDSTORE_DECRYPTED_PASSWORD_LABEL,
    __(ENG, "Decrypted Password:"),
    __(ESP, "Contraseña Descifrada:"),
    __(VAL, "Contrasenya Descifrada:")
)

_(PASSWORDSTORE_PASSWORD_OUTPUT_HINT,
    __(ENG, "Password output"),
    __(ESP, "Salida de contraseña"),
    __(VAL, "Eixida de contrasenya")
)

_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EDIT_PASSWORD_BUTTON,
    __(ENG, "Edit password"),
    __(ESP, "Editar contraseña"),
    __(VAL, "Editar contrasenya")
)

_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_RENAME_ITEM_BUTTON,
    __(ENG, "Rename"),
    __(ESP, "Renombrar"),
    __(VAL, "Renombrar")
)

_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EXPORT_FOLDER_BUTTON,
    __(ENG, "Export as .clav"),
    __(ESP, "Exportar como .clav"),
    __(VAL, "Exportar com a .clav")
)

#pragma endregion

#pragma region Buttons
_(MISC_SECONDS,
    __(ENG, "Seconds:"),
    __(ESP, "Segundos:"),
    __(VAL, "Segons:")
)

_(MISC_SAVE_BUTTON,
    __(ENG, "Save"),
    __(ESP, "Guardar"),
    __(VAL, "Guardar"),
)
_(MISC_CLOSE_BUTTON,
    __(ENG, "Close"),
    __(ESP, "Cerrar"),
    __(VAL, "Tancar"),
)
_(MISC_DELETE_BUTTON,
    __(ENG, "Delete"),
    __(ESP, "Borrar"),
    __(VAL, "Esborrar")
)
_(MISC_DELETE_ELEMENT_PROMPT,
    __(ENG, "Delete {0}?"),
    __(ESP, "¿Borrar {0}?"),
    __(VAL, "Esborrar {0}?")
)
_(MISC_DELETE_ELEMENT_PROMPT_LABEL,
    __(ENG, "This action cannot be undone."),
    __(ESP, "Esta acción no se puede deshacer."),
    __(VAL, "Aquesta acció no es pot desfer.")
)
_(MISC_CANCEL_BUTTON,
    __(ENG, "Cancel"),
    __(ESP, "Cancelar"),
    __(VAL, "Cancel·lar"),
)
_(MISC_ACCEPT_BUTTON,
    __(ENG, "Accept"),
    __(ESP, "Aceptar"),
    __(VAL, "Acceptar")
)

_(MISC_YES_BUTTON,
    __(ENG, "Yes"),
    __(ESP, "Sí"),
    __(VAL, "Sí"),
)

_(MISC_NO_BUTTON,
    __(ENG, "No"),
    __(ESP, "No"),
    __(VAL, "No")
)

_(MISC_OVERWRITE_BUTTON,
    __(ENG, "Overwrite"),
    __(ESP, "Sobreescribir"),
    __(VAL, "Sobreescriure")
)

_(MISC_CONFIRM_SELECTION_BUTTON,
    __(ENG, "Confirm selection"),
    __(ESP, "Confirmar selección"),
    __(VAL, "Confirmar selecció")
)

_(MISC_BROWSE_BUTTON,
    __(ENG, "Browse"),
    __(ESP, "Buscar"),
    __(VAL, "Buscar")
)

_(MISC_EXPORT_BUTTON,
    __(ENG, "Export"),
    __(ESP, "Exportar"),
    __(VAL, "Exportar")
)
_(MISC_IMPORT_BUTTON,
    __(ENG, "Import"),
    __(ESP, "Importar"),
    __(VAL, "Importar")
)

_(MISC_DONE_BUTTON,
    __(ENG, "Done"),
    __(ESP, "Hecho"),
    __(VAL, "Fet")
)
#pragma endregion

#pragma region Errors

_(UNEXPECTED_ERROR_MESSAGE,
    __(ENG, "Unexpected Error"),
    __(ESP, "Error Inesperado"),
    __(VAL, "Error inesperat"),
)

_(CLAVIS_RAN_INTO_AN_ERROR_MESSAGE,
    __(ENG, "Clavis ran into an error:"),
    __(ESP, "Clavis encontró un error:"),
    __(VAL, "Clavis s'ha trobat amb un error:"),
)

_(PLEASE_TRY_AGAIN_ERROR_MESSAGE,
    __(ENG, "Please try again.\nIf the problem persists, or if you believe this to be a bug in Clavis,\nplease copy the error details with the button below,\nand send the data to the Clavis developers so we can fix this issue!"),
    __(ESP, "Por favor, prueba de nuevo.\nSi el problema persiste, o si piensas que esto puede ser un bug en Clavis.\npor favor, copia los detalles del error con el botón que hay más abajo,\ny envía dichos datos a los desarrolladores de Clavis para que podamos resolver este problema!"),
    __(VAL, "Per favor, intenta-ho una altra vegada.\nSi el problema persisteix, o si penses que açó pot ser degut a un bug en Clavis,\nper favor, copia els detalls del error amb el botó que hi ha més avall,\ni envía les dades als desenvolupadors de Clavis,\nper a que puguem resoldre aquest problema!"),
)

_(COPY_ERROR_INFORMATION_BUTTON_LABEL,
    __(ENG, "Copy error information"),
    __(ESP, "Copiar información del error"),
    __(VAL, "Copiar informació de l'error"),
)
_(ERROR_COPYING_TO_CLIPBOARD_TEXT,
    __(ENG, "Error copying to clipboard"),
    __(ESP, "Error copiando al portapapeles"),
    __(VAL, "Error copiant al porta-retalls"),
)

_(ERROR_NOT_A_PASSWORD,
    __(ENG, "'{0}': Not a password"),
    __(ESP, "'{0}': No es una contraseña"),
    __(VAL, "'{0}': No és una contrasenya")
)

_(ERROR_COULD_NOT_DECRYPT_PASSWORD,
    __(ENG, "Could not decrypt password: {0}"),
    __(ESP, "No se pudo descifrar la contraseña: {0}"),
    __(VAL, "No s'ha pogut descifrar la contrasenya: {0}")
)

_(ERROR_GPG_ID_FILE_NOT_FOUND,
    __(ENG, "The .gpg-id file does not exist in the Password Store. Expected at {0}"),
    __(ESP, "El archivo .gpg-id no existe en el Repositorio de Contraseñas. Lo esperábamos en {0}"),
    __(VAL, "El fitxer .gpg-id no existe en el Repositori de Contrasenyes. L'esperàvem a {0}")
)
_(ERROR_CANNOT_READ_GPGID_FILE,
    __(ENG, "Unable to read .gpg-id file: {0}"),
    __(ESP, "No se pudo leer el archivo .gpg-id: {0}"),
    __(VAL, "No s'ha pogut llegir el fitxer .gpg-id: {0}")
)

_(ERROR_PASSWORD_NOT_DECRYPTED,
    __(ENG, "The password is not yet decrypted."),
    __(ESP, "La contraseña no ha sido descifrada todavía."),
    __(VAL, "La contrasenya no s'ha descifrat encara.")
)

_(ERROR_PASSWORD_NOT_ON_DISK,
    __(ENG, "The password is not on disk."),
    __(ESP, "La contraseña no está en el disco."),
    __(VAL, "La contrasenya no està en el disc.")
)

_(UNNAMED_LAMBDA_SUBSITUTION,
    __(ENG, "Unnamed Lambda"),
    __(ESP, "Lambda Anónima"),
    __(VAL, "Lambda Anònima")
)

_(ERROR_UNABLE_TO_CREATE_DIRECTORY,
    __(ENG, "Unable to create directory: '{0}'"),
    __(ESP, "No se pudo crear la carpeta: '{0}'"),
    __(VAL, "No s'ha pogut crear la carpeta: '{0}'")
)

_(ERROR_NOT_A_DIRECTORY,
    __(ENG, "'{0}': Not a directory"),
    __(ESP, "'{0}': No es una carpeta"),
    __(VAL, "'{0}': No és una carpeta")
)

_(ERROR_NOT_A_FILE,
    __(ENG, "'{0}': Not a file"),
    __(ESP, "'{0}': No es un archivo"),
    __(VAL, "'{0}': No és un arxiu")
)

_(ERROR_OPENING_FILE_FOR_READING,
    __(ENG, "Error opening file for reading: '{0}'"),
    __(ESP, "Error abriendo el archivo para lectura: '{0}'"),
    __(VAL, "No s'ha pogut obrir l'arxiu per llegir: '{0}'")
)

_(ERROR_GUID_FROM_VECTOR_INCORRECT_SIZE,
    __(ENG, "Wrong size of vector when attempting to generate a GUID. Expected {0} but got {1}"),
    __(ESP, "Recibido un vector con tamaño incorrecto al intentar generar una GUID. Esperábamos {0} pero hemos recibido {1}"),
    __(VAL, "Rebut un vector amb mida incorrecta al intentar generar una GUID. Esperàbem {0}, però hem rebut {1}"),
)

_(ERROR_GUID_FROM_STRING_INCORRECT_SIZE,
    __(ENG, "Given string does not correspond to a GUID: {0}"),
    __(ESP, "El texto proporcionado no se puede convertir a una GUID: {0}"),
    __(VAL, "El text proporcionat no es pot interpretar com un GUID: {0}"),
)

_(ERROR_NOT_IMPLEMENTED,
    __(ENG, "Not yet implemented. Coming soon!"),
    __(ESP, "No implementado todavía. Pronto!"),
    __(VAL, "No implementat encara. Aguarda!")
)

_(ERROR_NOT_IMPLEMENTED_ON_PLATFORM,
    __(ENG, "This feature is not implemented on {0}..."),
    __(ESP, "Esta funcionalidad no está implementada en {0}..."),
    __(VAL, "Aquesta funcionalitat no està implementada a {0}..."),
)

_(ERROR_CREATING_PIPE,
    __(ENG, "Error creating pipe"),
    __(ESP, "Error creando canal"),
    __(VAL, "Error creant canal")
)
_(ERROR_CREATING_NAMED_PIPE,
    __(ENG, "Error creating named pipe: {0}"),
    __(ESP, "Error creando canal con nombre: {0}"),
    __(VAL, "Error creant canal amb nom: {0}")
)
_(ERROR_CALLING_FORK,
    __(ENG, "Error calling fork"),
    __(ESP, "Error llamando fork"),
    __(VAL, "Error llant fork")
)
_(ERROR_UNABLE_TO_START_PROCESS,
    __(ENG, "Error starting process"),
    __(ESP, "Error iniciando proceso"),
    __(VAL, "Error iniciant el procés")
)
_(ERROR_UNKNOWN_EXECUTABLE,
    __(ENG, "Unknown executable: {0}"),
    __(ESP, "Ejecutable desconocido: {0}"),
    __(VAL, "Executable desconegut: {0}")
)

_(ERROR_SAVING_PASSWORD,
    __(ENG, "Error saving password: {0}"),
    __(ESP, "Error guardando contraseña: {0}"),
    __(VAL, "Error guardant contrasenya: {0}")
)

_(ERROR_DIRECTORY_ALREADY_EXISTS,
    __(ENG, "Directory already exists: {0}"),
    __(ESP, "La carpeta ya existe: {0}"),
    __(VAL, "La carpeta ja existeix: {0}")
)

_(ERROR_UNKNOWN_ENUM_VALUE,
    __(ENG, "Unknown value: {0}"),
    __(ESP, "Valor desconocido: {0}"),
    __(VAL, "Valor desconegut: {0}")
)

_(CLAVIS_ERROR_FILE_NOT_FOUND,
    __(ENG, "File not found: {0}"),
    __(ESP, "Archivo no encontrado: {0}"),
    __(VAL, "Arxiu no trobat: {0}")
)

_(ERROR_UNABLE_TO_FIND_ASSETS_FOLDER,
    __(ENG, "Unable to find assets folder. Did you install Clavis properly?"),
    __(ESP, "No se pudo encontrar la carpeta de assets. ¿Has instalado Clavis correctamente?"),
    __(VAL, "No s'ha pogut trobar la carpeta d'assets. Has instal·lat Clavis correctament?")
)
#pragma endregion

#pragma region Git

_(GIT_IS_NOT_INITIALIZED_TOOLTIP,
    __(ENG, "Git is not initialized"),
    __(ESP, "Git no está inicializado"),
    __(VAL, "Git no està inicialitzat")
)

_(GIT_PUSH_PASSWORDS,
    __(ENG, "Upload Passwords to Git"),
    __(ESP, "Subir Contraseñas a Git"),
    __(VAL, "Pujar Contrasenyes a Git")
)
_(GIT_PULL_PASSWORDS,
    __(ENG, "Download Passwords from Git"),
    __(ESP, "Descargar Contraseñas de Git"),
    __(VAL, "Baixar Contrasenyes de Git")
)
_(GIT_SYNC_PASSWORDS,
    __(ENG, "Sync All Passwords"),
    __(ESP, "Sincronizar Todas las Contraseñas"),
    __(VAL, "Sincronitzar Totes les Contrasenyes")
)

_(GIT_PUSHING_PASSWORDS,
    __(ENG, "Uploading passwords to Git..."),
    __(ESP, "Subiendo contraseñas a Git..."),
    __(VAL, "Pujant contrasenyes a Git...")
)
_(GIT_PULLING_PASSWORDS,
    __(ENG, "Downloading passwords from Git..."),
    __(ESP, "Descargando contraseñas de Git..."),
    __(VAL, "Baixant contrasenyes de Git...")
)
_(GIT_SYNCING_PASSWORDS,
    __(ENG, "Syncing passwords..."),
    __(ESP, "Sincronizando contraseñas..."),
    __(VAL, "Sincronitzant contrasenyes...")
)

_(GIT_PUSHED_PASSWORDS,
    __(ENG, "Passwords uploaded"),
    __(ESP, "Contraseñas subidas"),
    __(VAL, "Contrasenyes pujades")
)
_(GIT_PULLED_PASSWORDS,
    __(ENG, "Passwords downloaded"),
    __(ESP, "Contraseñas bajadas"),
    __(VAL, "Contrasenyes baixades")
)
_(GIT_SYNCED_PASSWORDS,
    __(ENG, "Passwords synced"),
    __(ESP, "Contraseñas sincronizadas"),
    __(VAL, "Contrasenyes sincronitzades")
)

_(GIT_ERROR_PUSHING_PASSWORDS,
    __(ENG, "Error uploading"),
    __(ESP, "Error subiendo"),
    __(VAL, "Error pujant")
)
_(GIT_ERROR_PULLING_PASSWORDS,
    __(ENG, "Error downloading"),
    __(ESP, "Error descargando"),
    __(VAL, "Error baixant")
)
_(GIT_ERROR_SYNCING_PASSWORDS,
    __(ENG, "Error syncing"),
    __(ESP, "Error sincronizando"),
    __(VAL, "Error sincronitzant")
)

_(GIT_ADDED_PASSWORD_COMMIT_MESSAGE,
    __(ENG, "Clavis: Added password '{0}' to Password Store"),
    __(ESP, "Clavis: Añadido contraseña '{0}' al Repositorio de Contraseñas"),
    __(VAL, "Clavis: Afegit contrasenya '{0}' al Repositori de Contrasenyes")
)

_(GIT_REMOVED_PASSWORD_COMMIT_MESSAGE,
    __(ENG, "Clavis: Removed password '{0}' from Password Store"),
    __(ESP, "Clavis: Eliminado contraseña '{0}' del Repositorio de Contraseñas"),
    __(VAL, "Clavis: Eliminat contrasenya '{0}' del Repositori de Contrasenyes")
)

_(GIT_REMOVED_DIRECTORY_COMMIT_MESSAGE,
    __(ENG, "Clavis: Removed folder '{0}' from Password Store"),
    __(ESP, "Clavis: Eliminado carpeta '{0}' del Repositorio de Contraseñas"),
    __(VAL, "Clavis: Eliminat carpeta '{0}' del Repositori de Contrasenyes")
)

_(GIT_MOVED_ELEMENT_COMMIT_MESSAGE,
    __(ENG, "Clavis: Moved '{0}' to '{1}'"),
    __(ESP, "Clavis: Movido '{0}' a '{1}'"),
    __(VAL, "Clavis: Mogut '{0}' a '{1}'")
)

_(GIT_IMPORTED_PASSWORD_STORE_COMMIT_MESSAGE,
    __(ENG, "Clavis: Imported password store '{0}'"),
    __(ESP, "Clavis: Importado repositorio de contraseñas '{0}'"),
    __(VAL, "Clavis: Importat repositori de contrasenyes '{0}'")
)

_(ERROR_GIT_NOT_INITIALIZED,
    __(ENG, "Git repository not initialized"),
    __(ESP, "Repositorio Git no inicializado"),
    __(VAL, "Repositori Git no inicialitzat")
)

_(ERROR_GIT_OPERATION_FAILED,
    __(ENG, "Git operation failed. {0}"),
    __(ESP, "La operación de Git falló. {0}"),
    __(VAL, "L'operació de Git ha fallat. {0}")
)

#pragma endregion

#pragma region Create GPG Key Palette


_(CREATE_NEW_KEY_PALETTE_TITLE,
    __(ENG, "Clavis Master Key creation"),
    __(ESP, "Crear Clave Maestra de Clavis"),
    __(VAL, "Crear Clau Mestra de Clavis")
)
_(CREATE_NEW_KEY_PALETTE_MAIN_LABEL,
    __(ENG, "Configure your new GPG key:"),
    __(ESP, "Configura tu nueva clave GPG:"),
    __(VAL, "Configura la teva nova clau GPG:")
)
_(CREATE_NEW_KEY_PALETTE_PERSONAL_INFORMATION_LABEL,
    __(ENG, "Personal information:"),
    __(ESP, "Información personal:"),
    __(VAL, "Informació personal:")
)
_(CREATE_NEW_KEY_PALETTE_USERNAME_PLACEHOLDER,
    __(ENG, "Your name"),
    __(ESP, "Tu nombre"),
    __(VAL, "El teu nom")
)
_(CREATE_NEW_KEY_PALETTE_KEY_NAME_PLACEHOLDER,
    __(ENG, "Key name"),
    __(ESP, "Nombre de la clave"),
    __(VAL, "Nom de la clau")
)
_(CREATE_NEW_KEY_PALETTE_COMMENT_PLACEHOLDER,
    __(ENG, "Comment (Optional)"),
    __(ESP, "Comentario (Opcional)"),
    __(VAL, "Comentari (Opcional)")
)
_(CREATE_NEW_KEY_SET_KEY_PASSWORD_LABEL,
    __(ENG, "Set key's passord:"),
    __(ESP, "Contraseña de la clave:"),
    __(VAL, "Contrasenya de la clau:")
)
_(CREATE_NEW_KEY_PASSWORD_PLACEHOLDER,
    __(ENG, "Password"),
    __(ESP, "Contraseña"),
    __(VAL, "Contrasenya")
)
_(CREATE_NEW_KEY_REPEAT_PASSWORD_PLACEHOLDER,
    __(ENG, "Repeat password"),
    __(ESP, "Repetir contraseña"),
    __(VAL, "Repetir contrasenya")
)

_(CREATE_NEW_KEY_ADVANCED_OPTIONS_LABEL,
    __(ENG, "Show advanced options"),
    __(ESP, "Mostrar opciones avanzadas"),
    __(VAL, "Mostrar opcions avançades")
)
_(CREATE_NEW_KEY_KEY_TYPE_LABEL,
    __(ENG, "Key type:"),
    __(ESP, "Tipo de clave:"),
    __(VAL, "Tipus de clau:")
)
_(CREATE_NEW_KEY_KEY_LENGTH_LABEL,
    __(ENG, "Key length:"),
    __(ESP, "Longitud de la clave:"),
    __(VAL, "Longitud de la clau:")
)
_(DO_CREATE_KEY_BUTTON,
    __(ENG, "Create key"),
    __(ESP, "Crear clave"),
    __(VAL, "Crear clau")
)

#pragma endregion

#pragma region First Run Workflow

// FIRST WINDOW
_(FIRST_RUN_WORKFLOW_TITLE,
    __(ENG, "Welcome to Clavis!"),
    __(ESP, "¡Bienvenido a Clavis!"),
    __(VAL, "Benvingut a Clavis!")
)

_(FIRST_RUN_WORKFLOW_LABEL_1,
    __(ENG, "Hey! It seems this is your first time running Clavis."),
    __(ESP, "¡Hola! Parece que es tu primera vez usando Clavis."),
    __(VAL, "Hola! Pareix que es la primera vegada que uses Clavis.")
)

_(FIRST_RUN_WORKFLOW_LABEL_2,
    __(ENG, "Initialize Password Store?"),
    __(ESP, "¿Inicializar el Repositorio de Contraseñas?"),
    __(VAL, "Inicialitzar el Repositori de Contrasenyes?")
)

// SECOND WINDOW (PATH SELECTION)
_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_TITLE,
    __(ENG, "Choose passwords location"),
    __(ESP, "Localización de las contraseñas"),
    __(VAL, "Localització de les contrasenyes")
)
_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_LABEL_TEXT,
    __(ENG, "Choose where to store the passwords:"),
    __(ESP, "Elige dónde guardar las contraseñas:"),
    __(VAL, "Tria on guardar les contrasenyes:")
)

_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_DOES_NOT_EXIST_INFO_LABEL,
    __(ENG, "Folder does not exist and will be created"),
    __(ESP, "La carpeta no existe y será creada"),
    __(VAL, "La carpeta no existeix i serà creada")
)
_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_PATH_IS_FILE_INFO_LABEL,
    __(ENG, "The given path is a file"),
    __(ESP, "La dirección dada es un archivo"),
    __(VAL, "La direcció donada es un arxiu")
)
_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_EMPTY_INFO_LABEL,
    __(ENG, "Folder already exists and is empty"),
    __(ESP, "La carpeta ya existe y está vacía"),
    __(VAL, "La carpeta ja existeix i està buida")
)
_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_NOT_EMPTY_INFO_LABEL,
    __(ENG, "Folder already exists and is not empty"),
    __(ESP, "La carpeta ya existe y no está vacía"),
    __(VAL, "La carpeta ja existeix i no està buida")
)
_(FIRST_RUN_CHOOSE_PASSWORD_STORE_PATH_FOLDER_IS_PASSWORDSTORE_INFO_LABEL,
    __(ENG, "Folder already exists and contains a Password Store"),
    __(ESP, "La carpeta ya existe y contiene un Repositorio de Contraseñas"),
    __(VAL, "La carpeta ja existeix i conté un Repositori de Contrasenyes")
)

// THIRD WINDOW (GPG KEY)
_(FIRST_RUN_GPG_KEY_PALETTE_TITLE,
    __(ENG, "Select your Clavis Master Key"),
    __(ESP, "Selecciona tu Clave Maestra de Clavis"),
    __(VAL, "Selecciona la teua Clau Mestra de Clavis")
)
_(FIRST_RUN_GPG_KEY_PALETTE_LABEL,
    __(ENG, "Choose a GPG key, import one or create a new one"),
    __(ESP, "Selecciona una clave GPG, impórtala o crea una nueva"),
    __(VAL, "Selecciona una clau GPG, importa-la o crea una nova")
)
_(GPG_KEY_PALETTE_IMPORT_KEY_BUTTON,
    __(ENG, "Import key"),
    __(ESP, "Importar clave"),
    __(VAL, "Importar clau")
)
_(GPG_KEY_PALETTE_EXPORT_KEY_BUTTON,
    __(ENG, "Export key"),
    __(ESP, "Exportar clave"),
    __(VAL, "Exportar clau")
)
_(IMPORT_GPG_KEY_TITLE,
    __(ENG, "Import GPG key"),
    __(ESP, "Importar clave GPG"),
    __(VAL, "Importar clau GPG")
)
_(EXPORT_GPG_KEY_TITLE,
    __(ENG, "Export GPG key"),
    __(ESP, "Exportar clave GPG"),
    __(VAL, "Exportar clau GPG")
)
_(DO_EXPORT_PRIVATE_KEY_CHECKBUTTON,
    __(ENG, "Include private key"),
    __(ESP, "Incluir clave privada"),
    __(VAL, "Incluïr clau privada")
)

// EXPORT PASSWORD STORE PALETTE
_(EXPORT_PASSWORD_STORE_PALETTE_TITLE,
    __(ENG, "Export Password Store"),
    __(ESP, "Exportar Repositorio de Contraseñas"),
    __(VAL, "Exportar Repositori de Contrasenyes")
)
_(EXPORT_PASSWORD_STORE_PALETTE_FOLDER_LABEL,
    __(ENG, "Folder:"),
    __(ESP, "Carpeta:"),
    __(VAL, "Carpeta:")
)
_(EXPORT_PASSWORD_STORE_PALETTE_PATH_PLACEHOLDER,
    __(ENG, "Output file path..."),
    __(ESP, "Ruta del archivo de salida..."),
    __(VAL, "Ruta del fitxer de eixida...")
)
_(EXPORT_PASSWORD_STORE_PALETTE_ENCRYPTION_LABEL,
    __(ENG, "Encryption"),
    __(ESP, "Cifrado"),
    __(VAL, "Xifratge")
)
_(EXPORT_PASSWORD_STORE_PALETTE_NO_ENCRYPTION,
    __(ENG, "No encryption"),
    __(ESP, "Sin cifrado"),
    __(VAL, "Sense xifratge")
)
_(EXPORT_PASSWORD_STORE_PALETTE_ENCRYPT_PASSWORD,
    __(ENG, "Encrypt with password"),
    __(ESP, "Cifrar con contraseña"),
    __(VAL, "Xifrar amb contrasenya")
)
_(EXPORT_PASSWORD_STORE_PALETTE_ENCRYPT_GPG_KEY,
    __(ENG, "Encrypt with GPG key"),
    __(ESP, "Cifrar con clave GPG"),
    __(VAL, "Xifrar amb clau GPG")
)
_(EXPORT_PASSWORD_STORE_PALETTE_PASSWORD_PLACEHOLDER,
    __(ENG, "Encryption password..."),
    __(ESP, "Contraseña de cifrado..."),
    __(VAL, "Contrasenya de xifratge...")
)
_(EXPORT_PASSWORD_STORE_PALETTE_SHOW_PASSWORD,
    __(ENG, "Show"),
    __(ESP, "Mostrar"),
    __(VAL, "Mostrar")
)
_(EXPORT_PASSWORD_STORE_PALETTE_HIDE_PASSWORD,
    __(ENG, "Hide"),
    __(ESP, "Ocultar"),
    __(VAL, "Ocultar")
)
_(GPG_KEY_PALETTE_CREATE_NEW_KEY_BUTTON,
    __(ENG, "Create new key"),
    __(ESP, "Crear nueva clave"),
    __(VAL, "Crear nova clau")
)

// FOURTH WINDOW (GIT SERVER CONFIG)
_(GIT_SERVER_CONFIG_TITLE,
    __(ENG, "Configure Git server"),
    __(ESP, "Configurar servidor de Git"),
    __(VAL, "Configurar servidor de Git")
)

_(GIT_SERVER_CONFIG_CHOOSE_METHOD_LABEL,
    __(ENG, "Choose how to use Git:"),
    __(ESP, "Elige cómo usar Git"),
    __(VAL, "Tria com usar git")
)

_(GIT_SERVER_CONFIG_DO_NOT_USE_CHECKBUTTON,
    __(ENG, "Do not setup Git"),
    __(ESP, "No configurar Git"),
    __(VAL, "No configurar Git"),
)

_(GIT_SERVER_CONFIG_CREATE_A_NEW_REPO_LABEL,
    __(ENG, "Create a new Git Repository"),
    __(ESP, "Crear un nuevo Repositorio de Git"),
    __(VAL, "Crear un nou Repositori de Git")
)

_(GIT_SERVER_CONFIG_CLONE_AN_EXISTING_REPO_LABEL,
    __(ENG, "Clone an existing Git Repository"),
    __(ESP, "Clonar un Repositorio de Git existente"),
    __(VAL, "Clonar un Repositori de Git existent")
)

_(GIT_SERVER_CONFIG_USER_SETTINGS_LABEL,
    __(ENG, "Git user settings:"),
    __(ESP, "Configuración del usuario de Git:"),
    __(VAL, "Configuració de l'usuari de Git:")
)

_(GIT_SERVER_CONFIG_LOCATION_LABEL,
    __(ENG, "Location:"),
    __(ESP, "Ubicación:"),
    __(VAL, "Ubicació")
)

_(GIT_SERVER_CONFIG_GIT_UPSTREAM_LABEL,
    __(ENG, "Upstream:"),
    __(ESP, "Upstream:"),
    __(VAL, "Upstream:")
)

_(GIT_SERVER_CONFIG_GIT_USERNAME_LABEL,
    __(ENG, "Git user:"),
    __(ESP, "Usuario de Git:"),
    __(VAL, "Usuari de Git:")
)

_(GIT_SERVER_CONFIG_GIT_EMAIL_LABEL,
    __(ENG, "Email:"),
    __(ESP, "Email:"),
    __(VAL, "Email:")
)

#pragma endregion

#pragma region GPG Wrapper

_(ERROR_INVALID_KEY_TYPE,
    __(ENG, "Invalid key type: {0}"),
    __(ESP, "Tipo de clave inválido: {0}"),
    __(VAL, "Tipus de clau invàlid: {0}")
)

_(UNABLE_TO_PARSE_KEYTYPE_CODE,
    __(ENG, "Unable to parse keytype code: {0}"),
    __(ESP, "No se pudo parsear el código de clave: {0}"),
    __(VAL, "No s'ha pogut parsejar el codi de la clau: {0}")
)

_(MISC_RECOMMENDED,
    __(ENG, "Recommended"),
    __(ESP, "Recomendado"),
    __(VAL, "Recomanat")
)

_(ERROR_GPG_KEY_IS_NOT_VALID,
    __(ENG, "Key is not valid"),
    __(ESP, "La clave no es válida"),
    __(ENG, "La clau no es vàlida")
)

_(ERROR_FAILED_CREATING_KEY,
    __(ENG, "Could not create key:\n{0}"),
    __(ESP, "No se pudo crear la clave:\n{0}"),
    __(VAL, "No s'ha pogut crear la clau: \n{0}")
)

_(ERROR_UNABLE_TO_EXPORT_KEY,
    __(ENG, "Could not export the key"),
    __(ESP, "No se pudo exportar la clave"),
    __(VAL, "No s'ha pogut exportar la clau")
)

_(ERROR_UNABLE_TO_IMPORT_KEY,
    __(ENG, "Could not import the key"),
    __(ESP, "No se pudo importar la clave"),
    __(VAL, "No s'ha pogut importar la clau")
)

_(ERROR_INVALID_TRUST_LEVEL,
    __(ENG, "Invalid trust level: {0}. Must be between 1 and 5"),
    __(ESP, "Nivel de confianza inválido: {0}. Debe estar entre 1 y 5"),
    __(VAL, "Nivell de confiança invàlid: {0}. Deu estar entre 1 i 5")
)

#pragma endregion

#pragma region Misc Errors

_(ERROR_COULD_NOT_WRITE_FILE,
    __(ENG, "Could not write file: {0}"),
    __(ESP, "No se ha podido escribir el archivo: {0}"),
    __(VAL, "No s'ha pogut escriure el fitxer: {0}")
)

_(ERROR_COULD_NOT_READ_FILE,
    __(ENG, "Could not read file: {0}"),
    __(ESP, "No se ha podido leer el archivo: {0}"),
    __(VAL, "No s'ha pogut escriure el fitxer: {0}")
)

_(ERROR_EXPORT_PATH_EMPTY,
    __(ENG, "Export path cannot be empty."),
    __(ESP, "La ruta de exportación no puede estar vacía."),
    __(VAL, "La ruta d'exportació no pot estar buida.")
)

_(ERROR_EXPORT_FAILED,
    __(ENG, "Failed to export password store."),
    __(ESP, "Error al exportar el repositorio de contraseñas."),
    __(VAL, "Error en exportar el repositori de contrasenyes.")
)

#pragma endregion

#pragma region Import Password Store Palette

_(IMPORT_PASSWORD_STORE_PALETTE_TITLE,
    __(ENG, "Import Password Store"),
    __(ESP, "Importar Repositorio de Contraseñas"),
    __(VAL, "Importar Repositori de Contrasenyes")
)
_(IMPORT_PASSWORD_STORE_PALETTE_PATH_PLACEHOLDER,
    __(ENG, "Path to .clav file..."),
    __(ESP, "Ruta del archivo .clav..."),
    __(VAL, "Ruta del fitxer .clav...")
)
_(IMPORT_PASSWORD_STORE_PALETTE_PASSWORD_PLACEHOLDER,
    __(ENG, "Decryption password..."),
    __(ESP, "Contraseña de descifrado..."),
    __(VAL, "Contrasenya de desxifratge...")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_NOT_A_CLAV,
    __(ENG, "Not a valid .clav file"),
    __(ESP, "No es un archivo .clav válido"),
    __(VAL, "No és un fitxer .clav vàlid")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_UNSUPPORTED_VERSION,
    __(ENG, "Unsupported .clav file version"),
    __(ESP, "Versión de archivo .clav no soportada"),
    __(VAL, "Versió de fitxer .clav no suportada")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_KEY_MISMATCH,
    __(ENG, "The passwords in this file are encrypted for a different GPG key"),
    __(ESP, "Las contraseñas de este archivo están cifradas para una clave GPG diferente"),
    __(VAL, "Les contrasenyes d'aquest fitxer estan xifrades per a una clau GPG diferent")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_CANNOT_DECRYPT,
    __(ENG, "Cannot decrypt: the required GPG key is not available"),
    __(ESP, "No se puede descifrar: la clave GPG necesaria no está disponible"),
    __(VAL, "No es pot desxifrar: la clau GPG necessària no està disponible")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_VALID_NO_ENCRYPTION,
    __(ENG, "Valid .clav file (not encrypted)"),
    __(ESP, "Archivo .clav válido (sin cifrado)"),
    __(VAL, "Fitxer .clav vàlid (sense xifratge)")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_VALID_GPG,
    __(ENG, "Valid .clav file, encrypted with your GPG key"),
    __(ESP, "Archivo .clav válido, cifrado con tu clave GPG"),
    __(VAL, "Fitxer .clav vàlid, xifrat amb la teua clau GPG")
)
_(IMPORT_PASSWORD_STORE_PALETTE_STATUS_PASSWORD_ENCRYPTED,
    __(ENG, "Password-encrypted. Enter the password to import."),
    __(ESP, "Cifrado con contraseña. Introduce la contraseña para importar."),
    __(VAL, "Xifrat amb contrasenya. Introduïx la contrasenya per a importar.")
)
_(ERROR_IMPORT_FAILED,
    __(ENG, "Failed to import .clav file."),
    __(ESP, "Error al importar el archivo .clav."),
    __(VAL, "Error en importar el fitxer .clav.")
)
_(ERROR_IMPORT_WRONG_PASSWORD,
    __(ENG, "Wrong password."),
    __(ESP, "Contraseña incorrecta."),
    __(VAL, "Contrasenya incorrecta.")
)
_(ERROR_IMPORT_GPG_KEY_MISMATCH,
    __(ENG, "These passwords were encrypted for a different GPG key."),
    __(ESP, "Estas contraseñas están cifradas para una clave GPG diferente."),
    __(VAL, "Aquestes contrasenyes estan xifrades per a una clau GPG diferent.")
)

#pragma endregion

#pragma region Settings Palette

_(SETTINGS_PALETTE_TITLE,
    __(ENG, "Settings"),
    __(ESP, "Configuración"),
    __(VAL, "Configuració")
)

_(SETTINGS_SECTION_GENERAL,
    __(ENG, "General"),
    __(ESP, "General"),
    __(VAL, "General")
)
_(SETTINGS_SECTION_APPEARANCE,
    __(ENG, "Appearance"),
    __(ESP, "Apariencia"),
    __(VAL, "Aparença")
)
_(SETTINGS_SECTION_PASSWORD_STORE,
    __(ENG, "Password Store"),
    __(ESP, "Repositorio de Contraseñas"),
    __(VAL, "Repositori de Contrasenyes")
)
_(SETTINGS_SECTION_SECURITY,
    __(ENG, "Security"),
    __(ESP, "Seguridad"),
    __(VAL, "Seguretat")
)
_(SETTINGS_SECTION_PASSWORD_BROWSER,
    __(ENG, "Password Browser"),
    __(ESP, "Explorador de Contraseñas"),
    __(VAL, "Explorador de Contrasenyes")
)
_(SETTINGS_SECTION_PASSWORD_GENERATOR,
    __(ENG, "Password Generator"),
    __(ESP, "Generador de Contraseñas"),
    __(VAL, "Generador de Contrasenyes")
)
_(SETTINGS_SECTION_DEVELOPER,
    __(ENG, "Developer"),
    __(ESP, "Desarrollador"),
    __(VAL, "Desenvolupador")
)

_(SETTINGS_LANGUAGE_LABEL,
    __(ENG, "Language"),
    __(ESP, "Idioma"),
    __(VAL, "Idioma")
)
_(SETTINGS_DO_USE_DARK_THEME_LABEL,
    __(ENG, "Use dark theme"),
    __(ESP, "Usar tema oscuro"),
    __(VAL, "Usar tema fosc")
)
_(SETTINGS_WINDOW_DECORATIONS_LABEL,
    __(ENG, "Window decorations"),
    __(ESP, "Decoraciones de ventana"),
    __(VAL, "Decoracions de finestra")
)
_(SETTINGS_WINDOW_DECORATIONS_CLAVIS_CSD,
    __(ENG, "Clavis (custom title bar)"),
    __(ESP, "Clavis (barra de título personalizada)"),
    __(VAL, "Clavis (barra de títol personalitzada)")
)
_(SETTINGS_WINDOW_DECORATIONS_GTK_CSD,
    __(ENG, "System (GTK default)"),
    __(ESP, "Sistema (GTK por defecto)"),
    __(VAL, "Sistema (GTK per defecte)")
)
_(SETTINGS_WINDOW_DECORATIONS_FORCE_NO_CSD,
    __(ENG, "Server-side decorations"),
    __(ESP, "Decoraciones del servidor"),
    __(VAL, "Decoracions del servidor")
)
_(SETTINGS_FORCE_CLAVIS_STYLE_LABEL,
    __(ENG, "Force Clavis menu style"),
    __(ESP, "Forzar estilo de menú Clavis"),
    __(VAL, "Forçar estil de menú Clavis")
)
_(SETTINGS_DISABLE_SHADOWS_LABEL,
    __(ENG, "Disable window shadows"),
    __(ESP, "Desactivar sombras de ventana"),
    __(VAL, "Desactivar ombres de finestra")
)
_(SETTINGS_CLAVIS_FONT_LABEL,
    __(ENG, "Font"),
    __(ESP, "Fuente"),
    __(VAL, "Font")
)
_(SETTINGS_CLAVIS_THEME_LABEL,
    __(ENG, "Theme"),
    __(ESP, "Tema"),
    __(VAL, "Tema")
)
_(SETTINGS_PASSWORD_STORE_PATH_LABEL,
    __(ENG, "Password store path"),
    __(ESP, "Ruta del repositorio de contraseñas"),
    __(VAL, "Ruta del repositori de contrasenyes")
)
_(SETTINGS_SHOW_HIDDEN_FILES_LABEL,
    __(ENG, "Show hidden files"),
    __(ESP, "Mostrar archivos ocultos"),
    __(VAL, "Mostrar fitxers ocults")
)
_(SETTINGS_CLIPBOARD_CLEAR_SECONDS_LABEL,
    __(ENG, "Clear clipboard after (seconds, 0 = never)"),
    __(ESP, "Borrar portapapeles tras (segundos, 0 = nunca)"),
    __(VAL, "Esborrar porta-retalls després de (segons, 0 = mai)")
)
_(SETTINGS_CLIPBOARD_CLEAR_ENABLED_LABEL,
    __(ENG, "Clear clipboard after copy"),
    __(ESP, "Borrar portapapeles al copiar"),
    __(VAL, "Esborrar porta-retalls en copiar")
)
_(SETTINGS_CLEAR_PASSWORD_DISPLAY_LABEL,
    __(ENG, "Clear decrypted password after display"),
    __(ESP, "Borrar contraseña descifrada tras mostrarla"),
    __(VAL, "Esborrar contrasenya desxifrada després de mostrar-la")
)
_(SETTINGS_CLIPBOARD_CLEAR_DELAY_LABEL,
    __(ENG, "Delay (seconds)"),
    __(ESP, "Retardo (segundos)"),
    __(VAL, "Retard (segons)")
)
_(SETTINGS_FILTER_CASE_SENSITIVE_LABEL,
    __(ENG, "Case-sensitive search"),
    __(ESP, "Búsqueda sensible a mayúsculas"),
    __(VAL, "Cerca sensible a majúscules")
)
_(SETTINGS_PASSWORD_GENERATOR_LENGTH_LABEL,
    __(ENG, "Default password length"),
    __(ESP, "Longitud de contraseña por defecto"),
    __(VAL, "Longitud de contrasenya per defecte")
)
_(SETTINGS_PASSWORD_GENERATOR_LOWERCASE_LABEL,
    __(ENG, "Use lowercase letters"),
    __(ESP, "Usar letras minúsculas"),
    __(VAL, "Usar lletres minúscules")
)
_(SETTINGS_PASSWORD_GENERATOR_UPPERCASE_LABEL,
    __(ENG, "Use uppercase letters"),
    __(ESP, "Usar letras mayúsculas"),
    __(VAL, "Usar lletres majúscules")
)
_(SETTINGS_PASSWORD_GENERATOR_NUMERALS_LABEL,
    __(ENG, "Use numerals"),
    __(ESP, "Usar números"),
    __(VAL, "Usar números")
)
_(SETTINGS_PASSWORD_GENERATOR_SYMBOLS_LABEL,
    __(ENG, "Use symbols"),
    __(ESP, "Usar símbolos"),
    __(VAL, "Usar símbols")
)
_(SETTINGS_PASSWORD_GENERATOR_PRONOUNCEABLE_LABEL,
    __(ENG, "Generate pronounceable passwords"),
    __(ESP, "Generar contraseñas pronunciables"),
    __(VAL, "Generar contrasenyes pronunciables")
)
_(SETTINGS_RUN_GTK_CSS_INSPECTOR_LABEL,
    __(ENG, "Show GTK CSS inspector on startup"),
    __(ESP, "Mostrar inspector CSS de GTK al inicio"),
    __(VAL, "Mostrar inspector CSS de GTK a l'inici")
)

#pragma endregion

#pragma region Two Factor Authentication

_(PASSWORDSTORE_DECRYPTED_TWO_FACTOR_LABEL,
    __(ENG, "Two-factor code"),
    __(ESP, "Código de doble factor"),
    __(VAL, "Codi de doble factor")
)

_(PASSWORDSTORE_DECRYPTION_FAILED_HINT,
    __(ENG, "Decryption failed"),
    __(ESP, "Fallo al descifrar"),
    __(VAL, "Ha fallat el desxifrat")
)

_(PASSWORDSTORE_TWO_FACTOR_OUTPUT_HINT,
    __(ENG, "Select a 2FA entry to generate a code"),
    __(ESP, "Selecciona una entrada 2FA para generar un código"),
    __(VAL, "Selecciona una entrada 2FA per a generar un codi")
)

_(PASSWORDSTORE_TWO_FACTOR_SECONDS_REMAINING,
    __(ENG, "{0}s"),
    __(ESP, "{0}s"),
    __(VAL, "{0}s")
)

_(PASSWORDSTORE_TWO_FACTOR_COUNTER_LABEL,
    __(ENG, "#{0}"),
    __(ESP, "#{0}"),
    __(VAL, "#{0}")
)

_(PASSWORDSTORE_TWO_FACTOR_NEXT_CODE_TOOLTIP,
    __(ENG, "Generate the next code. This advances the counter and records a commit."),
    __(ESP, "Generar el siguiente código. Esto avanza el contador y registra un commit."),
    __(VAL, "Generar el codi següent. Això avança el comptador i registra un commit.")
)

_(PASSWORDSTORE_MANAGER_NEW_TWO_FACTOR_TOOLTIP,
    __(ENG, "New 2FA"),
    __(ESP, "Nuevo 2FA"),
    __(VAL, "Nou 2FA")
)

_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_EDIT_TWO_FACTOR_BUTTON,
    __(ENG, "Edit 2FA"),
    __(ESP, "Editar 2FA"),
    __(VAL, "Editar 2FA")
)

_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_SHOW_DETAILS_BUTTON,
    __(ENG, "Show details"),
    __(ESP, "Ver detalles"),
    __(VAL, "Veure detalls")
)

_(PASSWORD_STORE_MANAGER_CONTEXT_MENU_TRANSFER_TWO_FACTOR_BUTTON,
    __(ENG, "Transfer 2FA"),
    __(ESP, "Transferir 2FA"),
    __(VAL, "Transferir 2FA")
)

_(NEW_TWO_FACTOR_PALETTE_TITLE,
    __(ENG, "New 2FA"),
    __(ESP, "Nuevo 2FA"),
    __(VAL, "Nou 2FA")
)

_(NEW_TWO_FACTOR_PALETTE_EDIT_TITLE,
    __(ENG, "Edit 2FA"),
    __(ESP, "Editar 2FA"),
    __(VAL, "Editar 2FA")
)

_(NEW_TWO_FACTOR_PALETTE_LABEL_TITLE,
    __(ENG, "Add a two-factor code"),
    __(ESP, "Añadir un código de doble factor"),
    __(VAL, "Afegir un codi de doble factor")
)

_(NEW_TWO_FACTOR_PALETTE_EDIT_LABEL_TITLE,
    __(ENG, "Edit this two-factor code"),
    __(ESP, "Editar este código de doble factor"),
    __(VAL, "Editar aquest codi de doble factor")
)

_(NEW_TWO_FACTOR_PALETTE_NAME_LABEL,
    __(ENG, "Set the 2FA name:"),
    __(ESP, "Nombre del 2FA:"),
    __(VAL, "Nom del 2FA:")
)

_(NEW_TWO_FACTOR_PALETTE_CURRENT_NAME_LABEL,
    __(ENG, "Current 2FA name:"),
    __(ESP, "Nombre actual del 2FA:"),
    __(VAL, "Nom actual del 2FA:")
)

_(NEW_TWO_FACTOR_PALETTE_URI_LABEL,
    __(ENG, "Paste the secret/URI:"),
    __(ESP, "Pega el secreto/URI:"),
    __(VAL, "Apega el secret/URI:")
)

_(NEW_TWO_FACTOR_PALETTE_PASTE_TOOLTIP,
    __(ENG, "Paste from clipboard"),
    __(ESP, "Pegar del portapapeles"),
    __(VAL, "Apegar del porta-retalls")
)

_(NEW_TWO_FACTOR_PALETTE_DETAILS_LABEL,
    __(ENG, "Advanced Settings"),
    __(ESP, "Ajustes avanzados"),
    __(VAL, "Ajustos avançats")
)

// Separate from the plain issuer/account labels, which the read-only details window reuses.
_(NEW_TWO_FACTOR_PALETTE_ISSUER_OPTIONAL_LABEL,
    __(ENG, "Issuer (optional):"),
    __(ESP, "Emisor (opcional):"),
    __(VAL, "Emissor (opcional):")
)

_(NEW_TWO_FACTOR_PALETTE_ACCOUNT_OPTIONAL_LABEL,
    __(ENG, "Account (optional):"),
    __(ESP, "Cuenta (opcional):"),
    __(VAL, "Compte (opcional):")
)

_(NEW_TWO_FACTOR_PALETTE_TYPE_LABEL,
    __(ENG, "Type:"),
    __(ESP, "Tipo:"),
    __(VAL, "Tipus:")
)

_(NEW_TWO_FACTOR_PALETTE_ISSUER_LABEL,
    __(ENG, "Issuer:"),
    __(ESP, "Emisor:"),
    __(VAL, "Emissor:")
)

_(NEW_TWO_FACTOR_PALETTE_ACCOUNT_LABEL,
    __(ENG, "Account:"),
    __(ESP, "Cuenta:"),
    __(VAL, "Compte:")
)

_(NEW_TWO_FACTOR_PALETTE_SECRET_LABEL,
    __(ENG, "Secret (Base32):"),
    __(ESP, "Secreto (Base32):"),
    __(VAL, "Secret (Base32):")
)

_(NEW_TWO_FACTOR_PALETTE_ALGORITHM_LABEL,
    __(ENG, "Algorithm:"),
    __(ESP, "Algoritmo:"),
    __(VAL, "Algorisme:")
)

_(NEW_TWO_FACTOR_PALETTE_DIGITS_LABEL,
    __(ENG, "Digits:"),
    __(ESP, "Dígitos:"),
    __(VAL, "Dígits:")
)

_(NEW_TWO_FACTOR_PALETTE_PERIOD_LABEL,
    __(ENG, "Period (s):"),
    __(ESP, "Periodo (s):"),
    __(VAL, "Període (s):")
)

_(NEW_TWO_FACTOR_PALETTE_COUNTER_LABEL,
    __(ENG, "Counter:"),
    __(ESP, "Contador:"),
    __(VAL, "Comptador:")
)

_(NEW_TWO_FACTOR_PALETTE_RECOVERY_LABEL,
    __(ENG, "Recovery codes:"),
    __(ESP, "Códigos de recuperación:"),
    __(VAL, "Codis de recuperació:")
)

_(NEW_TWO_FACTOR_PALETTE_RECOVERY_ADD_HINT,
    __(ENG, "Add a recovery code, or paste a list"),
    __(ESP, "Añade un código de recuperación, o pega una lista"),
    __(VAL, "Afig un codi de recuperació, o apega una llista")
)

_(NEW_TWO_FACTOR_PALETTE_RECOVERY_ADD_TOOLTIP,
    __(ENG, "Add this recovery code"),
    __(ESP, "Añadir este código de recuperación"),
    __(VAL, "Afegir aquest codi de recuperació")
)

_(NEW_TWO_FACTOR_PALETTE_RECOVERY_DELETE_TOOLTIP,
    __(ENG, "Remove this recovery code"),
    __(ESP, "Eliminar este código de recuperación"),
    __(VAL, "Eliminar aquest codi de recuperació")
)

_(NEW_TWO_FACTOR_PALETTE_NOTES_LABEL,
    __(ENG, "Notes:"),
    __(ESP, "Notas:"),
    __(VAL, "Notes:")
)

_(NEW_TWO_FACTOR_PALETTE_PREVIEW_LABEL,
    __(ENG, "Current code: {0}"),
    __(ESP, "Código actual: {0}"),
    __(VAL, "Codi actual: {0}")
)

_(NEW_TWO_FACTOR_PALETTE_COPY_CODE_TOOLTIP,
    __(ENG, "Copy the current code"),
    __(ESP, "Copiar el código actual"),
    __(VAL, "Copiar el codi actual")
)

_(NEW_TWO_FACTOR_PALETTE_PREVIEW_INVALID,
    __(ENG, "Enter a valid secret to preview the code"),
    __(ESP, "Introduce un secreto válido para previsualizar el código"),
    __(VAL, "Introduïx un secret vàlid per a previsualitzar el codi")
)

_(NEW_TWO_FACTOR_PALETTE_ELEMENT_ALREADY_EXISTS_TEXT,
    __(ENG, "A 2FA entry named '{0}' already exists. Overwrite it?"),
    __(ESP, "Ya existe una entrada 2FA llamada '{0}'. ¿Sobrescribirla?"),
    __(VAL, "Ja existix una entrada 2FA anomenada '{0}'. Voleu sobreescriure-la?")
)

_(TWO_FACTOR_DETAILS_PALETTE_TITLE,
    __(ENG, "2FA details: {0}"),
    __(ESP, "Detalles del 2FA: {0}"),
    __(VAL, "Detalls del 2FA: {0}")
)

_(TWO_FACTOR_DETAILS_PALETTE_RECOVERY_LABEL,
    __(ENG, "Recovery codes"),
    __(ESP, "Códigos de recuperación"),
    __(VAL, "Codis de recuperació")
)

_(TWO_FACTOR_DETAILS_PALETTE_NO_RECOVERY,
    __(ENG, "No recovery codes stored for this entry."),
    __(ESP, "No hay códigos de recuperación guardados para esta entrada."),
    __(VAL, "No hi ha codis de recuperació guardats per a aquesta entrada.")
)

_(TWO_FACTOR_DETAILS_PALETTE_NOTES_LABEL,
    __(ENG, "Notes"),
    __(ESP, "Notas"),
    __(VAL, "Notes")
)

_(TRANSFER_TWO_FACTOR_PALETTE_TITLE,
    __(ENG, "Transfer 2FA: {0}"),
    __(ESP, "Transferir 2FA: {0}"),
    __(VAL, "Transferir 2FA: {0}")
)

_(TRANSFER_TWO_FACTOR_PALETTE_INSTRUCTIONS,
    __(ENG, "Scan this code with another authenticator app to add the same 2FA there."),
    __(ESP, "Escanea este código con otra aplicación de autenticación para añadir el mismo 2FA."),
    __(VAL, "Escaneja aquest codi amb una altra aplicació d'autenticació per a afegir el mateix 2FA.")
)

_(TRANSFER_TWO_FACTOR_PALETTE_WARNING,
    __(ENG, "This code contains your shared secret. Anyone who sees it can generate your codes."),
    __(ESP, "Este código contiene tu secreto compartido. Cualquiera que lo vea puede generar tus códigos."),
    __(VAL, "Aquest codi conté el teu secret compartit. Qualsevol que el veja pot generar els teus codis.")
)

_(TRANSFER_TWO_FACTOR_PALETTE_URI_LABEL,
    __(ENG, "Or copy the URI:"),
    __(ESP, "O copia la URI:"),
    __(VAL, "O copia la URI:")
)

_(IMPORT_TWO_FACTOR_PALETTE_TITLE,
    __(ENG, "Import 2FA codes"),
    __(ESP, "Importar códigos 2FA"),
    __(VAL, "Importar codis 2FA")
)

_(IMPORT_TWO_FACTOR_PALETTE_LABEL,
    __(ENG, "Paste an otpauth-migration:// URI exported from another authenticator:"),
    __(ESP, "Pega una URI otpauth-migration:// exportada desde otro autenticador:"),
    __(VAL, "Apega una URI otpauth-migration:// exportada des d'un altre autenticador:")
)

_(IMPORT_TWO_FACTOR_PALETTE_PARSE_BUTTON,
    __(ENG, "Read codes"),
    __(ESP, "Leer códigos"),
    __(VAL, "Llegir codis")
)

_(IMPORT_TWO_FACTOR_PALETTE_UNSUPPORTED,
    __(ENG, "{0} account(s) use an algorithm Clavis cannot generate and were skipped."),
    __(ESP, "{0} cuenta(s) usan un algoritmo que Clavis no puede generar y se han omitido."),
    __(VAL, "{0} compte(s) usen un algorisme que Clavis no pot generar i s'han omés.")
)

_(IMPORT_TWO_FACTOR_PALETTE_SUMMARY,
    __(ENG, "Found {0} account(s). Choose which ones to import:"),
    __(ESP, "Se han encontrado {0} cuenta(s). Elige cuáles importar:"),
    __(VAL, "S'han trobat {0} compte(s). Tria quines importar:")
)

_(MAINMENU_FILE_MENU_NEW_TWO_FACTOR,
    __(ENG, "New 2FA"),
    __(ESP, "Nuevo 2FA"),
    __(VAL, "Nou 2FA")
)

_(MAINMENU_FILE_MENU_IMPORT_TWO_FACTOR,
    __(ENG, "Import 2FA codes"),
    __(ESP, "Importar códigos 2FA"),
    __(VAL, "Importar codis 2FA")
)

_(GIT_ADDED_TWO_FACTOR_COMMIT_MESSAGE,
    __(ENG, "Clavis: Added 2FA '{0}'"),
    __(ESP, "Clavis: Añadido 2FA '{0}'"),
    __(VAL, "Clavis: Afegit 2FA '{0}'")
)

_(GIT_EDITED_TWO_FACTOR_COMMIT_MESSAGE,
    __(ENG, "Clavis: Edited 2FA '{0}'"),
    __(ESP, "Clavis: Editado 2FA '{0}'"),
    __(VAL, "Clavis: Editat 2FA '{0}'")
)

_(GIT_ADVANCED_HOTP_COUNTER_COMMIT_MESSAGE,
    __(ENG, "Clavis: Advanced HOTP counter for '{0}'"),
    __(ESP, "Clavis: Avanzado el contador HOTP de '{0}'"),
    __(VAL, "Clavis: Avançat el comptador HOTP de '{0}'")
)

_(GIT_IMPORTED_TWO_FACTOR_COMMIT_MESSAGE,
    __(ENG, "Clavis: Imported {0} 2FA code(s)"),
    __(ESP, "Clavis: Importados {0} código(s) 2FA"),
    __(VAL, "Clavis: Importats {0} codi(s) 2FA")
)

_(PASSWORD_STORE_DATA_PALETTE_NUM_TWO_FACTOR_LABEL,
    __(ENG, "Number of 2FA codes:"),
    __(ESP, "Número de códigos 2FA:"),
    __(VAL, "Nombre de codis 2FA:")
)

_(NEW_PASSWORD_PALETTE_STORE_LOCKED_HINT,
    __(ENG, "Store locked - existing password not loaded. Saving will replace it."),
    __(ESP, "Almacén bloqueado - no se ha cargado la contraseña actual. Al guardar se reemplazará."),
    __(VAL, "Magatzem bloquejat - no s'ha carregat la contrasenya actual. En guardar es reemplaçarà.")
)

_(ERROR_COULD_NOT_LOCK_VAULT,
    __(ENG, "Could not clear the GPG key cache. The vault may still be unlocked."),
    __(ESP, "No se ha podido vaciar la caché de claves GPG. El almacén puede seguir desbloqueado."),
    __(VAL, "No s'ha pogut buidar la memòria cau de claus GPG. El magatzem pot continuar desbloquejat.")
)

_(ERROR_NOT_A_TWO_FACTOR,
    __(ENG, "'{0}': Not a 2FA entry"),
    __(ESP, "'{0}': No es una entrada 2FA"),
    __(VAL, "'{0}': No és una entrada 2FA")
)

_(ERROR_INVALID_TWO_FACTOR_FILE,
    __(ENG, "'{0}' is not a valid 2FA file. Its first line must be an otpauth:// URI."),
    __(ESP, "'{0}' no es un archivo 2FA válido. Su primera línea debe ser una URI otpauth://."),
    __(VAL, "'{0}' no és un fitxer 2FA vàlid. La seua primera línia ha de ser una URI otpauth://.")
)

_(ERROR_INVALID_OTPAUTH_URI,
    __(ENG, "That is not a valid otpauth:// URI."),
    __(ESP, "Esa no es una URI otpauth:// válida."),
    __(VAL, "Eixa no és una URI otpauth:// vàlida.")
)

_(ERROR_INVALID_BASE32_SECRET,
    __(ENG, "The secret is not valid Base32."),
    __(ESP, "El secreto no es Base32 válido."),
    __(VAL, "El secret no és Base32 vàlid.")
)

_(ERROR_QR_GENERATION_FAILED,
    __(ENG, "Could not generate the QR code."),
    __(ESP, "No se ha podido generar el código QR."),
    __(VAL, "No s'ha pogut generar el codi QR.")
)

_(ERROR_MIGRATION_PARSE_FAILED,
    __(ENG, "Could not read that migration URI."),
    __(ESP, "No se ha podido leer esa URI de migración."),
    __(VAL, "No s'ha pogut llegir eixa URI de migració.")
)

_(ERROR_SAVING_TWO_FACTOR,
    __(ENG, "Could not save the 2FA entry '{0}'"),
    __(ESP, "No se ha podido guardar la entrada 2FA '{0}'"),
    __(VAL, "No s'ha pogut guardar l'entrada 2FA '{0}'")
)

#pragma endregion

#pragma region Workspaces

_(PASSWORDSTORE_MANAGER_NEW_WORKSPACE_TOOLTIP,
    __(ENG, "New Workspace"),
    __(ESP, "Nuevo Espacio de Trabajo"),
    __(VAL, "Nou Espai de Treball")
)

_(WORKSPACE_PALETTE_NEW_TITLE,
    __(ENG, "New Workspace"),
    __(ESP, "Nuevo Espacio de Trabajo"),
    __(VAL, "Nou Espai de Treball")
)

_(WORKSPACE_PALETTE_EDIT_TITLE,
    __(ENG, "Edit Workspace"),
    __(ESP, "Editar Espacio de Trabajo"),
    __(VAL, "Editar Espai de Treball")
)

_(WORKSPACE_PALETTE_NAME_LABEL,
    __(ENG, "Workspace name:"),
    __(ESP, "Nombre del espacio de trabajo:"),
    __(VAL, "Nom de l'espai de treball:")
)

_(WORKSPACE_PALETTE_EMPTY_HINT,
    __(ENG, "This Workspace is empty. Right-click any password and choose \"Add to Workspace\" to fill it."),
    __(ESP, "Este espacio de trabajo está vacío. Haz clic derecho en cualquier contraseña y elige \"Añadir al Espacio de Trabajo\" para llenarlo."),
    __(VAL, "Aquest espai de treball està buit. Fes clic dret en qualsevol contrasenya i tria \"Afegir a l'Espai de Treball\" per a omplir-lo.")
)

_(WORKSPACE_PICKER_TITLE,
    __(ENG, "Add to Workspace"),
    __(ESP, "Añadir al Espacio de Trabajo"),
    __(VAL, "Afegir a l'Espai de Treball")
)

_(WORKSPACE_PICKER_LABEL,
    __(ENG, "Add {0} to which Workspace?"),
    __(ESP, "¿A qué espacio de trabajo añadir {0}?"),
    __(VAL, "A quin espai de treball afegir {0}?")
)

_(WORKSPACE_PICKER_EMPTY,
    __(ENG, "There are no Workspaces yet. Create one first with the Workspace button."),
    __(ESP, "Todavía no hay espacios de trabajo. Crea uno primero con el botón de Espacio de Trabajo."),
    __(VAL, "Encara no hi ha espais de treball. Crea'n un primer amb el botó d'Espai de Treball.")
)

_(WORKSPACE_ADD_NAME_PROMPT_TITLE,
    __(ENG, "Name in Workspace"),
    __(ESP, "Nombre en el Espacio de Trabajo"),
    __(VAL, "Nom en l'Espai de Treball")
)

_(WORKSPACE_ADD_NAME_PROMPT_LABEL,
    __(ENG, "Name for {0} inside this Workspace:"),
    __(ESP, "Nombre de {0} dentro de este espacio de trabajo:"),
    __(VAL, "Nom de {0} dins d'aquest espai de treball:")
)

_(WORKSPACE_CONTEXT_MENU_ADD_TO_WORKSPACE,
    __(ENG, "Add to Workspace"),
    __(ESP, "Añadir al Espacio de Trabajo"),
    __(VAL, "Afegir a l'Espai de Treball")
)

_(WORKSPACE_CONTEXT_MENU_EDIT_WORKSPACE,
    __(ENG, "Edit Workspace"),
    __(ESP, "Editar Espacio de Trabajo"),
    __(VAL, "Editar Espai de Treball")
)

_(WORKSPACE_CONTEXT_MENU_RENAME_IN_WORKSPACE,
    __(ENG, "Rename in Workspace"),
    __(ESP, "Renombrar en el Espacio de Trabajo"),
    __(VAL, "Renombrar en l'Espai de Treball")
)

_(WORKSPACE_CONTEXT_MENU_REMOVE_FROM_WORKSPACE,
    __(ENG, "Remove from Workspace"),
    __(ESP, "Quitar del Espacio de Trabajo"),
    __(VAL, "Llevar de l'Espai de Treball")
)

_(WORKSPACE_RENAME_TITLE,
    __(ENG, "Rename Workspace"),
    __(ESP, "Renombrar Espacio de Trabajo"),
    __(VAL, "Renombrar Espai de Treball")
)

_(WORKSPACE_RENAME_LABEL,
    __(ENG, "Rename {0} to (a path moves it):"),
    __(ESP, "Renombrar {0} a (una ruta lo mueve):"),
    __(VAL, "Renombrar {0} a (una ruta el mou):")
)

_(WORKSPACE_RENAME_ENTRY_TITLE,
    __(ENG, "Rename in Workspace"),
    __(ESP, "Renombrar en el Espacio de Trabajo"),
    __(VAL, "Renombrar en l'Espai de Treball")
)

_(WORKSPACE_RENAME_ENTRY_LABEL,
    __(ENG, "Name of {0} inside this Workspace:"),
    __(ESP, "Nombre de {0} dentro de este espacio de trabajo:"),
    __(VAL, "Nom de {0} dins d'aquest espai de treball:")
)

_(WORKSPACE_DELETE_PROMPT,
    __(ENG, "Delete Workspace {0}?"),
    __(ESP, "¿Borrar el espacio de trabajo {0}?"),
    __(VAL, "Esborrar l'espai de treball {0}?")
)

_(WORKSPACE_DELETE_PROMPT_LABEL,
    __(ENG, "The Workspace will be removed. None of the passwords it points to are deleted."),
    __(ESP, "El espacio de trabajo será eliminado. Ninguna de las contraseñas a las que apunta se borrará."),
    __(VAL, "L'espai de treball serà eliminat. Cap de les contrasenyes a les quals apunta s'esborrarà.")
)

_(WORKSPACE_ELEMENT_MISSING_TOOLTIP,
    __(ENG, "Element missing from disk"),
    __(ESP, "El elemento no existe en el disco"),
    __(VAL, "L'element no existeix al disc")
)

_(WORKSPACE_ELEMENT_MISSING_PROMPT,
    __(ENG, "The element {0} is missing from disk. Delete from Workspace?"),
    __(ESP, "El elemento {0} no existe en el disco. ¿Borrarlo del espacio de trabajo?"),
    __(VAL, "L'element {0} no existeix al disc. Esborrar-lo de l'espai de treball?")
)

_(WORKSPACE_DELETE_WARNING,
    __(ENG, "{0} exists in Workspaces: {1}."),
    __(ESP, "{0} existe en los espacios de trabajo: {1}."),
    __(VAL, "{0} existeix en els espais de treball: {1}.")
)

_(ERROR_WORKSPACE_ALREADY_EXISTS,
    __(ENG, "A Workspace named '{0}' already exists here"),
    __(ESP, "Ya existe un espacio de trabajo llamado '{0}' aquí"),
    __(VAL, "Ja existeix un espai de treball anomenat '{0}' ací")
)

_(ERROR_WORKSPACE_TARGET_ALREADY_PRESENT,
    __(ENG, "{0} is already in the Workspace '{1}'"),
    __(ESP, "{0} ya está en el espacio de trabajo '{1}'"),
    __(VAL, "{0} ja està en l'espai de treball '{1}'")
)

_(ERROR_WORKSPACE_INVALID_PATH,
    __(ENG, "'{0}' is not a valid location inside the Password Store"),
    __(ESP, "'{0}' no es una ubicación válida dentro del Repositorio de Contraseñas"),
    __(VAL, "'{0}' no és una ubicació vàlida dins del Repositori de Contrasenyes")
)

_(ERROR_WORKSPACE_NOT_FOUND,
    __(ENG, "The Workspace '{0}' no longer exists"),
    __(ESP, "El espacio de trabajo '{0}' ya no existe"),
    __(VAL, "L'espai de treball '{0}' ja no existeix")
)

_(ERROR_UNABLE_TO_SAVE_WORKSPACES,
    __(ENG, "Unable to save the Workspaces file"),
    __(ESP, "No se pudo guardar el archivo de espacios de trabajo"),
    __(VAL, "No s'ha pogut guardar el fitxer d'espais de treball")
)

_(WORKSPACE_PATH_LABEL,
    __(ENG, "{0} Workspace"),
    __(ESP, "Espacio de Trabajo {0}"),
    __(VAL, "Espai de Treball {0}")
)

_(GIT_WORKSPACE_CREATED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Created Workspace '{0}'"),
    __(ESP, "Clavis: Creado el espacio de trabajo '{0}'"),
    __(VAL, "Clavis: Creat l'espai de treball '{0}'")
)

_(GIT_WORKSPACE_UPDATED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Updated the contents of Workspace '{0}'"),
    __(ESP, "Clavis: Actualizado el contenido del espacio de trabajo '{0}'"),
    __(VAL, "Clavis: Actualitzat el contingut de l'espai de treball '{0}'")
)

_(GIT_WORKSPACE_RENAMED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Renamed Workspace '{0}' to '{1}'"),
    __(ESP, "Clavis: Renombrado el espacio de trabajo '{0}' a '{1}'"),
    __(VAL, "Clavis: Renombrat l'espai de treball '{0}' a '{1}'")
)

_(GIT_WORKSPACE_MOVED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Moved Workspace '{0}' to '{1}'"),
    __(ESP, "Clavis: Movido el espacio de trabajo '{0}' a '{1}'"),
    __(VAL, "Clavis: Mogut l'espai de treball '{0}' a '{1}'")
)

_(GIT_WORKSPACE_DELETED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Deleted Workspace '{0}'"),
    __(ESP, "Clavis: Borrado el espacio de trabajo '{0}'"),
    __(VAL, "Clavis: Esborrat l'espai de treball '{0}'")
)

_(GIT_WORKSPACE_ENTRY_ADDED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Added '{0}' as '{1}' to Workspace '{2}'"),
    __(ESP, "Clavis: Añadido '{0}' como '{1}' al espacio de trabajo '{2}'"),
    __(VAL, "Clavis: Afegit '{0}' com a '{1}' a l'espai de treball '{2}'")
)

_(GIT_WORKSPACE_ENTRY_REMOVED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Removed '{0}' from Workspace '{1}'"),
    __(ESP, "Clavis: Quitado '{0}' del espacio de trabajo '{1}'"),
    __(VAL, "Clavis: Llevat '{0}' de l'espai de treball '{1}'")
)

_(GIT_WORKSPACE_ENTRY_RENAMED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Renamed '{0}' to '{1}' in Workspace '{2}'"),
    __(ESP, "Clavis: Renombrado '{0}' a '{1}' en el espacio de trabajo '{2}'"),
    __(VAL, "Clavis: Renombrat '{0}' a '{1}' en l'espai de treball '{2}'")
)

_(GIT_WORKSPACE_RETARGETED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Updated Workspaces after moving '{0}' to '{1}'"),
    __(ESP, "Clavis: Actualizados los espacios de trabajo tras mover '{0}' a '{1}'"),
    __(VAL, "Clavis: Actualitzats els espais de treball després de moure '{0}' a '{1}'")
)

_(GIT_WORKSPACE_PRUNED_COMMIT_MESSAGE,
    __(ENG, "Clavis: Removed the deleted '{0}' from Workspaces: {1}"),
    __(ESP, "Clavis: Quitado '{0}', ya borrado, de los espacios de trabajo: {1}"),
    __(VAL, "Clavis: Llevat '{0}', ja esborrat, dels espais de treball: {1}")
)

#pragma endregion


#pragma region Macros and Cleanup
#ifdef _
#undef _
#endif
#define _(x, ...) Clavis::Language::GetResourceString(x, std::vector<Clavis::Language::StrArg>{ __VA_ARGS__ })

// MACRO CLEANUP
#ifdef TRANSLATE_CHECK
#undef TRANSLATE_CHECK
#endif

#ifdef _COMPILETIME_CHECK
#undef _COMPILETIME_CHECK
#endif

#ifdef _COMPILETIME_FUNC_CHECK
#undef _COMPILETIME_FUNC_CHECK
#endif

#ifdef __
#undef __
#endif

#pragma endregion
