#!/bin/bash
#
# Cloudlab Hadoop Patcher
#
# Description:
#     Cloudlab's current hadoop configuration has an issue where the host
#     file configuration doesn't map to slaves correctly. This bash script
#     fixes that by faking their addresses as local IP addresses that are
#     given in the /etc/hosts file.
#
#     ROOT IS REQUIRED. Run with sudo. Don't worry, it isn't dangerous. :)
#
# Author:
#     Clara Nguyen (@iDestyKK)
#

# Check if we are root or not
if [ "$EUID" -ne 0 ]; then
	printf "[FATAL] Not running as root. Editing /etc/hosts requires it.\n"
	exit 1
fi

# Configure paths
HADOOP_PATH="/usr/local/hadoop-2.7.6"

# Grab all IP/hostname pairs
HOSTS=$(
	${HADOOP_PATH}/bin/hadoop dfsadmin -report \
		2> /dev/null                                       \
		|  grep 'Name\|Hostname'                           \
		|  tr '\n' ' '                                     \
		|  sed 's/Name\|$/\nName/2g'                       \
		|  sed 's/Name: \(.*\):.*Hostname: \(.*\)$/\1 \2/' \
		|  sed '/^Name$/d'
)

echo "Assigning the following addresses:"
echo "$HOSTS"

# Now for each line, let's edit /etc/hosts
HF=$(cat /etc/hosts)

# Edit the text in memory... with sed
while read -r line; do
	IP=$(echo "$line" | sed 's/^\(.*\) .*$/\1/')
	HOST=$(echo "$line" | sed 's/^.* \(.*\)$/\1/')

	# Yes
	HF=$(echo "$HF" | sed "s/^\(${IP}.*\)$/\1 ${HOST}/")
done <<< "$HOSTS"

# Overwrite
echo "$HF" > /etc/hosts
