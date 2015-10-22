# Autoexp.dat #

Put the following applicable in adequate sections.

## `AutoExpand` ##

```
; Tamy
Vector=x=<x> y=<y> z=<z> w=<w>
Quaternion=x=<x> y=<y> z=<z> w=<w>
Plane=a=<a> b=<b> c=<c> d=<d>
```

## `Visualizer` ##

```
; Tamy
Vector{
    preview
    (
        #(
            "(",
            [$e.x,g], ", ",
            [$e.y,g], ", ",
            [$e.z,g], ", ",
            [$e.w,g],
            ")"
        )
    )

    children
    (
        #(
            [len]: sqrt(($e.x*$e.x) + ($e.y*$e.y) + ($e.z*$e.z)),
         )
    )
}

Quaternion{
    preview
    (
        #(
            "(",
            [$e.x,g], ", ",
            [$e.y,g], ", ",
            [$e.z,g], ", ",
            [$e.w,g],
            ")"
        )
    )
	
	children
    (
    )
}

Plane{
    preview
    (
        #(
            "(",
            [$e.a,g], ", ",
            [$e.b,g], ", ",
            [$e.c,g], ", ",
            [$e.d,g],
            ")"
        )
    )
	
	children
    (
    )
}

Matrix{
    preview
    (
        #(
            "(",
            [$e.m11,g], ", ",
            [$e.m12,g], ", ",
            [$e.m13,g], ", ",
            [$e.m14,g],
            ")",
			"(",
            [$e.m21,g], ", ",
            [$e.m22,g], ", ",
            [$e.m23,g], ", ",
            [$e.m24,g],
            ")",
			"(",
            [$e.m31,g], ", ",
            [$e.m32,g], ", ",
            [$e.m33,g], ", ",
            [$e.m34,g],
            ")",
			"(",
            [$e.m41,g], ", ",
            [$e.m42,g], ", ",
            [$e.m43,g], ", ",
            [$e.m44,g],
            ")",
        )
    )
	
	children
    (
    )
}

```