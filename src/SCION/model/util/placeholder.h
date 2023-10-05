#pragma once

#define __CONCAT(x,y) x ## y
#define _CONCAT(x,y) __CONCAT(x,y)
#define _ _CONCAT(__tmp_ignore_var_, __LINE__)