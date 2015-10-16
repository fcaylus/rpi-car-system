#ifndef _ILIXI_ILIXICONFIG_H
#define _ILIXI_ILIXICONFIG_H 1
 
/* ilixi/ilixiConfig.h. Generated automatically at end of configure. */
/* ilixi_config.h.  Generated from ilixi_config.h.in by configure.  */
/* ilixi_config.h.in.  Generated from configure.ac by autoheader.  */

/* ilixi binary directory */
#ifndef ILIXI_BINDIR
#define ILIXI_BINDIR "/opt/dfb/bin/"
#endif

/* ilixi data directory */
#ifndef ILIXI_DATADIR
#define ILIXI_DATADIR "/opt/dfb/share/ilixi-1.0.0/"
#endif

/* Define to 1 if translation of program messages to the user's native
   language is requested. */
#ifndef ILIXI_ENABLE_NLS
#define ILIXI_ENABLE_NLS 1
#endif

/* Define to 1 if you have the MacOS X function CFLocaleCopyCurrent in the
   CoreFoundation framework. */
/* #undef HAVE_CFLOCALECOPYCURRENT */

/* Define to 1 if you have the MacOS X function CFPreferencesCopyAppValue in
   the CoreFoundation framework. */
/* #undef HAVE_CFPREFERENCESCOPYAPPVALUE */

/* Define if the GNU dcgettext() function is already present or preinstalled.
   */
#ifndef ILIXI_HAVE_DCGETTEXT
#define ILIXI_HAVE_DCGETTEXT 1
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef ILIXI_HAVE_DLFCN_H
#define ILIXI_HAVE_DLFCN_H 1
#endif

/* Enable FusionDale support */
#ifndef ILIXI_HAVE_FUSIONDALE
#define ILIXI_HAVE_FUSIONDALE 1
#endif

/* Enable FusionSound support */
#ifndef ILIXI_HAVE_FUSIONSOUND
#define ILIXI_HAVE_FUSIONSOUND 1
#endif

/* Define if the GNU gettext() function is already present or preinstalled. */
#ifndef ILIXI_HAVE_GETTEXT
#define ILIXI_HAVE_GETTEXT 1
#endif

/* Define if you have the iconv() function and it works. */
/* #undef HAVE_ICONV */

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef ILIXI_HAVE_INTTYPES_H
#define ILIXI_HAVE_INTTYPES_H 1
#endif

/* Enable Reflex support */
/* #undef HAVE_LIBREFLEX */

/* Enable Wnn support */
/* #undef HAVE_LIBWNN */

/* Define to 1 if you have the <memory.h> header file. */
#ifndef ILIXI_HAVE_MEMORY_H
#define ILIXI_HAVE_MEMORY_H 1
#endif

/* Enable NLS support */
#ifndef ILIXI_HAVE_NLS
#define ILIXI_HAVE_NLS 1
#endif

/* Enable SaWMan support */
#ifndef ILIXI_HAVE_SAWMAN
#define ILIXI_HAVE_SAWMAN 1
#endif

/* Define to 1 if you have the <spawn.h> header file. */
#ifndef ILIXI_HAVE_SPAWN_H
#define ILIXI_HAVE_SPAWN_H 1
#endif

/* Define to 1 if you have the <stdarg.h> header file. */
#ifndef ILIXI_HAVE_STDARG_H
#define ILIXI_HAVE_STDARG_H 1
#endif

/* Define to 1 if stdbool.h conforms to C99. */
#ifndef ILIXI_HAVE_STDBOOL_H
#define ILIXI_HAVE_STDBOOL_H 1
#endif

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef ILIXI_HAVE_STDINT_H
#define ILIXI_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdio.h> header file. */
#ifndef ILIXI_HAVE_STDIO_H
#define ILIXI_HAVE_STDIO_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef ILIXI_HAVE_STDLIB_H
#define ILIXI_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef ILIXI_HAVE_STRINGS_H
#define ILIXI_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef ILIXI_HAVE_STRING_H
#define ILIXI_HAVE_STRING_H 1
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef ILIXI_HAVE_SYS_STAT_H
#define ILIXI_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef ILIXI_HAVE_SYS_TYPES_H
#define ILIXI_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have the <time.h> header file. */
#ifndef ILIXI_HAVE_TIME_H
#define ILIXI_HAVE_TIME_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef ILIXI_HAVE_UNISTD_H
#define ILIXI_HAVE_UNISTD_H 1
#endif

/* Define to 1 if the system has the type `_Bool'. */
#ifndef ILIXI_HAVE__BOOL
#define ILIXI_HAVE__BOOL 1
#endif

/* ilixi image directory */
#ifndef ILIXI_IMAGEDIR
#define ILIXI_IMAGEDIR "/opt/dfb/share/ilixi-1.0.0/images/"
#endif

/* Enable logger */
#ifndef ILIXI_LOGGER_ENABLED
#define ILIXI_LOGGER_ENABLED /**/
#endif

/* Log debug messages */
#ifndef ILIXI_LOG_DEBUG_ENABLED
#define ILIXI_LOG_DEBUG_ENABLED /**/
#endif

/* Define to the sub-directory in which libtool stores uninstalled libraries.
   */
#ifndef ILIXI_LT_OBJDIR
#define ILIXI_LT_OBJDIR ".libs/"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef ILIXI_PACKAGE_BUGREPORT
#define ILIXI_PACKAGE_BUGREPORT "tarik@ilixi.org"
#endif

/* Define to the full name of this package. */
#ifndef ILIXI_PACKAGE_NAME
#define ILIXI_PACKAGE_NAME "ilixi"
#endif

/* Define to the full name and version of this package. */
#ifndef ILIXI_PACKAGE_STRING
#define ILIXI_PACKAGE_STRING "ilixi 1.0.0"
#endif

/* Define to the one symbol short name of this package. */
#ifndef ILIXI_PACKAGE_TARNAME
#define ILIXI_PACKAGE_TARNAME "ilixi"
#endif

/* Define to the home page for this package. */
#ifndef ILIXI_PACKAGE_URL
#define ILIXI_PACKAGE_URL "http://www.ilixi.org"
#endif

/* Define to the version of this package. */
#ifndef ILIXI_PACKAGE_VERSION
#define ILIXI_PACKAGE_VERSION "1.0.0"
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef ILIXI_STDC_HEADERS
#define ILIXI_STDC_HEADERS 1
#endif

/* Enable side by side stereoscopy output */
/* #undef STEREO_OUTPUT */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */
 
/* once: _ILIXI_ILIXICONFIG_H */
#endif
