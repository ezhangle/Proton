default:
	gcc -O2 -fopenmp proton.c bmpwrite.c protontypes.c protonmath.c protonscene.c protonrender.c -o proton
