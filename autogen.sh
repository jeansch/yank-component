#!/bin/bash

aclocal -I macros
automake --gnu --add-missing
autoconf
