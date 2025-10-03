#pragma once

#define NO_RETURN __attribute__((noreturn))
#define YUJI_INTERNAL __attribute__((visibility("hidden")))

#ifndef YUJI_VERSION_MAJOR
#warning "YUJI_VERSION_MAJOR is not defined"
#define YUJI_VERSION_MAJOR 0
#endif

#ifndef YUJI_VERSION_MINOR
#warning "YUJI_VERSION_MINOR is not defined"
#define YUJI_VERSION_MINOR 0
#endif

#ifndef YUJI_VERSION_PATCH
#warning "YUJI_VERSION_PATCH is not defined"
#define YUJI_VERSION_PATCH 0
#endif


#ifndef YUJI_VERSION_STRING
#error "YUJI_VERSION_STRING is not defined"
#endif
