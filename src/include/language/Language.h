#pragma once

#include <array>
#include <string>
#include <vector>
#include <map>

#include <glibmm/ustring.h>

//// // Uncomment to assert at compile-time that all languages have valid translations
//#define TRANSLATE_CHECK 1
//#define TRANSLATE_CHECK ENG

#include <language/LanguageDefinitions.h>


namespace Clavis::Language {
    Glib::ustring GetResourceString(_I18N_MAP_ strings, std::vector<std::string> arguments);

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

#pragma endregion

#pragma region Buttons
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

_(ERROR_FILE_NOT_FOUND,
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

#pragma endregion

#pragma region Macros and Cleanup
#ifdef _
#undef _
#endif
#define _(x, ...) Clavis::Language::GetResourceString(x, std::vector<std::string>{ __VA_ARGS__ })

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
