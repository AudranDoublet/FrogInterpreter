/** File with a LOT of includes :D **/

#ifndef __FROG_H__
#define __FROG_H__

#define UNUSED(x) (void)(x)

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

typedef uint32_t fchar; /** UTF-32 character **/
typedef long long ssize_t; /** signed size_t **/

#include "object.h"

#include "common/strutils.h"
#include "common/hashmap.h"

#include "objects/frogint.h"
#include "objects/frogstr.h"
#include "objects/frognone.h"
#include "objects/frogrun.h"
#include "objects/froghybrid.h"
#include "objects/frogenv.h"
#include "objects/frogbool.h"
#include "objects/frogblock.h"

#include "error.h"

#include "parser/parser.h"
#endif
