The libnurbs and client directories define their classes without the So
prefix.  The So prefix is added by the use of a script and special include
files.  The process of adding the prefices has already been done and the
files are checked in so the include files don't have to be generated every
time a make happens.

The script is called _gendefs.  _gendefs traverses directories of files
looking for class and structure definitions.  Those definitions are 
accumulated into two include files, _defines.h and _undefs.h which are
used to define the class and structure names to corresponding names but
with the So prefix added.  The _defines.h and _undefs.h files have already
been generated and are checked in.  The _gendefs script is also checked in,
but is not used during the build process.  _gendefs should only be run if
class or structure names change in either the head or libnurbs subdirectories.

Finally, the include file SoAddPrefix.h gets included in all source code
which might contain a class or structure which needs an So prefix added.
This include file controls the use of the _defines.h and _undefs.h files.


