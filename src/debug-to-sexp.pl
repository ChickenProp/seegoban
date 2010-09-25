#! /usr/bin/perl -lan
# Convert debug output of seegoban into a form that can be read by classify.lisp
# with argument 'rgb' or no args, ((x y) r g b)
# with argument 'bs', ((x y) brightness saturation)

BEGIN {
	our $mode = shift() || 'rgb';
	print '(';
}
END { print ')'; }

if ($mode eq "rgb") {
	print "(($F[0] $F[1]) $F[4] $F[5] $F[6])";
}
elsif ($mode eq "bs") {
	print "(($F[0] $F[1]) $F[7] $F[8])";
}
else {
	warn "Unknown mode $mode.\n";
}
