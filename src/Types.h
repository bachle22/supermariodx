#pragma once

// Efficently interpret x
// by treating 4 byte (float) as an integer
#define AS_INT(x) (*(int*)&(x))

#define AS_SHORT(x) (*(short*)&(x))