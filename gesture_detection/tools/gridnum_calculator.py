import sys
import argparse

def main():
	#http://stackoverflow.com/questions/11604653/add-command-line-arguments-with-flags-in-python3
	parser = argparse.ArgumentParser()

	parser.add_argument('tl', type=int)
	parser.add_argument('tw', type=int)
	parser.add_argument('gl', type=int)
	parser.add_argument('gw', type=int)
	parser.add_argument('-g', type=int)
	parser.add_argument('-y', type=int)
	parser.add_argument('-x', type=int)

	args = parser.parse_args()

	if args.g and (args.y or args.x):
		sys.exit('Only specify grid number or the row and column.')
	elif not (args.g or (args.y and args.x)):
		sys.exit('Specify both the row and column.')

	if args.g:
		calculateRowCol(args.tl, args.tw, args.gl, args.gw, args.g)
	else:
		calculateGrid(args.tl, args.tw, args.gl, args.gw, args.y, args.x)

def calculateRowCol(total_length, total_width, grid_length, grid_width, gridnum):
	total_rows = total_length // grid_length;
	total_cols = total_width // grid_width;
	row = gridnum // total_cols
	col = gridnum % total_cols

	print(str(row)+','+str(col))

def calculateGrid(total_length, total_width, grid_length, grid_width, y, x):
	total_cols = total_width // grid_width;
	row = y // grid_length
	col = x // grid_width
	gridnum = row * total_cols + col;

	print(str(gridnum))

if __name__ == '__main__':
	main()