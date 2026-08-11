#!/bin/bash

fichero=$1
columna=$2

cut -d'|' -f"$columna" "$fichero" | sort | uniq | grep -v "^$"
