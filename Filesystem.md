# Filesystem #

Access layer to the filesystem.

Using it, you can create, open and delete files and directories.

A file is represented by the `File` class.

## File path ##
The filesystem contents are identified by a construct called `FilePath`.
It's a path relative to the filesystem root directory.

What it means for the application - it means, that you write your file paths and then keep moving the your assets dir wherever you like, and they still will be valid.

So how do you use it - simply create a FilePath object, passing it a relative path to the file you have in mind, like so:

```
FilePath path( "/Renderer/Shaders/myShader.tvsh" );
```

And voila.

The path will get normalized, meaning that the following two paths will point at the same file:

```
FilePath path( "/Renderer/Shaders/myShader.tvsh" );
FilePath samePath( "Renderer\Shaders/myShader.tvsh" );
```

If you want to retrieve the global path, you can use the `FilePath.toAbsolutePath` method, which accepts a reference to the `Filesystem` instance with respect to which you want to translate the path.


## Reading file contents ##

You can of course use `File.read` method, but it's not very convenient.
For simplicity's sake, we provide a neat tool for doing just that - `StreamBuffer`.

It's a template class, meaning you can specify how the data in the file should be interpreted.

For instance, if you know your file's a text file and you want to initialize a string with it, this is how you can do it:

```
File* file;

// ... file initialization etc....

StringBuffer< char > buf( *file );
std::string textFileContents = buf.getBuffer();

// cleanup
delete file;

```

As easy as that.