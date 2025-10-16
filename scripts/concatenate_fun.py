#!/usr/bin/env python3
import os
import re

# Directory containing the pcap files
ft_fun_dir = './ft_fun'

# Dictionary to store file_number -> content
files_content = {}

# Read all .pcap files
for filename in os.listdir(ft_fun_dir):
	if filename.endswith('.pcap'):
		filepath = os.path.join(ft_fun_dir, filename)
		with open(filepath, 'r') as f:
			content = f.read()
			match = re.search(r'//file(\d+)', content)
			if match:
				file_num = int(match.group(1))
				content = content.replace(match.group(0), '')
				files_content[file_num] = content

# Sort by file number and concatenate
sorted_numbers = sorted(files_content.keys())
combined_content = ''

for num in sorted_numbers:
	combined_content += files_content[num]

# Write to output file
output_file = './combined_fun.c'
with open(output_file, 'w') as f:
	f.write(combined_content)


