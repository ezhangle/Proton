default:
	gcc -O2 -fopenmp proton.c bmpwrite.c rgbe.c protonimage.c protontypes.c protonmath.c protonscene.c protonrender.c -o proton
