#ifndef __FROGMODULES_SDL__
#define __FROGMODULES_SDL__
FrogObject *frogsdl_Init(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_Quit(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_CreateTexture(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_LoadTexture(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_FillTexture(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_BlitTexture(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_FlushWindow(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_SetWindowIcon(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_SetWindowTitle(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_OpenWindow(FrogObject **args, size_t count, stack *p);

FrogObject *frogsdl_WaitEvents(FrogObject **args, size_t count, stack *p);
#endif
