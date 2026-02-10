#!/bin/bash

sed -ibak -E -e "s/\\\\usepackage\{caption\}/\\\\usepackage{etoolbox}\
\\\\robustify\\\\+\
\\\\usepackage{caption}/" "$3/latex/refman.tex"

sed -ibak -E -e "s/\\\\usepackage\[[^\]*\]\{fontenc\}/\\\\usepackage\[$1\]\{fontenc\}\
\\\\usepackage\[$2\]\{babel\}/" "$3/latex/refman.tex"

sed -ibak -E -e "s/documentclass\[twoside\]/documentclass\[oneside\]/" "$3/latex/refman.tex"
