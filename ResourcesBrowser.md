# Resources browser #

`ResourcesBrowser` allows you to perform various operations on the filesystem.


## Projects ##
You organize your resources into projects. Once your game is done, you can deploy such a project using a dedicated project editor.

`Project` is a resource that combines both the static and runtime date.

### Static project data ###
It's a string that contains `;` separated project directories.

All project directories are actually managed by a dedicated `FilesystemSection` that's created when the `Project` resource is being loaded or first created, and then the data from it are exported to the mentioned string before the resource gets saved.

### Runtime project data ###

These consist of:
  * a filesystem section that keeps track of project directories ( as mentioned in the previous section )
  * a list of active resource editors - `Project` has a responsibility of managing those, so that they can all be nicely closed when the project is being deactivated.