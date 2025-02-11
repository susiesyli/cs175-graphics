#!/usr/bin/env python3

def convert_ppm_to_newlines(input_file, output_file):
    """
    Converts a PPM file to use newline separation for all values.
    Preserves header structure and comments.
    """
    try:
        with open(input_file, 'r') as f:
            lines = f.readlines()
        
        # Process the file
        header_count = 0
        processed_lines = []
        
        for line in lines:
            line = line.strip()
            
            # Skip empty lines
            if not line:
                continue
                
            # Preserve comments exactly as they are
            if line.startswith('#'):
                processed_lines.append(line + '\n')
                continue
            
            # Handle header lines (P3, dimensions, max value)
            if header_count < 3:
                # Split and rejoin header values with newlines
                values = line.split()
                processed_lines.extend(val + '\n' for val in values)
                header_count += len(values)
                continue
            
            # Process color values
            values = line.split()
            processed_lines.extend(val + '\n' for val in values)
        
        # Write the converted file
        with open(output_file, 'w') as f:
            f.writelines(processed_lines)
            
        print(f"Successfully converted {input_file} to {output_file}")
        
    except FileNotFoundError:
        print(f"Error: Could not find input file {input_file}")
    except Exception as e:
        print(f"Error during conversion: {str(e)}")

def main():
    import sys
    if len(sys.argv) != 3:
        print("Usage: python ppm_converter.py input.ppm output.ppm")
        sys.exit(1)
        
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    convert_ppm_to_newlines(input_file, output_file)

if __name__ == "__main__":
    main()