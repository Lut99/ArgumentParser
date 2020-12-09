# BAKERY.sh
#   by Lut99
#
# This script 'bakes', i.e., includes a non-C file at compile time, all the
# standard ADL system types defined in src/lib/adl/*.adl. It does so by
# collecting them all as strings, and then injecting those in
# ADLBaked_template.hpp s.t. the compiler knows them as static strings.
#

# Start by making sure we're running in the correct directory (root of the project)
parent_path=$( cd "$(dirname "${BASH_SOURCE[0]}")" ; pwd -P )
cd "$parent_path"
cd "../../.."

# Then, check arguments
if [ "$#" -lt 3 ]; then
    echo "Usage: $0 <template_path> <adl_path>... <output_path>"
    exit 0
fi

n_args="$#"
output="${!n_args}"
template="$1"

# Read all the .adl files
names=""
files=""
for ((i=2;i<n_args;i++)); do
    file="$(cat ${!i})"

    if [ "$i" -gt 2 ]; then
        names="$names, "
        files="$files, "
    fi
    
    clean_i=$(basename "${!i}")
    clean_name=${clean_i//".adl"/""}
    clean_file=${file//"\\"/"\\\\"}
    clean_file=${clean_file//"\""/"\\\""}
    clean_file=$(echo "$clean_file" | sed 's/$/\\n/g')
    clean_file=$(echo "$clean_file" | tr -d '\n')
    names="$names\"$clean_name\""
    files="$files\"$clean_file\""
done

# Once done, replace the correct occurences in the template with the just generated names and files
result_file=$(cat "$template")
result_file=${result_file//"*****NAMEPASTE*****"/"$names"}
result_file=${result_file//"*****FILEPASTE*****"/"$files"}
n_files=$((n_args-2))
result_file=${result_file//"*****COUNTPASTE*****"/"$n_files"}
echo "$result_file" > "$output"

# Done!
