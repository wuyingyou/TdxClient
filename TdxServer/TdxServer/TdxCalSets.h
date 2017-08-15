#ifndef __TCALC_FUNC_SETS
#define __TCALC_FUNC_SETS
#include "TdxPlugin.h"

#ifdef __cplusplus
extern "C"
{
#endif //__cplusplus
	__declspec(dllexport) BOOL RegisterTdxFunc(PluginTCalcFuncInfo** pFun);
#ifdef __cplusplus
}
#endif //__cplusplus

#endif //__TCALC_FUNC_SETS


bool getAuth(LPSTR pszOutBuffer);
void PopuldateOutput(int DataLen, float* pfOUT, int value);