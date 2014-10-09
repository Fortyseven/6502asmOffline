#ifndef VERSION_H
#define VERSION_H

namespace AutoVersion
{

//Software Status
static char STATUS[] = "Custom";
static char STATUS_SHORT[] = "";

//Standard Version Type
static long MAJOR = 0;
static long MINOR = 0;
static long BUILD = 79;
static long REVISION = 441;

//Miscellaneous Version Types
static long BUILDS_COUNT = 207;
#define RC_FILEVERSION 0,0,79,441
#define RC_FILEVERSION_STRING "0, 0, 79, 441\0"
static char FULLVERSION_STRING[] = "0.0.79.441";

}
#endif //VERSION_h
