#define WIN32_LEAN_AND_MEAN //No agrega librerías que no se vayan a utilizar
#include <Windows.h>
#include <Mmsystem.h> // para reproducir la cancion
#include <chrono> // para contar los segundos de la cancion
#include <iostream>
#include <stdio.h>
#include <string>
#include <ctime>
#include <cmath>
#include <fstream>
#include <dinput.h>
#include <Xinput.h> // para el mapeo del gamepad de xbox
#include <gdiplus.h>
#include <gl/gl.h>
#include <gl/glu.h>
#pragma comment(lib,"XInput.lib") // para el mapeo del gamepad de xbox
#pragma comment(lib,"Xinput9_1_0.lib") // para el mapeo del gamepad de xbox
#pragma comment(lib,"Winmm.lib") // para reproducir la cancion
using namespace Gdiplus;
using namespace std;
using namespace chrono;
//Variables constantes
#pragma region
/* Inicio y termino de un timer */
auto start = high_resolution_clock::now();

/* Variable constante para calcular el ancho de la ventana */
const int ANCHO_VENTANA = 1024;

/* Variable constante para calcular el alto de la ventana */
const int ALTO_VENTANA = 768;

/* Variable constante que define la cantidad de bytes por pixel, usada en las operaciones de desplegar sprites/imagenes en pantalla */
const int BPP = 4;

/* Variable constante que define el intervalo del contador o timer en milisegundos, 
	con cada TICK del contador se ejecuta el codigo dentro del case WM_TIMER en la funcion WndProc */
const int TICK = 80;

/* Variables constantes de los colores primarios de un pixel de 32 bits */
const unsigned int BLUE = 0xFF0000FF;
const unsigned int GREEN = 0xFF00FF00;
const unsigned int RED = 0xFFFF0000;

/* Estructura con las coordenadas de los sprites en pantalla en un plano 2D */
struct POSITION {
	int X;
	int Y;
};

/* Estructura con las dimensiones de los sprites a cargar y desplegar en pantalla */
struct DIMENSION {
	int ANCHO;
	int ALTO;
};

/* Estructura con la enumeracion de algunas teclas.
	Se tiene un objeto o variable del tipo de esta estructura, llamado 'input' 
	que sera para acceder a cada uno de las elementos de la enumeracion; ejemplo:
	input.A para la tecla 'A'.*/
struct Input
{
	enum Keys
	{
		Backspace = 0x08, Tab,
		Clear = 0x0C, Enter,
		Shift = 0x10, Control, Alt,
		Escape = 0x1B,
		Space = 0x20, PageUp, PageDown, End, Home, Left, Up, Right, Down,
		Zero = 0x30, One, Two, Three, Four, Five, Six, Seven, Eight, Nine,
		A = 0x41, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
		NumPad0 = 0x60, NumPad1, NumPad2, NumPad3, NumPad4, NumPad5, NumPad6, NumPad7, NumPad8, NumPad9, 
		F1 = 0x70, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12
	};
}input /* declaracion del objeto de la estructura Input */;
#pragma endregion
//Declaracion de funciones
#pragma region
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void Init();
void MainRender(HWND hWnd);
void checkStart();
void Animate();
void KeysEvents();
bool CheckColisionNota(POSITION hitbox, POSITION nota, bool isFake);
bool CheckColisionItem(POSITION hitbox, POSITION item);
bool CheckColisionBoost(POSITION hitbox, POSITION nota, bool isFake);

unsigned char* CargaImagen(WCHAR rutaImagen[], DIMENSION* dmn);
POSITION setPosition(int x, int y);
void DibujaScreen(int* buffer, int* screen, DIMENSION dmn);

void DibujaNubes(int* buffer, int* imagen, DIMENSION dmn, int incremento);
void DibujaCity(int* buffer, int* imagen, DIMENSION dmn, int incremento);
void DibujaPalms(int* buffer, int* imagen, DIMENSION dmn, int incremento);
void DibujaCalle(int* buffer, int* imagen, DIMENSION dmn, int incremento);

void DibujaCarro(int* buffer, int* carro, DIMENSION dmn1, POSITION pos);
void DibujaBox(int* buffer, int* box, DIMENSION dmn, POSITION pos);

void DibujaNota(int* buffer, int* nota, DIMENSION dmn, POSITION pos, int indi);
void DibujaItem(int* buffer, int* item, DIMENSION dmn, POSITION pos, int indi);

void DibujaFever(int* buffer, int* fever, DIMENSION dmn, POSITION pos);
void DibujaScore(int* buffer, int* score, DIMENSION dmn, POSITION pos);
void DibujaNumber(int* buffer, int* number, DIMENSION dmn, POSITION pos);
void DibujaText(int* buffer, int* text, DIMENSION dmn, POSITION pos);
#pragma endregion
// Clases, Estructuras y Funciones
#pragma region
class Nota {
private:
	int beat; // momento en el que saldrá en pantalla
	bool fake;
	bool show;
	int color; // 1 = morado, 2 = azul, 3 = rosa
	WCHAR* ptr;
	POSITION pos;
	int indi;
public:
	Nota(int b, bool f, int c) {
		beat = b;
		fake = f;
		show = false;
		color = c;

		if (!fake) {
			switch (color) {
			case 1: {
				ptr = TEXT("Sprites/Notas/morado.png");
				pos.X = 157;
				pos.Y = 126;
				break;
			}
			case 2: {
				ptr = TEXT("Sprites/Notas/azul.png");
				pos.X = 317;
				pos.Y = 166;
				break;
			}
			case 3: {
				ptr = TEXT("Sprites/Notas/rosa.png");
				pos.X = 730;
				pos.Y = 205;
				break;
			}
			}
		}
		else {
			switch (color) {
			case 1: {
				ptr = TEXT("Sprites/Notas/moradoFake.png");
				pos.X = 157;
				pos.Y = 126;
				break;
			}
			case 2: {
				ptr = TEXT("Sprites/Notas/azulFake.png");
				pos.X = 317;
				pos.Y = 166;
				break;
			}
			case 3: {
				ptr = TEXT("Sprites/Notas/rosaFake.png");
				pos.X = 730;
				pos.Y = 205;
				break;
			}
			}
		}

		indi = 0;
	}
	int getBeat() {
		return beat;
	}
	bool isFake() {
		return fake;
	}
	bool getShow() {
		return show;
	}
	void setShow(int i) {
		if (i == 0) {
			show = true;
		}
		else {
			show = false;
		}
	}
	int getColor() {
		return color;
	}
	WCHAR* getPtr() {
		return ptr;
	}
	POSITION getPos() {
		return pos;
	}
	void resetPos(int color) {
		if (!fake) {
			switch (color) {
			case 1: {
				pos.X = 157;
				pos.Y = 126;
				break;
			}
			case 2: {
				pos.X = 317;
				pos.Y = 166;
				break;
			}
			case 3: {
				pos.X = 730;
				pos.Y = 205;
				break;
			}
			}
		}
		else {
			switch (color) {
			case 1: {
				pos.X = 157;
				pos.Y = 126;
				break;
			}
			case 2: {
				pos.X = 317;
				pos.Y = 166;
				break;
			}
			case 3: {
				pos.X = 730;
				pos.Y = 205;
				break;
			}
			}
		}
	}
	int getIndi() {
		return indi;
	}
	void incIndi() {
		pos.X -= 60;

		if (!fake) {
			indi++;
			indi = indi >= 4 ? 0 : indi;
		}
		
		switch (color) {
		case 1: {
			if (pos.X <= -850) {
				show = false;
			}
			break;
		}
		case 2: {
			if (pos.X <= -690) {
				show = false;
			}
			break;
		}
		case 3: {
			if (pos.X <= -277) {
				show = false;
			}
			break;
		}
		}
	}
	void boostNote() {
		if (!fake) {
			ptr = TEXT("Sprites/Notas/amarillo.png");
		}
		else {
			ptr = TEXT("Sprites/Notas/amarilloFake.png");
		}
	}
};

int getCarril() { // funcion para obtener un carril aleatorio (regresa 1, 2 o 3)
	srand(time(0));
	return (rand() % 3) + 1;
}
class Item {
private:
	int beat;
	bool show;
	POSITION pos;
	int carril;
	int indi;
public:
	Item(int b) {
		beat = b;
		switch (getCarril()) {
		case 1: {
			carril = 1;
			pos.X = 157;
			pos.Y = 126;
			break;
		}
		case 2: {
			carril = 2;
			pos.X = 317;
			pos.Y = 166;
			break;
		} 
		case 3: {
			carril = 3;
			pos.X = 730;
			pos.Y = 205;
			break;
		}
		}
		indi = 0;
	}
	int getBeat() {
		return beat;
	}
	bool getShow() {
		return show;
	}
	void setShow(int i) {
		if (i == 0) {
			show = true;
		}
		else {
			show = false;
		}
	}
	POSITION getPos() {
		return pos;
	}
	int getIndi() {
		return indi;
	}
	void incIndi() {
		if (show) {
			pos.X -= 60;

			indi++;
			indi = indi >= 4 ? 0 : indi;

			switch (carril) {
			case 1: {
				if (pos.X <= -850) {
					this->show = false;
				}
				break;
			}
			case 2: {
				if (pos.X <= -690) {
					this->show = false;
				}
				break;
			}
			case 3: {
				if (pos.X <= -277) {
					this->show = false;
				}
				break;
			}
			}
		}
	}
};

struct Number {
	POSITION pos;
	wchar_t title[150];
	bool visible;
	int value = 0;
	// 0, 1, 2, 3, 4, 5, 6, 7, 8, 9
	void updatePtr() {
		wchar_t numberBuffer[50];
		wcsncpy(title, L"UI/Numbers/", 150);
		swprintf_s(numberBuffer, L"%d", value);
		wcscat(title, numberBuffer);
		wcscat(title, L".png");
	}
};

bool boostMode = false;
bool gameWon = false;
bool getRandom() {
	srand(time(NULL));
	int r = rand() % 2;
	if (r == 0) {
		return false;
	}
	else {
		return true;
	}
}
int totalScore = 0;
int highScore;
void saveHighScore() {
	fstream file;
	file.open("Other/highScore.txt", ios::in);
	if (file.is_open()) {
		file >> highScore;
		if (totalScore > highScore) {
			file.close();
			file.open("Other/highScore.txt", ios::trunc | ios::out);
			file << totalScore;
			highScore = totalScore;
		}
		else {
			file.close();
		}
	}
	else {
		MessageBox(NULL, L"No se pudo abrir 'highScore.txt'", L"Warning", MB_ICONERROR);
	}
}
struct Conductor {
	// int bpm = 128; // beats por minuto
	float crotchet = 0.46875; // duracion de cada beat en segundos, 60/bpm
	float songPos = 0; // segundos que lleva la cancion
	float lastPos = 0;
	int currBeat = 0;
	int beatTracker = 0; // indice para el beatmap
	int itemTracker = 0;
	int boostLength = 5; // el boost dura hasta 5 segundos
#pragma region beatmap
	Nota beatmap[25] = { {Nota(0, false, 1)}, {Nota(1, false, 2)}, {Nota(2, false, 3)}, {Nota(3, false, 1)}, {Nota(4, false, 2)},
						{Nota(5, true, 2)}, {Nota(6, !getRandom(), 2)}, {Nota(7, false, 1)}, {Nota(7, getRandom(), 3)}, {Nota(12, !getRandom(), 3)},
						{Nota(12, false, 1)}, {Nota(13, true, 1)}, {Nota(13, false, 2)}, {Nota(14, getRandom(), 3)}, {Nota(15, !getRandom(), 3)},
						{Nota(15, true, 2)}, {Nota(16, false, 1)}, {Nota(17, false, 2)}, {Nota(17, !getRandom(), 2)}, {Nota(18, false, 3)},
						{Nota(19, false, 2)}, {Nota(20, false, 1)}, {Nota(21, true, 2)}, {Nota(21, getRandom(), 2)}, {Nota(22, false, 3)} };
#pragma endregion
	Item itemmap[3] = { {Item(5)}, {Item(11)}, {Item(17)} };
	void update() {
		auto end = high_resolution_clock::now();
		songPos = duration_cast<seconds>(end - start).count();
		if (songPos > lastPos + crotchet) { // conteo de beats en la cancion
			lastPos += crotchet * 2;
			//lastPos += crotchet;
			currBeat++;
			if (boostMode) boostLength--;
		}
		if (boostLength == 0) {
			boostMode = false;
			boostLength = 5;
		}
		if (beatTracker < 25) { // si todavia no se ha terminado el nivel, continua
			if (currBeat == beatmap[beatTracker].getBeat()) {
				beatTracker++; // incrementa el indice del beatmap
			}
		}
		if (itemTracker < 3) { 
			if (currBeat == itemmap[itemTracker].getBeat()) {
				itemTracker++; // incrementa el indice del beatmap
			}
		}
		if (currBeat == 25) { // ultimo beat + idle
			saveHighScore();
			gameWon = true;
		}
	}
};
#pragma endregion
//Variables Globales
#pragma region
int *ptrBuffer;
unsigned char* ptrNubes;
unsigned char* ptrCity;
unsigned char* ptrPalms;
unsigned char* ptrCalle;
DIMENSION dmnNubes, dmnCity, dmnPalms, dmnCalle;
int initNubes, initCity, initPalms, initCalle;
int increNubes = 0;
int increCity = 0;
int increPalms = 0;
int increCalle = 0;
///
unsigned char* ptrNota;
DIMENSION dmnNota;
int scaleNotas = 5;
///
unsigned char* ptrBox;
DIMENSION dmnBox;
POSITION posBox;
///
unsigned char* ptrTempo;
DIMENSION dmnTempo;
///
unsigned char* ptrCarro;
DIMENSION dmnCarro;
POSITION posCar;
int scaleCarro = 3;
int indiCarro = 1;
///
unsigned char* ptrItem;
DIMENSION dmnItem;
int scaleItem = 3;
///
unsigned char* ptrFever;
DIMENSION dmnFever;
POSITION posFever;
///
unsigned char* ptrScore;
DIMENSION dmnScore;
POSITION posScore;
///
Number myScore[8];
Number highArray[8];
unsigned char* ptrNumber;
DIMENSION dmnNumber;
string scoreString;
///
unsigned char* ptrText;
DIMENSION dmnText;
POSITION posText;
int hitText = 1; // 1, 2, 3, 4
bool showText = true;
///
unsigned char* ptrStart;
DIMENSION dmnStart;
bool gameStart = false;
unsigned char* ptrOver;
DIMENSION dmnOver;
bool gameOver = false;
unsigned char* ptrWon;
DIMENSION dmnWon;
//
Conductor conductor;
bool KEYS[256];
bool playing = false;
int feverMeter = 50;
#pragma endregion

int WINAPI wWinMain(HINSTANCE hInstance, 
					 HINSTANCE hPrevInstance, 
					 PWSTR pCmdLine, 
					 int nCmdShow)
{
	WNDCLASSEX wc;									// Windows Class Structure
	HWND hWnd;
	MSG msg;

	TCHAR szAppName[] = TEXT("MyWinAPIApp");		
	TCHAR szAppTitle[] = TEXT("Glitch City Racing");

	hInstance = GetModuleHandle(NULL);				// Grab An Instance For Our Window

	wc.cbSize			= sizeof(WNDCLASSEX);
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance	
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= (HBRUSH) (COLOR_WINDOW + 1);			// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu	
	wc.lpszClassName	= szAppName;							// Set The Class Name
	wc.hIconSm			= LoadIcon(NULL, IDI_APPLICATION);
	
	if (!RegisterClassEx(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,
			L"Fallo al registrar clase (Failed To Register The Window Class).",
			L"ERROR",
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}

	hWnd = CreateWindowEx(	
		WS_EX_CLIENTEDGE | WS_EX_APPWINDOW | WS_EX_WINDOWEDGE,	// Extended Style For The Window
		szAppName,							// Class Name
		szAppTitle,							// Window Title
		WS_OVERLAPPEDWINDOW |				// Defined Window Style
		WS_CLIPSIBLINGS |					// Required Window Style
		WS_CLIPCHILDREN,					// Required Window Style
		0, 0,								// Window Position
		ANCHO_VENTANA,						// Calculate Window Width
		ALTO_VENTANA,						// Calculate Window Height
		NULL,								// No Parent Window
		NULL,								// No Menu
		hInstance,							// Instance
		NULL);								// Pass this class To WM_CREATE								

	if(hWnd == NULL) {
		MessageBox(NULL, 
			L"Error al crear ventana (Window Creation Error).", 
			L"ERROR", 
			MB_OK|MB_ICONEXCLAMATION);
		return 0;
	}
		
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	Init();
	ShowWindow(hWnd, nCmdShow);
	SetFocus(hWnd);

	SetTimer(hWnd, TICK, TICK, NULL);
	ZeroMemory(&msg, sizeof(MSG));

	while(GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	
	return(int)msg.wParam;
}

/* Funcion tipo Callback para el manejo de los eventos de la ventana. */
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch(uMsg)									// Check For Windows Messages
	{
		case WM_TIMER:
			if(wParam == TICK)
			{
				MainRender(hWnd);	
			}
			if (gameStart && !gameOver && !gameWon) {
				conductor.update();
			}
			break;
		case WM_PAINT:
			{
				HDC hdc; 
				PAINTSTRUCT ps;
				hdc = BeginPaint(hWnd, &ps);

				BITMAP bm;
				HBITMAP h_CMC = CreateBitmap(ANCHO_VENTANA, ALTO_VENTANA, 1, 32, ptrBuffer);
				HDC hdcMem = CreateCompatibleDC(hdc);
				HBITMAP hbmOld = (HBITMAP) SelectObject(hdcMem, h_CMC);
				GetObject(h_CMC, sizeof(bm), &bm);

				BitBlt(hdc, 0, 0, ANCHO_VENTANA, ALTO_VENTANA, hdcMem, 0, 0, SRCCOPY);

				DeleteObject(h_CMC);
				SelectObject(hdcMem, hbmOld);
				DeleteDC(hdcMem);
				DeleteObject(hbmOld);
			}
			break;		
		case WM_KEYDOWN:							
			{
				KEYS[ wParam ] = true;
			}
			break;
		case WM_KEYUP:
			{
				KEYS[ wParam ] = false;
			}
			break;
		case WM_CLOSE: 
			{
				DestroyWindow(hWnd);
			}
			break;
		case WM_DESTROY: //Send A Quit Message
			{
				KillTimer(hWnd, TICK);
				PostQuitMessage(0);
			}
			break;
		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

/* Funcion usada para la inicializacion de variables y reserva de espacio en memoria.*/
void Init() 
{
	posCar.X = -20;
	posCar.Y = 123;
	posBox.X = (posCar.X + 88);
	posBox.Y = (posCar.Y + 52);

	posFever.X = 220;
	posFever.Y = -70;
	posScore.X = 650;
	posScore.Y = -73;

	posText.X = 200;
	posText.Y = 77;

	int offset = 0;
	wchar_t numberTitle[150];
	wchar_t numberBuffer[50];
	for (int i = 0; i < 8; i++) {
		myScore[i].pos.Y = -58;
		myScore[i].pos.X = 45 + offset;
		offset += 22;
		myScore[i].visible = true;
		myScore[i].updatePtr();
	}

	for(int i = 0; i < 256; i++)
	{
		KEYS[i] = false;
	}

	//Inicializar el puntero tipo int 'ptrBuffer' que contiene la direccion inicial del area de memoria reservada para el despliegue de sprites/imagenes.
	ptrBuffer = new int[ANCHO_VENTANA * ALTO_VENTANA];

	//Inicializar el puntero tipo unsigned char 'ptrBack' que contiene la direccion inicial en memoria del arreglo de pixeles de la imagen especificada en el primer parametro
	//y en la variable dmnBack de tipo DIMENSION* estan los valores de ANCHO y ALTO de la imagen.
	ptrStart = CargaImagen(TEXT("Screens/GameStart.png"), &dmnStart);
	ptrOver = CargaImagen(TEXT("Screens/GameOver.png"), &dmnOver);
	ptrWon = CargaImagen(TEXT("Screens/GameWon.png"), &dmnWon);
	ptrNubes = CargaImagen(TEXT("Screens/Fondo/nubes.png"), &dmnNubes);
	ptrCity = CargaImagen(TEXT("Screens/Fondo/city.png"), &dmnCity);
	ptrPalms = CargaImagen(TEXT("Screens/Fondo/palms.png"), &dmnPalms);
	ptrCalle = CargaImagen(TEXT("Screens/Fondo/calle.png"), &dmnCalle); //puntero a la imagen
	ptrBox = CargaImagen(TEXT("Sprites/hitbox.png"), &dmnBox);
	ptrTempo = CargaImagen(TEXT("UI/tempoBar.png"), &dmnTempo);
	ptrCarro = CargaImagen(TEXT("Sprites/carro.png"), &dmnCarro);
	ptrItem = CargaImagen(TEXT("Sprites/ItemBox.png"), &dmnItem);
	ptrScore = CargaImagen(TEXT("UI/ScoreBG.png"), &dmnScore);
}

/* Funcion principal. Encargada de hacer el redibujado en pantalla cada intervalo (o "tick") del timer que se haya creado.
	@param hWnd. Manejador de la ventana.*/
void MainRender(HWND hWnd)
{
	//gameStart = true;
	if (!gameStart) {
		DibujaScreen(ptrBuffer, (int*)ptrStart, dmnStart);
		checkStart();
	}
	else {
		if (gameStart && !gameOver && !gameWon) {
			DibujaNubes(ptrBuffer, (int*)ptrNubes, dmnNubes, increNubes);
			DibujaCity(ptrBuffer, (int*)ptrCity, dmnCity, increCity);
			DibujaPalms(ptrBuffer, (int*)ptrPalms, dmnPalms, increPalms);
			DibujaCalle(ptrBuffer, (int*)ptrCalle, dmnCalle, increCalle);   //recibe lo de incremento

			DibujaScreen(ptrBuffer, (int*)ptrTempo, dmnTempo);

			Nota* auxNota = &conductor.beatmap[conductor.beatTracker - 1];
			if (conductor.currBeat == auxNota->getBeat()) {
				if (boostMode) auxNota->boostNote();
				auxNota->setShow(0);
			}
			for (int i = 0; i < 25; i++) {
				auxNota = &conductor.beatmap[i];
				if (auxNota->getShow()) {
					ptrNota = CargaImagen(auxNota->getPtr(), &dmnNota);
					DibujaNota(ptrBuffer, (int*)ptrNota, dmnNota, auxNota->getPos(), auxNota->getIndi());
					auxNota->incIndi();
				}
			}

			Item* auxItem = &conductor.itemmap[conductor.itemTracker - 1];
			if (conductor.currBeat == auxItem->getBeat()) auxItem->setShow(0);
			for (int i = 0; i < 3; i++) {
				auxItem = &conductor.itemmap[i];
				if (auxItem->getShow()) {
					DibujaItem(ptrBuffer, (int*)ptrItem, dmnItem, auxItem->getPos(), auxItem->getIndi());
					auxItem->incIndi();
				}
			}

			DibujaCarro(ptrBuffer, (int*)ptrCarro, dmnCarro, posCar);
			DibujaBox(ptrBuffer, (int*)ptrBox, dmnBox, posBox);

			// Checa la cantidad de fever para asignar la imagen a ptrFever
			if (feverMeter > 110) feverMeter = 110;
			if (feverMeter < 0) feverMeter = 0;
#pragma region
			if (feverMeter <= 10) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever0.png"), &dmnFever);
			}
			else if (feverMeter > 10 && feverMeter <= 20) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever10.png"), &dmnFever);
			}
			else if (feverMeter > 20 && feverMeter <= 30) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever20.png"), &dmnFever);
			}
			else if (feverMeter > 30 && feverMeter <= 40) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever30.png"), &dmnFever);
			}
			else if (feverMeter > 40 && feverMeter <= 50) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever40.png"), &dmnFever);
			}
			else if (feverMeter > 50 && feverMeter <= 60) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever50.png"), &dmnFever);
			}
			else if (feverMeter > 60 && feverMeter <= 70) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever60.png"), &dmnFever);
			}
			else if (feverMeter > 70 && feverMeter <= 80) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever70.png"), &dmnFever);
			}
			else if (feverMeter > 80 && feverMeter <= 90) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever80.png"), &dmnFever);
			}
			else if (feverMeter > 90 && feverMeter <= 100) {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever90.png"), &dmnFever);
			}
			else {
				ptrFever = CargaImagen(TEXT("UI/Fever/Fever100.png"), &dmnFever);
			}
#pragma endregion
			DibujaFever(ptrBuffer, (int*)ptrFever, dmnFever, posFever);
			DibujaScore(ptrBuffer, (int*)ptrScore, dmnScore, posScore);

			hitText = 0;
			KeysEvents();
			Animate();

			if (totalScore > 99999999) totalScore = 99999999;
			if (totalScore < 0) totalScore = 0;
			scoreString = to_string(totalScore);
			int j = scoreString.size() - 1;
			for (int i = 7; i > -1; i--) {
				if (j >= 0) {
					myScore[i].value = scoreString.at(j) - 48;
					myScore[i].visible = true;
					j--;
				}
				else {
					myScore[i].visible = false;
				}
				if (myScore[i].visible) {
					myScore[i].updatePtr();
					ptrNumber = CargaImagen(myScore[i].title, &dmnNumber);
					DibujaNumber(ptrBuffer, (int*)ptrNumber, dmnNumber, myScore[i].pos);
				}
			}

			if (showText) {
				switch (hitText) {
				case 0: {
					ptrText = CargaImagen(L"UI/empty.png", &dmnText);
					break;
				}
				case 1: {
					ptrText = CargaImagen(L"UI/perfect.png", &dmnText);
					break;
				}
				case 2: {
					ptrText = CargaImagen(L"UI/cool.png", &dmnText);
					break;
				}
				case 3: {
					ptrText = CargaImagen(L"UI/sad.png", &dmnText);
					break;
				}
				case 4: {
					ptrText = CargaImagen(L"UI/miss.png", &dmnText);
					break;
				}
				case 5: {
					ptrText = CargaImagen(L"UI/fake.png", &dmnText);
					break;
				}
				default:
					break;
				}
				DibujaText(ptrBuffer, (int*)ptrText, dmnText, posText);
			}
		}
		if (gameOver) {
			PlaySound(NULL, NULL, SND_ASYNC);
			DibujaScreen(ptrBuffer, (int*)ptrOver, dmnOver);
		}
		if (gameWon) {
			DibujaScreen(ptrBuffer, (int*)ptrWon, dmnWon);

			// your score
			int offset = 0;
			wchar_t numberTitle[150];
			wchar_t numberBuffer[50];
			for (int i = 0; i < 8; i++) {
				myScore[i].pos.Y = -7;
				myScore[i].pos.X = 500 + offset;
				offset += 22;
				myScore[i].visible = true;
				myScore[i].updatePtr();
			}
			scoreString = to_string(totalScore);
			int j = scoreString.size() - 1;
			for (int i = 7; i > -1; i--) {
				if (j >= 0) {
					myScore[i].value = scoreString.at(j) - 48;
					myScore[i].visible = true;
					j--;
				}
				else {
					myScore[i].visible = false;
				}
				if (myScore[i].visible) {
					myScore[i].updatePtr();
					ptrNumber = CargaImagen(myScore[i].title, &dmnNumber);
					DibujaNumber(ptrBuffer, (int*)ptrNumber, dmnNumber, myScore[i].pos);
				}
			}

			// high score
			offset = 0;
			for (int i = 0; i < 8; i++) {
				highArray[i].pos.Y = 18;
				highArray[i].pos.X = 342 + offset;
				offset += 22;
				highArray[i].visible = true;
				highArray[i].updatePtr();
			}
			scoreString = to_string(highScore);
			j = scoreString.size() - 1;
			for (int i = 7; i > -1; i--) {
				if (j >= 0) {
					highArray[i].value = scoreString.at(j) - 48;
					highArray[i].visible = true;
					j--;
				}
				else {
					highArray[i].visible = false;
				}
				if (highArray[i].visible) {
					highArray[i].updatePtr();
					ptrNumber = CargaImagen(highArray[i].title, &dmnNumber);
					DibujaNumber(ptrBuffer, (int*)ptrNumber, dmnNumber, highArray[i].pos);
				}
			}
		}
	}

	if (feverMeter == 0) {
		gameOver = true;
	}

	//Funciones que deberan estar el final de la funcion de Render.
	InvalidateRect(hWnd, NULL, FALSE);
	UpdateWindow(hWnd);
}

/* Checa si se quiere empezar el juego */
void checkStart() {
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD connected = XInputGetState(0, &state);

	bool START_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_START) != 0);

	if (KEYS[input.Enter] || START_pressed) {
		gameStart = true;
		PlaySoundA("Other/song.WAV", NULL, SND_ASYNC);
		start = high_resolution_clock::now(); // empieza a contar los segundos
	}
}

/* Funcion para mostrar animaciones durante el render */
void Animate() {
	indiCarro++;
	indiCarro = indiCarro >= 4 ? 0 : indiCarro;
	//////
	if (increNubes == 900) {
		increNubes = 0;
	}
	increNubes += 1;

	if (increCity > 900) {
		increCity = 0;
	}
	increCity += 10;

	if (increPalms > 900) {
		increPalms = 0;
	}
	increPalms += 20;

	if (increCalle > 2200) {
		increCalle = 0;
	}
	increCalle += 80;
}

/* Funcion para manejar eventos del teclado dependiendo de la(s) tecla(s) que se haya(n) presionado.
Incluye el mapeo del gamepad con XINPUT*/
void KeysEvents() 
{
#pragma region gamepad
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	DWORD connected = XInputGetState(0, &state);

	bool A_button_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_A) != 0);
	bool LEFT_SHOULDER_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) != 0);
	bool RIGHT_SHOULDER_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) != 0);
	bool DPAD_UP_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP) != 0);
	bool DPAD_DOWN_pressed = ((state.Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN) != 0);
#pragma endregion
#pragma region movimiento
	if(KEYS[input.S] || KEYS[input.Down] || DPAD_DOWN_pressed || RIGHT_SHOULDER_pressed) {
		if (posCar.Y < 160) { //para que tope con el carril de abajo
			posCar.Y += 40;
			posCar.X += 160;
			posBox.X = (posCar.X + 88);
			posBox.Y = (posCar.Y + 52);
		}
	}
	
	if(KEYS[input.W] || KEYS[input.Up] || DPAD_UP_pressed || LEFT_SHOULDER_pressed) {
		if (posCar.Y > 95) { //para que tope con el carril de arriba
			posCar.Y -= 40;
			posCar.X -= 160;
			posBox.X = (posCar.X + 88);
			posBox.Y = (posCar.Y + 52);
		}

	}
#pragma endregion
#pragma region colision
	Nota* auxNota = &conductor.beatmap[conductor.beatTracker - 1];
	if (KEYS[input.Space] || KEYS[input.Z] || A_button_pressed) {
		for (int i = 0; i < 25; i++) {
			auxNota = &conductor.beatmap[i];
			if (auxNota->getShow()) {
				if (!boostMode) {
					if (CheckColisionNota(posBox, auxNota->getPos(), auxNota->isFake())) {
						auxNota->setShow(1);
						//showText = true;
					}
				}
				else {
					if (CheckColisionBoost(posBox, auxNota->getPos(), auxNota->isFake())) {
						auxNota->setShow(1);
						//showText = true;
					}
				}
			}
		}
	}
	// cuando la nota llego al extremo izq de la pantalla y no se detecto colision, es un "miss"
	// la posicion es igual a missNotaX - 90
	for (int i = 0; i < 25; i++) {
		auxNota = &conductor.beatmap[i];
		if (auxNota->getShow()) {
			switch (auxNota->getColor()) {
			case 1: {
				if (auxNota->getPos().X <= -803) {
					if (!auxNota->isFake()) {
						hitText = 4;
						feverMeter -= 6;
					}
				}
				break;
			}
			case 2: {
				if (auxNota->getPos().X <= -643) {
					if (!auxNota->isFake()) {
						hitText = 4;
						feverMeter -= 6;
					}
				}
				break;
			}
			case 3: {
				if (auxNota->getPos().X <= -230) {
					if (!auxNota->isFake()) {
						hitText = 4;
						feverMeter -= 6;
					}
				}
				break;
			}
			}
		}
	}
#pragma endregion
#pragma region item
	Item* auxItem = &conductor.itemmap[conductor.beatTracker - 1];
	for (int i = 0; i < 3; i++) {
		auxItem = &conductor.itemmap[i];
		if (auxItem->getShow()) {
			if (CheckColisionItem(posBox, auxItem->getPos())) {
				auxItem->setShow(1);
				boostMode = true;
			}
		}
	}
	
#pragma endregion
}

/* Funcion para decidir si hubo una colision entre el carro y la nota musical por carril. */
bool CheckColisionNota(POSITION box, POSITION nota, bool isFake) {
	int boxX, boxY;
	int notaY = nota.Y;
	int notaX = nota.X; // posicion exacta
	int outNotaX, sadNotaX, maxNotaX, hitNotaX, missNotaX;
	int result = 0; // si se cambia a 1, hubo colision
	int fake = (isFake) ? 1 : 0;
	switch (notaY) {
		case 126: { // nota en carril 1
			boxY = box.Y - 9;
			outNotaX = -443; // posicion fuera de rango a la derecha
			sadNotaX = -533; // punto entre "sad" y fuera de rango
			maxNotaX = -593; // max posible posicion a la derecha
			hitNotaX = -623; // posicion exacta de la nota
			missNotaX = -713; // min posicion a la izquierda
			break;
		}
		case 166: { // nota en carril 2
			boxY = box.Y - 9;
			outNotaX = -283;
			sadNotaX = -373;
			maxNotaX = -433;
			hitNotaX = -463;
			missNotaX = -553;
			break;
		}
		case 205: { // nota en carril 3
			boxY = box.Y - 10;
			outNotaX = 130;
			sadNotaX = 40;
			maxNotaX = -20;
			hitNotaX = -50;
			missNotaX = -140;
			break;
		}
		default:
			break;
	}

	__asm {
		cld
		mov eax, boxY
		cmp eax, notaY // checa si estan en el mismo carril
		jne EndColision
			mov eax, notaX
			cmp eax, hitNotaX 
			je ColisionPerf 
				cmp eax, outNotaX
				jg EndColision // la nota esta demasiado lejos, fuera del rango de colision
				cmp eax, sadNotaX 
				jg ColisionSad 
				cmp eax, maxNotaX
				jg ColisionCool 
				cmp eax, missNotaX
				jg ColisionCool
			mov eax, fake
			cmp eax, 1
			je ColisionFake
			// ColisionMiss:
			mov hitText, 4 // notaX < missNotaX == timing "miss", muy lento (0 pts, -6 fever)
			sub feverMeter, 6
			jmp Result
			ColisionCool: // notaX < sadNotaX && notaX > maxNotaX == timing "cool", casi a tiempo (50 pts, +4 fever)
				mov eax, fake
				cmp eax, 1
				je ColisionFake
				//
				mov hitText, 2
				add totalScore, 50
				add feverMeter, 4
			jmp Result
			ColisionSad: // notaX > sadNotaX == timing "sad", muy rapido (10 pts, -2 fever)
				mov eax, fake
				cmp eax, 1
				je ColisionFake
				//
				mov hitText, 3
				add totalScore, 10
				sub feverMeter, 2
			jmp Result
			ColisionPerf: // notaX == hitNotaX == timing "perfect", justo a tiempo (150 pts, +8 fever)
				mov eax, fake
				cmp eax, 1
				je ColisionFake
				//
				mov hitText, 1
				add totalScore, 150
				add feverMeter, 8
			jmp Result
			ColisionFake: // la colision fue con una nota falsa (-50 pts, -4 fever)
				mov hitText, 5
				sub totalScore, 50
				sub feverMeter, 4
			Result:
				inc result
		EndColision : // si no aplica nada de lo anterior, no hubo colision
	}

	if (result == 1) {
		return true;
	}
	else {
		return false;
	}
}

/* Funcion para decidir si hubo una colision entre el carro y un item box por carril. */
bool CheckColisionItem(POSITION box, POSITION item) {
	int boxX, boxY;
	int itemY = item.Y;
	int itemX = item.X; // posicion exacta
	int hitItemX, maxItemX, minItemX;
	int result = 0; // si se cambia a 1, hubo colision
	switch (itemY) {
	case 126: { // item en carril 1
		boxY = box.Y - 9;
		maxItemX = -593; // max posible posicion a la derecha
		hitItemX = -623; // posicion exacta del item
		minItemX = -713; // min posicion a la izquierda
		break;
	}
	case 166: { // item en carril 2
		boxY = box.Y - 9;
		maxItemX = -433;
		hitItemX = -463;
		minItemX = -553;
		break;
	}
	case 205: { // item en carril 3
		boxY = box.Y - 10;
		maxItemX = -20;
		hitItemX = -50;
		minItemX = -140;
		break;
	}
	default:
		break;
	}

	__asm {
		cld
		mov eax, boxY
		cmp eax, itemY // checa si estan en el mismo carril
		jne EndColision
			mov eax, itemX
			cmp eax, hitItemX
			je Colision // si el item se agarro en el espacio exacto
			cmp eax, maxItemX
			jg EndColision // el item esta demasiado lejos (itemX < maxItemX)
			cmp eax, minItemX
			jl EndColision // el item no se agarro a tiempo (itemX > minItemX)
			Colision:
				inc result
		EndColision : // si no aplica nada de lo anterior, no hubo colision
	}

	if (result == 1) {
		return true;
	}
	else {
		return false;
	}
}

/* Igual que CheckColisionNota, pero duplica los valores que vale cada hit en el score y fever. */
bool CheckColisionBoost(POSITION box, POSITION nota, bool isFake) {
	int boxX, boxY;
	int notaY = nota.Y;
	int notaX = nota.X; // posicion exacta
	int outNotaX, sadNotaX, maxNotaX, hitNotaX, missNotaX;
	int result = 0; // si se cambia a 1, hubo colision
	int fake = (isFake) ? 1 : 0;
	switch (notaY) {
	case 126: { // nota en carril 1
		boxY = box.Y - 9;
		outNotaX = -443; // posicion fuera de rango a la derecha
		sadNotaX = -533; // punto entre "sad" y fuera de rango
		maxNotaX = -593; // max posible posicion a la derecha
		hitNotaX = -623; // posicion exacta de la nota
		missNotaX = -713; // min posicion a la izquierda
		break;
	}
	case 166: { // nota en carril 2
		boxY = box.Y - 9;
		outNotaX = -283;
		sadNotaX = -373;
		maxNotaX = -433;
		hitNotaX = -463;
		missNotaX = -553;
		break;
	}
	case 205: { // nota en carril 3
		boxY = box.Y - 10;
		outNotaX = 130;
		sadNotaX = 40;
		maxNotaX = -20;
		hitNotaX = -50;
		missNotaX = -140;
		break;
	}
	default:
		break;
	}

	__asm {
		cld
		mov eax, boxY
		cmp eax, notaY // checa si estan en el mismo carril
		jne EndColision
				mov eax, notaX
				cmp eax, hitNotaX
				je ColisionPerf
				cmp eax, outNotaX
				jg EndColision // la nota esta demasiado lejos, fuera del rango de colision
					cmp eax, sadNotaX
					jg ColisionSad
					cmp eax, maxNotaX
					jg ColisionCool
					cmp eax, missNotaX
					jg ColisionCool
				mov eax, fake
				cmp eax, 1
				je ColisionFake
			// ColisionMiss:
				mov hitText, 4 // notaX < missNotaX == timing "miss", muy lento (0 pts, -6 fever)
				sub feverMeter, 6
				jmp Result
			ColisionCool : // notaX < sadNotaX && notaX > maxNotaX == timing "cool", casi a tiempo (100 pts, +4 fever)
				mov eax, fake
				cmp eax, 1
				je ColisionFake
				//
				mov hitText, 2
				add totalScore, 100
				add feverMeter, 4
			jmp Result
			ColisionSad : // notaX > sadNotaX == timing "sad", muy rapido (20 pts, -2 fever)
				mov eax, fake
				cmp eax, 1
				je ColisionFake
				//
				mov hitText, 3
				add totalScore, 20
				sub feverMeter, 2
			jmp Result
			ColisionPerf : // notaX == hitNotaX == timing "perfect", justo a tiempo (300 pts, +8 fever)
				mov eax, fake
				cmp eax, 1
				je ColisionFake
				//
				mov hitText, 1
				add totalScore, 300
				add feverMeter, 8
			jmp Result
			ColisionFake : // la colision fue con una nota falsa (-100 pts, -4 fever)
				mov hitText, 5
				sub totalScore, 100
				sub feverMeter, 4
			Result :
				inc result
		EndColision : // si no aplica nada de lo anterior, no hubo colision
	}

	if (result == 1) {
		return true;
	}
	else {
		return false;
	}
}

/* Funcion para cargar imagenes y obtener un puntero al area de memoria reservada para la misma.
	@param rutaImagen.			Nombre o ruta de la imagen a cargar en memoria.
	@return unsigned char *.	Direccion base de la imagen.*/
unsigned char * CargaImagen(WCHAR rutaImagen[], DIMENSION * dmn)
{
	unsigned char * ptrImagen;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR  gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	Bitmap *bitmap=new Bitmap(rutaImagen);
	BitmapData *bitmapData=new BitmapData;

	dmn->ANCHO = bitmap->GetWidth();
	dmn->ALTO = bitmap->GetHeight();

	Rect rect(0, 0, dmn->ANCHO, dmn->ALTO);

	//Reservamos espacio en memoria para la imagen
	bitmap->LockBits(&rect, ImageLockModeRead, PixelFormat32bppRGB, bitmapData);

	//"pixels" es el puntero al area de memoria que ocupa la imagen
	unsigned char* pixels = (unsigned char*)bitmapData->Scan0;

	//"tamaño" lo usaremos para reservar los bytes que necesita la imagen. 
	//Para calcular la cantidad de bytes total necesitamos multiplicamos el area de la imagen * 4. 
	//Se multiplica por 4 debido a que cada pixel ocupa 4 bytes de memoria. Noten el 3er parametro de la funcion LockBits, dos lineas de codigo arriba.
	//PixelFormat32bppARGB -> Specifies that the format is 32 bits per pixel; 8 bits each are used for the alpha, red, green, and blue components.
	//Mas info: https://msdn.microsoft.com/en-us/library/system.drawing.imaging.pixelformat(v=vs.110).aspx
	int tamaño;
	tamaño = dmn->ANCHO * dmn->ALTO * 4;
	//hagamos un try de la reserva de memoria
	try
	{
		ptrImagen = new unsigned char [tamaño]; 
	}
	catch(...)
	{
		return NULL;
	}

	//Después de este for, ptrImagen contiene la direccion en memoria de la imagen.
	for(int i=0, j=tamaño; i < j; i++)
	{
		ptrImagen[i]=pixels[i];
	}

	//Es necesario liberar el espacio en memoria, de lo contrario marcaria una excepcion de no hay espacio de memoria suficiente.
	bitmap->UnlockBits(bitmapData);
	delete bitmapData;
	delete bitmap;
	  
	GdiplusShutdown(gdiplusToken);

	return ptrImagen;
}

/* Funcion que regresa la posicion del sprite en pantalla.
	@param x. Coordenada X en la ventana.
	@param y. Coordenada Y en la ventana.*/
POSITION setPosition(int x, int y) {
	POSITION p;
	p.X = x;
	p.Y = y;
	return p;
}

/* Funciones para dibujar los sprites */
#pragma region
void DibujaScreen(int* buffer, int* imagen, DIMENSION dmn) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	__asm {
		cld
		mov esi, imagen
		mov edi, buffer

		mov ecx, ALTO_VENTANA
		PonerA :
		push ecx  //el alto lo pongo al inicio de mi linea
			mov ecx, ANCHO_VENTANA //recorre el ancho
			PonerL :
		mov eax, [esi]     //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je salir   //si es igual salta a colorro
			//or eax, coloresdif
			mov[edi], eax      //eax lo pongo en mi ventana  (paso mi imagen a la ventana)
			//jmp salir
			//colorro :
				//mov eax, BPP
				//add edi, eax
		salir:
		add edi, BPP	//incremento mi pixel 4 bytes
			add esi, BPP
			loop PonerL
			mov eax, 1024                   //ancho de mi imagen
			mul BPP                          //multiplico por 4 bytes
			add esi, eax                     //incremento para que me pase a la linea de abajo
			mov eax, ANCHO_VENTANA
			mul BPP
			sub esi, eax                     //resto para posicionarlo al principio de mi linea
			pop ecx   //para sacar el alto y restarle uno
			loop PonerA
	}
}

void DibujaNubes(int* buffer, int* imagen, DIMENSION dmn, int incremento) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	__asm {
		cld
		mov esi, imagen
		mov edi, buffer

		mov ecx, ALTO_VENTANA

		mov eax, incremento   //mi parametro para incrementar los pixeles, lo mando a eax
		mul BPP               //multiplico por los bytes
		add esi, eax          //se lo agrego a mi imagen para que se recorra

		PonerA :
			push ecx  //el alto lo pongo al inicio de mi linea
			mov ecx, ANCHO_VENTANA //recorre el ancho
			PonerL :
				mov eax, [esi]     //mi imagen la paso a eax
				mov[edi], eax      //eax lo pongo en mi ventana  (paso mi imagen a la ventana)
				add edi, BPP       //incremento mi pixel 4 bytes
				add esi, BPP
			loop PonerL
			mov eax, 1024                   //ancho de mi imagen
			mul BPP                          //multiplico por 4 bytes
			add esi, eax                     //incremento para que me pase a la linea de abajo
			mov eax, ANCHO_VENTANA
			mul BPP
			sub esi, eax                     //resto para posicionarlo al principio de mi linea
			pop ecx   //para sacar el alto y restarle uno
		loop PonerA
	}
}

void DibujaCity(int* buffer, int* imagen, DIMENSION dmn, int incremento) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	__asm {
		cld
		mov esi, imagen
		mov edi, buffer

		mov eax, incremento   //mi parametro para incrementar los pixeles, lo mando a eax
		mul BPP               //multiplico por los bytes
		add esi, eax          //se lo agrego a mi imagen para que se recorra

		mov ecx, ALTO_VENTANA
		PonerA :
			push ecx  //el alto lo pongo al inicio de mi linea
			mov ecx, ANCHO_VENTANA //recorre el ancho
			PonerL :
				mov eax, [esi]     //mi imagen la paso a eax
				cmp eax, 0FFFF0000h    //comparo lo rojo 
				je salir   //si es igual salta a colorro
				mov[edi], eax      //eax lo pongo en mi ventana  (paso mi imagen a la ventana)
				salir:
					add edi, BPP	//incremento mi pixel 4 bytes
					add esi, BPP
			loop PonerL
			mov eax, 1024                   //ancho de mi imagen
			mul BPP                          //multiplico por 4 bytes
			add esi, eax                     //incremento para que me pase a la linea de abajo
			mov eax, ANCHO_VENTANA
			mul BPP
			sub esi, eax                     //resto para posicionarlo al principio de mi linea
			pop ecx   //para sacar el alto y restarle uno
		loop PonerA
	}
}

void DibujaPalms(int* buffer, int* imagen, DIMENSION dmn, int incremento) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	__asm {
		cld
		mov esi, imagen
		mov edi, buffer

		mov eax, incremento   //mi parametro para incrementar los pixeles, lo mando a eax
		mul BPP               //multiplico por los bytes
		add esi, eax          //se lo agrego a mi imagen para que se recorra

		mov ecx, ALTO_VENTANA
		PonerA :
		push ecx  //el alto lo pongo al inicio de mi linea
			mov ecx, ANCHO_VENTANA //recorre el ancho
			PonerL :
		mov eax, [esi]     //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je salir   //si es igual salta a colorro
			mov[edi], eax      //eax lo pongo en mi ventana  (paso mi imagen a la ventana)
			salir :
		add edi, BPP	//incremento mi pixel 4 bytes
			add esi, BPP
			loop PonerL
			mov eax, 1024                   //ancho de mi imagen
			mul BPP                          //multiplico por 4 bytes
			add esi, eax                     //incremento para que me pase a la linea de abajo
			mov eax, ANCHO_VENTANA
			mul BPP
			sub esi, eax                     //resto para posicionarlo al principio de mi linea
			pop ecx   //para sacar el alto y restarle uno
			loop PonerA
	}
}

void DibujaCalle(int * buffer, int * imagen, DIMENSION dmn, int incremento) {
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	__asm {
		cld
		mov esi, imagen
		mov edi, buffer

		mov eax, incremento   //mi parametro para incrementar los pixeles, lo mando a eax
		mul BPP               //multiplico por los bytes
		add esi, eax          //se lo agrego a mi imagen para que se recorra

		mov ecx, ALTO_VENTANA
		PonerA :
			push ecx  //el alto lo pongo al inicio de mi linea
			mov ecx, ANCHO_VENTANA //recorre el ancho
			PonerL :
				mov eax, [esi]     //mi imagen la paso a eax
				cmp eax, 0FFFF0000h    //comparo lo rojo 
				je salir   //si es igual salta a colorro
				//or eax, coloresdif
				mov[edi], eax      //eax lo pongo en mi ventana  (paso mi imagen a la ventana)
				//jmp salir
				//colorro :
					//mov eax, BPP
					//add edi, eax
				salir :
					add edi, BPP	//incremento mi pixel 4 bytes
					add esi, BPP
			loop PonerL
			mov eax, 3149                   //ancho de mi imagen
			mul BPP                          //multiplico por 4 bytes
			add esi, eax                     //incremento para que me pase a la linea de abajo
			mov eax, ANCHO_VENTANA
			mul BPP
			sub esi, eax                     //resto para posicionarlo al principio de mi linea
			pop ecx   //para sacar el alto y restarle uno
		loop PonerA
	}
}

void DibujaNota(int* buffer, int* nota, DIMENSION dmn, POSITION pos, int indi)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	__asm {
		cld
		//cargo el sprite
		mov esi, nota
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 769000

		mov eax, 16   //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		mul indi
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx
		ScaladoY :
			push ecx
			mov ecx, scaleNotas
			repite :
				push ecx
				mov ecx, 16
				mostrar :
					mov eax, [esi]         //mi imagen la paso a eax
					cmp eax, 0FFFF0000h    //comparo lo rojo 
					je colorro   //si es igual salta a colorro
					//or eax, coloresdif
					push ecx
					mov ecx, scaleNotas
					scalax :
						mov[edi], eax
						add edi, BPP
					loop scalax
					pop ecx
					jmp Salir
					colorro :
						mov eax, BPP
						mul scaleNotas
						add edi, eax
					Salir :
						add esi, BPP
				loop mostrar
				mov eax, ANCHO_VENTANA
				mul BPP
				add edi, eax
				MOV EAX, 16
				MUL BPP
				sub esi, eax
				mul scaleNotas
				SUB EDI, EAX
				pop ecx
			loop repite
			mov eax, w
			mul BPP
			add esi, eax
			pop ecx
		loop ScaladoY
	}

}

void DibujaItem(int* buffer, int* item, DIMENSION dmn, POSITION pos, int indi)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	__asm {
		cld
		//cargo el sprite
		mov esi, item
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 769000

		mov eax, 24   //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		mul indi    //para moverlo con la tecla que asigne, si le pico una ves el 0 default se convierte en uno, de multiplica por 59 y se mueve el sprite
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx
		ScaladoY :
		push ecx
			mov ecx, scaleItem
			repite :
		push ecx
			mov ecx, 24
			mostrar :
			mov eax, [esi]         //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je colorro   //si es igual salta a colorro
			//or eax, coloresdif
			push ecx
			mov ecx, scaleItem
			scalax :
		mov[edi], eax
			add edi, BPP
			loop scalax
			pop ecx
			jmp Salir
			colorro :
		mov eax, BPP
			mul scaleItem
			add edi, eax
			Salir :
		add esi, BPP
			loop mostrar
			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			MOV EAX, 24
			MUL BPP
			sub esi, eax
			mul scaleItem
			SUB EDI, EAX
			pop ecx
			loop repite
			mov eax, w
			mul BPP
			add esi, eax

			pop ecx
			loop ScaladoY
	}

}

void DibujaCarro(int *buffer, int *carro, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = posCar.X;
	int y = posCar.Y;

	__asm{
		cld
		//cargo el sprite
		mov esi, carro
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 769000

		mov eax, 105   //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		mul indiCarro    //para moverlo con la tecla que asigne, si le pico una ves el 0 default se convierte en uno, de multiplica por 59 y se mueve el sprite
		add esi, eax

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx
		ScaladoY :
			push ecx
			mov ecx, scaleCarro
			repite :
				push ecx
				mov ecx, 105
				mostrar :
					mov eax, [esi]         //mi imagen la paso a eax
					cmp eax, 0FFFF0000h    //comparo lo rojo 
				je colorro   //si es igual salta a colorro
					//or eax, coloresdif
					push ecx
					mov ecx, scaleCarro
					scalax :
						mov[edi], eax
						add edi, BPP
					loop scalax
					pop ecx
				jmp Salir
				colorro :
					mov eax, BPP
					mul scaleCarro
					add edi, eax
				Salir :
					add esi, BPP
				loop mostrar
				mov eax, ANCHO_VENTANA
				mul BPP
				add edi, eax
				MOV EAX, 105
				MUL BPP
				sub esi, eax
				mul scaleCarro
				SUB EDI, EAX
				pop ecx
			loop repite
			mov eax, w
			mul BPP
			add esi, eax
			
			pop ecx
		loop ScaladoY
	}
	
}

void DibujaBox(int* buffer, int* box, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	__asm {
		cld

		mov esi, box
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 680000


		mov eax, 80        //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		add esi, eax

		xor ecx, ecx
		sub h, 1
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 80

			//PARA QUITAR EL FONDO ROJO
			muestra :
			mov eax, [esi]         //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je color1   //si es igual salta a color1
			mov[edi], eax
			color1 :
		add esi, BPP   //incrementa y pasa el pixel rojo
			add edi, BPP
			loop muestra

			//suma el total para luego restar y posicionarlo en la linea de abajo         
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 80
			mul BPP
			sub edi, eax

			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 80
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}

void DibujaFever(int* buffer, int* fever, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;
	__asm {
		cld

		mov esi, fever
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 680000

		xor ecx, ecx
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 263

			//PARA QUITAR EL FONDO ROJO
			muestra :
				mov eax, [esi]         //mi imagen la paso a eax
				cmp eax, 0FFFF0000h    //comparo lo rojo 
				je color1   //si es igual salta a color1
				/*or eax, coloresdif*/
				mov[edi], eax
				color1 :
				add esi, BPP   //incrementa y pasa el pixel rojo
				add edi, BPP
			loop muestra

			//suma el total para luego restar y posicionarlo en la linea de abajo         
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 263
			mul BPP
			sub edi, eax

			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 263
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}

void DibujaScore(int* buffer, int* score, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	__asm {
		cld

		mov esi, score
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 680000


		mov eax, 295        //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		add esi, eax

		xor ecx, ecx
		sub h, 5
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 295

			//PARA QUITAR EL FONDO ROJO
			muestra :
			mov eax, [esi]         //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je color1   //si es igual salta a color1
			mov[edi], eax
			color1 :
		add esi, BPP   //incrementa y pasa el pixel rojo
			add edi, BPP
			loop muestra

			//suma el total para luego restar y posicionarlo en la linea de abajo         
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 295
			mul BPP
			sub edi, eax

			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 295
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}

void DibujaNumber(int* buffer, int* number, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;

	__asm {
		cld

		mov esi, number
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 680000


		mov eax, 23        //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		add esi, eax

		xor ecx, ecx
		sub h, 1
		mov ecx, h    //asigno mi altura a ecx

		repetir :
		push ecx     //guardo en mi pila ecx
			mov ecx, 23

			//PARA QUITAR EL FONDO ROJO
			muestra :
			mov eax, [esi]         //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je color1   //si es igual salta a color1
			mov[edi], eax
			color1 :
		add esi, BPP   //incrementa y pasa el pixel rojo
			add edi, BPP
			loop muestra

			//suma el total para luego restar y posicionarlo en la linea de abajo         
			mov eax, ANCHO_VENTANA
			mul BPP

			add edi, eax
			mov eax, 23
			mul BPP
			sub edi, eax

			//MUEVE EL PUNTERO A LA SIGUIENTE LINEA DE LA IMAGEN PARA SEGUIR DIBUJANDO
			mov eax, w
			mul BPP

			add esi, eax
			mov eax, 23
			mul BPP
			sub esi, eax

			pop ecx
			loop repetir
	}

}

void DibujaText(int* buffer, int* text, DIMENSION dmn, POSITION pos)
{
	int w = dmn.ANCHO;
	int h = dmn.ALTO;
	int x = pos.X;
	int y = pos.Y;
	int scale = 2;

	__asm {
		cld
		//cargo el sprite
		mov esi, text
		mov edi, buffer

		//posicionar mi sprite		
		mov eax, y
		mov ebx, 9200
		mul ebx
		add edi, eax

		mov eax, x
		mul BPP
		add edi, eax

		add edi, 680000

		mov eax, 100   //porque es la cantidad de pixeles de cada cuadrito
		mul BPP
		add esi, eax

		xor ecx, ecx
		sub h, 1
		mov ecx, h    //asigno mi altura a ecx
		ScaladoY :
		push ecx
			mov ecx, scale
			repite :
		push ecx
			mov ecx, 100
			mostrar :
			mov eax, [esi]         //mi imagen la paso a eax
			cmp eax, 0FFFF0000h    //comparo lo rojo 
			je colorro   //si es igual salta a colorro
				//or eax, coloresdif
			push ecx
			mov ecx, scale
			scalax :
		mov[edi], eax
			add edi, BPP
			loop scalax
			pop ecx
			jmp Salir
			colorro :
		mov eax, BPP
			mul scale
			add edi, eax
			Salir :
		add esi, BPP
			loop mostrar
			mov eax, ANCHO_VENTANA
			mul BPP
			add edi, eax
			MOV EAX, 100
			MUL BPP
			sub esi, eax
			mul scale
			SUB EDI, EAX
			pop ecx
			loop repite
			mov eax, w
			mul BPP
			add esi, eax
			pop ecx
			loop ScaladoY
	}

}
#pragma endregion