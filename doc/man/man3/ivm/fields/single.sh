#!/bin/ksh

# Shell script to generate the stuff that is the same for all fields

if test $# -ne 2 ; then
	echo "Usage: " $0 " className valueRef"
	echo "  e.g: " $0 " SoSFShort short"
	echo "  e.g: " $0 " SoSFColor 'const SbColor &'"
	exit
fi

className=$1
valueRef=$2

cat <<EOF
METHOD "" static SoType getClassTypeId() {}
METHOD "" virtual void getTypeId() const {
Returns the type for this class or a particular object of this class.
}
METHOD Get $valueRef getValue() const {
Returns this field's value.
}
METHOD " " $valueRef operator =($valueRef newValue) {}
METHOD Set void setValue($valueRef newValue) {
Sets this field to newValue.
}
METHOD IsEq int operator ==(const $className &f) const {}
METHOD IsNEq int operator !=(const $className &f) const {
Returns TRUE if f is of the same type and has the same value as this
field.
}
EOF

