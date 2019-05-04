#!/bin/bash

inkscape \
	--verb=FitCanvasToDrawing \
	--verb=FileSave \
	--verb=FileClose \
	**"/svg/"*".svg"
