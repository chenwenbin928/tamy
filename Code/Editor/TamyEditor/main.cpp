#include "tamyeditor.h"
#include <QtGui/QApplication>
#include <QMessageBox.h>
#include "assertDialog.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

// type includes
#include "core/TypesRegistry.cpp"
#include "core-MVC/TypesRegistry.cpp"
#include "core-Renderer/TypesRegistry.cpp"
#include "core-AI/TypesRegistry.cpp"
#include "core-AppFlow/TypesRegistry.cpp"

///////////////////////////////////////////////////////////////////////////////

class TamyEditorApp : public QApplication
{
public:
   TamyEditorApp(int argc, char* argv[])
   : QApplication(argc, argv)
   {}

protected:
   bool notify(QObject* receiver, QEvent* event )
   {
      bool result = false;
      try
      {
         result = QApplication::notify(receiver, event);
      }
      catch (std::exception& ex)
      {
         QMessageBox::warning(NULL, tr("Tamy editor error"),
            QString(ex.what()),
            QMessageBox::Ok);
      }

      return result;
   }
};

///////////////////////////////////////////////////////////////////////////////

int main(int argc, char *argv[])
{
   TamyEditorApp a(argc, argv);

   registerAssertCallback( &AssertDialog::createDialog );

   // get the directory the application was started from and acquire path to the data repository
   char szDirectory[ MAX_PATH ];
   if( !::GetCurrentDirectoryA( sizeof( szDirectory ) - 1, szDirectory ) )
   {
      return -1;
   }

   // the application is started from Build\$(ProjectName)\$(Configuration)\ subdir - we have to escape
   // from there, and there should be our Assets directory
   std::string assetsDir;
   FilesystemUtils::normalize( szDirectory, assetsDir ); 
   FilesystemUtils::leaveDir( assetsDir, 3, assetsDir ); 
   assetsDir += "/Assets/";

   TamyEditor::createInstance( a, assetsDir.c_str() );
   TamyEditor& w = TamyEditor::getInstance();

   // start the application
   w.show();
   bool result = a.exec();

   // deinitialize
   TamyEditor::destroyInstance();
   MaterialsDB::deinitialize();
   ResourcesManager::deinitialize();
   IDStringsPool::deinitialize();
   Profiler::deinitialize();
   ReflectionTypesRegistry::deinitialize();
   MemoryRouter::deinitialize();

   return result;
}

///////////////////////////////////////////////////////////////////////////////
