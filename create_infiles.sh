#!/bin/bash

################### Defined values

inputFile=citizenRecordsFile

################### Get input arguments

for arg in "$@"; do
    index=$(echo $arg | cut -f1 -d=)
    val=$(echo $arg | cut -f2 -d=)
    case $index in
        $inputFile) recordList=$val;;
        $2) inputDir=$val;;
        $3) filesPerDir=$val;;
        *)
    esac
done

################### Check Input - Start

inputOk=true
dirExists=false
overwrite=false

# Check the number of arguments given
[ $# -ne 3 ] && inputOk=false && echo "Wrong number of arguments"

[ ! -f "$recordList" ] && echo "Input file not found." && inputOk=false

if [[ $inputOk == true && -d "$inputDir" ]]; then
    echo "Input directory already exists."
    echo "Type y or Y to overwrite it or any other key to abort."
    dirExists=true
fi

# Fix the directory name if it contained /
[[ `echo -n $inputDir| tail -c -1` == '/' ]] && inputDir=${inputDir%?}

if [[ $dirExists == true ]]; then
    read answer
    [[ $answer == y || $answer == Y ]] && overwrite=true || inputOk=false
fi

[[ $dirExists == true && $overwrite == false ]] && echo "Aborting..." && exit 1

(( filesPerDir < 1 )) && inputOk=false && echo "[numFilesPerDirectory] must be greater than 0."

echo "Input was: $1, $2, $3"

if [[ $inputOk == false ]]; then
    printf "\nCorrect Usage:\n./create_infiles.sh [$inputFile] [input_dir] [numFilesPerDirectory]\n"
    exit 1
fi

################### Check Input - End

start=$(date +%s.%N) # Start clock

################### Prepare input directory and read input file

[[ $dirExists == true && $overwrite == true ]] && rm -rf $inputDir
mkdir $inputDir

# Records file
declare -a recordArray;
file="$recordList"
while read line; do recordArray+=("$line"); done < $file
printf "Records: %d\n" $((${#recordArray[@]}))

# This is an associative array with type: [countryCount] = count
# where 1 < count < numFilesPerDirectory. It's used to simulate the
# Round-Robin algorithm and store evenly the records in the available files.
declare -A countryRecordCount

################### Main Program

for ((counter=0; counter<${#recordArray[@]}; counter++)) do

    line=${recordArray[$counter]}
    # Split current line's arguments
    arguments=(${line// / })
    # And retrieve the country value to construct the filepath for this record
    country=${arguments[3]}
    dirPath=$inputDir/$country

    # Check if this is the first record of this country
    if [[ ! -d $dirPath ]]; then
        mkdir $dirPath
        for ((i=1; i<=$filesPerDir; i++)) do
            filePath=$dirPath/$country-$i.txt
            touch $filePath
        done
        countryRecordCount[$country]=0
    fi

    # At this point we know all the files of the current country exist
    # We increment its fileNumber counter until we store in the last file
    ((countryRecordCount[$country]++))

    # Build the current txt file path
    fileNumber=${countryRecordCount[$country]}
    filePath=$dirPath/$country-$fileNumber.txt

    # Insert the record in the appropriate txt file
    echo ${recordArray[$counter]} >> $filePath

    # If we wrote to the last file, reset the country-file counter
    (( $fileNumber == $filesPerDir )) && countryRecordCount[$country]=0
    
done

end=$(date +%s.%N) # Stop clock
runningTime=$(echo "$end - $start" | bc)
echo "Script runned for" $runningTime "seconds."
echo "Output files stored in "`pwd`/$inputDir/