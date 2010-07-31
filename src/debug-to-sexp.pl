#! /usr/bin/perl -lan
# Convert debug output of seegoban into a form that can be read by classify.lisp
# with argument 'rgb', ((x y) r g b)
# otherwise, ((x y) brightness saturation)

BEGIN { print '('; }
END { print ')'; }

if (shift eq "rgb") {
	print "(($F[0] $F[1]) $F[4] $F[5] $F[6])";
}
else {
	print"(($F[0] $F[1]) $F[7] $F[8])";
}
