class Nota {
private:
	bool isFake;
	bool showNota;
	int color; // 1 = morado, 2 = azul, 3 = rosa
	unsigned char* ptrNota;
	POSITION posNota;
public:
	Nota(bool f, int c, POSITION pos) {
		showNota = true;
		isFake = f;
		color = c;

		if (!isFake) {
			switch (color) {
				case 1: {
					ptrNota = CargaImagen(TEXT("Notas/morado.png"), &dmnNota);
					break;
				}
				case 2: {
					ptrNota = CargaImagen(TEXT("Notas/azul.png"), &dmnNota);
					break;
				}
				case 3: {
					ptrNota = CargaImagen(TEXT("Notas/rosa.png"), &dmnNota);
					break;
				}
				default: {
					ptrNota = CargaImagen(TEXT("Notas/morado.png"), &dmnNota);
					break;
				}
			}
		}
		else {
			switch (color) {
				case 1: {
					ptrNota = CargaImagen(TEXT("Notas/moradoFake.png"), &dmnNota);
					break;
				}
				case 2: {
					ptrNota = CargaImagen(TEXT("Notas/azulFake.png"), &dmnNota);
					break;
				}
				case 3: {
					ptrNota = CargaImagen(TEXT("Notas/rosaFake.png"), &dmnNota);
					break;
				}
				default: {
					ptrNota = CargaImagen(TEXT("Notas/moradoFake.png"), &dmnNota);
					break;
				}
			}
		}
		
		posNota = pos;
	}
	bool getShowNota() {
		return showNota;
	}
};