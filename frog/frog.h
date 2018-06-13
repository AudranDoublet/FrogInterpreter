/** File with a LOT of includes :D **/

#ifndef __FROG_H__
#define __FROG_H__

#define _GNU_SOURCE

#define UNUSED(x) (void)(x)

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <err.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

typedef uint32_t fchar; /** UTF-32 character **/
typedef __ssize_t ssize_t; /** signed size_t **/

#include "object.h"

#include "common/strutils.h"
#include "common/hashmap.h"

#include "objects/frogint.h"
#include "objects/frogfloat.h"
#include "objects/frogstr.h"
#include "objects/frognone.h"
#include "objects/frogrun.h"
#include "objects/frogbool.h"
#include "objects/frogfunc.h"
#include "objects/frogmodule.h"
#include "objects/frogbifunc.h"
#include "objects/frogclass.h"
#include "objects/froginstance.h"
#include "objects/froglist.h"
#include "objects/frogdict.h"
#include "objects/frogset.h"
#include "objects/frogiter.h"
#include "objects/frogtuple.h"

#include "objects/modules.h"

#include "modules/global.h"
#include "modules/str.h"
#include "modules/list.h"
#include "modules/dict.h"
#include "modules/set.h"
#include "modules/pthread.h"
#include "modules/sdl.h"

#include "error.h"

#include "parser/ast.h"
#include "parser/parser.h"
#include "parser/ast2bc.h"

#include "exec/eval.h"
#endif
