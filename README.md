# Clavis

Clavis is an easy to use password manager, developed primarily towards Linux, although full Windows compatibility is on the way. It has been designed from the ground up with three main design aspects in mind:

* Security
* Usability
* Performance

## Security

Clavis encrypts all your passwords with the best encryption technologies, and safely stores them into a specified directory:

### Linux
* By default, passwords are stored in ```$HOME/.password-store/```.
* Clavis will follow the environment variable ```$PASSWORD_STORE_DIR``` if specified, and store passwords there.

### Windows
* Passwords are stored in ```$HOMEDIR\Clavis_Passwords```.
  
Clavis uses GPG asymmetric encryption, and allows users to create, import and export their GPG keys, in order to import their passwords into different devices. It is also capable of syncronizing between different devices by means of the git protocol.

Whenever a password is to be retrieved, Clavis will ask the user for their Master password, which is the associated password to the selected GPG key.

## Usability

Clavis features a responsive interface. It has been built with the GTK library for the C programming language, combining usability and responsiveness with arguably the most efficient programming language out there.

Clavis makes it easier than ever to create secure and complicated passwords, allowing the user to securely syncronize them between devices, without the fear of them being stolen by third-parties, given the strong encryption algorithms used.

### First run / Welcoming new user

When Clavis is run for the first time, the user will be prompted to initialize their new Password Store. In fact, if the user has used any other ```pass```-compatible password managers, this will not happen.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272598-d07f59f1-0094-457a-85b5-490579dda3a3.png">
</p>

Accepting this message will prompt the user to select which GPG key to use for Clavis. If none available, or if the user wants, they are allowed to create a new key, or import one from file. Also, the user is allowed to export the selected key to a file.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272633-b4bf8d0b-4125-430b-9ea9-71b6a6f3b3cc.png">
</p>

The GPG key creation window has any configuration that the user might need. If this key is selected as the Master key, the password introduced here will be used as Clavis' Master Password.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272687-621e5be0-73d3-4125-97a3-cb142e2612ee.png"> <img src="https://user-images.githubusercontent.com/98960550/188272689-7a76bc80-f877-4319-9cad-97c379ba8212.png">
</p>

Once the key has been created, the user will be able to select it as their main Clavis key.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272708-9eced979-ec47-4eae-b3ea-afd36625b067.png">
</p>

### Clavis main screen

From the main screen, the user will be able to navigate through their password folders, create new folders and add new passwords.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272786-34ce18dd-f35f-4259-a71b-28e10c6a71dc.png"> <img src="https://user-images.githubusercontent.com/98960550/188272789-7104eb7a-92bb-43dc-9d53-8a1b6163aaf2.png">
</p>

### Adding new passwords

The New Password window allows the user to input any preferred password, as well as randomly and cryptographically-securely generate a new password of any length, with the following options:
* Uppercase / Lowercase
  * Enables / disables the addition of uppercase/lowercase letters to the random password
* Numerals
  * Enables / disables the addition of numbers to the random password
* Symbols
  * Enables / disables the addition of symbols gathered from an included symbol list to the random password
* Pronunceable
  * Overrides the Numerals and Symbols options, disabling them.
  * Generated password will only contain letters (uppercase/lowercase depending on selection)
  * The password will be a sequence like: consonant, vowel, consonant, vowel... letters, making the password easier to remember.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272845-7e065d18-2b7c-4a4b-86b3-d51e57980709.png">
</p>

Once the password has been added to the repository, it will automatically be displayed in Clavis' main window.


### Password decryption

Clicking on a password in Clavis' main window will attempt to retrieve its content, prompting the user for their Master password, and decrypting the selected password.

* When a password has been retrieved and decrypted, the user is allowed to view it and automatically copy it to the system's clipboard.
* On top of that, clicking the "Automatically Write" button [Only on Clavis Popup Mode] will close Clavis' interface, and automatically write it to the last place where the user had focus. This is the safest option, as password will not be stored in the clipboard.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188272865-ca0b39e7-e4e3-48a7-b005-254adec11d00.png">
</p>

### Menu bar

From the menubar, the user can access the File and Edit menus, as well as the Help menu.

* From the File menu, the user can create passwords and folders, manage their GPG key and explore their password store's data.
* From the Edit menu, the user can syncronize their passwords with other devices, re-encrypt the whole password store, and relocate their git repository.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/188273131-a417fe5a-66e0-45bf-a072-2f089d30be4a.png"> <img src="https://user-images.githubusercontent.com/98960550/188273137-984b0068-14d3-49fc-b1a1-92f3973e1cdc.png">
</p>


## Performance

Clavis' backend source code has been purposefuly written to maximize both time and memory performance, making the program work in even the least powerful devices.

## Installation

### On Windows

Simply run the installer from the latest release, or run in portable mode.

### On any Linux Distribution

Clone the repository, and from within its main folder, run the command ```sudo make install```

### On Arch Linux

Installation for Arch Linux is automatized and improved, as program is available on the AUR.

Just install ```clavis``` from your favourite AUR helper. For example, with YaY: ```yay -S clavis```.

If you want to always have the latest changes and bleeding-edge functionality, the ```clavis-git``` package is also available, although user caution is adviced, as it might contain bugs

The ```clavis``` package is thoroughly tested for bugs, so it should be safe to use.
