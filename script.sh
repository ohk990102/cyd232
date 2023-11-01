#! /bin/bash

# Define function build that takes one argument folder and run make in that folder
build() {
    folder=$1
    make -C $folder
}

# Temporally cd into the folder/src and pack all files
pack() {
    folder=$1
    cd $folder/src
    zip -r $folder.zip *
    mv $folder.zip ../..
    cd ../..
}

# Clean all files that is produced by build and zip
clean() {
    folder=$1
    make clean -C $folder
    rm $folder.zip
}

# Get one argument from the comment line
# and assign it to the variable 'command'

command=$1

# If the variable 'command' is not empty

if [ -n "$command" ]
then




# Check if command is one of the following:
# 'build', 'zip', 'clean'

if [ "$command" = "build" ]
then
# Get the second argument from the comment line
# and assign it to the variable 'folder'
    folder=$2
    # List of folders in the current directory
    folders=$(ls -d */ | sed 's#/##')

    # Check if the variable 'folder' is not empty
    if [ -n "$folder" ]
    then
        # Check if the variable 'folder' is in the list of folders
        if [[ $folders == *"$folder"* ]]
        then
            # Call the function 'build' with the argument 'folder'
            echo "Building $folder"
            build $folder
            echo "Done"
            exit 0
        else
            echo "Folder $folder does not exist"
            exit 1
        fi
    else
        # Recursively build all folders in the list of folders
        for f in $folders
        do
            echo "Building $f"
            build $f
            echo "Done"
        done
        exit 0
    fi
elif [ "$command" = "zip" ]
then
    folder=$2
    folders=$(ls -d */ | sed 's#/##')

    if [ -n "$folder" ]
    then
        if [[ $folders == *"$folder"* ]]
        then
            echo "Zipping $folder"
            pack $folder
            echo "Done"
            exit 0
        else
            echo "Folder $folder does not exist"
            exit 1
        fi
    else
        for f in $folders
        do
            echo "Zipping $f"
            pack $f
            echo "Done"
        done
        exit 0
    fi
elif [ "$command" = "clean" ]
then
    # recursively clean all folders in the list of folders
    folders=$(ls -d */ | sed 's#/##')
    for f in $folders
    do
        echo "Cleaning $f"
        clean $f
        echo "Done"
    done
    exit 0
fi
fi


# Print help message (folder is optional)
echo "Usage: ./script.sh <command> [folder]"
echo "Commands:"
echo "  build: build all folders or a specific folder"
echo "  zip: zip all folders or a specific folder"
echo "  clean: clean all folders"
exit 1
