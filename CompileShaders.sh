#!/bin/bash


shaderDir=Shaders
outputDir=CompiledShaders
glslcExtensions="glsl frag vert geom comp tese tesc mesh task rgen rint rmiss rahit rchit rcall"


if [ -d $outputDir ]; then
	rm -rf $outputDir/
fi
mkdir $outputDir

for f in $shaderDir/*; do
	fileName=${f##*/}
	outputName=$outputDir/${fileName%.*}.spv
	if [ -f $outputName ]; then 
		echo "Duplicate output name for ${fileName} resulting in ${outputName}"
		exit
	fi
	echo Compiling $f;
	glslc $f -o $outputName
done 

echo "All shaders compile SUCCESSFULLY "





