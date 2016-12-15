<<<<<<< HEAD
#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


#define SAFE_DELETE(p)      { if (p) { delete (p);      (p)=NULL; } }
#define SAFE_RELEASE(p)     { if (p) { (p)->Release();  (p)=NULL; } }
#define SAFE_DELARRAY(p)    { if (p) { delete[] (p);    (p)=NULL; } }
#define SAFE_DELDC(p)       { if (p) { DeleteDC (p);   (p)=NULL; } }

#define PI 3.141592
//#define PI 3.141592
=======
#pragma once

#include <windows.h>
#include <d3d9.h>
#include <d3dx9.h>
#include <tchar.h>

#include <cstdio>
#include <cstdlib>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>


#define SAFE_DELETE(p)      { if (p) { delete (p);      (p)=NULL; } }
#define SAFE_RELEASE(p)     { if (p) { (p)->Release();  (p)=NULL; } }
#define SAFE_DELARRAY(p)    { if (p) { delete[] (p);    (p)=NULL; } }
#define SAFE_DELDC(p)       { if (p) { DeleteDC (p);   (p)=NULL; } }

#define PI 3.141592
//#define PI 3.141592
>>>>>>> ce3f62ea4134193100bc149d5570779d788696d6
#define CALC_ERROR 0.000001