# QT configuration #

You need to install and configure a QT opensource SDK in order to be able to access the editor's code.

Steps to do that:

  * Open the Visual Studio Command Line Tool
  * Enter the QT SDK installation directory
  * issue the command: `configure -opensource -no-qt3support -no-phonon -no-multimedia -no-webkit -no-script -no-scripttools`
  * Config tool will ask you about a license you want to use - just select `y`
  * Once the config tool is done and the prompt is back, simply type in `nmake`
  * This may take a while, so be patient...