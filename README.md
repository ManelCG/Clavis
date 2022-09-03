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
  
Clavis uses GPG asymmetric encryption, and allows users to create, import and export their GPG keys, in order to import their passwords into different devices.

Whenever a password is to be retrieved, Clavis will ask the user for their Master password, which is the associated password to the selected GPG key.

## Usability

Clavis features a responsive interface. It has been built with the GTK library for the C programming language, combining usability and responsiveness with arguably the most efficient programming language out there.

### First run / Welcoming new user

When Clavis is run for the first time, the user will be prompted to initialize their new Password Store. In fact, if the user has used any other ```pass```-compatible password managers, this will not happen.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/182032324-e41a1505-5713-495a-bb52-202a7e4f5a11.png">
</p>

Accepting this message will prompt the user to select which GPG key to use for Clavis. If none available, or if the user wants, they are allowed to create a new key, or import one from file. Also, the user is allowed to export the selected key to a file.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/182032379-60c713e5-0a32-4bdc-89f4-d17c4eaac350.png">
</p>

The GPG key creation window has any configuration that the user might need. If this key is selected as the Master key, the password introduced here will be used as Clavis' Master Password.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/182032460-cb7f046a-d73c-4795-83b1-5806bd98462c.png"> <img src="https://user-images.githubusercontent.com/98960550/182032462-fbe2fd24-e0fe-4ce9-87a6-543bfa4f34cf.png">
</p>

Once the key has been created, the user will be able to select it as their main Clavis key.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/182032465-7935928b-41d8-40df-ae52-06cdbb60ff40.png">
</p>

### Clavis main screen

From the main screen, the user will be able to navigate through their password folders, create new folders and add new keys.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/182032822-30aba8ad-5cc3-4426-8c51-8812471b20c6.png"> <img src="https://user-images.githubusercontent.com/98960550/182032869-e54caa8b-4027-440c-9b81-fba151f3c35a.png">
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
  <img src="https://user-images.githubusercontent.com/98960550/182032975-6ead4de0-4cbf-44f6-bbd7-b9cbf235bf50.png">
</p>

Once the password has been added to the repository, it will automatically be displayed in Clavis' main window.


### Password decryption

Clicking on a password in Clavis' main window will attempt to retrieve its content, prompting the user for their Master password, and decrypting the selected password.

* When a password has been retrieved and decrypted, the user is allowed to view it and automatically copy it to the system's clipboard.
* On top of that, clicking the "Automatically Write" button [Only on Clavis Popup Mode] will close Clavis' interface, and automatically write it to the last place where the user had focus. This is the safest option, as password will not be stored in the clipboard.

<p align="center">
  <img src="https://user-images.githubusercontent.com/98960550/182033325-22d1fc75-9557-4fea-8f2f-d8936f18e625.png">
</p>



## Performance

Clavis' backend source code has been purposefuly written to maximize both time and memory performance, making the program work in even the least powerful devices.

## Installation

### On any Linux Distribution

Clone the repository, and from within its main folder, run the command ```sudo make install```

### On Arch Linux

Installation for Arch Linux is automatized and improved, as program is available on the AUR.

Just install ```clavis``` from your favourite AUR helper. For example, with YaY: ```yay -S clavis```.

If you want to always have the latest changes and bleeding-edge functionality, the ```clavis-git``` package is also available, although user caution is adviced, as it might contain bugs

The ```clavis``` package is thoroughly tested for bugs, so it should be safe to use.

## Further development

* Full Windows compatibility is one of the first things on the list.
