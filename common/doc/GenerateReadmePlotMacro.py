def formatline(line):

    is_comment = False
    
    while line[0] == ' ': line = line[1:]
    if line.find('/*') != -1 or line.find('*/') != -1 or line[0]=='*': is_comment = True
    
    while line[0] == '/' or line[0] == '*': line = line[1:]

    line += '\n'
    return is_comment, line



macro_file = open('plot_root.C', 'r')
output_file = open('doc/README_plot_macro.md', 'w')


in_comment_block = False
in_code_block = False
for line in macro_file:

    is_comment, formated_line = formatline(line)
    
       
    if is_comment and not in_comment_block and in_code_block:
        #output_file.write('\n~~~~~~~~~~~~~~~\n\n<br/>')
        output_file.write('\n```\n\n<br/>')
        output_file.write('\n---\n')
        in_comment_block = True
        in_code_block = False

    if is_comment:
        in_comment_block = True

        
    if not is_comment and in_comment_block:
        in_comment_block = False
        in_code_block = True
        #output_file.write('\n~~~~~~~~~~~~~~~{.c}\n')
        output_file.write('\n```cpp\n')
    
    if is_comment: 
        output_file.write(formated_line)
    else:
        output_file.write(line)

    
output_file.write('\n~~~~~~~~~~~~~~~\n')

macro_file.close()
output_file.close()


