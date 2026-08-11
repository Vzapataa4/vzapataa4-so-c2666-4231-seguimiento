#!/bin/bash

directorio=$1

find "$directorio" -name ".keep" -exec git rm {} \;
