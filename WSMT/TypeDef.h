#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include <string>

typedef	void				Void;
typedef	bool				Bool;

typedef char				Char;
typedef unsigned char		UChar;
typedef	short				Short;
typedef	unsigned short		UShort;
typedef int					Int;
typedef unsigned int		UInt;

typedef __int64				Int64;
typedef unsigned __int64	UInt64;

typedef	float				Float;
typedef	double				Double;

typedef std::string			String;

typedef UShort				DepthPixelType;
typedef UChar				ImagePixelType;

#define MAX_TEXT_LENGTH		200

#define Max(x, y)                   ((x)>(y)?(x):(y))                                                 ///< max of (x, y)
#define Min(x, y)                   ((x)<(y)?(x):(y))                                                 ///< min of (x, y)

#endif